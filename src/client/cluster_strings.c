#include "internal/client/dcc_cluster_internal.h"

const char *dcc_cluster_health_state_string(dcc_cluster_health_state_t state) {
    switch (state) {
        case DCC_CLUSTER_HEALTH_HEALTHY:
            return "healthy";
        case DCC_CLUSTER_HEALTH_STARTING:
            return "starting";
        case DCC_CLUSTER_HEALTH_DRAINING:
            return "draining";
        case DCC_CLUSTER_HEALTH_DEGRADED:
            return "degraded";
        case DCC_CLUSTER_HEALTH_CRITICAL:
            return "critical";
        case DCC_CLUSTER_HEALTH_UNKNOWN:
        default:
            return "unknown";
    }
}

const char *dcc_cluster_recovery_action_string(dcc_cluster_recovery_action_t action) {
    switch (action) {
        case DCC_CLUSTER_RECOVERY_NONE:
            return "none";
        case DCC_CLUSTER_RECOVERY_WAIT:
            return "wait";
        case DCC_CLUSTER_RECOVERY_RECONNECT:
            return "reconnect";
        case DCC_CLUSTER_RECOVERY_REARM_RESTART:
            return "rearm_restart";
        case DCC_CLUSTER_RECOVERY_INVESTIGATE:
            return "investigate";
        case DCC_CLUSTER_RECOVERY_UNKNOWN:
        default:
            return "unknown";
    }
}

const char *dcc_cluster_shard_status_string(dcc_cluster_shard_status_t status) {
    switch (status) {
        case DCC_CLUSTER_SHARD_CREATED:
            return "created";
        case DCC_CLUSTER_SHARD_STARTING:
            return "starting";
        case DCC_CLUSTER_SHARD_READY:
            return "ready";
        case DCC_CLUSTER_SHARD_STOPPING:
            return "stopping";
        case DCC_CLUSTER_SHARD_STOPPED:
            return "stopped";
        case DCC_CLUSTER_SHARD_ERROR:
            return "error";
        default:
            return "unknown";
    }
}
