#include "event_wait_convenience_smoke_support.h"

#include "internal/events/dcc_events_internal.h"

#include <stdio.h>
#include <string.h>

void *event_wait_convenience_gateway_ready_wait_thread_main(void *arg) {
    dcc_event_wait_thread_state_t *state = (dcc_event_wait_thread_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_gateway_ready_or_resumed(state->client, 2000, &state->snapshot);
    return NULL;
}

void *event_wait_convenience_gateway_ready_or_close_wait_thread_main(void *arg) {
    dcc_event_wait_thread_state_t *state = (dcc_event_wait_thread_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_gateway_ready_resumed_or_close(state->client, 2000, &state->snapshot);
    return NULL;
}

void *event_wait_convenience_gateway_admission_wait_thread_main(void *arg) {
    dcc_event_wait_thread_state_t *state = (dcc_event_wait_thread_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_until_gateway_ready(state->client, 2000, &state->snapshot);
    return NULL;
}

void *event_wait_convenience_interaction_or_close_wait_thread_main(void *arg) {
    dcc_event_wait_thread_state_t *state = (dcc_event_wait_thread_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_interaction_or_close(state->client, 2000, &state->snapshot);
    return NULL;
}

void *event_wait_convenience_interaction_admission_wait_thread_main(void *arg) {
    dcc_event_wait_thread_state_t *state = (dcc_event_wait_thread_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_until_interaction(state->client, 2000, &state->snapshot);
    return NULL;
}

void *event_wait_convenience_interaction_or_close_owned_wait_thread_main(void *arg) {
    dcc_event_wait_thread_state_t *state = (dcc_event_wait_thread_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->interaction = NULL;
    state->status = dcc_client_wait_for_interaction_or_close_owned(
        state->client,
        2000,
        &state->snapshot,
        &state->interaction
    );
    return NULL;
}

void *event_wait_convenience_interaction_admission_owned_wait_thread_main(void *arg) {
    dcc_event_wait_thread_state_t *state = (dcc_event_wait_thread_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->interaction = NULL;
    state->status = dcc_client_wait_until_interaction_owned(
        state->client,
        2000,
        &state->snapshot,
        &state->interaction
    );
    return NULL;
}

void *event_wait_convenience_client_wait_thread_main(void *arg) {
    dcc_client_wait_thread_state_t *state = (dcc_client_wait_thread_state_t *)arg;
    state->status = dcc_client_wait(state->client);
    return NULL;
}

int event_wait_convenience_dispatch_gateway_ready(dcc_client_t *client) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_READY;
    event.raw.type = DCC_EVENT_READY;
    event.raw.name = "READY";
    event.data.ready.session_id = "";
    return dcc_event_bus_dispatch(&client->events, client, &event) == DCC_OK ? 0 : -1;
}

int event_wait_convenience_dispatch_slash_command(dcc_client_t *client) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_SLASH_COMMAND;
    event.raw.type = DCC_EVENT_INTERACTION_CREATE;
    event.raw.sequence = 42;
    event.raw.name = "INTERACTION_CREATE";
    event.data.interaction.type = 2U;
    event.data.interaction.command_type = 1U;
    event.data.interaction.name = "wait-helper";
    return dcc_event_bus_dispatch(&client->events, client, &event) == DCC_OK ? 0 : -1;
}

int event_wait_convenience_dispatch_socket_close(dcc_client_t *client) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_SOCKET_CLOSE;
    event.raw.type = DCC_EVENT_SOCKET_CLOSE;
    event.raw.sequence = 43;
    event.raw.name = "SOCKET_CLOSE";
    event.data.socket_close.code = 1006U;
    event.data.socket_close.reason = "local-smoke";
    return dcc_event_bus_dispatch(&client->events, client, &event) == DCC_OK ? 0 : -1;
}

int event_wait_convenience_check_invalid_contracts(dcc_client_t *client) {
    dcc_event_wait_policy_t policy;
    dcc_event_snapshot_t snapshot = {
        .size = sizeof(snapshot),
    };
    dcc_event_snapshot_t voice_snapshots[2] = {
        { .size = sizeof(voice_snapshots[0]) },
        { .size = sizeof(voice_snapshots[1]) }
    };
    dcc_interaction_t *wait_interaction = NULL;
    size_t voice_completed = 99U;

    if (dcc_event_wait_policy_init_gateway_ready_or_resumed(NULL, 10) != DCC_ERR_INVALID_ARG ||
        dcc_event_wait_policy_init_gateway_ready_or_resumed(&policy, 10) != DCC_OK ||
        policy.mode != DCC_EVENT_WAIT_POLICY_ANY ||
        policy.target_count != 1U ||
        policy.timeout_ms != 10U ||
        policy.type_count != 2U ||
        policy.types == NULL ||
        policy.types[0] != DCC_EVENT_READY ||
        policy.types[1] != DCC_EVENT_RESUMED ||
        dcc_event_wait_policy_init_voice_session_descriptor(&policy, 20) != DCC_OK ||
        policy.mode != DCC_EVENT_WAIT_POLICY_SET ||
        policy.target_count != 2U ||
        policy.timeout_ms != 20U ||
        policy.type_count != 2U ||
        policy.types == NULL ||
        policy.types[0] != DCC_EVENT_VOICE_STATE_UPDATE ||
        policy.types[1] != DCC_EVENT_VOICE_SERVER_UPDATE ||
        dcc_event_wait_policy_init_gateway_ready_resumed_or_close(&policy, 15) != DCC_OK ||
        policy.mode != DCC_EVENT_WAIT_POLICY_ANY ||
        policy.target_count != 1U ||
        policy.timeout_ms != 15U ||
        policy.type_count != 3U ||
        policy.types == NULL ||
        policy.types[0] != DCC_EVENT_READY ||
        policy.types[1] != DCC_EVENT_RESUMED ||
        policy.types[2] != DCC_EVENT_SOCKET_CLOSE ||
        dcc_event_wait_policy_init_interaction_or_close(&policy, 30) != DCC_OK ||
        policy.mode != DCC_EVENT_WAIT_POLICY_ANY ||
        policy.target_count != 1U ||
        policy.timeout_ms != 30U ||
        policy.type_count != 9U ||
        policy.types == NULL ||
        policy.types[0] != DCC_EVENT_SLASH_COMMAND ||
        policy.types[7] != DCC_EVENT_INTERACTION_CREATE ||
        policy.types[8] != DCC_EVENT_SOCKET_CLOSE ||
        dcc_client_wait_for_gateway_ready_or_resumed(NULL, 1, &snapshot) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_gateway_ready_or_resumed(client, 1, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_gateway_ready_or_resumed(client, 1, &snapshot) != DCC_ERR_STATE ||
        dcc_client_wait_for_gateway_ready_resumed_or_close(NULL, 1, &snapshot) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_gateway_ready_resumed_or_close(client, 1, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_gateway_ready_resumed_or_close(client, 1, &snapshot) != DCC_ERR_STATE ||
        dcc_client_wait_until_gateway_ready(NULL, 1, &snapshot) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_until_gateway_ready(client, 1, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_until_gateway_ready(client, 1, &snapshot) != DCC_ERR_STATE ||
        dcc_client_wait_for_interaction_or_close(NULL, 1, &snapshot) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_interaction_or_close(client, 1, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_interaction_or_close(client, 1, &snapshot) != DCC_ERR_STATE ||
        dcc_client_wait_until_interaction(NULL, 1, &snapshot) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_until_interaction(client, 1, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_until_interaction(client, 1, &snapshot) != DCC_ERR_STATE ||
        dcc_client_wait_for_interaction_or_close_owned(NULL, 1, &snapshot, &wait_interaction) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_interaction_or_close_owned(client, 1, &snapshot, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_interaction_or_close_owned(client, 1, &snapshot, &wait_interaction) !=
            DCC_ERR_STATE ||
        dcc_client_wait_until_interaction_owned(NULL, 1, &snapshot, &wait_interaction) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_until_interaction_owned(client, 1, &snapshot, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_until_interaction_owned(client, 1, &snapshot, &wait_interaction) !=
            DCC_ERR_STATE ||
        wait_interaction != NULL ||
        dcc_client_wait_for_voice_session_descriptor(NULL, 1, voice_snapshots, 2U, &voice_completed) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_voice_session_descriptor(client, 1, voice_snapshots, 1U, &voice_completed) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_voice_session_descriptor(client, 1, voice_snapshots, 2U, &voice_completed) !=
            DCC_ERR_STATE ||
        voice_completed != 0U) {
        dcc_interaction_free(wait_interaction);
        fprintf(stderr, "gateway ready wait helper contract failed\n");
        return 1;
    }

    return 0;
}
