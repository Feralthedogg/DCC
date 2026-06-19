#include "cluster_restart_suppress_smoke_support.h"

#if !defined(_WIN32)

#include <stdatomic.h>
#include <string.h>
#include <unistd.h>

void *suppress_monitor_main(void *arg) {
    suppress_monitor_t *monitor = (suppress_monitor_t *)arg;
    for (unsigned i = 0; i < 1500U; ++i) {
        dcc_cluster_shard_info_t info = {
            .size = sizeof(info),
        };
        dcc_status_t status = dcc_cluster_shard_info(monitor->cluster, 0, &info);
        if (status != DCC_OK) {
            atomic_store(&monitor->bad, 1U);
            break;
        }
        if (info.status == DCC_CLUSTER_SHARD_ERROR &&
            info.restart_attempts == 1 &&
            info.restarts == 1 &&
            info.restart_suppressed == 1 &&
            info.restart_budget_used == 1 &&
            info.log_errors >= 2 &&
            strcmp(info.last_error, "restart limit reached") == 0) {
            atomic_store(&monitor->suppressed_seen, 1U);
            (void)dcc_cluster_stop(monitor->cluster);
            return NULL;
        }
        (void)usleep(10000U);
    }

    atomic_store(&monitor->timeout_seen, 1U);
    (void)dcc_cluster_stop(monitor->cluster);
    return NULL;
}

void suppress_on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)event;
    suppress_monitor_t *monitor = (suppress_monitor_t *)user_data;
    atomic_store(&monitor->bad, 1U);
    (void)dcc_cluster_stop(monitor->cluster);
}

#endif
