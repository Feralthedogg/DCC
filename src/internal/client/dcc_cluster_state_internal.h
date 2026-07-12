#ifndef DCC_CLUSTER_STATE_INTERNAL_H
#define DCC_CLUSTER_STATE_INTERNAL_H

#include "internal/client/dcc_client_state_internal.h"
#include "internal/client/dcc_cluster_identify_coordinator_internal.h"

#include <dcc/cluster.h>

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

struct dcc_cluster {
    dcc_client_t **shards;
    dcc_cluster_shard_info_t *health;
    dcc_cluster_operation_status_t operation;
    atomic_flag health_lock;
    uint32_t shard_count;
    uint32_t first_shard_id;
    uint32_t total_shard_count;
    uint32_t gateway_max_concurrency;
    uint32_t started_count;
    dcc_cluster_restart_policy_t restart_policy;
    dcc_cluster_supervisor_status_t supervisor_status;
    dcc_cluster_supervisor_policy_fn supervisor_policy;
    void *supervisor_policy_user_data;
    dcc_cluster_supervisor_decision_fn supervisor_decision_callback;
    void *supervisor_decision_user_data;
    atomic_bool supervisor_stop;
    atomic_bool gateway_admission_open;
    dcc_cluster_identify_coordinator_t identify_coordinator;
    bool started;
    bool gateway_info_loaded;
};

#endif
