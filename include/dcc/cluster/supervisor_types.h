#ifndef DCC_CLUSTER_SUPERVISOR_TYPES_H
#define DCC_CLUSTER_SUPERVISOR_TYPES_H

#include <dcc/client.h>
#include <dcc/cluster/base.h>
#include <dcc/cluster/shard_types.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_cluster_supervisor_decision {
    DCC_CLUSTER_SUPERVISOR_DEFAULT = 0,
    DCC_CLUSTER_SUPERVISOR_RESTART,
    DCC_CLUSTER_SUPERVISOR_SUPPRESS
} dcc_cluster_supervisor_decision_t;

typedef struct dcc_cluster_supervisor_event dcc_cluster_supervisor_event_t;
typedef struct dcc_cluster_supervisor_decision_event dcc_cluster_supervisor_decision_event_t;
typedef struct dcc_cluster_supervisor_action dcc_cluster_supervisor_action_t;

typedef dcc_status_t (*dcc_cluster_supervisor_policy_fn)(
    const dcc_cluster_t *cluster,
    const dcc_cluster_supervisor_event_t *event,
    dcc_cluster_supervisor_action_t *action,
    void *user_data
);
typedef void (*dcc_cluster_supervisor_decision_fn)(
    const dcc_cluster_t *cluster,
    const dcc_cluster_supervisor_decision_event_t *event,
    void *user_data
);

typedef struct dcc_cluster_options {
    size_t size;
    dcc_client_options_t client_options;
    uint32_t shard_count;
    uint32_t first_shard_id;
    uint32_t total_shard_count;
    uint32_t gateway_max_concurrency;
    uint32_t shard_restart_max;
    uint32_t shard_restart_base_delay_ms;
    uint32_t shard_restart_max_delay_ms;
    dcc_cluster_supervisor_policy_fn supervisor_policy;
    void *supervisor_policy_user_data;
    dcc_cluster_supervisor_decision_fn supervisor_decision_callback;
    void *supervisor_decision_user_data;
} dcc_cluster_options_t;

typedef struct dcc_cluster_restart_policy {
    size_t size;
    uint32_t max_restarts;
    uint32_t base_delay_ms;
    uint32_t max_delay_ms;
} dcc_cluster_restart_policy_t;

struct dcc_cluster_supervisor_event {
    size_t size;
    uint32_t shard_index;
    uint32_t shard_id;
    dcc_status_t gateway_status;
    dcc_cluster_shard_info_t shard;
    dcc_cluster_restart_policy_t restart_policy;
};

struct dcc_cluster_supervisor_action {
    size_t size;
    dcc_cluster_supervisor_decision_t decision;
    uint32_t delay_ms;
    uint8_t reserved[8];
};

typedef struct dcc_cluster_supervisor_status {
    size_t size;
    uint64_t decisions;
    uint64_t restart_scheduled;
    uint64_t restart_suppressed;
    uint64_t policy_errors;
    uint32_t last_shard_index;
    uint32_t last_shard_id;
    dcc_status_t last_gateway_status;
    dcc_status_t last_action_status;
    dcc_cluster_supervisor_decision_t last_decision;
    uint32_t last_delay_ms;
    uint8_t last_restart_scheduled;
    uint8_t reserved[7];
} dcc_cluster_supervisor_status_t;

struct dcc_cluster_supervisor_decision_event {
    size_t size;
    uint32_t shard_index;
    uint32_t shard_id;
    dcc_status_t gateway_status;
    dcc_status_t action_status;
    dcc_cluster_supervisor_action_t action;
    dcc_cluster_shard_info_t shard;
    dcc_cluster_restart_policy_t restart_policy;
    dcc_cluster_supervisor_status_t status;
    uint8_t restart_scheduled;
    uint8_t restart_suppressed;
    uint8_t reserved[6];
};

#ifdef __cplusplus
}
#endif

#endif
