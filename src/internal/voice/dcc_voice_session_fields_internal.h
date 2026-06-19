#ifndef DCC_VOICE_SESSION_FIELDS_INTERNAL_H
#define DCC_VOICE_SESSION_FIELDS_INTERNAL_H

#include "internal/voice/dcc_voice_limits_internal.h"

#include <dcc/dcc.h>

#include <stdint.h>

#define DCC_VOICE_SESSION_STATE_FIELDS \
    dcc_client_t *client; \
    dcc_snowflake_t guild_id; \
    dcc_snowflake_t channel_id; \
    dcc_voice_session_state_t session_state; \
    uint8_t self_mute; \
    uint8_t self_deaf; \
    uint8_t reconnect_requested; \
    uint8_t full_reconnect_requested

#define DCC_VOICE_SESSION_DESCRIPTOR_STATE_FIELDS \
    uint32_t reconnect_attempts; \
    uint32_t heartbeat_interval_ms; \
    int32_t receive_sequence; \
    uint16_t last_close_code; \
    char session_id[DCC_VOICE_SESSION_ID_CAP]; \
    char session_token[DCC_VOICE_SESSION_TOKEN_CAP]; \
    char endpoint[DCC_VOICE_ENDPOINT_CAP]; \
    char last_close_reason[DCC_VOICE_CLOSE_REASON_CAP]

#endif
