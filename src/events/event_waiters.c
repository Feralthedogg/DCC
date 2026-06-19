#include "internal/events/dcc_events_internal.h"

#include <errno.h>
#include <string.h>

dcc_status_t dcc_client_wait_for_event(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
) {
    if (client == NULL || out == NULL || out->size < sizeof(*out) || type < 0 || type >= DCC_EVENT_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_client_wait_for_event_owned(client, type, timeout_ms, DCC_EVENT_WAIT_SNAPSHOT, out, NULL);
}

dcc_status_t dcc_client_wait_for_event_owned(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_wait_kind_t kind,
    dcc_event_snapshot_t *out_snapshot,
    void **out_owned
) {
    if (client == NULL || type < 0 || type >= DCC_EVENT_MAX ||
        (out_snapshot != NULL && out_snapshot->size < sizeof(*out_snapshot)) ||
        (kind != DCC_EVENT_WAIT_SNAPSHOT && out_owned == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (out_owned != NULL) {
        *out_owned = NULL;
    }
    if (!client->runtime.initialized) {
        return DCC_ERR_STATE;
    }

    llam_channel_t *done = llam_channel_create(1);
    if (done == NULL) {
        return dcc_event_wait_status_from_errno(errno);
    }

    dcc_event_waiter_node_t waiter;
    memset(&waiter, 0, sizeof(waiter));
    waiter.type = type;
    waiter.kind = kind;
    waiter.done = done;
    waiter.snapshot.size = sizeof(waiter.snapshot);
    waiter.owned_status = DCC_OK;

    dcc_event_bus_lock(&client->events);
    waiter.next = client->events.waiters;
    client->events.waiters = &waiter;
    dcc_event_bus_unlock(&client->events);

    dcc_status_t status = dcc_event_waiter_wait_channel(&waiter, timeout_ms);

    dcc_event_bus_lock(&client->events);
    dcc_event_waiter_remove_locked(&client->events, &waiter);
    uint8_t completed = waiter.completed;
    dcc_event_snapshot_t snapshot = waiter.snapshot;
    void *owned = waiter.owned;
    dcc_status_t owned_status = waiter.owned_status;
    waiter.owned = NULL;
    dcc_event_bus_unlock(&client->events);

    dcc_event_waiter_drain_channel(done);
    (void)llam_channel_close(done);
    (void)llam_channel_destroy(done);

    if (completed && status == DCC_ERR_TIMEOUT) {
        status = DCC_OK;
    }

    if (status == DCC_OK && completed && owned_status != DCC_OK) {
        dcc_event_waiter_free_owned(kind, owned);
        return owned_status;
    }

    if (status == DCC_OK && completed) {
        if (out_snapshot != NULL) {
            size_t out_size = out_snapshot->size;
            size_t copy_size = out_size < sizeof(snapshot) ? out_size : sizeof(snapshot);
            memcpy(out_snapshot, &snapshot, copy_size);
            out_snapshot->size = out_size;
        }
        if (out_owned != NULL) {
            *out_owned = owned;
            owned = NULL;
        }
    }
    dcc_event_waiter_free_owned(kind, owned);
    return status;
}
