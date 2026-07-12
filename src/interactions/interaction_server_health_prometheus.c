#include <dcc/interaction_server/health.h>

#include <stdio.h>
#include <stddef.h>

dcc_status_t dcc_interaction_server_health_snapshot_prometheus(
    const dcc_interaction_server_health_snapshot_t *snapshot,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (snapshot == NULL || snapshot->size < offsetof(dcc_interaction_server_health_snapshot_t, protection) ||
        out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_interaction_server_protection_stats_t protection = {0};
    if (snapshot->size >= offsetof(dcc_interaction_server_health_snapshot_t, protection) +
            sizeof(snapshot->protection)) {
        protection = snapshot->protection;
    }
    int len = snprintf(
        out,
        out_size,
        "# TYPE dcc_interaction_server_ready gauge\n"
        "dcc_interaction_server_ready %u\n"
        "# TYPE dcc_interaction_server_accepting gauge\n"
        "dcc_interaction_server_accepting %u\n"
        "# TYPE dcc_interaction_server_active_requests gauge\n"
        "dcc_interaction_server_active_requests %llu\n"
        "# TYPE dcc_interaction_server_accepted_total counter\n"
        "dcc_interaction_server_accepted_total %llu\n"
        "# TYPE dcc_interaction_server_completed_total counter\n"
        "dcc_interaction_server_completed_total %llu\n"
        "# TYPE dcc_interaction_server_responses_total counter\n"
        "dcc_interaction_server_responses_total{class=\"2xx\"} %llu\n"
        "dcc_interaction_server_responses_total{class=\"4xx\"} %llu\n"
        "dcc_interaction_server_responses_total{class=\"5xx\"} %llu\n"
        "# TYPE dcc_interaction_server_overloaded_total counter\n"
        "dcc_interaction_server_overloaded_total %llu\n"
        "# TYPE dcc_interaction_server_replayed_total counter\n"
        "dcc_interaction_server_replayed_total %llu\n"
        "# TYPE dcc_interaction_server_deadline_exceeded_total counter\n"
        "dcc_interaction_server_deadline_exceeded_total %llu\n",
        (unsigned)(snapshot->ready != 0U),
        (unsigned)(snapshot->accepting != 0U),
        (unsigned long long)snapshot->stats.active_requests,
        (unsigned long long)snapshot->stats.accepted_connections,
        (unsigned long long)snapshot->stats.completed_requests,
        (unsigned long long)snapshot->stats.response_2xx,
        (unsigned long long)snapshot->stats.response_4xx,
        (unsigned long long)snapshot->stats.response_5xx,
        (unsigned long long)protection.overloaded_responses,
        (unsigned long long)protection.replayed_requests,
        (unsigned long long)protection.deadline_exceeded_requests
    );
    if (len < 0) return DCC_ERR_RUNTIME;
    if (out_len != NULL) *out_len = (size_t)len;
    return (size_t)len < out_size ? DCC_OK : DCC_ERR_NOMEM;
}
