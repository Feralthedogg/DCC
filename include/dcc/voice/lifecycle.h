#ifndef DCC_VOICE_LIFECYCLE_H
#define DCC_VOICE_LIFECYCLE_H

#include <dcc/voice/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API uint8_t dcc_voice_opus_is_available(void);
DCC_API dcc_status_t dcc_voice_client_create(dcc_client_t *client, dcc_voice_client_t **out);
DCC_API void dcc_voice_client_destroy(dcc_voice_client_t *voice_client);
DCC_API dcc_client_t *dcc_voice_client_owner(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_set_channel_id(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t channel_id
);
DCC_API dcc_snowflake_t dcc_voice_client_channel_id(const dcc_voice_client_t *voice_client);
DCC_API uint32_t dcc_voice_client_ssrc(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_map_ssrc(
    dcc_voice_client_t *voice_client,
    uint32_t ssrc,
    dcc_snowflake_t user_id
);
DCC_API dcc_status_t dcc_voice_client_unmap_ssrc(dcc_voice_client_t *voice_client, uint32_t ssrc);
DCC_API dcc_snowflake_t dcc_voice_client_user_for_ssrc(
    const dcc_voice_client_t *voice_client,
    uint32_t ssrc
);
DCC_API dcc_status_t dcc_voice_client_handle_gateway_frame(
    dcc_voice_client_t *voice_client,
    const char *json,
    size_t json_len
);
DCC_API dcc_status_t dcc_voice_client_start_session(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t channel_id,
    uint8_t self_mute,
    uint8_t self_deaf,
    uint8_t enable_dave
);
DCC_API dcc_status_t dcc_voice_client_connect(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t channel_id,
    uint8_t self_mute,
    uint8_t self_deaf,
    uint8_t enable_dave
);
DCC_API dcc_status_t dcc_voice_client_leave(dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_apply_voice_state(
    dcc_voice_client_t *voice_client,
    const dcc_voice_state_t *voice_state,
    dcc_snowflake_t self_user_id
);
DCC_API dcc_status_t dcc_voice_client_apply_voice_server_update(
    dcc_voice_client_t *voice_client,
    const dcc_voice_server_update_t *voice_server
);
DCC_API dcc_status_t dcc_voice_client_session_info(
    const dcc_voice_client_t *voice_client,
    dcc_voice_session_info_t *out
);
DCC_API dcc_status_t dcc_voice_client_stats(
    const dcc_voice_client_t *voice_client,
    dcc_voice_client_stats_t *out
);
DCC_API dcc_status_t dcc_voice_client_health_snapshot(
    const dcc_voice_client_t *voice_client,
    dcc_voice_health_snapshot_t *out
);
DCC_API dcc_status_t dcc_voice_client_wait_until_ready(
    dcc_voice_client_t *voice_client,
    uint32_t timeout_ms,
    dcc_voice_health_snapshot_t *out
);
DCC_API dcc_status_t dcc_voice_client_wait_until_active(
    dcc_voice_client_t *voice_client,
    uint32_t timeout_ms,
    dcc_voice_health_snapshot_t *out
);
DCC_API const char *dcc_voice_health_string(dcc_voice_health_t health);
DCC_API const char *dcc_voice_session_state_string(dcc_voice_session_state_t state);
DCC_API dcc_status_t dcc_voice_client_health_snapshot_json(
    const dcc_voice_health_snapshot_t *snapshot,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_client_health_snapshot_prometheus(
    const dcc_voice_health_snapshot_t *snapshot,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API dcc_voice_session_state_t dcc_voice_client_session_state(const dcc_voice_client_t *voice_client);
DCC_API uint8_t dcc_voice_client_session_ready(const dcc_voice_client_t *voice_client);
DCC_API uint8_t dcc_voice_client_session_active(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_mark_session_active(dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_disconnect_session(dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_handle_disconnect(
    dcc_voice_client_t *voice_client,
    uint16_t close_code,
    const char *reason
);
DCC_API dcc_voice_reconnect_action_t dcc_voice_reconnect_action_for_close_code(uint16_t close_code);
DCC_API dcc_voice_reconnect_action_t dcc_voice_client_reconnect_action(
    const dcc_voice_client_t *voice_client
);
DCC_API const char *dcc_voice_reconnect_action_string(dcc_voice_reconnect_action_t action);
DCC_API uint8_t dcc_voice_client_reconnect_requested(const dcc_voice_client_t *voice_client);
DCC_API uint8_t dcc_voice_client_full_reconnect_requested(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_clear_reconnect_request(dcc_voice_client_t *voice_client);

#ifdef __cplusplus
}
#endif

#endif
