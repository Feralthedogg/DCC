#include "internal/client/dcc_cluster_internal.h"

#include <stdlib.h>

dcc_status_t dcc_cluster_on(
    dcc_cluster_t *cluster,
    dcc_event_type_t type,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_ids,
    size_t out_ids_count
) {
    if (cluster == NULL || cb == NULL || type >= DCC_EVENT_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    if (out_ids != NULL && out_ids_count < cluster->shard_count) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_listener_id_t *ids = out_ids;
    dcc_listener_id_t *owned_ids = NULL;
    if (ids == NULL) {
        owned_ids = (dcc_listener_id_t *)calloc(cluster->shard_count, sizeof(owned_ids[0]));
        if (owned_ids == NULL) {
            return DCC_ERR_NOMEM;
        }
        ids = owned_ids;
    }

    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        dcc_listener_id_t id = 0;
        dcc_status_t status = dcc_client_on(cluster->shards[i], type, cb, user_data, &id);
        if (status != DCC_OK) {
            for (uint32_t j = 0; j < i; ++j) {
                dcc_listener_id_t old_id = ids[j];
                if (old_id != 0) {
                    (void)dcc_client_off(cluster->shards[j], type, old_id);
                }
            }
            free(owned_ids);
            return status;
        }
        ids[i] = id;
    }

    free(owned_ids);
    return DCC_OK;
}
