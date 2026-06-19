#ifndef DCC_EVENT_WAITER_POLICY_INTERNAL_H
#define DCC_EVENT_WAITER_POLICY_INTERNAL_H

#include "internal/events/dcc_events_internal.h"

typedef enum dcc_event_waiter_policy_count {
    DCC_EVENT_WAITER_POLICY_COUNT_SEQUENCE = 0,
    DCC_EVENT_WAITER_POLICY_COUNT_SET,
    DCC_EVENT_WAITER_POLICY_COUNT_MATCHES
} dcc_event_waiter_policy_count_t;

dcc_status_t dcc_event_waiter_policy_wait(
    dcc_client_t *client,
    dcc_event_waiter_node_t *waiter,
    uint32_t timeout_ms,
    dcc_event_waiter_policy_count_t count_mode,
    size_t *out_completed
);

#endif
