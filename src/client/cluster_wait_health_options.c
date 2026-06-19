#include "internal/client/dcc_cluster_internal.h"

#include <stddef.h>

int dcc_cluster_health_wait_valid_target(dcc_cluster_health_state_t state) {
    return state == DCC_CLUSTER_HEALTH_HEALTHY ||
        state == DCC_CLUSTER_HEALTH_STARTING ||
        state == DCC_CLUSTER_HEALTH_DRAINING ||
        state == DCC_CLUSTER_HEALTH_DEGRADED ||
        state == DCC_CLUSTER_HEALTH_CRITICAL;
}

void dcc_cluster_health_wait_options_init(dcc_cluster_health_wait_options_t *options) {
    if (options == NULL) {
        return;
    }
    *options = (dcc_cluster_health_wait_options_t){
        .size = sizeof(*options),
        .target_health = DCC_CLUSTER_HEALTH_HEALTHY,
        .timeout_ms = 30000U,
        .interval_ms = 10U,
        .fail_on_critical = 1U,
    };
}

dcc_status_t dcc_cluster_health_wait_effective_options(
    const dcc_cluster_health_wait_options_t *options,
    dcc_cluster_health_wait_options_t *effective
) {
    if (effective == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_cluster_health_wait_options_init(effective);
    if (options == NULL) {
        return DCC_OK;
    }
    if (options->size < offsetof(dcc_cluster_health_wait_options_t, target_health) +
            sizeof(options->target_health)) {
        return DCC_ERR_INVALID_ARG;
    }
    effective->target_health = options->target_health;
    if (options->size >= offsetof(dcc_cluster_health_wait_options_t, timeout_ms) +
            sizeof(options->timeout_ms)) {
        effective->timeout_ms = options->timeout_ms;
    }
    if (options->size >= offsetof(dcc_cluster_health_wait_options_t, interval_ms) +
            sizeof(options->interval_ms)) {
        effective->interval_ms = options->interval_ms;
    }
    if (options->size >= offsetof(dcc_cluster_health_wait_options_t, fail_on_critical) +
            sizeof(options->fail_on_critical)) {
        effective->fail_on_critical = options->fail_on_critical;
    }
    return DCC_OK;
}
