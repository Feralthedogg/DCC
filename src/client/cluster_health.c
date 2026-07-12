#include "internal/client/dcc_cluster_internal.h"

#include <string.h>

dcc_status_t dcc_cluster_shard_info(
    const dcc_cluster_t *cluster,
    uint32_t index,
    dcc_cluster_shard_info_t *out
) {
    if (cluster == NULL || out == NULL || out->size < sizeof(*out) || index >= cluster->shard_count) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_t *mutable_cluster = (dcc_cluster_t *)cluster;
    dcc_cluster_health_lock(mutable_cluster);
    dcc_cluster_shard_info_t snapshot = cluster->health[index];
    dcc_cluster_health_unlock(mutable_cluster);

    snapshot.size = sizeof(snapshot);
    *out = snapshot;
    return DCC_OK;
}

dcc_status_t dcc_cluster_health_summary(
    const dcc_cluster_t *cluster,
    dcc_cluster_health_summary_t *out
) {
    if (cluster == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_t *mutable_cluster = (dcc_cluster_t *)cluster;
    dcc_cluster_health_summary_t summary;
    memset(&summary, 0, sizeof(summary));
    summary.size = sizeof(summary);

    dcc_cluster_health_lock(mutable_cluster);
    summary.total_shards = cluster->shard_count;
    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        const dcc_cluster_shard_info_t *info = &cluster->health[i];
        dcc_cluster_summary_add_status(&summary, info->status);
        summary.starts += info->starts;
        summary.restart_attempts += info->restart_attempts;
        summary.restarts += info->restarts;
        summary.restart_suppressed += info->restart_suppressed;
        summary.restart_budget_used += info->restart_budget_used;
        summary.ready_events += info->ready_events;
        summary.resumed_events += info->resumed_events;
        summary.socket_closes += info->socket_closes;
        summary.log_errors += info->log_errors;
    }
    dcc_cluster_health_unlock(mutable_cluster);

    *out = summary;
    return DCC_OK;
}

dcc_status_t dcc_cluster_identify_stats(
    const dcc_cluster_t *cluster,
    dcc_cluster_identify_stats_t *out
) {
    if (cluster == NULL || out == NULL || out->size < sizeof(*out)) return DCC_ERR_INVALID_ARG;
    dcc_cluster_t *mutable_cluster = (dcc_cluster_t *)cluster;
    dcc_cluster_identify_stats_t stats;
    memset(&stats, 0, sizeof(stats));
    stats.size = sizeof(stats);
    stats.waits = atomic_load_explicit(&mutable_cluster->identify_coordinator.waits, memory_order_acquire);
    stats.reservations = atomic_load_explicit(
        &mutable_cluster->identify_coordinator.reservations,
        memory_order_acquire
    );
    while (atomic_flag_test_and_set_explicit(
        &mutable_cluster->identify_coordinator.lock,
        memory_order_acquire
    )) {
    }
    stats.remaining = mutable_cluster->identify_coordinator.remaining;
    stats.total = mutable_cluster->identify_coordinator.total;
    stats.max_concurrency = mutable_cluster->identify_coordinator.max_concurrency;
    stats.reset_at_ms = mutable_cluster->identify_coordinator.reset_at_ms;
    atomic_flag_clear_explicit(&mutable_cluster->identify_coordinator.lock, memory_order_release);
    *out = stats;
    return DCC_OK;
}
