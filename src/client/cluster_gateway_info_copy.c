#include "internal/client/dcc_cluster_internal.h"

#include <stdlib.h>

dcc_status_t dcc_cluster_copy_gateway_info_from_first(dcc_cluster_t *cluster) {
    if (cluster == NULL || cluster->shard_count == 0 || cluster->shards[0] == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    const dcc_client_t *source = cluster->shards[0];
    for (uint32_t i = 1; i < cluster->shard_count; ++i) {
        dcc_client_t *target = cluster->shards[i];
        char *gateway_cached_url = NULL;
        if (source->gateway_cached_url != NULL && source->gateway_cached_url[0] != '\0') {
            gateway_cached_url = dcc_strdup(source->gateway_cached_url);
            if (gateway_cached_url == NULL) {
                return DCC_ERR_NOMEM;
            }
        }

        free(target->gateway_cached_url);
        target->gateway_cached_url = gateway_cached_url;
        target->gateway_recommended_shards = source->gateway_recommended_shards;
        target->gateway_max_concurrency = source->gateway_max_concurrency;
        target->gateway_remaining_identifies = source->gateway_remaining_identifies;
        target->gateway_session_total_identifies = source->gateway_session_total_identifies;
        target->gateway_session_reset_after_ms = source->gateway_session_reset_after_ms;
        target->gateway_session_reset_at_ms = source->gateway_session_reset_at_ms;
    }

    if (source->gateway_max_concurrency != 0) {
        cluster->gateway_max_concurrency = source->gateway_max_concurrency;
    }
    return dcc_cluster_apply_identify_delays(cluster);
}
