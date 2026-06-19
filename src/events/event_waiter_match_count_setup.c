#include "internal/events/dcc_event_waiter_match_internal.h"

dcc_status_t dcc_event_waiter_set_event_count(
    dcc_event_waiter_node_t *waiter,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count
) {
    if (waiter == NULL || types == NULL || type_count == 0 || target_count == 0 ||
        out_snapshots == NULL || out_snapshot_count < target_count) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < type_count; ++i) {
        if (!dcc_event_type_valid(types[i])) {
            return DCC_ERR_INVALID_ARG;
        }
        dcc_event_waiter_add_type(waiter, types[i]);
    }
    for (size_t i = 0; i < target_count; ++i) {
        if (out_snapshots[i].size < sizeof(out_snapshots[i])) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    waiter->match_count = 1;
    waiter->count_target = target_count;
    waiter->count_completed = 0;
    waiter->count_snapshots = out_snapshots;
    return DCC_OK;
}

dcc_status_t dcc_event_waiter_set_event_quorum(
    dcc_event_waiter_node_t *waiter,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count
) {
    if (waiter == NULL || types == NULL || type_count == 0 || target_count == 0 ||
        target_count > type_count || out_snapshots == NULL || out_snapshot_count < target_count) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < type_count; ++i) {
        if (!dcc_event_type_valid(types[i]) ||
            dcc_event_waiter_mask_has(waiter->type_mask, types[i])) {
            return DCC_ERR_INVALID_ARG;
        }
        dcc_event_waiter_add_type(waiter, types[i]);
    }
    for (size_t i = 0; i < target_count; ++i) {
        if (out_snapshots[i].size < sizeof(out_snapshots[i])) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    waiter->match_quorum = 1;
    waiter->count_target = target_count;
    waiter->count_completed = 0;
    waiter->count_snapshots = out_snapshots;
    return DCC_OK;
}
