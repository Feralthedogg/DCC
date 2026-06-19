#ifndef DCC_VOICE_NOTIFY_H
#define DCC_VOICE_NOTIFY_H

#include <dcc/voice/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_voice_client_notify_buffer_send(
    dcc_voice_client_t *voice_client,
    uint64_t buffer_size,
    size_t packets_left
);
DCC_API dcc_status_t dcc_voice_client_notify_ready(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t voice_channel_id
);
DCC_API dcc_status_t dcc_voice_client_notify_receive(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id,
    const uint8_t *audio,
    size_t audio_size
);
DCC_API dcc_status_t dcc_voice_client_notify_receive_combined(
    dcc_voice_client_t *voice_client,
    const uint8_t *audio,
    size_t audio_size
);
DCC_API dcc_status_t dcc_voice_client_notify_track_marker(
    dcc_voice_client_t *voice_client,
    const char *track_meta
);
DCC_API dcc_status_t dcc_voice_client_notify_speaking(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id,
    uint32_t ssrc
);
DCC_API dcc_status_t dcc_voice_client_notify_disconnect(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id
);
DCC_API dcc_status_t dcc_voice_client_notify_platform(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id,
    dcc_voice_client_platform_t platform
);

#ifdef __cplusplus
}
#endif

#endif
