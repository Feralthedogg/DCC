#!/usr/bin/env sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$script_dir"

reloads=${DCC_HOT_RELOAD_SOAK_RELOADS:-3}
interval=${DCC_HOT_RELOAD_SOAK_INTERVAL_SECONDS:-2}
ready_timeout=${DCC_HOT_RELOAD_SOAK_READY_TIMEOUT_SECONDS:-90}
reload_timeout=${DCC_HOT_RELOAD_SOAK_RELOAD_TIMEOUT_SECONDS:-30}
log_file=${DCC_HOT_RELOAD_SOAK_LOG:-build/live-soak.log}
host=build/dcc_hot_reload_test_host
module=build/dcc_hot_reload_test_bot.so
worker=build/dcc_hot_reload_worker

is_true() {
    case "${1:-0}" in
        1|ON|on|TRUE|true|YES|yes) return 0 ;;
        *) return 1 ;;
    esac
}

require_numeric() {
    name=$1
    value=$2
    case "$value" in
        ''|*[!0-9]*)
        printf '%s must be numeric\n' "$name" >&2
        exit 2
        ;;
    esac
}

require_numeric DCC_HOT_RELOAD_SOAK_RELOADS "$reloads"
require_numeric DCC_HOT_RELOAD_SOAK_INTERVAL_SECONDS "$interval"
require_numeric DCC_HOT_RELOAD_SOAK_READY_TIMEOUT_SECONDS "$ready_timeout"
require_numeric DCC_HOT_RELOAD_SOAK_RELOAD_TIMEOUT_SECONDS "$reload_timeout"

if is_true "${DCC_HOT_RELOAD_SOAK_DRY_RUN:-0}"; then
    printf 'hot reload live soak dry run\n'
    printf 'reloads=%s interval=%s ready_timeout=%s reload_timeout=%s\n' \
        "$reloads" "$interval" "$ready_timeout" "$reload_timeout"
    printf 'host=%s module=%s worker=%s log=%s\n' "$host" "$module" "$worker" "$log_file"
    exit 0
fi

has_token=0
if [ -n "${BOT_TOKEN:-}" ] || [ -n "${DISCORD_TOKEN:-}" ]; then
    has_token=1
elif [ -f .env ] && grep -Eq '^(BOT_TOKEN|DISCORD_TOKEN)=' .env; then
    has_token=1
elif [ -f ../../.env ] && grep -Eq '^(BOT_TOKEN|DISCORD_TOKEN)=' ../../.env; then
    has_token=1
fi
if [ "$has_token" -ne 1 ]; then
    printf '%s\n' "BOT_TOKEN or DISCORD_TOKEN is required in the environment, .env, or ../../.env" >&2
    exit 2
fi

wait_for_log() {
    pattern=$1
    timeout=$2
    elapsed=0
    while [ "$elapsed" -lt "$timeout" ]; do
        if grep -q "$pattern" "$log_file" 2>/dev/null; then
            return 0
        fi
        if [ -n "${host_pid:-}" ] && ! kill -0 "$host_pid" 2>/dev/null; then
            printf 'host exited while waiting for log pattern: %s\n' "$pattern" >&2
            printf '%s\n' "--- $log_file ---" >&2
            tail -n 80 "$log_file" >&2 || true
            return 1
        fi
        sleep 1
        elapsed=$((elapsed + 1))
    done
    printf 'timed out waiting for log pattern: %s\n' "$pattern" >&2
    printf '%s\n' "--- $log_file ---" >&2
    tail -n 80 "$log_file" >&2 || true
    return 1
}

cleanup() {
    if [ -n "${host_pid:-}" ] && kill -0 "$host_pid" 2>/dev/null; then
        kill "$host_pid" 2>/dev/null || true
        wait "$host_pid" 2>/dev/null || true
    fi
}
trap cleanup EXIT INT TERM

mkdir -p build
rm -f "$log_file"

printf '%s\n' "building hot reload host/module/worker"
make all

printf 'starting hot reload host; log=%s\n' "$log_file"
"./$host" "$module" "$worker" >"$log_file" 2>&1 &
host_pid=$!

wait_for_log 'Hot reload test bot ready' "$ready_timeout"
wait_for_log 'LLAM poll task' "$ready_timeout"
printf '%s\n' "host is READY; starting reload soak"

generation=1
i=0
while [ "$i" -lt "$reloads" ]; do
    i=$((i + 1))
    generation=$((generation + 1))
    sleep "$interval"
    printf 'reload %u/%u -> expected generation %u\n' "$i" "$reloads" "$generation"
    make module
    wait_for_log "active generation: $generation" "$reload_timeout"
done

printf '%s\n' "hot reload live soak completed"
tail -n 40 "$log_file" || true
