#include "internal/client/dcc_cluster_internal.h"

dcc_status_t dcc_cluster_gateway_info(const dcc_cluster_t *cluster, dcc_gateway_info_t *out) {
    if (cluster == NULL || cluster->shard_count == 0 || cluster->shards[0] == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_client_gateway_info(cluster->shards[0], out);
}
