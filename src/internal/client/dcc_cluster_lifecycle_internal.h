#ifndef DCC_CLUSTER_LIFECYCLE_INTERNAL_H
#define DCC_CLUSTER_LIFECYCLE_INTERNAL_H

#include "internal/client/dcc_cluster_state_internal.h"

#include <dcc/cluster.h>

#include <stdint.h>

typedef struct dcc_cluster_create_plan {
    uint32_t shard_count;
    uint32_t first_shard_id;
    uint32_t total_shard_count;
    uint32_t max_concurrency;
} dcc_cluster_create_plan_t;

dcc_status_t dcc_cluster_create_plan_from_options(
    const dcc_cluster_options_t *options,
    dcc_cluster_create_plan_t *out
);
dcc_status_t dcc_cluster_alloc_from_plan(
    const dcc_cluster_options_t *options,
    const dcc_cluster_create_plan_t *plan,
    dcc_cluster_t **out
);
dcc_status_t dcc_cluster_create_shards(
    dcc_cluster_t *cluster,
    const dcc_cluster_options_t *options,
    const dcc_cluster_create_plan_t *plan
);
void dcc_cluster_destroy_partial(dcc_cluster_t *cluster);

#endif
