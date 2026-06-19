#ifndef DCC_GATEWAY_VOICE_STATE_UPDATE_INTERNAL_H
#define DCC_GATEWAY_VOICE_STATE_UPDATE_INTERNAL_H

#include <dcc/dcc.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_gateway_send_voice_state_update(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t channel_id,
    uint8_t self_mute,
    uint8_t self_deaf
);

#ifdef __cplusplus
}
#endif

#endif
