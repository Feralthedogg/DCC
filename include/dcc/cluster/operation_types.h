#ifndef DCC_CLUSTER_OPERATION_TYPES_H
#define DCC_CLUSTER_OPERATION_TYPES_H

#include <dcc/error.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_cluster_rolling_reconnect_options {
    size_t size;
    uint32_t first_index;
    uint32_t shard_count;
    uint32_t batch_size;
    uint32_t delay_ms;
    uint8_t resume;
} dcc_cluster_rolling_reconnect_options_t;

typedef enum dcc_cluster_operation_type {
    DCC_CLUSTER_OPERATION_NONE = 0,
    DCC_CLUSTER_OPERATION_ROLLING_RECONNECT = 1
} dcc_cluster_operation_type_t;

typedef struct dcc_cluster_operation_status {
    size_t size;
    dcc_cluster_operation_type_t type;
    uint64_t generation;
    uint8_t active;
    uint8_t completed;
    uint8_t resume;
    uint8_t reserved;
    uint32_t first_index;
    uint32_t shard_count;
    uint32_t batch_size;
    uint32_t delay_ms;
    uint32_t next_index;
    uint32_t completed_shards;
    uint32_t failed_shards;
    uint32_t current_batch;
    uint32_t total_batches;
    dcc_status_t last_status;
} dcc_cluster_operation_status_t;

#ifdef __cplusplus
}
#endif

#endif
