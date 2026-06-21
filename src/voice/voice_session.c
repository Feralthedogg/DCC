#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_start_session(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t channel_id,
    uint8_t self_mute,
    uint8_t self_deaf,
    uint8_t enable_dave
) {
    if (voice_client == NULL || guild_id == 0 || channel_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    voice_client->guild_id = guild_id;
    voice_client->channel_id = channel_id;
    voice_client->self_mute = self_mute ? 1U : 0U;
    voice_client->self_deaf = self_deaf ? 1U : 0U;
    voice_client->dave_enabled = enable_dave ? 1U : 0U;
    voice_client->dave_version = enable_dave ? DCC_VOICE_DAVE_VERSION_1 : DCC_VOICE_DAVE_NONE;
    voice_client->dave_pending_version = DCC_VOICE_DAVE_NONE;
    voice_client->dave_transition_pending = 0;
    voice_client->dave_transition_ready = 0;
    voice_client->dave_transition_id = 0;
    voice_client->reconnect_requested = 0;
    voice_client->full_reconnect_requested = 0;
    voice_client->last_close_code = 0;
    voice_client->last_close_reason[0] = '\0';
    voice_client->heartbeat_interval_ms = 0;
    voice_client->receive_sequence = -1;
    dcc_voice_clear_session_descriptor(voice_client);
    dcc_voice_reset_transport_state(voice_client);
    voice_client->session_state = DCC_VOICE_SESSION_REQUESTED;
    return DCC_OK;
}

dcc_status_t dcc_voice_client_connect(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t channel_id,
    uint8_t self_mute,
    uint8_t self_deaf,
    uint8_t enable_dave
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (guild_id == 0U &&
        channel_id != 0U &&
        voice_client->client != NULL &&
        voice_client->client->infer_guild_id_from_channel != 0U) {
        dcc_status_t infer_status =
            dcc_client_infer_guild_id_from_channel(voice_client->client, channel_id, &guild_id);
        if (infer_status != DCC_OK) {
            dcc_set_error(voice_client->client, "could not infer guild id from voice channel id");
            return infer_status;
        }
    }

    dcc_status_t status =
        dcc_voice_client_start_session(voice_client, guild_id, channel_id, self_mute, self_deaf, enable_dave);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_gateway_send_voice_state_update(
        voice_client->client,
        guild_id,
        channel_id,
        voice_client->self_mute,
        voice_client->self_deaf
    );
    if (status != DCC_OK) {
        (void)dcc_voice_client_disconnect_session(voice_client);
    }
    return status;
}

dcc_status_t dcc_voice_client_leave(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_client->guild_id == 0) {
        return DCC_ERR_STATE;
    }

    dcc_status_t status = dcc_gateway_send_voice_state_update(
        voice_client->client,
        voice_client->guild_id,
        0,
        voice_client->self_mute,
        voice_client->self_deaf
    );
    if (status != DCC_OK) {
        return status;
    }
    return dcc_voice_client_disconnect_session(voice_client);
}
