#include "internal/client/dcc_cluster_internal.h"

dcc_status_t dcc_cluster_supervisor_status(
    const dcc_cluster_t *cluster,
    dcc_cluster_supervisor_status_t *out
) {
    if (cluster == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_t *mutable_cluster = (dcc_cluster_t *)cluster;
    dcc_cluster_health_lock(mutable_cluster);
    dcc_cluster_supervisor_status_t snapshot = cluster->supervisor_status;
    dcc_cluster_health_unlock(mutable_cluster);
    snapshot.size = sizeof(snapshot);
    *out = snapshot;
    return DCC_OK;
}

const char *dcc_cluster_supervisor_decision_string(
    dcc_cluster_supervisor_decision_t decision
) {
    switch (decision) {
        case DCC_CLUSTER_SUPERVISOR_DEFAULT:
            return "default";
        case DCC_CLUSTER_SUPERVISOR_RESTART:
            return "restart";
        case DCC_CLUSTER_SUPERVISOR_SUPPRESS:
            return "suppress";
        default:
            return "unknown";
    }
}
