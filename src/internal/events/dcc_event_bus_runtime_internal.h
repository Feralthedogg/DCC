#ifndef DCC_EVENT_BUS_RUNTIME_INTERNAL_H
#define DCC_EVENT_BUS_RUNTIME_INTERNAL_H

#include "internal/events/dcc_event_state_internal.h"

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_event_bus_lock(dcc_event_bus_t *bus);
void dcc_event_bus_unlock(dcc_event_bus_t *bus);
void dcc_event_bus_signal_waiters_locked(dcc_event_bus_t *bus, const dcc_event_t *event);

#ifdef __cplusplus
}
#endif

#endif
