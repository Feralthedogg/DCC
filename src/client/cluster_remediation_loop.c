#include "internal/client/dcc_cluster_internal.h"

dcc_status_t dcc_cluster_remediate_loop(
    dcc_cluster_t *cluster,
    const dcc_cluster_remediation_loop_options_t *options,
    dcc_cluster_remediation_loop_result_t *out
) {
    if (cluster == NULL || (out != NULL && out->size < sizeof(*out))) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_remediation_loop_options_t effective;
    dcc_status_t status =
        dcc_cluster_remediation_loop_options_effective(options, &effective);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cluster_remediation_loop_result_t result;
    dcc_cluster_remediation_loop_result_init(&result);

    for (uint32_t i = 0; i < effective.max_attempts; ++i) {
        dcc_cluster_remediation_result_t step = {
            .size = sizeof(step),
        };
        status = dcc_cluster_remediate(cluster, &effective.remediation, &step);
        result.attempts++;
        result.last = step;
        result.last_status = status;
        result.final_health = step.plan.health;
        result.final_action = step.plan.action;
        if (step.action_attempted != 0) {
            result.actions_attempted++;
        }

        if (status != DCC_OK) {
            result.stopped_error = 1U;
            return dcc_cluster_remediation_loop_finish(out, &result, status);
        }

        if (effective.stop_on_healthy != 0 &&
            step.plan.health == DCC_CLUSTER_HEALTH_HEALTHY) {
            result.reached_healthy = 1U;
            return dcc_cluster_remediation_loop_finish(out, &result, DCC_OK);
        }

        if (effective.stop_on_no_action != 0 && step.action_attempted == 0) {
            result.stopped_no_action = 1U;
            return dcc_cluster_remediation_loop_finish(out, &result, DCC_OK);
        }

        if (i + 1U < effective.max_attempts && effective.interval_ms != 0) {
            status = dcc_cluster_remediation_loop_delay(cluster, effective.interval_ms);
            if (status != DCC_OK) {
                result.last_status = status;
                result.stopped_error = 1U;
                return dcc_cluster_remediation_loop_finish(out, &result, status);
            }
        }
    }

    result.exhausted_attempts = 1U;
    return dcc_cluster_remediation_loop_finish(out, &result, DCC_OK);
}
