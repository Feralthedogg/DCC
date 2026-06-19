#include "internal/client/dcc_cluster_internal.h"

dcc_cluster_health_state_t dcc_cluster_health_summary_state(
    const dcc_cluster_health_summary_t *summary
) {
    if (summary == NULL || summary->size < sizeof(*summary) || summary->total_shards == 0) {
        return DCC_CLUSTER_HEALTH_UNKNOWN;
    }

    if (summary->error_shards != 0 || summary->restart_suppressed != 0) {
        return DCC_CLUSTER_HEALTH_CRITICAL;
    }

    if (summary->ready_shards == summary->total_shards) {
        return summary->restart_attempts != 0 ||
                summary->restarts != 0 ||
                summary->restart_budget_used != 0 ||
                summary->socket_closes != 0 ||
                summary->log_errors != 0
            ? DCC_CLUSTER_HEALTH_DEGRADED
            : DCC_CLUSTER_HEALTH_HEALTHY;
    }

    uint64_t draining_shards =
        (uint64_t)summary->stopping_shards + (uint64_t)summary->stopped_shards;
    if (draining_shards == (uint64_t)summary->total_shards) {
        return DCC_CLUSTER_HEALTH_DRAINING;
    }

    uint64_t pending_shards =
        (uint64_t)summary->created_shards +
        (uint64_t)summary->starting_shards +
        (uint64_t)summary->ready_shards;
    if (pending_shards == (uint64_t)summary->total_shards) {
        return DCC_CLUSTER_HEALTH_STARTING;
    }

    return DCC_CLUSTER_HEALTH_DEGRADED;
}
