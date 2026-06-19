#include "cluster_smoke_support.h"

#include <stdint.h>
#include <stdio.h>
#include <stdatomic.h>

dcc_status_t cluster_smoke_supervisor_policy_probe(
    const dcc_cluster_t *cluster,
    const dcc_cluster_supervisor_event_t *event,
    dcc_cluster_supervisor_action_t *action,
    void *user_data
) {
    (void)cluster;
    (void)event;
    (void)user_data;
    if (action == NULL || action->size < sizeof(*action)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_cluster_supervisor_action_init(action);
    action->decision = DCC_CLUSTER_SUPERVISOR_DEFAULT;
    return DCC_OK;
}

void cluster_smoke_supervisor_decision_probe(
    const dcc_cluster_t *cluster,
    const dcc_cluster_supervisor_decision_event_t *event,
    void *user_data
) {
    (void)cluster;
    (void)event;
    (void)user_data;
}

void cluster_smoke_state_init(cluster_smoke_state_t *state, dcc_cluster_t *cluster) {
    state->cluster = cluster;
    atomic_init(&state->ready_count, 0U);
    atomic_init(&state->bad_event, 0U);
    atomic_init(&state->drain_status, DCC_ERR_STATE);
    atomic_init(&state->drain_seen, 0U);
    atomic_init(&state->admission_reconnect_status, DCC_OK);
    atomic_init(&state->admission_restore_status, DCC_ERR_STATE);
    atomic_init(&state->planned_run_status, DCC_ERR_STATE);
    atomic_init(&state->planned_completed, 0U);
    atomic_init(&state->planned_failed_shards, 999U);
    atomic_init(&state->planned_last_status, DCC_ERR_STATE);
    for (uint32_t i = 0; i < 3; ++i) {
        atomic_init(&state->seen[i], 0U);
    }
}

void cluster_smoke_on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    cluster_smoke_state_t *state = (cluster_smoke_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready == NULL || ready->shard_count != 3 || ready->shard_id >= 3) {
        atomic_store(&state->bad_event, 1U);
        (void)dcc_cluster_stop(state->cluster);
        return;
    }

    if (ready->shard_id == 0 &&
        atomic_exchange(&state->drain_seen, 1U) == 0) {
        (void)dcc_cluster_set_gateway_admission(state->cluster, 0);
        atomic_store(
            &state->admission_reconnect_status,
            (unsigned)dcc_cluster_reconnect_shard(state->cluster, 0, 0)
        );
        atomic_store(
            &state->admission_restore_status,
            (unsigned)dcc_cluster_set_gateway_admission(state->cluster, 1)
        );
        atomic_store(&state->drain_status, (unsigned)dcc_cluster_drain_shard(state->cluster, 0));
    }

    unsigned expected = 0;
    if (atomic_compare_exchange_strong(&state->seen[ready->shard_id], &expected, 1U)) {
        unsigned count = atomic_fetch_add(&state->ready_count, 1U) + 1U;
        if (count == 3U) {
            dcc_status_t run_status = dcc_cluster_run_planned_operation(state->cluster);
            atomic_store(&state->planned_run_status, (unsigned)run_status);
            dcc_cluster_operation_status_t operation = {
                .size = sizeof(operation),
            };
            dcc_status_t snapshot_status = dcc_cluster_operation_status(state->cluster, &operation);
            atomic_store(&state->planned_last_status, (unsigned)operation.last_status);
            atomic_store(&state->planned_failed_shards, operation.failed_shards);
            if (run_status == DCC_ERR_STATE &&
                snapshot_status == DCC_OK &&
                operation.type == DCC_CLUSTER_OPERATION_ROLLING_RECONNECT &&
                operation.active == 0 &&
                operation.completed == 1U &&
                operation.first_index == 1U &&
                operation.shard_count == 2U &&
                operation.batch_size == 2U &&
                operation.total_batches == 1U &&
                operation.next_index == 3U &&
                operation.completed_shards == 0 &&
                operation.failed_shards == 2U &&
                operation.last_status == DCC_ERR_STATE) {
                atomic_store(&state->planned_completed, 1U);
            } else {
                fprintf(
                    stderr,
                    "planned operation callback status failed: run=%u snapshot=%u last=%u failed=%u completed=%u\n",
                    (unsigned)run_status,
                    (unsigned)snapshot_status,
                    (unsigned)operation.last_status,
                    operation.failed_shards,
                    operation.completed_shards
                );
            }
            (void)dcc_cluster_stop(state->cluster);
        }
    }
}
