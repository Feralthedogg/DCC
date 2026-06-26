#!/usr/bin/env sh
set -eu

step() {
    printf '\n==> %s\n' "$*"
}

is_true() {
    case "${1:-0}" in
        1|ON|on|TRUE|true|YES|yes) return 0 ;;
        *) return 1 ;;
    esac
}

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
source_dir=${DCC_SOURCE_DIR:-$(CDPATH= cd -- "$script_dir/.." && pwd)}

build_dir=${DCC_BUILD_DIR:-"$source_dir/build-release-check"}
asan_build_dir=${DCC_ASAN_BUILD_DIR:-"$source_dir/build-release-asan"}
noopus_build_dir=${DCC_NOOPUS_BUILD_DIR:-"$source_dir/build-release-noopus"}
fuzz_build_dir=${DCC_FUZZ_BUILD_DIR:-"$source_dir/build-release-fuzz"}
minimal_build_dir=${DCC_MINIMAL_BUILD_DIR:-"$source_dir/build-release-minimal"}
package_prefix=${DCC_PACKAGE_PREFIX:-"$source_dir/build-release-package"}
package_consumer_build_dir=${DCC_PACKAGE_CONSUMER_BUILD_DIR:-"$source_dir/build-release-package-consumer"}
minimal_package_prefix=${DCC_MINIMAL_PACKAGE_PREFIX:-"$source_dir/build-release-minimal-package"}
minimal_package_consumer_build_dir=${DCC_MINIMAL_PACKAGE_CONSUMER_BUILD_DIR:-"$source_dir/build-release-minimal-package-consumer"}

llam_root=${DCC_LLAM_ROOT:-"$source_dir/../LLAM"}
llam_library=${DCC_LLAM_LIBRARY:-"$llam_root/libllam_runtime.a"}
llam_use_subdirectory=${DCC_LLAM_USE_SUBDIRECTORY:-ON}
bundle_llam=${DCC_BUNDLE_LLAM:-OFF}
ctest_timeout=${DCC_CTEST_TIMEOUT:-180}
ctest_regex=${DCC_CTEST_REGEX:-^dcc_}
ctest_exclude=${DCC_CTEST_EXCLUDE:-}
asan_ctest_exclude=${DCC_ASAN_CTEST_EXCLUDE:-^dcc_cluster_gateway_smoke$}

cmake_configure() {
    build=$1
    shift
    if [ -n "${DCC_CMAKE_GENERATOR:-}" ]; then
        cmake -S "$source_dir" -B "$build" -G "$DCC_CMAKE_GENERATOR" "$@" ${DCC_EXTRA_CMAKE_ARGS:-}
    else
        cmake -S "$source_dir" -B "$build" "$@" ${DCC_EXTRA_CMAKE_ARGS:-}
    fi
}

cmake_build() {
    build=$1
    target=${2:-}
    if [ -n "$target" ]; then
        if [ -n "${DCC_BUILD_PARALLEL:-}" ]; then
            cmake --build "$build" --target "$target" --parallel "$DCC_BUILD_PARALLEL"
        else
            cmake --build "$build" --target "$target"
        fi
    else
        if [ -n "${DCC_BUILD_PARALLEL:-}" ]; then
            cmake --build "$build" --parallel "$DCC_BUILD_PARALLEL"
        else
            cmake --build "$build"
        fi
    fi
}

find_executable() {
    build=$1
    name=$2
    for dir in "$build" "$build/Debug" "$build/Release" "$build/RelWithDebInfo"; do
        if [ -x "$dir/$name" ]; then
            printf '%s\n' "$dir/$name"
            return 0
        fi
        if [ -x "$dir/$name.exe" ]; then
            printf '%s\n' "$dir/$name.exe"
            return 0
        fi
    done
    return 1
}

find_artifact() {
    build=$1
    name=$2
    for dir in "$build" "$build/Debug" "$build/Release" "$build/RelWithDebInfo"; do
        for suffix in "" ".so" ".dylib" ".dll"; do
            if [ -f "$dir/$name$suffix" ]; then
                printf '%s\n' "$dir/$name$suffix"
                return 0
            fi
        done
    done
    return 1
}

run_ctest() {
    build=$1
    exclude=${2:-$ctest_exclude}
    if [ -n "$exclude" ]; then
        ctest --test-dir "$build" -R "$ctest_regex" -E "$exclude" --output-on-failure --timeout "$ctest_timeout"
    else
        ctest --test-dir "$build" -R "$ctest_regex" --output-on-failure --timeout "$ctest_timeout"
    fi
}

package_llam_library_path() {
    if [ -f "$llam_library" ]; then
        printf '%s\n' "$llam_library"
    elif [ -f "$build_dir/_deps/llam/libllam_runtime.a" ]; then
        printf '%s\n' "$build_dir/_deps/llam/libllam_runtime.a"
    else
        printf '%s\n' "$llam_library"
    fi
}

