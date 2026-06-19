#ifndef DCC_CLUSTER_HEALTH_JSON_INTERNAL_H
#define DCC_CLUSTER_HEALTH_JSON_INTERNAL_H

#include "internal/client/dcc_cluster_json_internal.h"

#include <dcc/cluster.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_cluster_json_append_health_summary(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_health_summary_t *summary
);
int dcc_cluster_json_append_recovery_plan(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_recovery_plan_t *plan
);
int dcc_cluster_json_append_shard_info(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_shard_info_t *shard
);
int dcc_cluster_json_append_supervisor_status(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_supervisor_status_t *status
);
int dcc_cluster_json_append_remediation_result(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_remediation_result_t *result
);
int dcc_cluster_json_append_operation_status(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_operation_status_t *status
);

#ifdef __cplusplus
}
#endif

#endif
