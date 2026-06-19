#include "internal/client/dcc_cluster_internal.h"

#include <stdlib.h>

void dcc_cluster_destroy_partial(dcc_cluster_t *cluster) {
    if (cluster == NULL) {
        return;
    }
    if (cluster->shards != NULL) {
        for (uint32_t i = 0; i < cluster->shard_count; ++i) {
            dcc_client_destroy(cluster->shards[i]);
        }
    }
    free(cluster->shards);
    free(cluster->health);
    free(cluster);
}
