#include "internal/events/dcc_events_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_event_bus_dispatch(dcc_event_bus_t *bus, dcc_client_t *client, const dcc_event_t *event) {
    if (bus == NULL || client == NULL || event == NULL || event->type < 0 || event->type >= DCC_EVENT_MAX) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_t *mutable_event = (dcc_event_t *)event;
    mutable_event->shard_id = client->shard_id;
    mutable_event->cancelled = 0;

    dcc_event_listener_t *snapshot = NULL;
    size_t snapshot_len = 0;

    dcc_event_bus_lock(bus);
    dcc_event_listener_list_t *list = &bus->listeners[event->type];
    dcc_event_bus_signal_waiters_locked(bus, event);
    snapshot_len = list->len;
    if (snapshot_len > 0) {
        snapshot = (dcc_event_listener_t *)malloc(snapshot_len * sizeof(*snapshot));
        if (snapshot == NULL) {
            dcc_event_bus_unlock(bus);
            return DCC_ERR_NOMEM;
        }
        memcpy(snapshot, list->items, snapshot_len * sizeof(*snapshot));
    }
    bus->events_dispatched++;
    dcc_event_bus_unlock(bus);

    for (size_t i = 0; i < snapshot_len; ++i) {
        snapshot[i].cb(client, event, snapshot[i].user_data);
        if (mutable_event->cancelled) {
            break;
        }
    }

    free(snapshot);
    return DCC_OK;
}
