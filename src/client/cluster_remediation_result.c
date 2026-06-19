#include "internal/client/dcc_cluster_internal.h"

#include <string.h>

int dcc_cluster_remediation_has(
    const dcc_cluster_remediation_options_t *options,
    size_t offset,
    size_t size
) {
    return options != NULL && options->size >= offset && options->size - offset >= size;
}

void dcc_cluster_remediation_result_init(dcc_cluster_remediation_result_t *result) {
    memset(result, 0, sizeof(*result));
    result->size = sizeof(*result);
    result->plan.size = sizeof(result->plan);
    result->plan.health = DCC_CLUSTER_HEALTH_UNKNOWN;
    result->plan.action = DCC_CLUSTER_RECOVERY_UNKNOWN;
    result->plan.shard_index = DCC_CLUSTER_NO_SHARD_INDEX;
    result->plan.shard_id = DCC_CLUSTER_NO_SHARD_INDEX;
    result->plan.shard_status = (dcc_cluster_shard_status_t)-1;
    result->plan.summary.size = sizeof(result->plan.summary);
    result->plan.shard.size = sizeof(result->plan.shard);
    result->plan.shard.status = (dcc_cluster_shard_status_t)-1;
    result->action_status = DCC_OK;
}

dcc_status_t dcc_cluster_remediation_finish(
    dcc_cluster_remediation_result_t *out,
    const dcc_cluster_remediation_result_t *result,
    dcc_status_t status
) {
    if (out != NULL) {
        *out = *result;
    }
    return status;
}
