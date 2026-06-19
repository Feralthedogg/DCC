#include "internal/client/dcc_cluster_internal.h"

#include <string.h>

void dcc_cluster_operation_reset(dcc_cluster_operation_status_t *operation, uint64_t generation) {
    if (operation == NULL) {
        return;
    }
    memset(operation, 0, sizeof(*operation));
    operation->size = sizeof(*operation);
    operation->type = DCC_CLUSTER_OPERATION_NONE;
    operation->generation = generation;
    operation->last_status = DCC_OK;
}

dcc_status_t dcc_cluster_operation_status(
    const dcc_cluster_t *cluster,
    dcc_cluster_operation_status_t *out
) {
    if (cluster == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_t *mutable_cluster = (dcc_cluster_t *)cluster;
    dcc_cluster_health_lock(mutable_cluster);
    dcc_cluster_operation_status_t snapshot = cluster->operation;
    dcc_cluster_health_unlock(mutable_cluster);
    snapshot.size = sizeof(snapshot);
    *out = snapshot;
    return DCC_OK;
}

dcc_status_t dcc_cluster_clear_operation(dcc_cluster_t *cluster) {
    if (cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_health_lock(cluster);
    if (cluster->operation.active) {
        dcc_cluster_health_unlock(cluster);
        return DCC_ERR_STATE;
    }
    dcc_cluster_operation_reset(&cluster->operation, cluster->operation.generation);
    dcc_cluster_health_unlock(cluster);
    return DCC_OK;
}
