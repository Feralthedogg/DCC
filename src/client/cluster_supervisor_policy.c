#include "internal/client/dcc_cluster_internal.h"

#include <stddef.h>
#include <string.h>

void dcc_cluster_supervisor_action_init(
    dcc_cluster_supervisor_action_t *action
) {
    if (action == NULL) {
        return;
    }

    memset(action, 0, sizeof(*action));
    action->size = sizeof(*action);
    action->decision = DCC_CLUSTER_SUPERVISOR_DEFAULT;
}

dcc_status_t dcc_cluster_set_supervisor_policy(
    dcc_cluster_t *cluster,
    dcc_cluster_supervisor_policy_fn policy,
    void *user_data
) {
    if (cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (cluster->started) {
        return DCC_ERR_STATE;
    }
    cluster->supervisor_policy = policy;
    cluster->supervisor_policy_user_data = user_data;
    return DCC_OK;
}

dcc_status_t dcc_cluster_get_supervisor_policy(
    const dcc_cluster_t *cluster,
    dcc_cluster_supervisor_policy_fn *out_policy,
    void **out_user_data
) {
    if (cluster == NULL || (out_policy == NULL && out_user_data == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (out_policy != NULL) {
        *out_policy = cluster->supervisor_policy;
    }
    if (out_user_data != NULL) {
        *out_user_data = cluster->supervisor_policy_user_data;
    }
    return DCC_OK;
}
