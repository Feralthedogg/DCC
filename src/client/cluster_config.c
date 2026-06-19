#include "internal/client/dcc_cluster_internal.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

uint32_t dcc_cluster_effective_concurrency(const dcc_cluster_options_t *options) {
    if (options->size >= offsetof(dcc_cluster_options_t, gateway_max_concurrency) +
            sizeof(options->gateway_max_concurrency) &&
        options->gateway_max_concurrency != 0) {
        return options->gateway_max_concurrency;
    }
    if (options->client_options.size >= offsetof(dcc_client_options_t, gateway_max_concurrency) +
            sizeof(options->client_options.gateway_max_concurrency) &&
        options->client_options.gateway_max_concurrency != 0) {
        return options->client_options.gateway_max_concurrency;
    }
    return 1U;
}

dcc_status_t dcc_cluster_identify_delay(
    uint32_t index,
    uint32_t max_concurrency,
    uint32_t *out_delay_ms
) {
    if (out_delay_ms == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (max_concurrency == 0) {
        max_concurrency = 1U;
    }

    uint64_t batch = (uint64_t)index / (uint64_t)max_concurrency;
    uint64_t delay = batch * 5000ULL;
    if (delay > UINT32_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_delay_ms = (uint32_t)delay;
    return DCC_OK;
}

dcc_status_t dcc_cluster_apply_identify_delays(dcc_cluster_t *cluster) {
    if (cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        uint32_t delay_ms = 0;
        dcc_status_t status = dcc_cluster_identify_delay(i, cluster->gateway_max_concurrency, &delay_ms);
        if (status != DCC_OK) {
            return status;
        }
        cluster->shards[i]->gateway_identify_delay_ms = delay_ms;
    }
    return DCC_OK;
}
