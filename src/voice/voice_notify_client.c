#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

dcc_status_t dcc_voice_client_notify_speaking(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id,
    uint32_t ssrc
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_VOICE_CLIENT_SPEAKING;
    event.data.voice_client_speaking.voice_client = voice_client;
    event.data.voice_client_speaking.user_id = user_id;
    event.data.voice_client_speaking.ssrc = ssrc;
    return dcc_voice_dispatch(voice_client, &event);
}

dcc_status_t dcc_voice_client_notify_disconnect(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_VOICE_CLIENT_DISCONNECT;
    event.data.voice_client_disconnect.voice_client = voice_client;
    event.data.voice_client_disconnect.user_id = user_id;
    return dcc_voice_dispatch(voice_client, &event);
}

dcc_status_t dcc_voice_client_notify_platform(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id,
    dcc_voice_client_platform_t platform
) {
    if (voice_client == NULL ||
        (platform != DCC_VOICE_CLIENT_PLATFORM_DESKTOP && platform != DCC_VOICE_CLIENT_PLATFORM_MOBILE)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_VOICE_CLIENT_PLATFORM;
    event.data.voice_client_platform.voice_client = voice_client;
    event.data.voice_client_platform.user_id = user_id;
    event.data.voice_client_platform.platform = (uint8_t)platform;
    return dcc_voice_dispatch(voice_client, &event);
}
