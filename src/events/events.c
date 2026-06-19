#include "internal/events/dcc_events_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_event_bus_init(dcc_event_bus_t *bus) {
    if (bus == NULL) {
        return;
    }
    memset(bus, 0, sizeof(*bus));
    atomic_flag_clear(&bus->lock);
    bus->waiters = NULL;
    for (size_t i = 0; i < (size_t)DCC_EVENT_MAX; ++i) {
        bus->listeners[i].next_id = 1;
    }
}

void dcc_event_bus_deinit(dcc_event_bus_t *bus) {
    if (bus == NULL) {
        return;
    }
    for (size_t i = 0; i < (size_t)DCC_EVENT_MAX; ++i) {
        free(bus->listeners[i].items);
        bus->listeners[i].items = NULL;
        bus->listeners[i].len = 0;
        bus->listeners[i].cap = 0;
    }
    bus->waiters = NULL;
}
