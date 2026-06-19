#ifndef DCC_OBJECTS_GATEWAY_VOICE_STATE_H
#define DCC_OBJECTS_GATEWAY_VOICE_STATE_H

/* Voice state payload object. */

#include <dcc/objects/base.h>

typedef struct dcc_voice_state {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t user_id;
    const char *session_id;
    const char *request_to_speak_timestamp;
    uint8_t deaf;
    uint8_t mute;
    uint8_t self_deaf;
    uint8_t self_mute;
    uint8_t self_stream;
    uint8_t self_video;
    uint8_t suppress;
} dcc_voice_state_t;

#endif
