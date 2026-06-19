#include "internal/events/dcc_event_waiter_policy_internal.h"

#include <errno.h>

static size_t dcc_event_waiter_policy_completed_count(
    const dcc_event_waiter_node_t *waiter,
    dcc_event_waiter_policy_count_t count_mode
) {
    switch (count_mode) {
        case DCC_EVENT_WAITER_POLICY_COUNT_SEQUENCE:
            return waiter->sequence_index;
        case DCC_EVENT_WAITER_POLICY_COUNT_SET:
            return waiter->set_completed;
        case DCC_EVENT_WAITER_POLICY_COUNT_MATCHES:
        default:
            return waiter->count_completed;
    }
}

dcc_status_t dcc_event_waiter_policy_wait(
    dcc_client_t *client,
    dcc_event_waiter_node_t *waiter,
    uint32_t timeout_ms,
    dcc_event_waiter_policy_count_t count_mode,
    size_t *out_completed
) {
    if (!client->runtime.initialized) {
        return DCC_ERR_STATE;
    }

    llam_channel_t *done = llam_channel_create(1);
    if (done == NULL) {
        return dcc_event_wait_status_from_errno(errno);
    }

    waiter->kind = DCC_EVENT_WAIT_SNAPSHOT;
    waiter->done = done;
    waiter->snapshot.size = sizeof(waiter->snapshot);
    waiter->owned_status = DCC_OK;

    dcc_event_bus_lock(&client->events);
    waiter->next = client->events.waiters;
    client->events.waiters = waiter;
    dcc_event_bus_unlock(&client->events);

    dcc_status_t status = dcc_event_waiter_wait_channel(waiter, timeout_ms);

    dcc_event_bus_lock(&client->events);
    dcc_event_waiter_remove_locked(&client->events, waiter);
    uint8_t completed = waiter->completed;
    size_t completed_count = dcc_event_waiter_policy_completed_count(waiter, count_mode);
    dcc_event_bus_unlock(&client->events);

    dcc_event_waiter_drain_channel(done);
    (void)llam_channel_close(done);
    (void)llam_channel_destroy(done);

    if (completed && status == DCC_ERR_TIMEOUT) {
        status = DCC_OK;
    }
    if (out_completed != NULL) {
        *out_completed = completed_count;
    }
    return status;
}