installed_llam_bundle_check() {
    prefix=$1

    [ -f "$prefix/include/llam/runtime.h" ] || {
        printf 'installed package is missing bundled LLAM header\n' >&2
        return 1
    }
    [ -f "$prefix/lib/cmake/llam/llam-config.cmake" ] || {
        printf 'installed package is missing bundled LLAM CMake config\n' >&2
        return 1
    }
    [ -f "$prefix/lib/pkgconfig/llam.pc" ] || {
        printf 'installed package is missing bundled LLAM pkg-config metadata\n' >&2
        return 1
    }
    find "$prefix/lib" -maxdepth 1 \( -type f -o -type l \) -name 'libllam_runtime*' -print | grep . >/dev/null || {
        printf 'installed package is missing bundled LLAM runtime library\n' >&2
        return 1
    }
}

configure_package_consumer() {
    consumer_source=$1
    consumer_build=$2
    prefix=$3
    package_llam=${4:-}

    if [ -n "${DCC_CMAKE_GENERATOR:-}" ] && [ -n "$package_llam" ]; then
        cmake -S "$consumer_source" -B "$consumer_build" \
            -G "$DCC_CMAKE_GENERATOR" \
            -DCMAKE_BUILD_TYPE=Debug \
            -DCMAKE_PREFIX_PATH="$prefix" \
            -DDCC_LLAM_ROOT="$llam_root" \
            -DDCC_LLAM_LIBRARY="$package_llam"
    elif [ -n "${DCC_CMAKE_GENERATOR:-}" ]; then
        cmake -S "$consumer_source" -B "$consumer_build" \
            -G "$DCC_CMAKE_GENERATOR" \
            -DCMAKE_BUILD_TYPE=Debug \
            -DCMAKE_PREFIX_PATH="$prefix"
    elif [ -n "$package_llam" ]; then
        cmake -S "$consumer_source" -B "$consumer_build" \
            -DCMAKE_BUILD_TYPE=Debug \
            -DCMAKE_PREFIX_PATH="$prefix" \
            -DDCC_LLAM_ROOT="$llam_root" \
            -DDCC_LLAM_LIBRARY="$package_llam"
    else
        cmake -S "$consumer_source" -B "$consumer_build" \
            -DCMAKE_BUILD_TYPE=Debug \
            -DCMAKE_PREFIX_PATH="$prefix"
    fi
}

pkg_config_tool() {
    if command -v pkg-config >/dev/null 2>&1; then
        command -v pkg-config
        return 0
    fi
    if command -v pkgconf >/dev/null 2>&1; then
        command -v pkgconf
        return 0
    fi
    return 1
}

pkg_config_consumer_check() {
    prefix=$1
    build=$2
    package_llam=${3:-}
    pc_tool=$(pkg_config_tool || true)
    if [ -z "$pc_tool" ]; then
        step "skip pkg-config consumer check: pkg-config/pkgconf not found"
        return 0
    fi

    cc=${CC:-cc}
    if ! command -v "$cc" >/dev/null 2>&1; then
        step "skip pkg-config consumer check: C compiler '$cc' not found"
        return 0
    fi

    consumer_dir=$source_dir/tests/pkg_config_consumer
    consumer_build_dir=$build/pkg-config-consumer
    mkdir -p "$consumer_build_dir"
    if [ -n "$package_llam" ]; then
        package_llam_dir=$(dirname -- "$package_llam")
        package_llam_pc_private_libs="-pthread"
        case "$(uname -s)" in
            Linux) package_llam_pc_private_libs="-pthread -luring -lm" ;;
            NetBSD) package_llam_pc_private_libs="-pthread -lrt" ;;
        esac
        mkdir -p "$consumer_build_dir/pkgconfig"
        cat >"$consumer_build_dir/pkgconfig/llam.pc" <<EOF
prefix=$llam_root
exec_prefix=\${prefix}
libdir=$package_llam_dir
includedir=$llam_root/include

Name: LLAM
Description: Stackful user-thread runtime for C applications
Version: 2.1.0
Libs: -L\${libdir} -lllam_runtime
Libs.private: $package_llam_pc_private_libs
Cflags: -I\${includedir}
EOF
    fi

    old_pkg_config_path=${PKG_CONFIG_PATH:-}
    PKG_CONFIG_PATH="$consumer_build_dir/pkgconfig:$prefix/lib/pkgconfig:$prefix/lib64/pkgconfig:$llam_root/build/cmake-path-check:$llam_root/build:$llam_root/lib/pkgconfig"
    if [ -n "$old_pkg_config_path" ]; then
        PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$old_pkg_config_path"
    fi
    export PKG_CONFIG_PATH

    "$pc_tool" --exists dcc
    "$pc_tool" --modversion dcc >/dev/null

    cflags=$("$pc_tool" --cflags dcc)
    if libs=$("$pc_tool" --libs --static dcc 2>/dev/null); then
        :
    else
        libs=$("$pc_tool" --libs dcc)
    fi
    # Deliberately use shell splitting here: pkg-config returns compiler tokens.
    # shellcheck disable=SC2086
    "$cc" -std=c11 "$consumer_dir/main.c" $cflags $libs -o "$consumer_build_dir/dcc_pkg_config_consumer"
    "$consumer_build_dir/dcc_pkg_config_consumer"
}

