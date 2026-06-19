#ifndef DCC_EVENT_WAITER_MATCH_INTERNAL_H
#define DCC_EVENT_WAITER_MATCH_INTERNAL_H

#include "internal/events/dcc_events_internal.h"

void dcc_event_snapshot_fill(dcc_event_snapshot_t *snapshot, const dcc_event_t *event);
void dcc_event_snapshot_copy(dcc_event_snapshot_t *out, const dcc_event_snapshot_t *snapshot);
uint8_t dcc_event_type_valid(dcc_event_type_t type);
void dcc_event_waiter_add_type(dcc_event_waiter_node_t *waiter, dcc_event_type_t type);
uint8_t dcc_event_waiter_mask_has(
    const uint64_t mask[DCC_EVENT_WAIT_MASK_WORDS],
    dcc_event_type_t type
);
void dcc_event_waiter_mask_add(
    uint64_t mask[DCC_EVENT_WAIT_MASK_WORDS],
    dcc_event_type_t type
);

#endif
