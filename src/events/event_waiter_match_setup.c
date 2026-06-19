#include "internal/events/dcc_event_waiter_match_internal.h"

dcc_status_t dcc_event_waiter_set_types(
    dcc_event_waiter_node_t *waiter,
    const dcc_event_type_t *types,
    size_t type_count
) {
    if (waiter == NULL || types == NULL || type_count == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    waiter->match_any = 1;
    for (size_t i = 0; i < type_count; ++i) {
        if (!dcc_event_type_valid(types[i])) {
            return DCC_ERR_INVALID_ARG;
        }
        dcc_event_waiter_add_type(waiter, types[i]);
    }
    return DCC_OK;
}

dcc_status_t dcc_event_waiter_set_sequence(
    dcc_event_waiter_node_t *waiter,
    const dcc_event_type_t *types,
    size_t type_count,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count
) {
    if (waiter == NULL || types == NULL || type_count == 0 ||
        out_snapshots == NULL || out_snapshot_count < type_count) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < type_count; ++i) {
        if (!dcc_event_type_valid(types[i]) || out_snapshots[i].size < sizeof(out_snapshots[i])) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    waiter->match_sequence = 1;
    waiter->sequence = types;
    waiter->sequence_count = type_count;
    waiter->sequence_index = 0;
    waiter->sequence_snapshots = out_snapshots;
    return DCC_OK;
}

dcc_status_t dcc_event_waiter_set_event_set(
    dcc_event_waiter_node_t *waiter,
    const dcc_event_type_t *types,
    size_t type_count,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count
) {
    if (waiter == NULL || types == NULL || type_count == 0 ||
        out_snapshots == NULL || out_snapshot_count < type_count) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < type_count; ++i) {
        if (!dcc_event_type_valid(types[i]) || out_snapshots[i].size < sizeof(out_snapshots[i]) ||
            dcc_event_waiter_mask_has(waiter->type_mask, types[i])) {
            return DCC_ERR_INVALID_ARG;
        }
        dcc_event_waiter_add_type(waiter, types[i]);
    }
    waiter->match_set = 1;
    waiter->set_types = types;
    waiter->set_count = type_count;
    waiter->set_completed = 0;
    waiter->set_snapshots = out_snapshots;
    return DCC_OK;
}
