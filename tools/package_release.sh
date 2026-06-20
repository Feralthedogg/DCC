#!/usr/bin/env sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
source_dir=${DCC_SOURCE_DIR:-$(CDPATH= cd -- "$script_dir/.." && pwd)}
build_dir=${DCC_RELEASE_BUILD_DIR:-"$source_dir/build-release-dist"}
dist_dir=${DCC_DIST_DIR:-"$source_dir/target/dist"}
target=${1:-}

detect_target() {
    os=$(uname -s)
    arch=$(uname -m)
    case "$os" in
        Linux) os_name=linux ;;
        Darwin) os_name=macos ;;
        *) os_name=$(printf '%s' "$os" | LC_ALL=C tr '[:upper:]' '[:lower:]') ;;
    esac
    case "$arch" in
        x86_64|amd64) arch_name=x86_64 ;;
        arm64|aarch64) arch_name=aarch64 ;;
        *) arch_name=$arch ;;
    esac
    printf '%s-%s\n' "$os_name" "$arch_name"
}

project_version() {
    sed -n 's/^[[:space:]]*VERSION[[:space:]]\{1,\}\([0-9][0-9A-Za-z.+-]*\).*/\1/p' \
        "$source_dir/CMakeLists.txt" | head -n 1
}

if [ -z "$target" ]; then
    target=$(detect_target)
fi

cmake_version=$(project_version)
version=${DCC_RELEASE_VERSION:-${GITHUB_REF_NAME:-$cmake_version}}
version=${version#v}
if [ -z "$version" ]; then
    echo "cannot determine release version" >&2
    exit 1
fi
version_base=${version%%-*}
if [ "$version_base" != "$cmake_version" ] && [ "${DCC_ALLOW_VERSION_MISMATCH:-0}" != "1" ]; then
    echo "release version $version does not match CMake project version $cmake_version" >&2
    echo "update CMakeLists.txt or set DCC_ALLOW_VERSION_MISMATCH=1 for an explicit development package" >&2
    exit 2
fi

case "$version" in
    *[!abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._+-]*|"")
        echo "invalid release version: $version" >&2
        exit 2
        ;;
esac
case "$target" in
    *[!abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._+-]*|"")
        echo "invalid release target: $target" >&2
        exit 2
        ;;
esac
case "$target" in
    linux-x86_64|linux-aarch64|macos-x86_64|macos-aarch64|\
    freebsd-x86_64|freebsd-aarch64|openbsd-x86_64|openbsd-aarch64|\
    netbsd-x86_64|netbsd-aarch64|dragonflybsd-x86_64) ;;
    *)
        echo "unsupported release target: $target" >&2
        echo "supported targets: linux-x86_64 linux-aarch64 macos-x86_64 macos-aarch64 freebsd-x86_64 freebsd-aarch64 openbsd-x86_64 openbsd-aarch64 netbsd-x86_64 netbsd-aarch64 dragonflybsd-x86_64" >&2
        exit 2
        ;;
esac

