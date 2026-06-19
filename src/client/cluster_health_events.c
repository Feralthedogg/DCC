#include "internal/client/dcc_cluster_internal.h"

#include <limits.h>

static uint32_t dcc_cluster_index_for_client(const dcc_cluster_t *cluster, const dcc_client_t *client) {
    if (cluster == NULL || client == NULL) {
        return UINT32_MAX;
    }
    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        if (cluster->shards[i] == client) {
            return i;
        }
    }
    return UINT32_MAX;
}

static void dcc_cluster_health_event(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    dcc_cluster_t *cluster = (dcc_cluster_t *)user_data;
    uint32_t index = dcc_cluster_index_for_client(cluster, client);
    if (cluster == NULL || event == NULL || index >= cluster->shard_count) {
        return;
    }

    dcc_cluster_health_lock(cluster);
    dcc_cluster_shard_info_t *info = &cluster->health[index];
    if (event->type == DCC_EVENT_READY) {
        info->status = DCC_CLUSTER_SHARD_READY;
        info->ready_events++;
    } else if (event->type == DCC_EVENT_RESUMED) {
        info->status = DCC_CLUSTER_SHARD_READY;
        info->resumed_events++;
    } else if (event->type == DCC_EVENT_SOCKET_CLOSE) {
        const dcc_socket_close_event_t *close_event = dcc_event_socket_close(event);
        info->socket_closes++;
        if (close_event != NULL) {
            info->last_close_code = close_event->code;
            dcc_cluster_copy_text(
                info->last_close_reason,
                sizeof(info->last_close_reason),
                close_event->reason
            );
        }
    } else if (event->type == DCC_EVENT_LOG) {
        const dcc_log_event_t *log = dcc_event_log(event);
        if (log != NULL && log->level >= DCC_LOG_ERROR) {
            info->status = DCC_CLUSTER_SHARD_ERROR;
            info->log_errors++;
            dcc_cluster_copy_text(info->last_error, sizeof(info->last_error), log->message);
        }
    }
    dcc_cluster_health_unlock(cluster);
}

dcc_status_t dcc_cluster_register_health_events(dcc_cluster_t *cluster, uint32_t index) {
    if (cluster == NULL || index >= cluster->shard_count || cluster->shards[index] == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_client_t *shard = cluster->shards[index];
    dcc_status_t status = dcc_client_on(shard, DCC_EVENT_READY, dcc_cluster_health_event, cluster, NULL);
    if (status == DCC_OK) {
        status = dcc_client_on(shard, DCC_EVENT_RESUMED, dcc_cluster_health_event, cluster, NULL);
    }
    if (status == DCC_OK) {
        status = dcc_client_on(shard, DCC_EVENT_SOCKET_CLOSE, dcc_cluster_health_event, cluster, NULL);
    }
    if (status == DCC_OK) {
        status = dcc_client_on(shard, DCC_EVENT_LOG, dcc_cluster_health_event, cluster, NULL);
    }
    return status;
}
