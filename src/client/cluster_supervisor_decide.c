#include "internal/client/dcc_cluster_internal.h"

#include <stddef.h>

dcc_status_t dcc_cluster_supervisor_decide(
    dcc_cluster_t *cluster,
    uint32_t index,
    dcc_status_t gateway_status,
    dcc_cluster_supervisor_action_t *out
) {
    if (cluster == NULL || out == NULL || out->size < sizeof(*out) || index >= cluster->shard_count) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_supervisor_action_init(out);
    dcc_cluster_supervisor_policy_fn policy = cluster->supervisor_policy;
    if (policy == NULL) {
        return DCC_OK;
    }

    dcc_cluster_supervisor_event_t event;
    event.size = sizeof(event);
    event.shard_index = index;
    event.gateway_status = gateway_status;
    dcc_cluster_health_lock(cluster);
    event.shard_id = cluster->health[index].shard_id;
    event.shard = cluster->health[index];
    event.restart_policy = cluster->restart_policy;
    dcc_cluster_health_unlock(cluster);

    dcc_status_t status = policy(cluster, &event, out, cluster->supervisor_policy_user_data);
    if (status != DCC_OK) {
        return status;
    }
    if (out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }
    switch (out->decision) {
        case DCC_CLUSTER_SUPERVISOR_DEFAULT:
        case DCC_CLUSTER_SUPERVISOR_RESTART:
        case DCC_CLUSTER_SUPERVISOR_SUPPRESS:
            return DCC_OK;
        default:
            return DCC_ERR_INVALID_ARG;
    }
}