hot_reload_host_check() {
    host=$1
    module=$2
    worker=$3
    check_output=$(
        DCC_BOT_MODULE="$module" \
        DCC_HOT_RELOAD_WORKER="$worker" \
        DCC_HOT_RELOAD_HEALTH_ADDRESS=127.0.0.1 \
        DCC_HOT_RELOAD_HEALTH_PORT=18080 \
        DISCORD_TOKEN=release-check \
        "$host" --check
    )
    printf '%s\n' "$check_output" | grep -q 'module_readable=yes'
    printf '%s\n' "$check_output" | grep -q 'worker_found=yes'
    printf '%s\n' "$check_output" | grep -q 'command_scope=global'
    printf '%s\n' "$check_output" | grep -q 'health_sidecar=enabled'
    printf '%s\n' "$check_output" | grep -q 'health_port=18080'
    printf '%s\n' "$check_output" | grep -q 'poll_interval_ms=250'
    printf '%s\n' "$check_output" | grep -q 'worker_drain_timeout_ms=500'
    printf '%s\n' "$check_output" | grep -q 'token=set'

    check_output=$(
        DCC_BOT_MODULE="$module" \
        DCC_HOT_RELOAD_WORKER="$worker" \
        DCC_HOT_RELOAD_GUILD_ID=invalid-release-check-guild \
        DISCORD_TOKEN=release-check \
        "$host" --guild 123456789012345678 --check
    )
    printf '%s\n' "$check_output" | grep -q 'module_readable=yes'
    printf '%s\n' "$check_output" | grep -q 'worker_found=yes'
    printf '%s\n' "$check_output" | grep -q 'command_scope=guild'
    printf '%s\n' "$check_output" | grep -q 'guild_id=123456789012345678'
    printf '%s\n' "$check_output" | grep -q 'token=set'

    check_output=$(
        DCC_BOT_MODULE="$module" \
        DCC_HOT_RELOAD_WORKER="$worker" \
        DCC_HOT_RELOAD_GUILD_ID=123456789012345678 \
        DISCORD_TOKEN=release-check \
        "$host" --global --check
    )
    printf '%s\n' "$check_output" | grep -q 'module_readable=yes'
    printf '%s\n' "$check_output" | grep -q 'worker_found=yes'
    printf '%s\n' "$check_output" | grep -q 'command_scope=global'
    printf '%s\n' "$check_output" | grep -q 'token=set'
}

hot_reload_live_soak_script_check() {
    prefix=$1
    script=$prefix/share/dcc/examples/hot_reload/live_soak.sh
    if [ ! -f "$script" ]; then
        printf 'missing installed hot reload live soak helper: %s\n' "$script" >&2
        return 1
    fi
    DCC_HOT_RELOAD_SOAK_DRY_RUN=1 sh "$script" >/dev/null
}

hot_reload_example_build_check() {
    prefix=$1
    package_llam=${2:-}
    example_dir=$prefix/share/dcc/examples/hot_reload
    if [ ! -f "$example_dir/Makefile" ]; then
        printf 'missing installed hot reload example Makefile: %s\n' "$example_dir/Makefile" >&2
        return 1
    fi

    if [ -n "$package_llam" ]; then
        DCC_LLAM_ROOT="$llam_root" \
        DCC_LLAM_LIBRARY="$package_llam" \
            make -C "$example_dir" clean >/dev/null
        DCC_LLAM_ROOT="$llam_root" \
        DCC_LLAM_LIBRARY="$package_llam" \
            make -C "$example_dir" all >/dev/null
    else
        make -C "$example_dir" clean >/dev/null
        make -C "$example_dir" all >/dev/null
    fi

    if [ ! -x "$example_dir/build/dcc_hot_reload_test_host" ]; then
        printf 'installed hot reload example did not prepare host executable\n' >&2
        return 1
    fi
    if [ ! -x "$example_dir/build/dcc_hot_reload_worker" ]; then
        printf 'installed hot reload example did not prepare worker executable\n' >&2
        return 1
    fi
    if [ ! -f "$example_dir/build/dcc_hot_reload_test_bot.so" ]; then
        printf 'installed hot reload example did not prepare bot module\n' >&2
        return 1
    fi
}

