#include "internal/client/dcc_cluster_internal.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_cluster_create_plan_from_options(
    const dcc_cluster_options_t *options,
    dcc_cluster_create_plan_t *out
) {
    if (options == NULL || out == NULL ||
        options->size < offsetof(dcc_cluster_options_t, client_options) + sizeof(options->client_options)) {
        return DCC_ERR_INVALID_ARG;
    }

    uint32_t shard_count = options->shard_count != 0
        ? options->shard_count
        : options->client_options.shard_count;
    if (shard_count == 0) {
        shard_count = 1U;
    }

    uint32_t total_shard_count = options->size >= offsetof(dcc_cluster_options_t, total_shard_count) +
            sizeof(options->total_shard_count) &&
            options->total_shard_count != 0
        ? options->total_shard_count
        : shard_count;
    uint32_t first_shard_id = options->size >= offsetof(dcc_cluster_options_t, first_shard_id) +
            sizeof(options->first_shard_id)
        ? options->first_shard_id
        : 0U;
    if (first_shard_id >= total_shard_count || shard_count > total_shard_count - first_shard_id ||
        shard_count > (uint32_t)(SIZE_MAX / sizeof(dcc_client_t *))) {
        return DCC_ERR_INVALID_ARG;
    }

    uint32_t max_concurrency = dcc_cluster_effective_concurrency(options);
    uint32_t last_delay = 0;
    dcc_status_t status = dcc_cluster_identify_delay(shard_count - 1U, max_concurrency, &last_delay);
    if (status != DCC_OK) {
        return status;
    }

    out->shard_count = shard_count;
    out->first_shard_id = first_shard_id;
    out->total_shard_count = total_shard_count;
    out->max_concurrency = max_concurrency;
    return DCC_OK;
}
