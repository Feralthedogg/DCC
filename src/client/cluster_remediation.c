#include "internal/client/dcc_cluster_internal.h"

#include <stddef.h>

dcc_status_t dcc_cluster_remediate(
    dcc_cluster_t *cluster,
    const dcc_cluster_remediation_options_t *options,
    dcc_cluster_remediation_result_t *out
) {
    if (cluster == NULL ||
        (options != NULL && options->size < sizeof(options->size)) ||
        (out != NULL && out->size < sizeof(*out))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_remediation_result_t result;
    dcc_cluster_remediation_result_init(&result);

    dcc_cluster_recovery_plan_t plan = {
        .size = sizeof(plan),
    };
    dcc_status_t status = dcc_cluster_recovery_plan(cluster, &plan);
    if (status != DCC_OK) {
        result.action_status = status;
        return dcc_cluster_remediation_finish(out, &result, status);
    }
    result.plan = plan;
    result.action_status = DCC_OK;

    uint8_t allow_reconnect = 1U;
    if (dcc_cluster_remediation_has(
            options,
            offsetof(dcc_cluster_remediation_options_t, allow_reconnect),
            sizeof(options->allow_reconnect)
        )) {
        allow_reconnect = options->allow_reconnect;
    }

    uint8_t resume = 1U;
    if (dcc_cluster_remediation_has(
            options,
            offsetof(dcc_cluster_remediation_options_t, resume),
            sizeof(options->resume)
        )) {
        resume = options->resume;
    }

    uint8_t allow_rearm = 0U;
    if (dcc_cluster_remediation_has(
            options,
            offsetof(dcc_cluster_remediation_options_t, allow_rearm_restart),
            sizeof(options->allow_rearm_restart)
        )) {
        allow_rearm = options->allow_rearm_restart;
    }

    switch (plan.action) {
        case DCC_CLUSTER_RECOVERY_RECONNECT:
            if (plan.shard_index == DCC_CLUSTER_NO_SHARD_INDEX || allow_reconnect == 0U) {
                result.action_status = DCC_ERR_STATE;
                return dcc_cluster_remediation_finish(out, &result, DCC_ERR_STATE);
            }
            result.action_allowed = 1U;
            result.action_attempted = 1U;
            result.action_status = dcc_cluster_reconnect_shard(cluster, plan.shard_index, resume);
            return dcc_cluster_remediation_finish(out, &result, result.action_status);

        case DCC_CLUSTER_RECOVERY_REARM_RESTART:
            if (plan.shard_index == DCC_CLUSTER_NO_SHARD_INDEX || allow_rearm == 0U) {
                result.action_status = DCC_ERR_STATE;
                return dcc_cluster_remediation_finish(out, &result, DCC_ERR_STATE);
            }
            result.action_allowed = 1U;
            result.action_attempted = 1U;
            result.action_status = dcc_cluster_rearm_shard_restart(cluster, plan.shard_index);
            return dcc_cluster_remediation_finish(out, &result, result.action_status);

        case DCC_CLUSTER_RECOVERY_NONE:
        case DCC_CLUSTER_RECOVERY_WAIT:
        case DCC_CLUSTER_RECOVERY_INVESTIGATE:
        case DCC_CLUSTER_RECOVERY_UNKNOWN:
        default:
            return dcc_cluster_remediation_finish(out, &result, DCC_OK);
    }
}