hot_reload_deploy_template_check() {
    prefix=$1
    host=$2
    module=$3
    worker=$4
    deploy_dir=$prefix/share/dcc/deploy/hot-reload
    env_file=$deploy_dir/dcc-hot-reload.env
    service_file=$deploy_dir/dcc-hot-reload.service
    compose_file=$deploy_dir/docker-compose.yaml
    caddy_file=$deploy_dir/Caddyfile
    container_file=$deploy_dir/Containerfile

    for file in \
        "$deploy_dir/README.md" \
        "$env_file" \
        "$service_file" \
        "$deploy_dir/dcc-hot-reload.tmpfiles.conf" \
        "$compose_file" \
        "$caddy_file" \
        "$container_file"; do
        if [ ! -f "$file" ]; then
            printf 'missing installed hot reload deployment template: %s\n' "$file" >&2
            return 1
        fi
    done

    grep -q '^DCC_BOT_MODULE=' "$env_file"
    grep -q '^DCC_HOT_RELOAD_WORKER=' "$env_file"
    grep -q '^DCC_HOT_RELOAD_INTENTS=guilds$' "$env_file"
    grep -q '^DCC_HOT_RELOAD_HEALTH_ADDRESS=127.0.0.1$' "$env_file"
    grep -q '^DCC_HOT_RELOAD_HEALTH_PORT=18080$' "$env_file"
    grep -q '^DCC_HOT_RELOAD_POLL_MS=250$' "$env_file"
    grep -q '^DCC_HOT_RELOAD_SETTLE_MS=100$' "$env_file"
    grep -q '^DCC_HOT_RELOAD_WORKER_HEALTH_MS=2000$' "$env_file"
    grep -q '^DCC_HOT_RELOAD_WORKER_DRAIN_MS=500$' "$env_file"
    grep -q '^ExecStartPre=/usr/local/bin/dcc_hot_reload_host --check$' "$service_file"
    grep -q '^ExecStart=/usr/local/bin/dcc_hot_reload_host$' "$service_file"
    grep -q '^EnvironmentFile=/etc/dcc/dcc-hot-reload.env$' "$service_file"
    grep -q 'worker_events' "$deploy_dir/README.md"
    grep -q 'DCC_BOT_MODULE: /var/lib/dcc/hot_reload/bot.so' "$compose_file"
    grep -q 'DCC_HOT_RELOAD_WORKER: /usr/local/bin/dcc_hot_reload_worker' "$compose_file"
    grep -q 'DCC_HOT_RELOAD_HEALTH_ADDRESS: 0.0.0.0' "$compose_file"
    grep -q '127.0.0.1:18080:8080' "$compose_file"
    grep -q 'reverse_proxy hot-reload:18080' "$caddy_file"
    grep -q 'dcc_hot_reload_host' "$container_file"
    grep -q 'dcc_hot_reload_worker' "$container_file"

    check_output=$(
        DCC_TOKEN=release-check \
        DCC_BOT_MODULE="$module" \
        DCC_HOT_RELOAD_WORKER="$worker" \
        DCC_HOT_RELOAD_INTENTS=guilds \
        DCC_HOT_RELOAD_GUILD_ID= \
        DCC_HOT_RELOAD_HEALTH_ADDRESS=127.0.0.1 \
        DCC_HOT_RELOAD_HEALTH_PORT=18080 \
        DCC_HOT_RELOAD_POLL_MS=250 \
        DCC_HOT_RELOAD_SETTLE_MS=100 \
        DCC_HOT_RELOAD_WORKER_HEALTH_MS=2000 \
        DCC_HOT_RELOAD_WORKER_DRAIN_MS=500 \
        "$host" --check
    )
    printf '%s\n' "$check_output" | grep -q 'module_readable=yes'
    printf '%s\n' "$check_output" | grep -q 'worker_found=yes'
    printf '%s\n' "$check_output" | grep -q 'command_scope=global'
    printf '%s\n' "$check_output" | grep -q 'intents=1'
    printf '%s\n' "$check_output" | grep -q 'health_sidecar=enabled'
    printf '%s\n' "$check_output" | grep -q 'health_port=18080'
    printf '%s\n' "$check_output" | grep -q 'poll_interval_ms=250'
    printf '%s\n' "$check_output" | grep -q 'worker_health_timeout_ms=2000'
    printf '%s\n' "$check_output" | grep -q 'worker_drain_timeout_ms=500'
    printf '%s\n' "$check_output" | grep -q 'token=set'
}