write_sha256() {
    file=$1
    out=$2
    name=${file##*/}
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$file" >"$out"
    elif command -v shasum >/dev/null 2>&1; then
        shasum -a 256 "$file" >"$out"
    elif command -v openssl >/dev/null 2>&1; then
        hash=$(openssl dgst -sha256 "$file" | awk '{ print $NF }')
        printf '%s  %s\n' "$hash" "$name" >"$out"
    elif command -v sha256 >/dev/null 2>&1; then
        if hash=$(sha256 -q "$file" 2>/dev/null); then
            :
        else
            hash=$(sha256 "$file" | awk '{ print $NF }')
        fi
        printf '%s  %s\n' "$hash" "$name" >"$out"
    elif command -v cksum >/dev/null 2>&1; then
        hash=$(cksum -a sha256 "$file" 2>/dev/null | awk '{
            for (i = 1; i <= NF; i++) {
                if (length($i) == 64 && $i ~ /^[0-9A-Fa-f]+$/) {
                    print tolower($i)
                    exit
                }
            }
        }')
        if [ -n "$hash" ]; then
            printf '%s  %s\n' "$hash" "$name" >"$out"
        else
            echo "could not parse cksum sha256 output for $file" >&2
            exit 127
        fi
    else
        echo "sha256sum, shasum, openssl, sha256, or cksum is required" >&2
        exit 127
    fi
}

mkdir -p "$dist_dir"
rm -rf "$build_dir/package-output"

if [ -n "${DCC_CMAKE_GENERATOR:-}" ]; then
    # shellcheck disable=SC2086
    cmake -S "$source_dir" -B "$build_dir" -G "$DCC_CMAKE_GENERATOR" \
        -DCMAKE_BUILD_TYPE="${DCC_BUILD_TYPE:-Release}" \
        -DDCC_LLAM_ROOT="${DCC_LLAM_ROOT:-$source_dir/../LLAM}" \
        -DDCC_LLAM_LIBRARY="${DCC_LLAM_LIBRARY:-$source_dir/../LLAM/libllam_runtime.a}" \
        -DDCC_LLAM_USE_SUBDIRECTORY="${DCC_LLAM_USE_SUBDIRECTORY:-ON}" \
        -DDCC_BUNDLE_LLAM="${DCC_BUNDLE_LLAM:-ON}" \
        ${DCC_EXTRA_CMAKE_ARGS:-}
else
    # shellcheck disable=SC2086
    cmake -S "$source_dir" -B "$build_dir" \
        -DCMAKE_BUILD_TYPE="${DCC_BUILD_TYPE:-Release}" \
        -DDCC_LLAM_ROOT="${DCC_LLAM_ROOT:-$source_dir/../LLAM}" \
        -DDCC_LLAM_LIBRARY="${DCC_LLAM_LIBRARY:-$source_dir/../LLAM/libllam_runtime.a}" \
        -DDCC_LLAM_USE_SUBDIRECTORY="${DCC_LLAM_USE_SUBDIRECTORY:-ON}" \
        -DDCC_BUNDLE_LLAM="${DCC_BUNDLE_LLAM:-ON}" \
        ${DCC_EXTRA_CMAKE_ARGS:-}
fi

if [ -n "${DCC_BUILD_PARALLEL:-}" ]; then
    cmake --build "$build_dir" --parallel "$DCC_BUILD_PARALLEL"
else
    cmake --build "$build_dir"
fi

if [ "${DCC_SKIP_RELEASE_TESTS:-0}" != "1" ]; then
    release_ctest_regex=${DCC_RELEASE_CTEST_REGEX:-^dcc_}
    release_ctest_exclude=${DCC_RELEASE_CTEST_EXCLUDE:-^dcc_cluster_chaos_smoke$}
    if [ -n "$release_ctest_exclude" ]; then
        ctest --test-dir "$build_dir" \
            -R "$release_ctest_regex" \
            -E "$release_ctest_exclude" \
            --output-on-failure \
            --timeout "${DCC_CTEST_TIMEOUT:-180}"
    else
        ctest --test-dir "$build_dir" \
            -R "$release_ctest_regex" \
            --output-on-failure \
            --timeout "${DCC_CTEST_TIMEOUT:-180}"
    fi
fi

cpack -G TGZ \
    --config "$build_dir/CPackConfig.cmake" \
    -B "$build_dir/package-output" \
    -D "CPACK_PACKAGE_FILE_NAME=dcc-$version-$target"
cpack -G TGZ \
    --config "$build_dir/CPackSourceConfig.cmake" \
    -B "$build_dir/package-output" \
    -D "CPACK_PACKAGE_FILE_NAME=dcc-$version-source" \
    -D "CPACK_SOURCE_PACKAGE_FILE_NAME=dcc-$version-source"

package_archive=$(find "$build_dir/package-output" -maxdepth 1 -type f -name 'dcc-*.tar.gz' ! -name '*Source*' ! -name '*-source.tar.gz' -print | head -n 1)
source_archive=$(find "$build_dir/package-output" -maxdepth 1 -type f \( -name 'dcc-*Source*.tar.gz' -o -name 'dcc-*-source.tar.gz' \) -print | head -n 1)

if [ -z "$package_archive" ]; then
    echo "binary package archive was not created" >&2
    exit 1
fi
if [ -z "$source_archive" ]; then
    echo "source package archive was not created" >&2
    exit 1
fi

binary_out="$dist_dir/dcc-$version-$target.tar.gz"
source_out="$dist_dir/dcc-$version-source.tar.gz"
cp "$package_archive" "$binary_out"
cp "$source_archive" "$source_out"

(
    cd "$dist_dir"
    write_sha256 "$(basename "$binary_out")" "$(basename "$binary_out").sha256"
    write_sha256 "$(basename "$source_out")" "$(basename "$source_out").sha256"
)

tar -tzf "$binary_out" | grep '/include/dcc/dcc.h$' >/dev/null
tar -tzf "$binary_out" | grep '/include/llam/runtime.h$' >/dev/null
tar -tzf "$binary_out" | grep -E '/lib/libllam_runtime(\.a|\.so|\.dylib)(\.[0-9.]+)?$' >/dev/null
tar -tzf "$binary_out" | grep '/lib/cmake/llam/llam-config.cmake$' >/dev/null
tar -tzf "$binary_out" | grep '/lib/pkgconfig/llam.pc$' >/dev/null
tar -tzf "$binary_out" | grep '/lib/pkgconfig/dcc.pc$' >/dev/null
tar -tzf "$binary_out" | grep '/share/dcc/docs/api.md$' >/dev/null
tar -tzf "$source_out" | grep '/CMakeLists.txt$' >/dev/null
tar -tzf "$source_out" | grep '/docs/index.md$' >/dev/null

printf '%s\n' "$binary_out"
printf '%s\n' "$source_out"
