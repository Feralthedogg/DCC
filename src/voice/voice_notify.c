#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

dcc_status_t dcc_voice_client_notify_buffer_send(
    dcc_voice_client_t *voice_client,
    uint64_t buffer_size,
    size_t packets_left
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_VOICE_BUFFER_SEND;
    event.data.voice_buffer_send.voice_client = voice_client;
    event.data.voice_buffer_send.buffer_size = buffer_size;
    event.data.voice_buffer_send.packets_left = packets_left;
    return dcc_voice_dispatch(voice_client, &event);
}

dcc_status_t dcc_voice_client_notify_ready(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t voice_channel_id
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_voice_client_mark_session_active(voice_client);
    if (status != DCC_OK) {
        return status;
    }

    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_VOICE_READY;
    event.data.voice_ready.voice_client = voice_client;
    event.data.voice_ready.voice_channel_id = voice_channel_id;
    return dcc_voice_dispatch(voice_client, &event);
}
