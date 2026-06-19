#ifndef DCC_EVENT_BUS_STATE_INTERNAL_H
#define DCC_EVENT_BUS_STATE_INTERNAL_H

#include "internal/events/dcc_event_listener_state_internal.h"
#include "internal/events/dcc_event_waiter_state_internal.h"

#include <dcc/dcc.h>

#include <stdatomic.h>
#include <stdint.h>

typedef struct dcc_event_bus {
    atomic_flag lock;
    dcc_event_listener_list_t listeners[DCC_EVENT_MAX];
    dcc_event_waiter_node_t *waiters;
    uint64_t events_dispatched;
} dcc_event_bus_t;

#endif
