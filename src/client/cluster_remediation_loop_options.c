#include "internal/client/dcc_cluster_internal.h"

#include <stddef.h>
#include <string.h>

static int dcc_cluster_remediation_loop_has(
    const dcc_cluster_remediation_loop_options_t *options,
    size_t offset,
    size_t size
) {
    return options != NULL && options->size >= offset && options->size - offset >= size;
}

void dcc_cluster_remediation_loop_options_init(
    dcc_cluster_remediation_loop_options_t *options
) {
    if (options == NULL) {
        return;
    }

    memset(options, 0, sizeof(*options));
    options->size = sizeof(*options);
    options->remediation.size = sizeof(options->remediation);
    options->remediation.allow_reconnect = 1U;
    options->remediation.resume = 1U;
    options->max_attempts = 3U;
    options->interval_ms = 1000U;
    options->stop_on_healthy = 1U;
    options->stop_on_no_action = 1U;
}

dcc_status_t dcc_cluster_remediation_loop_options_effective(
    const dcc_cluster_remediation_loop_options_t *options,
    dcc_cluster_remediation_loop_options_t *effective
) {
    dcc_cluster_remediation_loop_options_init(effective);
    if (options == NULL) {
        return DCC_OK;
    }
    if (options->size < sizeof(options->size)) {
        return DCC_ERR_INVALID_ARG;
    }

    if (dcc_cluster_remediation_loop_has(
            options,
            offsetof(dcc_cluster_remediation_loop_options_t, remediation),
            sizeof(options->remediation)
        )) {
        effective->remediation = options->remediation;
        if (effective->remediation.size == 0) {
            effective->remediation.size = sizeof(effective->remediation);
        }
    }
    if (dcc_cluster_remediation_loop_has(
            options,
            offsetof(dcc_cluster_remediation_loop_options_t, max_attempts),
            sizeof(options->max_attempts)
        )) {
        effective->max_attempts = options->max_attempts;
    }
    if (dcc_cluster_remediation_loop_has(
            options,
            offsetof(dcc_cluster_remediation_loop_options_t, interval_ms),
            sizeof(options->interval_ms)
        )) {
        effective->interval_ms = options->interval_ms;
    }
    if (dcc_cluster_remediation_loop_has(
            options,
            offsetof(dcc_cluster_remediation_loop_options_t, stop_on_healthy),
            sizeof(options->stop_on_healthy)
        )) {
        effective->stop_on_healthy = options->stop_on_healthy;
    }
    if (dcc_cluster_remediation_loop_has(
            options,
            offsetof(dcc_cluster_remediation_loop_options_t, stop_on_no_action),
            sizeof(options->stop_on_no_action)
        )) {
        effective->stop_on_no_action = options->stop_on_no_action;
    }

    return effective->max_attempts == 0 ? DCC_ERR_INVALID_ARG : DCC_OK;
}
