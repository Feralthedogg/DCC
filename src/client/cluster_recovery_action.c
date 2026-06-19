#include "internal/client/dcc_cluster_recovery_internal.h"

unsigned dcc_cluster_recovery_action_priority(dcc_cluster_recovery_action_t action) {
    switch (action) {
        case DCC_CLUSTER_RECOVERY_REARM_RESTART:
            return 5U;
        case DCC_CLUSTER_RECOVERY_INVESTIGATE:
            return 4U;
        case DCC_CLUSTER_RECOVERY_RECONNECT:
            return 3U;
        case DCC_CLUSTER_RECOVERY_WAIT:
            return 2U;
        case DCC_CLUSTER_RECOVERY_NONE:
            return 1U;
        case DCC_CLUSTER_RECOVERY_UNKNOWN:
        default:
            return 0U;
    }
}

const char *dcc_cluster_recovery_default_reason(dcc_cluster_recovery_action_t action) {
    switch (action) {
        case DCC_CLUSTER_RECOVERY_REARM_RESTART:
            return "restart budget exhausted";
        case DCC_CLUSTER_RECOVERY_INVESTIGATE:
            return "cluster counters require investigation";
        case DCC_CLUSTER_RECOVERY_RECONNECT:
            return "shard should reconnect";
        case DCC_CLUSTER_RECOVERY_WAIT:
            return "shard transition in progress";
        case DCC_CLUSTER_RECOVERY_NONE:
            return "no recovery action";
        case DCC_CLUSTER_RECOVERY_UNKNOWN:
        default:
            return "unknown recovery state";
    }
}

dcc_cluster_recovery_action_t dcc_cluster_shard_recovery_action(
    const dcc_cluster_shard_info_t *info
) {
    if (info == NULL || info->size < sizeof(*info)) {
        return DCC_CLUSTER_RECOVERY_UNKNOWN;
    }

    switch (info->status) {
        case DCC_CLUSTER_SHARD_CREATED:
        case DCC_CLUSTER_SHARD_STARTING:
        case DCC_CLUSTER_SHARD_STOPPING:
            return DCC_CLUSTER_RECOVERY_WAIT;
        case DCC_CLUSTER_SHARD_READY:
            return info->restart_attempts != 0 ||
                    info->restarts != 0 ||
                    info->restart_suppressed != 0 ||
                    info->restart_budget_used != 0 ||
                    info->socket_closes != 0 ||
                    info->log_errors != 0 ||
                    info->last_close_code != 0 ||
                    info->last_close_reason[0] != '\0' ||
                    info->last_error[0] != '\0'
                ? DCC_CLUSTER_RECOVERY_INVESTIGATE
                : DCC_CLUSTER_RECOVERY_NONE;
        case DCC_CLUSTER_SHARD_STOPPED:
            return info->socket_closes != 0 ||
                    info->last_close_code != 0 ||
                    info->last_close_reason[0] != '\0' ||
                    info->last_error[0] != '\0'
                ? DCC_CLUSTER_RECOVERY_RECONNECT
                : DCC_CLUSTER_RECOVERY_NONE;
        case DCC_CLUSTER_SHARD_ERROR:
            return info->restart_suppressed != 0
                ? DCC_CLUSTER_RECOVERY_REARM_RESTART
                : DCC_CLUSTER_RECOVERY_INVESTIGATE;
        default:
            return DCC_CLUSTER_RECOVERY_UNKNOWN;
    }
}
