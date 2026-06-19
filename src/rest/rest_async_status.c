#include "internal/rest/dcc_rest_async_status_internal.h"

#include <string.h>

#define DCC_REST_ASYNC_STATUS_BASE_SIZE \
    (offsetof(dcc_rest_async_status_t, pending_high) + sizeof(((dcc_rest_async_status_t *)0)->pending_high))

dcc_status_t dcc_rest_async_status(
    dcc_client_t *client,
    dcc_rest_async_status_t *out
) {
    if (client == NULL || out == NULL || out->size < DCC_REST_ASYNC_STATUS_BASE_SIZE) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t out_size = out->size;

    dcc_rest_async_status_t snapshot;
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.size = sizeof(snapshot);

    dcc_rest_lock(client);
    dcc_rest_async_status_snapshot_locked(client, dcc_rest_now_ms(), &snapshot);
    dcc_rest_unlock(client);

    size_t copy_size = out_size < sizeof(snapshot) ? out_size : sizeof(snapshot);
    memcpy(out, &snapshot, copy_size);
    return DCC_OK;
}

dcc_status_t dcc_rest_async_pending(
    dcc_client_t *client,
    size_t *out_pending,
    size_t *out_active
) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_lock(client);
    size_t pending = dcc_rest_async_pending_count_locked(client);
    size_t active = client->rest_async_active;
    dcc_rest_unlock(client);

    if (out_pending != NULL) {
        *out_pending = pending;
    }
    if (out_active != NULL) {
        *out_active = active;
    }
    return DCC_OK;
}
