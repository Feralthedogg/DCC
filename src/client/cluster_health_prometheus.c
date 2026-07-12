#include <dcc/cluster/health.h>

#include <stdio.h>

dcc_status_t dcc_cluster_health_summary_prometheus(
    const dcc_cluster_health_summary_t *summary,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (summary == NULL || summary->size < sizeof(*summary) || out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    int len = snprintf(
        out,
        out_size,
        "# HELP dcc_cluster_shards Shards by lifecycle state.\n"
        "# TYPE dcc_cluster_shards gauge\n"
        "dcc_cluster_shards{state=\"total\"} %u\n"
        "dcc_cluster_shards{state=\"created\"} %u\n"
        "dcc_cluster_shards{state=\"starting\"} %u\n"
        "dcc_cluster_shards{state=\"ready\"} %u\n"
        "dcc_cluster_shards{state=\"stopping\"} %u\n"
        "dcc_cluster_shards{state=\"stopped\"} %u\n"
        "dcc_cluster_shards{state=\"error\"} %u\n"
        "# TYPE dcc_cluster_starts_total counter\n"
        "dcc_cluster_starts_total %llu\n"
        "# TYPE dcc_cluster_restarts_total counter\n"
        "dcc_cluster_restarts_total %llu\n"
        "# TYPE dcc_cluster_restart_suppressed_total counter\n"
        "dcc_cluster_restart_suppressed_total %llu\n"
        "# TYPE dcc_cluster_ready_events_total counter\n"
        "dcc_cluster_ready_events_total %llu\n"
        "# TYPE dcc_cluster_socket_closes_total counter\n"
        "dcc_cluster_socket_closes_total %llu\n",
        summary->total_shards,
        summary->created_shards,
        summary->starting_shards,
        summary->ready_shards,
        summary->stopping_shards,
        summary->stopped_shards,
        summary->error_shards,
        (unsigned long long)summary->starts,
        (unsigned long long)summary->restarts,
        (unsigned long long)summary->restart_suppressed,
        (unsigned long long)summary->ready_events,
        (unsigned long long)summary->socket_closes
    );
    if (len < 0) return DCC_ERR_RUNTIME;
    if (out_len != NULL) *out_len = (size_t)len;
    return (size_t)len < out_size ? DCC_OK : DCC_ERR_NOMEM;
}

dcc_status_t dcc_cluster_identify_stats_prometheus(
    const dcc_cluster_identify_stats_t *stats,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (stats == NULL || stats->size < sizeof(*stats) || out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    int len = snprintf(
        out,
        out_size,
        "# TYPE dcc_cluster_identify_waits_total counter\n"
        "dcc_cluster_identify_waits_total %llu\n"
        "# TYPE dcc_cluster_identify_reservations_total counter\n"
        "dcc_cluster_identify_reservations_total %llu\n"
        "# TYPE dcc_cluster_identify_remaining gauge\n"
        "dcc_cluster_identify_remaining %u\n"
        "# TYPE dcc_cluster_identify_limit gauge\n"
        "dcc_cluster_identify_limit{kind=\"total\"} %u\n"
        "dcc_cluster_identify_limit{kind=\"max_concurrency\"} %u\n",
        (unsigned long long)stats->waits,
        (unsigned long long)stats->reservations,
        stats->remaining,
        stats->total,
        stats->max_concurrency
    );
    if (len < 0) return DCC_ERR_RUNTIME;
    if (out_len != NULL) *out_len = (size_t)len;
    return (size_t)len < out_size ? DCC_OK : DCC_ERR_NOMEM;
}