interaction_webhook_deploy_template_check() {
    prefix=$1
    webhook=$2
    deploy_dir=$prefix/share/dcc/deploy/interaction-webhook
    env_file=$deploy_dir/dcc-interaction-webhook.env
    service_file=$deploy_dir/dcc-interaction-webhook.service
    k8s_file=$deploy_dir/kubernetes.yaml

    for file in \
        "$deploy_dir/README.md" \
        "$env_file" \
        "$service_file" \
        "$deploy_dir/nginx.conf" \
        "$deploy_dir/Caddyfile" \
        "$deploy_dir/docker-compose.yaml" \
        "$deploy_dir/Containerfile" \
        "$k8s_file"; do
        if [ ! -f "$file" ]; then
            printf 'missing installed interaction webhook deployment template: %s\n' "$file" >&2
            return 1
        fi
    done

    grep -q '^DISCORD_PUBLIC_KEY=' "$env_file"
    grep -q '^DCC_INTERACTION_ADDRESS=127\.0\.0\.1$' "$env_file"
    grep -q '^DCC_INTERACTION_PATH=/interactions$' "$env_file"
    grep -q '^EnvironmentFile=/etc/dcc/dcc-interaction-webhook.env$' "$service_file"
    grep -q '^ExecStartPre=/usr/local/bin/dcc_interaction_webhook --check$' "$service_file"
    grep -q '^ExecStart=/usr/local/bin/dcc_interaction_webhook$' "$service_file"
    grep -q 'kind: Deployment' "$k8s_file"
    grep -q 'DISCORD_PUBLIC_KEY' "$k8s_file"
    grep -q 'DCC_INTERACTION_ADDRESS: "0.0.0.0"' "$k8s_file"
    grep -q 'DCC_INTERACTION_PATH: "/interactions"' "$k8s_file"
    grep -q 'args: \["--check"\]' "$k8s_file"
    grep -q 'path: /healthz' "$k8s_file"
    grep -q 'path: /interactions' "$k8s_file"

    check_output=$(
        DISCORD_PUBLIC_KEY=0000000000000000000000000000000000000000000000000000000000000000 \
        DCC_INTERACTION_ADDRESS=127.0.0.1 \
        DCC_INTERACTION_PORT=8080 \
        DCC_INTERACTION_PATH=/interactions \
        "$webhook" --check
    )
    printf '%s\n' "$check_output" | grep -q 'public_key=set'
    printf '%s\n' "$check_output" | grep -q 'address=127.0.0.1'
    printf '%s\n' "$check_output" | grep -q 'port=8080'
    printf '%s\n' "$check_output" | grep -q 'interaction_path=/interactions'
    printf '%s\n' "$check_output" | grep -q 'health_path=/healthz'
    printf '%s\n' "$check_output" | grep -q 'server_create=yes'
    printf '%s\n' "$check_output" | grep -q 'routes=ready'
}

source_package_hygiene_check() {
    build=$1
    output_dir=$build/source-package-check
    list_file=$output_dir/source-package.lst
    rm -rf "$output_dir"
    mkdir -p "$output_dir"

    cpack -G TGZ --config "$build/CPackSourceConfig.cmake" -B "$output_dir" >/dev/null
    archive=$(find "$output_dir" -name '*.tar.gz' -print | head -n 1)
    if [ -z "$archive" ]; then
        printf 'source package archive was not created under %s\n' "$output_dir" >&2
        return 1
    fi

    tar -tf "$archive" >"$list_file"
    if grep -Eq '(^|/)\.env($|\.)' "$list_file"; then
        printf 'source package contains a hidden .env file:\n' >&2
        grep -E '(^|/)\.env($|\.)' "$list_file" >&2
        return 1
    fi
    if grep -Eq '(^|/)(__pycache__/|.*\.pyc$|compile_commands\.json$|\.DS_Store$|CMakeCache\.txt$|CMakeFiles/|cmake_install\.cmake$|install_manifest\.txt$|Testing/)' "$list_file"; then
        printf 'source package contains local cache/build metadata:\n' >&2
        grep -E '(^|/)(__pycache__/|.*\.pyc$|compile_commands\.json$|\.DS_Store$|CMakeCache\.txt$|CMakeFiles/|cmake_install\.cmake$|install_manifest\.txt$|Testing/)' "$list_file" >&2
        return 1
    fi

    grep -q '/deploy/interaction-webhook/dcc-interaction-webhook.env$' "$list_file"
    grep -q '/deploy/interaction-webhook/kubernetes.yaml$' "$list_file"
    grep -q '/deploy/hot-reload/dcc-hot-reload.env$' "$list_file"
    grep -q '/include/dcc/oauth2.h$' "$list_file"
    grep -q '/include/dcc/rest/official_surface.h$' "$list_file"
    grep -q '/include/dcc/sugar/official_surface.h$' "$list_file"
    grep -q '/include/dcc/webhook_events.h$' "$list_file"
    grep -q '/src/gateway/gateway_send_public.c$' "$list_file"
    grep -q '/src/oauth2.c$' "$list_file"
    grep -q '/src/rest/rest_official_surface.c$' "$list_file"
    grep -q '/src/webhook_events.c$' "$list_file"
    grep -q '/tests/official_surface_smoke.c$' "$list_file"
    grep -q '/tests/package_consumer/package_consumer_official_surface.c$' "$list_file"
    grep -q '/tests/support/http_smoke_official_surface.c$' "$list_file"
    grep -q '/tools/audit_discord_api_docs_surface.py$' "$list_file"
    grep -q '/tools/audit_official_events_surface.py$' "$list_file"
    grep -q '/tools/audit_official_surface.py$' "$list_file"
    grep -q '/docs/reference/official-api-surface.md$' "$list_file"
}

