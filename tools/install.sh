#!/bin/sh
set -eu

prefix="${DCC_INSTALL_PREFIX:-/usr/local}"
version="${DCC_INSTALL_VERSION:-latest}"
target="${DCC_INSTALL_TARGET:-}"
target_explicit=0
[ -n "$target" ] && target_explicit=1
base_url="${DCC_INSTALL_BASE_URL:-}"
llam_mode="${DCC_INSTALL_LLAM:-skip}"
llam_version="${DCC_INSTALL_LLAM_VERSION:-2.2.0}"
llam_installer_url="${DCC_LLAM_INSTALLER_URL:-}"
dry_run=0
force=0

usage() {
    cat <<'EOF'
usage: install.sh [--prefix <dir>] [--version <version|latest>] [--target <target>]
                  [--base-url <url>] [--install-llam] [--skip-llam] [--llam-version <version|latest>]
                  [--dry-run] [--force]

Installs DCC from a GitHub release archive. LLAM is a separate runtime
dependency; install LLAM first or pass --install-llam to install the
DCC-tested LLAM 2.2.0 runtime into the same prefix. Pass --llam-version
latest to intentionally use the newest LLAM release.
EOF
}

while [ "$#" -gt 0 ]; do
    case "$1" in
        --prefix)
            [ "$#" -ge 2 ] || { echo "--prefix requires a value" >&2; exit 2; }
            prefix="$2"
            shift 2
            ;;
        --prefix=*)
            prefix="${1#--prefix=}"
            shift
            ;;
        --version)
            [ "$#" -ge 2 ] || { echo "--version requires a value" >&2; exit 2; }
            version="$2"
            shift 2
            ;;
        --version=*)
            version="${1#--version=}"
            shift
            ;;
        --target)
            [ "$#" -ge 2 ] || { echo "--target requires a value" >&2; exit 2; }
            target="$2"
            target_explicit=1
            shift 2
            ;;
        --target=*)
            target="${1#--target=}"
            target_explicit=1
            shift
            ;;
        --base-url)
            [ "$#" -ge 2 ] || { echo "--base-url requires a value" >&2; exit 2; }
            base_url="$2"
            shift 2
            ;;
        --base-url=*)
            base_url="${1#--base-url=}"
            shift
            ;;
        --skip-llam)
            llam_mode=skip
            shift
            ;;
        --install-llam)
            llam_mode=latest
            shift
            ;;
        --llam-version)
            [ "$#" -ge 2 ] || { echo "--llam-version requires a value" >&2; exit 2; }
            llam_version="$2"
            llam_mode=latest
            shift 2
            ;;
        --llam-version=*)
            llam_version="${1#--llam-version=}"
            llam_mode=latest
            shift
            ;;
        --dry-run)
            dry_run=1
            shift
            ;;
        --force)
            force=1
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "unknown option: $1" >&2
            usage >&2
            exit 2
            ;;
    esac
done

fetch_url() {
    url=$1
    if command -v curl >/dev/null 2>&1; then
        curl -fsSL "$url"
    elif command -v wget >/dev/null 2>&1; then
        wget -qO- "$url"
    else
        echo "curl or wget is required" >&2
        exit 1
    fi
}

download_file() {
    url=$1
    out=$2
    if command -v curl >/dev/null 2>&1; then
        curl -fL "$url" -o "$out"
    elif command -v wget >/dev/null 2>&1; then
        wget -O "$out" "$url"
    else
        echo "curl or wget is required" >&2
        exit 1
    fi
}

sha256_file() {
    path=$1
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$path" | awk '{ print $1 }'
    elif command -v shasum >/dev/null 2>&1; then
        shasum -a 256 "$path" | awk '{ print $1 }'
    elif command -v openssl >/dev/null 2>&1; then
        openssl dgst -sha256 -r "$path" | awk '{ print $1 }'
    else
        echo "sha256sum, shasum, or openssl is required" >&2
        exit 1
    fi
}

latest_release_tag() {
    repo=$1
    tag=$(fetch_url "https://api.github.com/repos/$repo/releases/latest" |
        sed -n 's/.*"tag_name"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/p' |
        sed -n '1p')
    if [ -z "$tag" ]; then
        echo "could not resolve latest release tag for $repo" >&2
        exit 1
    fi
    printf '%s\n' "$tag"
}

detect_target() {
    os=$(uname -s)
    arch=$(uname -m)
    case "$os" in
        Linux) os_name=linux ;;
        Darwin) os_name=macos ;;
        FreeBSD) os_name=freebsd ;;
        OpenBSD) os_name=openbsd ;;
        NetBSD) os_name=netbsd ;;
        DragonFly) os_name=dragonflybsd ;;
        *)
            echo "unsupported OS for DCC installer: $os" >&2
            exit 1
            ;;
    esac
    case "$arch" in
        x86_64|amd64) arch_name=x86_64 ;;
        arm64|aarch64) arch_name=aarch64 ;;
        *)
            echo "unsupported architecture for DCC installer: $arch" >&2
            exit 1
            ;;
    esac
    printf '%s-%s\n' "$os_name" "$arch_name"
}

warn_cross_target() {
    [ "$target_explicit" -eq 1 ] || return 0
    host_target=$(detect_target 2>/dev/null || true)
    [ -n "$host_target" ] || return 0
    [ "$host_target" = "$target" ] && return 0
    echo "warning: installing DCC target $target on host $host_target; binaries and libraries may not run on this machine" >&2
}

validate_component() {
    name=$1
    value=$2
    case "$value" in
        ""|*[!abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._+-]*)
            echo "invalid $name: $value" >&2
            exit 2
            ;;
    esac
}

