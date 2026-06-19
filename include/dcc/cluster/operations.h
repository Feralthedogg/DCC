#ifndef DCC_CLUSTER_OPERATIONS_H
#define DCC_CLUSTER_OPERATIONS_H

#include <dcc/cluster/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_cluster_rolling_reconnect(
    dcc_cluster_t *cluster,
    const dcc_cluster_rolling_reconnect_options_t *options
);
DCC_API dcc_status_t dcc_cluster_plan_rolling_reconnect(
    dcc_cluster_t *cluster,
    const dcc_cluster_rolling_reconnect_options_t *options
);
DCC_API dcc_status_t dcc_cluster_run_planned_operation(dcc_cluster_t *cluster);
DCC_API dcc_status_t dcc_cluster_operation_status(
    const dcc_cluster_t *cluster,
    dcc_cluster_operation_status_t *out
);
DCC_API dcc_status_t dcc_cluster_operation_status_json(
    const dcc_cluster_operation_status_t *status,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API dcc_status_t dcc_cluster_clear_operation(dcc_cluster_t *cluster);

#ifdef __cplusplus
}
#endif

#endif
