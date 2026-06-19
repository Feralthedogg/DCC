#include "internal/events/dcc_events_internal.h"

#include <stdint.h>
#include <stdlib.h>

static int dcc_event_next_listener_cap(size_t current, size_t *out) {
    if (out == NULL ||
        current > SIZE_MAX / sizeof(dcc_event_listener_t)) {
        return -1;
    }

    size_t next = 4U;
    if (current != 0) {
        if (current > SIZE_MAX / 2U) {
            return -1;
        }
        next = current * 2U;
    }
    if (next > SIZE_MAX / sizeof(dcc_event_listener_t)) {
        return -1;
    }
    *out = next;
    return 0;
}

dcc_status_t dcc_event_bus_add(
    dcc_event_bus_t *bus,
    dcc_event_type_t type,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    if (bus == NULL || cb == NULL || type < 0 || type >= DCC_EVENT_MAX) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_bus_lock(bus);
    dcc_event_listener_list_t *list = &bus->listeners[type];
    if (list->len == list->cap) {
        size_t next_cap = 0;
        if (dcc_event_next_listener_cap(list->cap, &next_cap) != 0) {
            dcc_event_bus_unlock(bus);
            return DCC_ERR_NOMEM;
        }
        dcc_event_listener_t *next = (dcc_event_listener_t *)realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            dcc_event_bus_unlock(bus);
            return DCC_ERR_NOMEM;
        }
        list->items = next;
        list->cap = next_cap;
    }

    if (list->next_id == 0) {
        dcc_event_bus_unlock(bus);
        return DCC_ERR_NOMEM;
    }
    dcc_listener_id_t id = list->next_id++;
    list->items[list->len++] = (dcc_event_listener_t){
        .id = id,
        .cb = cb,
        .user_data = user_data,
    };
    dcc_event_bus_unlock(bus);

    if (out_id != NULL) {
        *out_id = id;
    }
    return DCC_OK;
}

dcc_status_t dcc_event_bus_remove(dcc_event_bus_t *bus, dcc_event_type_t type, dcc_listener_id_t id) {
    if (bus == NULL || type < 0 || type >= DCC_EVENT_MAX || id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_bus_lock(bus);
    dcc_event_listener_list_t *list = &bus->listeners[type];
    for (size_t i = 0; i < list->len; ++i) {
        if (list->items[i].id == id) {
            list->items[i] = list->items[list->len - 1];
            list->len--;
            dcc_event_bus_unlock(bus);
            return DCC_OK;
        }
    }
    dcc_event_bus_unlock(bus);
    return DCC_ERR_INVALID_ARG;
}
