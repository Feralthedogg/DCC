#include "internal/client/dcc_cluster_internal.h"

dcc_status_t dcc_cluster_set_supervisor_decision_callback(
    dcc_cluster_t *cluster,
    dcc_cluster_supervisor_decision_fn callback,
    void *user_data
) {
    if (cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (cluster->started) {
        return DCC_ERR_STATE;
    }
    cluster->supervisor_decision_callback = callback;
    cluster->supervisor_decision_user_data = user_data;
    return DCC_OK;
}

dcc_status_t dcc_cluster_get_supervisor_decision_callback(
    const dcc_cluster_t *cluster,
    dcc_cluster_supervisor_decision_fn *out_callback,
    void **out_user_data
) {
    if (cluster == NULL || (out_callback == NULL && out_user_data == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (out_callback != NULL) {
        *out_callback = cluster->supervisor_decision_callback;
    }
    if (out_user_data != NULL) {
        *out_user_data = cluster->supervisor_decision_user_data;
    }
    return DCC_OK;
}