official_discord_docs_audit() {
    docs_root=${DCC_DISCORD_API_DOCS_ROOT:-}
    if [ -z "$docs_root" ]; then
        step "skip official Discord API docs audit: DCC_DISCORD_API_DOCS_ROOT not set"
        return 0
    fi
    if [ ! -d "$docs_root/developers" ]; then
        printf 'official Discord API docs checkout missing developers/: %s\n' "$docs_root" >&2
        return 1
    fi

    python=${DCC_PYTHON:-python3}
    if ! command -v "$python" >/dev/null 2>&1; then
        printf 'official Discord API docs audit requires Python: %s\n' "$python" >&2
        return 1
    fi

    step "run official Discord API docs audit"
    DCC_DISCORD_API_DOCS_ROOT="$docs_root" "$python" "$source_dir/tools/audit_official_surface.py"
    DCC_DISCORD_API_DOCS_ROOT="$docs_root" "$python" "$source_dir/tools/audit_discord_api_docs_surface.py"
    DCC_DISCORD_API_DOCS_ROOT="$docs_root" "$python" "$source_dir/tools/audit_official_events_surface.py"
}

if ! is_true "${DCC_SKIP_OFFICIAL_DOCS_AUDIT:-0}"; then
    official_discord_docs_audit
fi

step "configure primary debug build"
cmake_configure "$build_dir" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DDCC_BUILD_EXAMPLES=ON \
    -DDCC_BUILD_TESTS=ON \
    -DDCC_BUILD_BENCHMARKS=ON \
    -DDCC_LLAM_USE_SUBDIRECTORY="$llam_use_subdirectory" \
    -DDCC_BUNDLE_LLAM="$bundle_llam" \
    -DDCC_LLAM_ROOT="$llam_root" \
    -DDCC_LLAM_LIBRARY="$llam_library"

step "build primary debug build"
cmake_build "$build_dir"

step "run primary test suite"
run_ctest "$build_dir"

if ! is_true "${DCC_SKIP_HOT_RELOAD_LIVE_SOAK_DRY_RUN:-0}"; then
    step "run hot reload live soak helper dry run"
    DCC_HOT_RELOAD_SOAK_DRY_RUN=1 sh "$source_dir/examples/hot_reload/live_soak.sh" >/dev/null
fi

if ! is_true "${DCC_SKIP_LIVE_VOICE_PROBE_DRY_RUN:-0}"; then
    live_voice_probe=$(find_executable "$build_dir" dcc_live_voice_probe)
    step "run live voice probe dry run"
    "$live_voice_probe" >/dev/null
fi

if ! is_true "${DCC_SKIP_HOT_RELOAD_HOST_CHECK:-0}"; then
    hot_reload_host=$(find_executable "$build_dir" dcc_hot_reload_host)
    hot_reload_worker=$(find_executable "$build_dir" dcc_hot_reload_worker)
    hot_reload_module=$(find_artifact "$build_dir" dcc_hot_reload_isolated_module)
    step "run hot reload host preflight"
    hot_reload_host_check "$hot_reload_host" "$hot_reload_module" "$hot_reload_worker"
fi

if ! is_true "${DCC_SKIP_BENCH:-0}"; then
    bench_exe=$(find_executable "$build_dir" dcc_json_gateway_bench)
    step "run gateway JSON benchmark"
    "$bench_exe"
fi

if ! is_true "${DCC_SKIP_CPACK:-0}"; then
    step "build CPack package"
    cmake_build "$build_dir" package

    if ! is_true "${DCC_SKIP_SOURCE_PACKAGE_CHECK:-0}"; then
        step "build source package hygiene check"
        source_package_hygiene_check "$build_dir"
    fi
fi

