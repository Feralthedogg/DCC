#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

dcc_status_t dcc_voice_client_notify_receive(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id,
    const uint8_t *audio,
    size_t audio_size
) {
    if (voice_client == NULL || (audio == NULL && audio_size != 0)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_VOICE_RECEIVE;
    event.data.voice_receive.voice_client = voice_client;
    event.data.voice_receive.user_id = user_id;
    event.data.voice_receive.audio = audio;
    event.data.voice_receive.audio_size = audio_size;
    return dcc_voice_dispatch(voice_client, &event);
}

dcc_status_t dcc_voice_client_notify_receive_combined(
    dcc_voice_client_t *voice_client,
    const uint8_t *audio,
    size_t audio_size
) {
    if (voice_client == NULL || (audio == NULL && audio_size != 0)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_VOICE_RECEIVE_COMBINED;
    event.data.voice_receive.voice_client = voice_client;
    event.data.voice_receive.audio = audio;
    event.data.voice_receive.audio_size = audio_size;
    return dcc_voice_dispatch(voice_client, &event);
}

dcc_status_t dcc_voice_client_notify_track_marker(
    dcc_voice_client_t *voice_client,
    const char *track_meta
) {
    if (voice_client == NULL || track_meta == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_VOICE_TRACK_MARKER;
    event.data.voice_track_marker.voice_client = voice_client;
    event.data.voice_track_marker.track_meta = track_meta;
    return dcc_voice_dispatch(voice_client, &event);
}
