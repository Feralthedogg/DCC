#ifndef DCC_EVENT_WAITER_RUNTIME_INTERNAL_H
#define DCC_EVENT_WAITER_RUNTIME_INTERNAL_H

#include "internal/events/dcc_event_state_internal.h"

#include <dcc/dcc.h>
#include <llam/runtime.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_event_wait_status_from_errno(int err);
uint64_t dcc_event_waiter_deadline_ns(uint32_t timeout_ms);
void dcc_event_waiter_sleep_ms(uint32_t ms);
void dcc_event_waiter_remove_locked(dcc_event_bus_t *bus, dcc_event_waiter_node_t *waiter);
void dcc_event_waiter_drain_channel(llam_channel_t *channel);
dcc_status_t dcc_event_waiter_wait_channel(dcc_event_waiter_node_t *waiter, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
