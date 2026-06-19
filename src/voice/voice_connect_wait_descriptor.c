#include "internal/voice/dcc_voice_connect_wait_internal.h"

#include <errno.h>
#include <string.h>

static const dcc_event_type_t dcc_voice_connect_wait_descriptor_types[] = {
    DCC_EVENT_VOICE_STATE_UPDATE,
    DCC_EVENT_VOICE_SERVER_UPDATE
};

dcc_status_t dcc_voice_connect_wait_descriptor(
    dcc_voice_client_t *voice_client,
    dcc_client_t *client,
    const dcc_voice_connect_wait_options_t *options,
    dcc_event_snapshot_t snapshots[2],
    size_t *out_completed,
    dcc_status_t *out_connect_status
) {
    dcc_event_waiter_node_t waiter;
    memset(&waiter, 0, sizeof(waiter));
    dcc_status_t status = dcc_event_waiter_set_event_set(
        &waiter,
        dcc_voice_connect_wait_descriptor_types,
        sizeof(dcc_voice_connect_wait_descriptor_types) /
            sizeof(dcc_voice_connect_wait_descriptor_types[0]),
        snapshots,
        2U
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!client->runtime.initialized) {
        return DCC_ERR_STATE;
    }

    llam_channel_t *done = llam_channel_create(1);
    if (done == NULL) {
        return dcc_event_wait_status_from_errno(errno);
    }

    waiter.kind = DCC_EVENT_WAIT_SNAPSHOT;
    waiter.done = done;
    waiter.snapshot.size = sizeof(waiter.snapshot);
    waiter.owned_status = DCC_OK;

    dcc_event_bus_lock(&client->events);
    waiter.next = client->events.waiters;
    client->events.waiters = &waiter;
    dcc_event_bus_unlock(&client->events);

    status = dcc_voice_client_connect(
        voice_client,
        options->guild_id,
        options->channel_id,
        options->self_mute,
        options->self_deaf,
        options->enable_dave
    );
    if (out_connect_status != NULL) {
        *out_connect_status = status;
    }
    if (status == DCC_OK) {
        status = dcc_event_waiter_wait_channel(&waiter, options->descriptor_timeout_ms);
    }

    dcc_event_bus_lock(&client->events);
    dcc_event_waiter_remove_locked(&client->events, &waiter);
    uint8_t completed = waiter.completed;
    size_t completed_count = waiter.set_completed;
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
