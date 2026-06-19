#include "internal/client/dcc_cluster_internal.h"

#include <string.h>

void dcc_cluster_remediation_loop_result_init(
    dcc_cluster_remediation_loop_result_t *result
) {
    memset(result, 0, sizeof(*result));
    result->size = sizeof(*result);
    result->last.size = sizeof(result->last);
    result->last.plan.size = sizeof(result->last.plan);
    result->last.plan.summary.size = sizeof(result->last.plan.summary);
    result->last.plan.shard.size = sizeof(result->last.plan.shard);
    result->last_status = DCC_OK;
    result->final_health = DCC_CLUSTER_HEALTH_UNKNOWN;
    result->final_action = DCC_CLUSTER_RECOVERY_UNKNOWN;
}

dcc_status_t dcc_cluster_remediation_loop_finish(
    dcc_cluster_remediation_loop_result_t *out,
    const dcc_cluster_remediation_loop_result_t *result,
    dcc_status_t status
) {
    if (out != NULL) {
        *out = *result;
    }
    return status;
}
