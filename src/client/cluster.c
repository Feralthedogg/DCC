#include "internal/client/dcc_cluster_internal.h"

dcc_status_t dcc_cluster_create(const dcc_cluster_options_t *options, dcc_cluster_t **out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_cluster_create_plan_t plan;
    dcc_status_t status = dcc_cluster_create_plan_from_options(options, &plan);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cluster_t *cluster = NULL;
    status = dcc_cluster_alloc_from_plan(options, &plan, &cluster);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_cluster_create_shards(cluster, options, &plan);
    if (status != DCC_OK) {
        dcc_cluster_destroy_partial(cluster);
        return status;
    }

    *out = cluster;
    return DCC_OK;
}

void dcc_cluster_destroy(dcc_cluster_t *cluster) {
    if (cluster == NULL) {
        return;
    }
    (void)dcc_cluster_stop(cluster);
    dcc_cluster_destroy_partial(cluster);
}

size_t dcc_cluster_shard_count(const dcc_cluster_t *cluster) {
    return cluster != NULL ? cluster->shard_count : 0U;
}

dcc_client_t *dcc_cluster_shard(dcc_cluster_t *cluster, uint32_t index) {
    if (cluster == NULL || index >= cluster->shard_count) {
        return NULL;
    }
    return cluster->shards[index];
}

const dcc_client_t *dcc_cluster_shard_const(const dcc_cluster_t *cluster, uint32_t index) {
    if (cluster == NULL || index >= cluster->shard_count) {
        return NULL;
    }
    return cluster->shards[index];
}
