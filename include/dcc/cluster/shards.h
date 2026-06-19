#ifndef DCC_CLUSTER_SHARDS_H
#define DCC_CLUSTER_SHARDS_H

#include <dcc/cluster/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API size_t dcc_cluster_shard_count(const dcc_cluster_t *cluster);
DCC_API dcc_client_t *dcc_cluster_shard(dcc_cluster_t *cluster, uint32_t index);
DCC_API const dcc_client_t *dcc_cluster_shard_const(const dcc_cluster_t *cluster, uint32_t index);

DCC_API dcc_status_t dcc_cluster_drain_shard(dcc_cluster_t *cluster, uint32_t index);
DCC_API dcc_status_t dcc_cluster_resume_shard(dcc_cluster_t *cluster, uint32_t index);
DCC_API dcc_status_t dcc_cluster_reconnect_shard(dcc_cluster_t *cluster, uint32_t index, uint8_t resume);
DCC_API dcc_status_t dcc_cluster_reconnect_all(dcc_cluster_t *cluster, uint8_t resume);

#ifdef __cplusplus
}
#endif

#endif
