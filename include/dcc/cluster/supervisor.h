#ifndef DCC_CLUSTER_SUPERVISOR_H
#define DCC_CLUSTER_SUPERVISOR_H

#include <dcc/cluster/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API void dcc_cluster_supervisor_action_init(
    dcc_cluster_supervisor_action_t *action
);
DCC_API dcc_status_t dcc_cluster_set_supervisor_policy(
    dcc_cluster_t *cluster,
    dcc_cluster_supervisor_policy_fn policy,
    void *user_data
);
DCC_API dcc_status_t dcc_cluster_get_supervisor_policy(
    const dcc_cluster_t *cluster,
    dcc_cluster_supervisor_policy_fn *out_policy,
    void **out_user_data
);
DCC_API dcc_status_t dcc_cluster_set_supervisor_decision_callback(
    dcc_cluster_t *cluster,
    dcc_cluster_supervisor_decision_fn callback,
    void *user_data
);
DCC_API dcc_status_t dcc_cluster_get_supervisor_decision_callback(
    const dcc_cluster_t *cluster,
    dcc_cluster_supervisor_decision_fn *out_callback,
    void **out_user_data
);
DCC_API dcc_status_t dcc_cluster_supervisor_status(
    const dcc_cluster_t *cluster,
    dcc_cluster_supervisor_status_t *out
);
DCC_API dcc_status_t dcc_cluster_supervisor_decision_event_json(
    const dcc_cluster_supervisor_decision_event_t *event,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API const char *dcc_cluster_supervisor_decision_string(
    dcc_cluster_supervisor_decision_t decision
);

#ifdef __cplusplus
}
#endif

#endif