if ! is_true "${DCC_SKIP_PACKAGE:-0}"; then
    step "install package"
    cmake --install "$build_dir" --prefix "$package_prefix"

    package_llam_library=
    if is_true "$llam_use_subdirectory" && is_true "$bundle_llam"; then
        step "verify installed LLAM bundle"
        installed_llam_bundle_check "$package_prefix"
    else
        package_llam_library=$(package_llam_library_path)
    fi

    if ! is_true "${DCC_SKIP_HOT_RELOAD_HOST_CHECK:-0}"; then
        installed_hot_reload_host=$(find_executable "$package_prefix/bin" dcc_hot_reload_host)
        installed_hot_reload_worker=$(find_executable "$package_prefix/bin" dcc_hot_reload_worker)
        if [ -z "${hot_reload_module:-}" ]; then
            hot_reload_module=$(find_artifact "$build_dir" dcc_hot_reload_isolated_module)
        fi
        step "run installed hot reload host preflight"
        hot_reload_host_check "$installed_hot_reload_host" "$hot_reload_module" "$installed_hot_reload_worker"
    fi

    if ! is_true "${DCC_SKIP_HOT_RELOAD_LIVE_SOAK_DRY_RUN:-0}"; then
        step "run installed hot reload live soak helper dry run"
        hot_reload_live_soak_script_check "$package_prefix"
    fi

    if ! is_true "${DCC_SKIP_HOT_RELOAD_DEPLOY_TEMPLATE_CHECK:-0}"; then
        if [ -z "${installed_hot_reload_host:-}" ]; then
            installed_hot_reload_host=$(find_executable "$package_prefix/bin" dcc_hot_reload_host)
        fi
        if [ -z "${installed_hot_reload_worker:-}" ]; then
            installed_hot_reload_worker=$(find_executable "$package_prefix/bin" dcc_hot_reload_worker)
        fi
        if [ -z "${hot_reload_module:-}" ]; then
            hot_reload_module=$(find_artifact "$build_dir" dcc_hot_reload_isolated_module)
        fi
        step "run installed hot reload deploy template preflight"
        hot_reload_deploy_template_check "$package_prefix" "$installed_hot_reload_host" "$hot_reload_module" "$installed_hot_reload_worker"
    fi

    if ! is_true "${DCC_SKIP_INTERACTION_WEBHOOK_DEPLOY_TEMPLATE_CHECK:-0}"; then
        installed_interaction_webhook=$(find_executable "$package_prefix/bin" dcc_interaction_webhook)
        step "run installed interaction webhook deploy template preflight"
        interaction_webhook_deploy_template_check "$package_prefix" "$installed_interaction_webhook"
    fi

    if ! is_true "${DCC_SKIP_HOT_RELOAD_EXAMPLE_BUILD:-0}"; then
        step "build installed hot reload example"
        hot_reload_example_build_check "$package_prefix" "$package_llam_library"
    fi

    step "build installed-package consumer"
    configure_package_consumer "$source_dir/tests/package_consumer" "$package_consumer_build_dir" \
        "$package_prefix" "$package_llam_library"
    cmake_build "$package_consumer_build_dir"
    consumer_exe=$(find_executable "$package_consumer_build_dir" dcc_package_consumer)
    "$consumer_exe"

    if ! is_true "${DCC_SKIP_PKG_CONFIG_CHECK:-0}"; then
        step "build installed pkg-config consumer"
        pkg_config_consumer_check "$package_prefix" "$package_consumer_build_dir" "$package_llam_library"
    fi
fi

if ! is_true "${DCC_SKIP_MINIMAL_PACKAGE:-0}"; then
    step "configure minimal tools package build"
    cmake_configure "$minimal_build_dir" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DDCC_BUILD_EXAMPLES=OFF \
        -DDCC_BUILD_TESTS=OFF \
        -DDCC_BUILD_BENCHMARKS=OFF \
        -DDCC_BUILD_FUZZERS=OFF \
        -DDCC_LLAM_USE_SUBDIRECTORY="$llam_use_subdirectory" \
        -DDCC_BUNDLE_LLAM="$bundle_llam" \
        -DDCC_LLAM_ROOT="$llam_root" \
        -DDCC_LLAM_LIBRARY="$llam_library"

    step "build minimal tools package build"
    cmake_build "$minimal_build_dir"

    step "install minimal tools package"
    cmake --install "$minimal_build_dir" --prefix "$minimal_package_prefix"

    package_llam_library=
    if is_true "$llam_use_subdirectory" && is_true "$bundle_llam"; then
        step "verify minimal installed LLAM bundle"
        installed_llam_bundle_check "$minimal_package_prefix"
    else
        package_llam_library=$(package_llam_library_path)
    fi

    if ! is_true "${DCC_SKIP_HOT_RELOAD_LIVE_SOAK_DRY_RUN:-0}"; then
        step "run minimal installed hot reload live soak helper dry run"
        hot_reload_live_soak_script_check "$minimal_package_prefix"
    fi

    if ! is_true "${DCC_SKIP_HOT_RELOAD_DEPLOY_TEMPLATE_CHECK:-0}"; then
        minimal_hot_reload_host=$(find_executable "$minimal_package_prefix/bin" dcc_hot_reload_host)
        minimal_hot_reload_worker=$(find_executable "$minimal_package_prefix/bin" dcc_hot_reload_worker)
        if [ -z "${hot_reload_module:-}" ]; then
            hot_reload_module=$(find_artifact "$build_dir" dcc_hot_reload_isolated_module)
        fi
        step "run minimal installed hot reload deploy template preflight"
        hot_reload_deploy_template_check "$minimal_package_prefix" "$minimal_hot_reload_host" "$hot_reload_module" "$minimal_hot_reload_worker"
    fi

    if ! is_true "${DCC_SKIP_INTERACTION_WEBHOOK_DEPLOY_TEMPLATE_CHECK:-0}"; then
        minimal_interaction_webhook=$(find_executable "$minimal_package_prefix/bin" dcc_interaction_webhook)
        step "run minimal installed interaction webhook deploy template preflight"
        interaction_webhook_deploy_template_check "$minimal_package_prefix" "$minimal_interaction_webhook"
    fi

    if ! is_true "${DCC_SKIP_HOT_RELOAD_EXAMPLE_BUILD:-0}"; then
        step "build minimal installed hot reload example"
        hot_reload_example_build_check "$minimal_package_prefix" "$package_llam_library"
    fi

    if ! is_true "${DCC_SKIP_HOT_RELOAD_HOST_CHECK:-0}"; then
        minimal_hot_reload_host=$(find_executable "$minimal_package_prefix/bin" dcc_hot_reload_host)
        minimal_hot_reload_worker=$(find_executable "$minimal_package_prefix/bin" dcc_hot_reload_worker)
        if [ -z "${hot_reload_module:-}" ]; then
            hot_reload_module=$(find_artifact "$build_dir" dcc_hot_reload_isolated_module)
        fi
        step "run minimal installed hot reload host preflight"
        hot_reload_host_check "$minimal_hot_reload_host" "$hot_reload_module" "$minimal_hot_reload_worker"
    fi

    step "build minimal installed-package consumer"
    configure_package_consumer "$source_dir/tests/package_consumer" "$minimal_package_consumer_build_dir" \
        "$minimal_package_prefix" "$package_llam_library"
    cmake_build "$minimal_package_consumer_build_dir"
    minimal_consumer_exe=$(find_executable "$minimal_package_consumer_build_dir" dcc_package_consumer)
    "$minimal_consumer_exe"

    if ! is_true "${DCC_SKIP_PKG_CONFIG_CHECK:-0}"; then
        step "build minimal installed pkg-config consumer"
        pkg_config_consumer_check "$minimal_package_prefix" "$minimal_package_consumer_build_dir" "$package_llam_library"
    fi
