#ifndef DCC_CLUSTER_HEALTH_SUMMARY_INTERNAL_H
#define DCC_CLUSTER_HEALTH_SUMMARY_INTERNAL_H

#include <dcc/cluster.h>

static inline void dcc_cluster_summary_add_status(
    dcc_cluster_health_summary_t *summary,
    dcc_cluster_shard_status_t status
) {
    switch (status) {
        case DCC_CLUSTER_SHARD_CREATED:
            summary->created_shards++;
            break;
        case DCC_CLUSTER_SHARD_STARTING:
            summary->starting_shards++;
            break;
        case DCC_CLUSTER_SHARD_READY:
            summary->ready_shards++;
            break;
        case DCC_CLUSTER_SHARD_STOPPING:
            summary->stopping_shards++;
            break;
        case DCC_CLUSTER_SHARD_STOPPED:
            summary->stopped_shards++;
            break;
        case DCC_CLUSTER_SHARD_ERROR:
            summary->error_shards++;
            break;
        default:
            break;
    }
}

#endif
