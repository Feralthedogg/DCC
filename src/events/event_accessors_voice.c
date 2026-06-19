#include "internal/events/dcc_event_state_internal.h"

const dcc_voice_channel_effect_t *dcc_event_voice_channel_effect(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_VOICE_CHANNEL_EFFECT_SEND
        ? &event->data.voice_channel_effect
        : NULL;
}

const dcc_voice_server_update_t *dcc_event_voice_server_update(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_VOICE_SERVER_UPDATE
        ? &event->data.voice_server_update
        : NULL;
}

const dcc_voice_channel_start_time_update_t *dcc_event_voice_channel_start_time_update(
    const dcc_event_t *event
) {
    return event != NULL && event->type == DCC_EVENT_VOICE_CHANNEL_START_TIME_UPDATE
        ? &event->data.voice_channel_start_time_update
        : NULL;
}

const dcc_voice_channel_status_update_t *dcc_event_voice_channel_status_update(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_VOICE_CHANNEL_STATUS_UPDATE
        ? &event->data.voice_channel_status_update
        : NULL;
}

const dcc_voice_buffer_send_event_t *dcc_event_voice_buffer_send(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_VOICE_BUFFER_SEND ? &event->data.voice_buffer_send : NULL;
}

const dcc_voice_ready_event_t *dcc_event_voice_ready(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_VOICE_READY ? &event->data.voice_ready : NULL;
}

const dcc_voice_receive_event_t *dcc_event_voice_receive(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_VOICE_RECEIVE:
        case DCC_EVENT_VOICE_RECEIVE_COMBINED:
            return &event->data.voice_receive;
        default:
            return NULL;
    }
}

const dcc_voice_track_marker_event_t *dcc_event_voice_track_marker(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_VOICE_TRACK_MARKER
        ? &event->data.voice_track_marker
        : NULL;
}

const dcc_voice_client_speaking_event_t *dcc_event_voice_client_speaking(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_VOICE_CLIENT_SPEAKING
        ? &event->data.voice_client_speaking
        : NULL;
}

const dcc_voice_client_disconnect_event_t *dcc_event_voice_client_disconnect(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_VOICE_CLIENT_DISCONNECT
        ? &event->data.voice_client_disconnect
        : NULL;
}

const dcc_voice_client_platform_event_t *dcc_event_voice_client_platform(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_VOICE_CLIENT_PLATFORM
        ? &event->data.voice_client_platform
        : NULL;
}

const dcc_voice_state_t *dcc_event_voice_state(const dcc_event_t *event) {
    return event != NULL && event->type == DCC_EVENT_VOICE_STATE_UPDATE ? &event->data.voice_state : NULL;
}
