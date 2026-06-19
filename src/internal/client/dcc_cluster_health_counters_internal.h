#ifndef DCC_CLUSTER_HEALTH_COUNTERS_INTERNAL_H
#define DCC_CLUSTER_HEALTH_COUNTERS_INTERNAL_H

#include <dcc/cluster.h>

static inline void dcc_cluster_reset_info_counters(dcc_cluster_shard_info_t *info) {
    if (info == NULL) {
        return;
    }
    info->starts = 0;
    info->restart_attempts = 0;
    info->restarts = 0;
    info->restart_suppressed = 0;
    info->restart_budget_used = 0;
    info->ready_events = 0;
    info->resumed_events = 0;
    info->socket_closes = 0;
    info->log_errors = 0;
    info->last_close_code = 0;
    info->last_close_reason[0] = '\0';
    info->last_error[0] = '\0';
}

#endif
