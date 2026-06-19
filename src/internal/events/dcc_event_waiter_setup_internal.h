#ifndef DCC_EVENT_WAITER_SETUP_INTERNAL_H
#define DCC_EVENT_WAITER_SETUP_INTERNAL_H

#include "internal/events/dcc_event_state_internal.h"

#include <dcc/dcc.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_event_waiter_set_types(
    dcc_event_waiter_node_t *waiter,
    const dcc_event_type_t *types,
    size_t type_count
);
dcc_status_t dcc_event_waiter_set_sequence(
    dcc_event_waiter_node_t *waiter,
    const dcc_event_type_t *types,
    size_t type_count,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count
);
dcc_status_t dcc_event_waiter_set_event_set(
    dcc_event_waiter_node_t *waiter,
    const dcc_event_type_t *types,
    size_t type_count,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count
);
dcc_status_t dcc_event_waiter_set_event_count(
    dcc_event_waiter_node_t *waiter,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count
);
dcc_status_t dcc_event_waiter_set_event_quorum(
    dcc_event_waiter_node_t *waiter,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count
);

#ifdef __cplusplus
}
#endif

#endif