verify_checksum() {
    archive=$1
    checksum=$2
    archive_name=$3
    expected=$(awk 'NF { print $1; exit }' "$checksum")
    checksum_target=$(awk 'NF { print $2; exit }' "$checksum")
    actual=$(sha256_file "$archive")
    case "$expected" in
        ""|*[!0123456789abcdefABCDEF]*)
            echo "invalid checksum digest for $archive_name" >&2
            exit 1
            ;;
    esac
    if [ "${#expected}" -ne 64 ]; then
        echo "invalid checksum length for $archive_name" >&2
        exit 1
    fi
    case "$checksum_target" in
        ""|"$archive_name"|\*"$archive_name") ;;
        *)
            echo "checksum target $checksum_target does not match $archive_name" >&2
            exit 1
            ;;
    esac
    expected=$(printf '%s' "$expected" | LC_ALL=C tr '[:upper:]' '[:lower:]')
    actual=$(printf '%s' "$actual" | LC_ALL=C tr '[:upper:]' '[:lower:]')
    if [ "$expected" != "$actual" ]; then
        echo "checksum mismatch for $archive_name" >&2
        exit 1
    fi
}

extract_tar_gz() {
    archive=$1
    dst=$2
    if tar -xzf "$archive" -C "$dst"; then
        return 0
    fi
    if command -v gzip >/dev/null 2>&1; then
        gzip -dc "$archive" | (cd "$dst" && tar -xf -)
        return $?
    fi
    echo "could not extract $archive; tar -z or gzip is required" >&2
    return 1
}

copy_dir() {
    src=$1
    dst=$2
    [ -d "$src" ] || return 0
    if [ "$force" -ne 1 ]; then
        (cd "$src" && find . \( -type f -o -type l \) -print) | while IFS= read -r rel; do
            rel=${rel#./}
            if [ -e "$dst/$rel" ] || [ -L "$dst/$rel" ]; then
                echo "refusing to overwrite $dst/$rel; pass --force" >&2
                exit 1
            fi
        done
    fi
    if [ "$dry_run" -eq 1 ]; then
        echo "copy $src -> $dst"
        return 0
    fi
    mkdir -p "$dst"
    cp -R "$src/." "$dst/"
}

install_tree() {
    root=$1
    [ -f "$root/include/dcc/dcc.h" ] || {
        echo "DCC archive does not contain include/dcc/dcc.h" >&2
        exit 1
    }

    copy_dir "$root/include" "$prefix/include"
    copy_dir "$root/lib" "$prefix/lib"
    copy_dir "$root/bin" "$prefix/bin"
    copy_dir "$root/share" "$prefix/share"
    [ "$dry_run" -eq 1 ] || echo "installed DCC into $prefix"
}

install_latest_llam() {
    [ "$llam_mode" = "skip" ] && return 0

    resolved_llam_version=$llam_version
    if [ "$resolved_llam_version" = "latest" ]; then
        resolved_llam_version=$(latest_release_tag Feralthedogg/LLAM)
    fi
    resolved_llam_version=${resolved_llam_version#v}
    validate_component "LLAM version" "$resolved_llam_version"
    resolved_llam_installer_url=${llam_installer_url:-https://github.com/Feralthedogg/LLAM/releases/download/v$resolved_llam_version/install.sh}

    if [ "$dry_run" -eq 1 ]; then
        echo "download $resolved_llam_installer_url"
        echo "install LLAM $resolved_llam_version into $prefix"
        return 0
    fi

    installer="$tmp_dir/llam-install.sh"
    download_file "$resolved_llam_installer_url" "$installer"
    sh "$installer" \
        --prefix "$prefix" \
        --version "$resolved_llam_version" \
        --target "$target" \
        --force
}

if [ -z "$target" ]; then
    target=$(detect_target)
fi
validate_component "target" "$target"
warn_cross_target

if [ "$version" = "latest" ]; then
    version=$(latest_release_tag Feralthedogg/DCC)
fi
release_tag=$version
version=${version#v}
validate_component "version" "$version"
case "$release_tag" in
    v*) ;;
    *) release_tag="v$release_tag" ;;
esac

base_url=${base_url:-https://github.com/Feralthedogg/DCC/releases/download/$release_tag}
package="dcc-$version-$target"
package_archive="$package.tar.gz"

tmp_dir=$(mktemp -d "${TMPDIR:-/tmp}/dcc-install.XXXXXX")
trap 'rm -rf "$tmp_dir"' EXIT HUP INT TERM

if [ "$dry_run" -eq 1 ]; then
    echo "download $base_url/$package_archive"
    echo "download $base_url/$package_archive.sha256"
    echo "verify $package_archive.sha256"
    echo "extract $package_archive"
    echo "install extracted $package -> $prefix"
    install_latest_llam
    exit 0
else
    download_file "$base_url/$package_archive" "$tmp_dir/$package_archive"
    download_file "$base_url/$package_archive.sha256" "$tmp_dir/$package_archive.sha256"
    verify_checksum "$tmp_dir/$package_archive" "$tmp_dir/$package_archive.sha256" "$package_archive"
    extract_tar_gz "$tmp_dir/$package_archive" "$tmp_dir"
fi

package_root=$(find "$tmp_dir" -type f -path '*/include/dcc/dcc.h' -print |
    sed 's#/include/dcc/dcc.h$##' |
    sed -n '1p')
if [ -z "$package_root" ]; then
    echo "could not find extracted DCC package root" >&2
    exit 1
fi
install_tree "$package_root"

install_latest_llam