fi

if ! is_true "${DCC_SKIP_ASAN:-0}"; then
    step "configure ASAN/UBSAN build"
    cmake_configure "$asan_build_dir" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DDCC_BUILD_TESTS=ON \
        -DDCC_ENABLE_SANITIZERS=ON \
        -DDCC_LLAM_USE_SUBDIRECTORY="$llam_use_subdirectory" \
        -DDCC_BUNDLE_LLAM="$bundle_llam" \
        -DDCC_LLAM_ROOT="$llam_root" \
        -DDCC_LLAM_LIBRARY="$llam_library"

    step "build ASAN/UBSAN build"
    cmake_build "$asan_build_dir"

    step "run ASAN/UBSAN test suite"
    # The primary suite still runs the full cluster gateway integration smoke.
    # Hosted macOS ASAN can occasionally leave that network-heavy test waiting
    # in teardown, so the sanitizer pass keeps to deterministic checks by default.
    run_ctest "$asan_build_dir" "$asan_ctest_exclude"
fi

if ! is_true "${DCC_SKIP_NOOPUS:-0}"; then
    step "configure no-opus build"
    cmake_configure "$noopus_build_dir" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DDCC_BUILD_TESTS=ON \
        -DDCC_WITH_OPUS=OFF \
        -DDCC_LLAM_USE_SUBDIRECTORY="$llam_use_subdirectory" \
        -DDCC_BUNDLE_LLAM="$bundle_llam" \
        -DDCC_LLAM_ROOT="$llam_root" \
        -DDCC_LLAM_LIBRARY="$llam_library"

    step "build no-opus build"
    cmake_build "$noopus_build_dir"

    step "run no-opus test suite"
    run_ctest "$noopus_build_dir"
fi

if ! is_true "${DCC_SKIP_FUZZER:-0}"; then
    fuzz_cc=${DCC_FUZZ_CC:-clang}
    if ! command -v "$fuzz_cc" >/dev/null 2>&1; then
        fuzz_cc=${CC:-cc}
    fi

    step "configure JSON fuzzer build with $fuzz_cc"
    if [ -n "${DCC_CMAKE_GENERATOR:-}" ]; then
        CC="$fuzz_cc" cmake -S "$source_dir" -B "$fuzz_build_dir" \
            -G "$DCC_CMAKE_GENERATOR" \
            -DCMAKE_BUILD_TYPE=RelWithDebInfo \
            -DDCC_BUILD_FUZZERS=ON \
            -DDCC_LLAM_USE_SUBDIRECTORY="$llam_use_subdirectory" \
            -DDCC_BUNDLE_LLAM="$bundle_llam" \
            -DDCC_LLAM_ROOT="$llam_root" \
            -DDCC_LLAM_LIBRARY="$llam_library" \
            ${DCC_EXTRA_CMAKE_ARGS:-}
    else
        CC="$fuzz_cc" cmake -S "$source_dir" -B "$fuzz_build_dir" \
            -DCMAKE_BUILD_TYPE=RelWithDebInfo \
            -DDCC_BUILD_FUZZERS=ON \
            -DDCC_LLAM_USE_SUBDIRECTORY="$llam_use_subdirectory" \
            -DDCC_BUNDLE_LLAM="$bundle_llam" \
            -DDCC_LLAM_ROOT="$llam_root" \
            -DDCC_LLAM_LIBRARY="$llam_library" \
            ${DCC_EXTRA_CMAKE_ARGS:-}
    fi

    step "build JSON fuzzer"
    cmake_build "$fuzz_build_dir" dcc_json_fuzzer

    fuzzer_exe=$(find_executable "$fuzz_build_dir" dcc_json_fuzzer)
    step "run JSON fuzz seed corpus"
    "$fuzzer_exe" -runs=1 "$source_dir/fuzz/corpus/json"
fi

step "release check completed"
