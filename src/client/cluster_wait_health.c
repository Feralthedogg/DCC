#include "internal/client/dcc_cluster_internal.h"

dcc_status_t dcc_cluster_wait_until_health(
    dcc_cluster_t *cluster,
    const dcc_cluster_health_wait_options_t *options,
    dcc_cluster_recovery_plan_t *out
) {
    dcc_cluster_health_wait_options_t effective;
    dcc_status_t option_status = dcc_cluster_health_wait_effective_options(options, &effective);
    if (option_status != DCC_OK) {
        return option_status;
    }
    if (cluster == NULL || (out != NULL && out->size < sizeof(*out)) ||
        !dcc_cluster_health_wait_valid_target(effective.target_health)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!cluster->started) {
        return DCC_ERR_STATE;
    }

    uint32_t interval_ms = effective.interval_ms != 0U ? effective.interval_ms : 1U;
    uint64_t start_ms = dcc_cluster_health_wait_now_ms();
    for (;;) {
        dcc_cluster_recovery_plan_t plan = {
            .size = sizeof(plan),
        };
        dcc_status_t status = dcc_cluster_recovery_plan(cluster, &plan);
        if (status != DCC_OK) {
            return status;
        }
        if (out != NULL) {
            *out = plan;
        }
        if (plan.health == effective.target_health) {
            return DCC_OK;
        }
        if (effective.fail_on_critical != 0U &&
            effective.target_health != DCC_CLUSTER_HEALTH_CRITICAL &&
            plan.health == DCC_CLUSTER_HEALTH_CRITICAL) {
            return DCC_ERR_STATE;
        }
        if (effective.timeout_ms != 0U &&
            dcc_cluster_health_wait_now_ms() - start_ms >= effective.timeout_ms) {
            return DCC_ERR_TIMEOUT;
        }
        dcc_cluster_health_wait_sleep_ms(interval_ms);
    }
}
