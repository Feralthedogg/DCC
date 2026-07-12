#include "voice_smoke_support.h"

#include <stdio.h>
#include <string.h>

int voice_session_state_smoke(dcc_client_t *client, dcc_voice_client_t *voice_client) {
    if (dcc_voice_client_owner(voice_client) != client) {
        fprintf(stderr, "dcc_voice_client_owner mismatch\n");
        return 1;
    }
    if (dcc_voice_client_set_channel_id(voice_client, 222) != DCC_OK ||
        dcc_voice_client_channel_id(voice_client) != 222 ||
        dcc_voice_client_ssrc(voice_client) != 0 ||
        dcc_voice_client_channel_id(NULL) != 0 ||
        dcc_voice_client_ssrc(NULL) != 0) {
        fprintf(stderr, "voice client state helpers failed\n");
        return 1;
    }
    if (dcc_voice_reconnect_action_for_close_code(0) != DCC_VOICE_RECONNECT_ACTION_NONE ||
        dcc_voice_reconnect_action_for_close_code(1000) != DCC_VOICE_RECONNECT_ACTION_NONE ||
        dcc_voice_reconnect_action_for_close_code(1006) != DCC_VOICE_RECONNECT_ACTION_RESUME ||
        dcc_voice_reconnect_action_for_close_code(4015) != DCC_VOICE_RECONNECT_ACTION_RESUME ||
        dcc_voice_reconnect_action_for_close_code(4014) != DCC_VOICE_RECONNECT_ACTION_TERMINATE ||
        strcmp(dcc_voice_reconnect_action_string(DCC_VOICE_RECONNECT_ACTION_NONE), "none") != 0 ||
        strcmp(dcc_voice_reconnect_action_string(DCC_VOICE_RECONNECT_ACTION_RESUME), "resume") != 0 ||
        strcmp(dcc_voice_reconnect_action_string(DCC_VOICE_RECONNECT_ACTION_FULL_RECONNECT), "full_reconnect") != 0 ||
        strcmp(dcc_voice_reconnect_action_string(DCC_VOICE_RECONNECT_ACTION_TERMINATE), "terminate") != 0 ||
        strcmp(dcc_voice_reconnect_action_string((dcc_voice_reconnect_action_t)99), "unknown") != 0 ||
        dcc_voice_client_reconnect_action(NULL) != DCC_VOICE_RECONNECT_ACTION_NONE ||
        dcc_voice_client_reconnect_action(voice_client) != DCC_VOICE_RECONNECT_ACTION_NONE) {
        fprintf(stderr, "voice reconnect action helpers failed\n");
        return 1;
    }

    dcc_voice_health_snapshot_t voice_health = {
        .size = sizeof(voice_health),
    };
    dcc_voice_health_snapshot_t voice_bad_health = {0};
    char voice_health_json[1024];
    size_t voice_health_json_len = 0;
    if (dcc_voice_client_health_snapshot(NULL, &voice_health) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_health_snapshot(voice_client, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_health_snapshot(voice_client, &voice_bad_health) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_health_snapshot(voice_client, &voice_health) != DCC_OK ||
        voice_health.health != DCC_VOICE_HEALTH_DISCONNECTED ||
        voice_health.action != DCC_VOICE_RECONNECT_ACTION_NONE ||
        voice_health.ok != 0U ||
        voice_health.media_ready != 0U ||
        voice_health.session.state != DCC_VOICE_SESSION_DISCONNECTED ||
        strcmp(dcc_voice_health_string(DCC_VOICE_HEALTH_DISCONNECTED), "disconnected") != 0 ||
        strcmp(dcc_voice_health_string((dcc_voice_health_t)99), "unknown") != 0 ||
        strcmp(dcc_voice_session_state_string(DCC_VOICE_SESSION_ACTIVE), "active") != 0 ||
        strcmp(dcc_voice_session_state_string((dcc_voice_session_state_t)99), "unknown") != 0 ||
        dcc_voice_client_health_snapshot_json(NULL, voice_health_json, sizeof(voice_health_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_health_snapshot_json(&voice_bad_health, voice_health_json, sizeof(voice_health_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_health_snapshot_json(&voice_health, NULL, sizeof(voice_health_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_health_snapshot_json(&voice_health, voice_health_json, sizeof(voice_health_json), &voice_health_json_len) !=
            DCC_OK ||
        voice_health_json_len == 0U ||
        strstr(voice_health_json, "\"health\":\"disconnected\"") == NULL ||
        strstr(voice_health_json, "\"state\":\"disconnected\"") == NULL) {
        fprintf(stderr, "voice health disconnected snapshot failed: %s\n", voice_health_json);
        return 1;
    }
    _Alignas(dcc_voice_health_snapshot_t)
        unsigned char legacy_storage[offsetof(dcc_voice_health_snapshot_t, dave)];
    memset(legacy_storage, 0, sizeof(legacy_storage));
    dcc_voice_health_snapshot_t *legacy = (dcc_voice_health_snapshot_t *)(void *)legacy_storage;
    legacy->size = sizeof(legacy_storage);
    if (dcc_voice_client_health_snapshot(voice_client, legacy) != DCC_OK ||
        dcc_voice_client_health_snapshot_json(
            legacy,
            voice_health_json,
            sizeof(voice_health_json),
            &voice_health_json_len
        ) != DCC_OK ||
        strstr(voice_health_json, "\"health\":\"disconnected\"") == NULL) {
        fprintf(stderr, "voice legacy health ABI failed\n");
        return 1;
    }

    voice_cancel_state_t cancel_state = {0};
    dcc_listener_id_t cancel_first_id = 0;
    dcc_listener_id_t cancel_second_id = 0;
    if (dcc_client_on(client, DCC_EVENT_VOICE_READY, voice_cancel_first, &cancel_state, &cancel_first_id) != DCC_OK ||
        dcc_client_on(client, DCC_EVENT_VOICE_READY, voice_cancel_second, &cancel_state, &cancel_second_id) != DCC_OK ||
        dcc_voice_client_notify_ready(voice_client, 999) != DCC_OK ||
        !cancel_state.first_seen ||
        !cancel_state.first_cancelled ||
        cancel_state.second_seen ||
        dcc_client_off(client, DCC_EVENT_VOICE_READY, cancel_first_id) != DCC_OK ||
        dcc_client_off(client, DCC_EVENT_VOICE_READY, cancel_second_id) != DCC_OK ||
        dcc_event_shard_id(NULL) != 0 ||
        dcc_event_is_cancelled(NULL) != 0) {
        fprintf(stderr, "voice event cancellation failed\n");
        return 1;
    }
    dcc_event_cancel(NULL);

    dcc_voice_session_info_t session_info = {
        .size = sizeof(session_info),
    };
    dcc_voice_client_stats_t voice_stats = {
        .size = sizeof(voice_stats),
    };
    dcc_voice_state_t self_voice_state = {
        .guild_id = 333,
        .channel_id = 222,
        .user_id = 999,
        .session_id = "voice-session",
    };
    dcc_voice_server_update_t self_voice_server = {
        .guild_id = 333,
        .token = "voice-token",
        .endpoint = "voice.example.com",
    };
    char voice_payload[1024];
    size_t voice_payload_len = 0;
    dcc_voice_state_t moved_voice_state = {
        .guild_id = 333,
        .channel_id = 555,
        .user_id = 999,
        .session_id = "voice-session-moved",
    };
    voice_dave_mls_state_t external_dave = {0};
    if (dcc_voice_client_set_dave_mls_handler(
            voice_client,
            voice_dave_mls_handler,
            &external_dave
        ) != DCC_OK ||
        dcc_voice_client_start_session(voice_client, 333, 222, 1, 0, 1) != DCC_OK ||
        dcc_voice_client_session_state(voice_client) != DCC_VOICE_SESSION_REQUESTED ||
        !dcc_voice_client_dave_enabled(voice_client) ||
        dcc_voice_client_dave_version(voice_client) != DCC_VOICE_DAVE_VERSION_1 ||
        dcc_voice_client_apply_voice_server_update(voice_client, &self_voice_server) != DCC_OK ||
        dcc_voice_client_session_ready(voice_client) ||
        dcc_voice_client_apply_voice_state(voice_client, &self_voice_state, 999) != DCC_OK ||
        !dcc_voice_client_session_ready(voice_client) ||
        dcc_voice_client_session_state(voice_client) != DCC_VOICE_SESSION_READY ||
        dcc_voice_client_session_info(voice_client, &session_info) != DCC_OK ||
        session_info.guild_id != 333 ||
        session_info.channel_id != 222 ||
        strcmp(session_info.session_id, "voice-session") != 0 ||
        strcmp(session_info.token, "voice-token") != 0 ||
        strcmp(session_info.endpoint, "voice.example.com") != 0 ||
        !session_info.ready ||
        session_info.active ||
        dcc_voice_client_build_identify_payload(
            voice_client,
            999,
            voice_payload,
            sizeof(voice_payload),
            &voice_payload_len
        ) != DCC_OK ||
        voice_payload_len != strlen(voice_payload) ||
        strcmp(
            voice_payload,
            "{\"op\":0,\"d\":{\"user_id\":\"999\",\"server_id\":\"333\","
            "\"session_id\":\"voice-session\",\"token\":\"voice-token\","
            "\"max_dave_protocol_version\":1}}"
        ) != 0 ||
        dcc_voice_client_receive_sequence(voice_client) != -1 ||
        dcc_voice_client_set_receive_sequence(voice_client, 123) != DCC_OK ||
        dcc_voice_client_build_resume_payload(
            voice_client,
            voice_payload,
            sizeof(voice_payload),
            &voice_payload_len
        ) != DCC_OK ||
        voice_payload_len != strlen(voice_payload) ||
        strcmp(
            voice_payload,
            "{\"op\":7,\"d\":{\"server_id\":\"333\",\"session_id\":\"voice-session\","
            "\"token\":\"voice-token\",\"seq_ack\":123}}"
        ) != 0 ||
        dcc_voice_client_build_heartbeat_payload(
            voice_client,
            77,
            voice_payload,
            sizeof(voice_payload),
            &voice_payload_len
        ) != DCC_OK ||
        voice_payload_len != strlen(voice_payload) ||
        strcmp(voice_payload, "{\"op\":3,\"d\":{\"t\":77,\"seq_ack\":123}}") != 0 ||
        dcc_voice_client_build_select_protocol_payload(
            "198.51.100.10",
            50000,
            DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE,
            voice_payload,
            sizeof(voice_payload),
            &voice_payload_len
        ) != DCC_OK ||
        voice_payload_len != strlen(voice_payload) ||
        strcmp(
            voice_payload,
            "{\"op\":1,\"d\":{\"protocol\":\"udp\",\"data\":{\"address\":\"198.51.100.10\","
            "\"port\":50000,\"mode\":\"aead_xchacha20_poly1305_rtpsize\"}}}"
        ) != 0 ||
        dcc_voice_client_build_websocket_url(
            voice_client,
            voice_payload,
            sizeof(voice_payload),
            &voice_payload_len
        ) != DCC_OK ||
        voice_payload_len != strlen(voice_payload) ||
        strcmp(voice_payload, "wss://voice.example.com/?v=8") != 0 ||
        dcc_voice_client_mark_session_active(voice_client) != DCC_OK ||
        !dcc_voice_client_session_active(voice_client) ||
        dcc_voice_client_handle_disconnect(voice_client, 1006, "network reset") != DCC_OK ||
        !dcc_voice_client_reconnect_requested(voice_client) ||
        dcc_voice_client_full_reconnect_requested(voice_client) ||
        dcc_voice_client_reconnect_action(voice_client) != DCC_VOICE_RECONNECT_ACTION_RESUME ||
        strcmp(
            dcc_voice_reconnect_action_string(dcc_voice_client_reconnect_action(voice_client)),
            "resume"
        ) != 0 ||
        dcc_voice_client_clear_reconnect_request(voice_client) != DCC_OK ||
        dcc_voice_client_reconnect_requested(voice_client) ||
        dcc_voice_client_reconnect_action(voice_client) != DCC_VOICE_RECONNECT_ACTION_NONE ||
        dcc_voice_client_session_state(voice_client) != DCC_VOICE_SESSION_READY) {
        fprintf(stderr, "voice session readiness/reconnect state failed\n");
        return 1;
    }
    for (size_t i = 0; i < 4; ++i) {
        if (dcc_voice_client_handle_disconnect(voice_client, 1006, "loop") != DCC_OK) {
            fprintf(stderr, "voice local reconnect loop failed\n");
            return 1;
        }
    }
    session_info.size = sizeof(session_info);
    voice_stats.size = sizeof(voice_stats);
    if (!dcc_voice_client_full_reconnect_requested(voice_client) ||
        dcc_voice_client_reconnect_requested(voice_client) ||
        dcc_voice_client_reconnect_action(voice_client) != DCC_VOICE_RECONNECT_ACTION_FULL_RECONNECT ||
        dcc_voice_client_session_state(voice_client) != DCC_VOICE_SESSION_REQUESTED ||
        dcc_voice_client_session_info(voice_client, &session_info) != DCC_OK ||
        session_info.ready ||
        session_info.reconnect_attempts != 5 ||
        session_info.last_close_code != 1006 ||
        strcmp(session_info.last_close_reason, "loop") != 0 ||
        dcc_voice_client_stats(voice_client, &voice_stats) != DCC_OK ||
        voice_stats.state != DCC_VOICE_SESSION_REQUESTED ||
        voice_stats.ready ||
        voice_stats.reconnect_requested ||
        !voice_stats.full_reconnect_requested ||
        voice_stats.reconnect_attempts != 5 ||
        voice_stats.last_close_code != 1006 ||
        voice_stats.websocket_loop_running ||
        voice_stats.websocket_loop_status != DCC_OK) {
        fprintf(stderr, "voice full reconnect escalation failed\n");
        return 1;
    }
    memset(&voice_health, 0, sizeof(voice_health));
    voice_health.size = sizeof(voice_health);
    if (dcc_voice_client_health_snapshot(voice_client, &voice_health) != DCC_OK ||
        voice_health.health != DCC_VOICE_HEALTH_RECONNECTING ||
        voice_health.action != DCC_VOICE_RECONNECT_ACTION_FULL_RECONNECT ||
        voice_health.ok != 0U ||
        voice_health.session.reconnect_attempts != 5 ||
        voice_health.session.last_close_code != 1006 ||
        dcc_voice_client_health_snapshot_json(&voice_health, voice_health_json, sizeof(voice_health_json), NULL) !=
            DCC_OK ||
        strstr(voice_health_json, "\"health\":\"reconnecting\"") == NULL ||
        strstr(voice_health_json, "\"action\":\"full_reconnect\"") == NULL) {
        fprintf(stderr, "voice health full reconnect snapshot failed: %s\n", voice_health_json);
        return 1;
    }
    if (dcc_voice_client_start_session(voice_client, 333, 222, 0, 0, 1) != DCC_OK ||
        dcc_voice_client_apply_voice_server_update(voice_client, &self_voice_server) != DCC_OK ||
        dcc_voice_client_apply_voice_state(voice_client, &moved_voice_state, 999) != DCC_OK ||
        dcc_voice_client_session_ready(voice_client) ||
        dcc_voice_client_channel_id(voice_client) != 555 ||
        !dcc_voice_client_dave_enabled(voice_client) ||
        dcc_voice_client_apply_voice_server_update(voice_client, &self_voice_server) != DCC_OK ||
        !dcc_voice_client_session_ready(voice_client)) {
        fprintf(stderr, "voice move session state failed\n");
        return 1;
    }
    session_info.size = sizeof(session_info);
    if (dcc_voice_client_mark_session_active(voice_client) != DCC_OK ||
        dcc_voice_client_handle_disconnect(voice_client, 4015, "server crashed") != DCC_OK ||
        !dcc_voice_client_reconnect_requested(voice_client) ||
        dcc_voice_client_full_reconnect_requested(voice_client) ||
        dcc_voice_client_reconnect_action(voice_client) != DCC_VOICE_RECONNECT_ACTION_RESUME ||
        dcc_voice_client_session_state(voice_client) != DCC_VOICE_SESSION_RECONNECTING ||
        dcc_voice_client_session_info(voice_client, &session_info) != DCC_OK ||
        session_info.last_close_code != 4015 ||
        strcmp(session_info.last_close_reason, "server crashed") != 0 ||
        dcc_voice_client_clear_reconnect_request(voice_client) != DCC_OK ||
        dcc_voice_client_reconnect_requested(voice_client) ||
        dcc_voice_client_reconnect_action(voice_client) != DCC_VOICE_RECONNECT_ACTION_NONE ||
        dcc_voice_client_session_state(voice_client) != DCC_VOICE_SESSION_READY) {
        fprintf(stderr, "voice server crash reconnect state failed\n");
        return 1;
    }
    session_info.size = sizeof(session_info);
    if (dcc_voice_client_handle_disconnect(voice_client, 4014, "disconnected") != DCC_OK ||
        dcc_voice_client_session_state(voice_client) != DCC_VOICE_SESSION_TERMINATED ||
        dcc_voice_client_reconnect_requested(voice_client) ||
        dcc_voice_client_full_reconnect_requested(voice_client) ||
        dcc_voice_client_reconnect_action(voice_client) != DCC_VOICE_RECONNECT_ACTION_TERMINATE ||
        dcc_voice_client_session_info(voice_client, &session_info) != DCC_OK ||
        session_info.last_close_code != 4014 ||
        strcmp(session_info.last_close_reason, "disconnected") != 0 ||
        dcc_voice_client_handle_disconnect(voice_client, 4004, "auth failed") != DCC_OK ||
        dcc_voice_client_session_state(voice_client) != DCC_VOICE_SESSION_TERMINATED ||
        dcc_voice_client_reconnect_requested(voice_client) ||
        dcc_voice_client_full_reconnect_requested(voice_client) ||
        dcc_voice_client_reconnect_action(voice_client) != DCC_VOICE_RECONNECT_ACTION_TERMINATE ||
        dcc_voice_client_disconnect_session(voice_client) != DCC_OK ||
        dcc_voice_client_session_state(voice_client) != DCC_VOICE_SESSION_DISCONNECTED ||
        dcc_voice_client_reconnect_action(voice_client) != DCC_VOICE_RECONNECT_ACTION_NONE ||
        dcc_voice_client_channel_id(voice_client) != 0 ||
        dcc_voice_client_set_channel_id(voice_client, 222) != DCC_OK) {
        fprintf(stderr, "voice move/fatal disconnect state failed\n");
        return 1;
    }
    if (dcc_voice_client_start_session(voice_client, 333, 222, 0, 0, 0) != DCC_OK ||
        dcc_voice_client_apply_voice_server_update(voice_client, &self_voice_server) != DCC_OK ||
        dcc_voice_client_apply_voice_state(voice_client, &self_voice_state, 999) != DCC_OK ||
        dcc_voice_client_mark_session_active(voice_client) != DCC_OK ||
        dcc_voice_client_handle_disconnect(voice_client, 4014, "stage gone") != DCC_OK) {
        fprintf(stderr, "voice fatal health setup failed\n");
        return 1;
    }
    memset(&voice_health, 0, sizeof(voice_health));
    voice_health.size = sizeof(voice_health);
    if (dcc_voice_client_health_snapshot(voice_client, &voice_health) != DCC_OK ||
        voice_health.health != DCC_VOICE_HEALTH_TERMINATED ||
        voice_health.action != DCC_VOICE_RECONNECT_ACTION_TERMINATE ||
        voice_health.ok != 0U ||
        voice_health.session.last_close_code != 4014 ||
        dcc_voice_client_health_snapshot_json(&voice_health, voice_health_json, sizeof(voice_health_json), NULL) !=
            DCC_OK ||
        strstr(voice_health_json, "\"health\":\"terminated\"") == NULL ||
        strstr(voice_health_json, "\"action\":\"terminate\"") == NULL) {
        fprintf(stderr, "voice health terminated snapshot failed: %s\n", voice_health_json);
        return 1;
    }
    if (dcc_voice_client_disconnect_session(voice_client) != DCC_OK ||
        dcc_voice_client_set_channel_id(voice_client, 222) != DCC_OK) {
        fprintf(stderr, "voice fatal health cleanup failed\n");
        return 1;
    }

    return 0;
}
