#ifndef DCC_CLUSTER_SHARD_TYPES_H
#define DCC_CLUSTER_SHARD_TYPES_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_cluster_shard_status {
    DCC_CLUSTER_SHARD_CREATED = 0,
    DCC_CLUSTER_SHARD_STARTING,
    DCC_CLUSTER_SHARD_READY,
    DCC_CLUSTER_SHARD_STOPPING,
    DCC_CLUSTER_SHARD_STOPPED,
    DCC_CLUSTER_SHARD_ERROR
} dcc_cluster_shard_status_t;

typedef struct dcc_cluster_shard_info {
    size_t size;
    uint32_t shard_id;
    uint32_t shard_count;
    dcc_cluster_shard_status_t status;
    uint64_t starts;
    uint64_t restart_attempts;
    uint64_t restarts;
    uint64_t restart_suppressed;
    uint64_t restart_budget_used;
    uint64_t ready_events;
    uint64_t resumed_events;
    uint64_t socket_closes;
    uint64_t log_errors;
    uint16_t last_close_code;
    char last_close_reason[128];
    char last_error[256];
} dcc_cluster_shard_info_t;

#define DCC_CLUSTER_NO_SHARD_INDEX UINT32_MAX

#ifdef __cplusplus
}
#endif

#endif
