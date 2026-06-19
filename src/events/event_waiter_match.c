#include "internal/events/dcc_event_waiter_match_internal.h"

static uint8_t dcc_event_waiter_matches(
    const dcc_event_waiter_node_t *waiter,
    dcc_event_type_t type
) {
    if (waiter == NULL || !dcc_event_type_valid(type)) {
        return 0;
    }
    if (!waiter->match_any) {
        return waiter->type == type;
    }
    return dcc_event_waiter_mask_has(waiter->type_mask, type);
}

void dcc_event_bus_signal_waiters_locked(dcc_event_bus_t *bus, const dcc_event_t *event) {
    if (bus == NULL || event == NULL) {
        return;
    }
    for (dcc_event_waiter_node_t *waiter = bus->waiters; waiter != NULL; waiter = waiter->next) {
        if (!waiter->completed && waiter->match_quorum) {
            if (dcc_event_waiter_mask_has(waiter->type_mask, event->type) &&
                !dcc_event_waiter_mask_has(waiter->set_seen_mask, event->type) &&
                waiter->count_completed < waiter->count_target) {
                dcc_event_snapshot_t snapshot;
                dcc_event_snapshot_fill(&snapshot, event);
                dcc_event_snapshot_copy(&waiter->count_snapshots[waiter->count_completed], &snapshot);
                dcc_event_waiter_mask_add(waiter->set_seen_mask, event->type);
                waiter->count_completed++;
                if (waiter->count_completed >= waiter->count_target) {
                    waiter->completed = 1;
                    (void)llam_channel_try_send(waiter->done, waiter);
                }
            }
            continue;
        }
        if (!waiter->completed && waiter->match_count) {
            if (dcc_event_waiter_mask_has(waiter->type_mask, event->type) &&
                waiter->count_completed < waiter->count_target) {
                dcc_event_snapshot_t snapshot;
                dcc_event_snapshot_fill(&snapshot, event);
                dcc_event_snapshot_copy(&waiter->count_snapshots[waiter->count_completed], &snapshot);
                waiter->count_completed++;
                if (waiter->count_completed >= waiter->count_target) {
                    waiter->completed = 1;
                    (void)llam_channel_try_send(waiter->done, waiter);
                }
            }
            continue;
        }
        if (!waiter->completed && waiter->match_set) {
            if (dcc_event_waiter_mask_has(waiter->type_mask, event->type) &&
                !dcc_event_waiter_mask_has(waiter->set_seen_mask, event->type)) {
                for (size_t i = 0; i < waiter->set_count; ++i) {
                    if (waiter->set_types[i] == event->type) {
                        dcc_event_snapshot_t snapshot;
                        dcc_event_snapshot_fill(&snapshot, event);
                        dcc_event_snapshot_copy(&waiter->set_snapshots[i], &snapshot);
                        dcc_event_waiter_mask_add(waiter->set_seen_mask, event->type);
                        waiter->set_completed++;
                        break;
                    }
                }
                if (waiter->set_completed >= waiter->set_count) {
                    waiter->completed = 1;
                    (void)llam_channel_try_send(waiter->done, waiter);
                }
            }
            continue;
        }
        if (!waiter->completed && waiter->match_sequence) {
            if (waiter->sequence != NULL && waiter->sequence_index < waiter->sequence_count &&
                waiter->sequence[waiter->sequence_index] == event->type) {
                dcc_event_snapshot_t snapshot;
                dcc_event_snapshot_fill(&snapshot, event);
                dcc_event_snapshot_copy(&waiter->sequence_snapshots[waiter->sequence_index], &snapshot);
                waiter->sequence_index++;
                if (waiter->sequence_index >= waiter->sequence_count) {
                    waiter->completed = 1;
                    (void)llam_channel_try_send(waiter->done, waiter);
                }
            }
            continue;
        }
        if (!waiter->completed && dcc_event_waiter_matches(waiter, event->type)) {
            dcc_event_snapshot_fill(&waiter->snapshot, event);
            waiter->owned_status = DCC_OK;
            waiter->owned = dcc_event_waiter_clone_owned(waiter->kind, event, &waiter->owned_status);
            waiter->completed = 1;
            (void)llam_channel_try_send(waiter->done, waiter);
        }
    }
}
