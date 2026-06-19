#include "internal/client/dcc_cluster_internal.h"

#include <stdatomic.h>

void dcc_cluster_supervisor_task(void *arg) {
    dcc_cluster_t *cluster = (dcc_cluster_t *)arg;
    while (!atomic_load_explicit(&cluster->supervisor_stop, memory_order_acquire)) {
        for (uint32_t i = 0; i < cluster->shard_count; ++i) {
            dcc_client_t *client = cluster->shards[i];
            if (client == NULL ||
                atomic_load_explicit(&client->stopping, memory_order_acquire) ||
                !atomic_load_explicit(&client->gateway_task_completed, memory_order_acquire) ||
                atomic_load_explicit(&client->gateway_task_running, memory_order_acquire)) {
                continue;
            }

            dcc_status_t gateway_status =
                (dcc_status_t)atomic_load_explicit(&client->gateway_last_status, memory_order_acquire);
            if (gateway_status == DCC_OK) {
                atomic_store_explicit(&client->gateway_task_completed, false, memory_order_release);
                continue;
            }

            int should_restart = 0;
            uint32_t delay_ms = 0;
            dcc_cluster_supervisor_action_t action;
            dcc_cluster_supervisor_action_init(&action);
            dcc_status_t action_status =
                dcc_cluster_supervisor_decide(cluster, i, gateway_status, &action);
            dcc_cluster_supervisor_apply_action(
                cluster,
                client,
                i,
                gateway_status,
                action_status,
                &action,
                &should_restart,
                &delay_ms
            );

            if (!should_restart) {
                continue;
            }
            if (dcc_cluster_supervisor_wait(cluster, delay_ms) != DCC_OK ||
                atomic_load_explicit(&client->stopping, memory_order_acquire)) {
                continue;
            }

            dcc_status_t status = dcc_gateway_start(client);
            dcc_cluster_supervisor_record_gateway_start_result(cluster, i, status);
        }

        (void)dcc_cluster_supervisor_wait(cluster, 100U);
    }
}
