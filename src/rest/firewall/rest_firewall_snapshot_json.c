#include <dcc/rest/firewall.h>

#include <stdio.h>

dcc_status_t dcc_rest_firewall_snapshot_json(
    const dcc_rest_firewall_snapshot_t *snapshot,
    char *buf,
    size_t len,
    size_t *out_len
) {
    if (snapshot == NULL || buf == NULL || len == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    int written = snprintf(
        buf,
        len,
        "{\"attached\":%s,"
        "\"invalid_request_count\":%llu,"
        "\"soft_limit_exceeded\":%s,"
        "\"hard_limit_exceeded\":%s,"
        "\"window_started_at_ms\":%llu,"
        "\"window_ends_at_ms\":%llu,"
        "\"total_requests\":%llu,"
        "\"total_invalid_requests\":%llu,"
        "\"total_delayed_requests\":%llu,"
        "\"total_rejected_requests\":%llu,"
        "\"invalid_request_soft_limit\":%u,"
        "\"invalid_request_hard_limit\":%u,"
        "\"invalid_request_window_ms\":%llu,"
        "\"hard_limit_action\":\"%s\","
        "\"rest\":{"
        "\"pending\":%llu,"
        "\"active\":%llu,"
        "\"rate_limit_buckets\":%llu,"
        "\"rate_limit_blocked_buckets\":%llu,"
        "\"global_wait_ms\":%llu"
        "}}",
        snapshot->attached ? "true" : "false",
        (unsigned long long)snapshot->invalid_request_count,
        snapshot->soft_limit_exceeded ? "true" : "false",
        snapshot->hard_limit_exceeded ? "true" : "false",
        (unsigned long long)snapshot->window_started_at_ms,
        (unsigned long long)snapshot->window_ends_at_ms,
        (unsigned long long)snapshot->total_requests,
        (unsigned long long)snapshot->total_invalid_requests,
        (unsigned long long)snapshot->total_delayed_requests,
        (unsigned long long)snapshot->total_rejected_requests,
        (unsigned)snapshot->options.invalid_request_soft_limit,
        (unsigned)snapshot->options.invalid_request_hard_limit,
        (unsigned long long)snapshot->options.invalid_request_window_ms,
        dcc_rest_firewall_hard_limit_action_string(snapshot->options.on_hard_limit),
        (unsigned long long)snapshot->rest.pending,
        (unsigned long long)snapshot->rest.active,
        (unsigned long long)snapshot->rest.rate_limit_buckets,
        (unsigned long long)snapshot->rest.rate_limit_blocked_buckets,
        (unsigned long long)snapshot->rest.global_wait_ms
    );
    if (written < 0) {
        return DCC_ERR_RUNTIME;
    }
    if (out_len != NULL) {
        *out_len = (size_t)written;
    }
    return (size_t)written < len ? DCC_OK : DCC_ERR_NOMEM;
}
