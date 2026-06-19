#ifndef DCC_REST_TYPES_VOICE_STATES_H
#define DCC_REST_TYPES_VOICE_STATES_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_voice_state_params {
    size_t size;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t user_id;
    dcc_snowflake_t channel_id;
    uint8_t suppress;
    const char *request_to_speak_timestamp;
} dcc_voice_state_params_t;

#ifdef __cplusplus
}
#endif

#endif
