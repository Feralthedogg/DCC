#include "internal/client/dcc_cluster_internal.h"

static int dcc_cluster_has_gateway_url(const dcc_cluster_t *cluster) {
    const dcc_client_t *first = cluster != NULL && cluster->shard_count > 0 ? cluster->shards[0] : NULL;
    return first != NULL &&
           ((first->gateway_url != NULL && first->gateway_url[0] != '\0') ||
            (first->gateway_cached_url != NULL && first->gateway_cached_url[0] != '\0'));
}

int dcc_cluster_should_fetch_gateway_info(const dcc_cluster_t *cluster) {
    const dcc_client_t *first = cluster != NULL && cluster->shard_count > 0 ? cluster->shards[0] : NULL;
    return first != NULL &&
           !cluster->gateway_info_loaded &&
           !dcc_cluster_has_gateway_url(cluster) &&
           first->token != NULL &&
           first->token[0] != '\0';
}

dcc_status_t dcc_cluster_fetch_gateway_info(dcc_cluster_t *cluster) {
    if (cluster == NULL || cluster->shard_count == 0 || cluster->shards[0] == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_client_t *first = cluster->shards[0];
    if (dcc_cluster_has_gateway_url(cluster)) {
        cluster->gateway_info_loaded = true;
        return dcc_cluster_copy_gateway_info_from_first(cluster);
    }
    if (first->token == NULL || first->token[0] == '\0') {
        cluster->gateway_info_loaded = true;
        return DCC_OK;
    }

    dcc_status_t status = dcc_runtime_init(&first->runtime);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_gateway_fetch_bot_info(first);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_cluster_copy_gateway_info_from_first(cluster);
    if (status == DCC_OK) {
        cluster->gateway_info_loaded = true;
    }
    return status;
}
