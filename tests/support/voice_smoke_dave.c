#include "voice_smoke_support.h"

#include <stdio.h>
#include <string.h>

int voice_dave_gateway_smoke(dcc_voice_client_t *voice_client) {
    const char *voice_seq_frame = "{\"op\":6,\"seq\":321,\"d\":null}";
    const char *voice_hello_frame = "{\"op\":8,\"d\":{\"heartbeat_interval\":41250}}";
    const char *voice_resumed_frame = "{\"op\":9,\"d\":null}";
    const char *voice_dave_prepare_frame =
        "{\"op\":21,\"d\":{\"transition_id\":7,\"protocol_version\":0}}";
    const char *voice_dave_execute_frame = "{\"op\":22,\"d\":{\"transition_id\":7}}";
    const char *voice_dave_execute_mismatch_frame = "{\"op\":22,\"d\":{\"transition_id\":54}}";
    const char *voice_dave_epoch_frame =
        "{\"op\":24,\"d\":{\"protocol_version\":1,\"epoch\":1}}";
    const char *voice_dave_invalid_frame = "{\"op\":31,\"d\":{\"transition_id\":9}}";
    const char *voice_dave_invalid_mismatch_frame = "{\"op\":31,\"d\":{\"transition_id\":54}}";
    const uint8_t voice_dave_external_sender_frame[] = {0x00, 0x07, 25, 0x11, 0x22, 0x33};
    const uint8_t voice_dave_announce_commit_frame[] = {0x00, 0x08, 29, 0x00, 0x37, 0xaa, 0xbb};
    const uint8_t voice_dave_short_announce_frame[] = {0x00, 0x09, 29, 0x00};
    const uint8_t voice_dave_rejected_announce_frame[] = {0x00, 0x09, 29, 0x00, 0x38, 0xff};
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
    dcc_voice_session_info_t session_info = {
        .size = sizeof(session_info),
    };
    dcc_voice_client_stats_t voice_stats = {
        .size = sizeof(voice_stats),
    };
    voice_dave_mls_state_t dave_mls_state;
    dcc_voice_dave_mls_frame_t dave_frame = {
        .size = sizeof(dave_frame),
    };
    char voice_payload[1024];
    size_t voice_payload_len = 0;
    uint8_t built_gateway_frame[16];
    size_t built_gateway_frame_len = 0;

    memset(&dave_mls_state, 0, sizeof(dave_mls_state));

    if (dcc_voice_client_disconnect_session(voice_client) != DCC_OK ||
        dcc_voice_client_start_session(voice_client, 333, 222, 0, 0, 1) != DCC_OK ||
        dcc_voice_client_apply_voice_server_update(voice_client, &self_voice_server) != DCC_OK ||
        dcc_voice_client_apply_voice_state(voice_client, &self_voice_state, 999) != DCC_OK ||
        dcc_voice_client_set_dave_enabled(voice_client, 1) != DCC_OK ||
        dcc_voice_client_handle_gateway_frame(voice_client, voice_seq_frame, strlen(voice_seq_frame)) != DCC_OK ||
        dcc_voice_client_receive_sequence(voice_client) != 321 ||
        dcc_voice_client_handle_gateway_frame(voice_client, voice_hello_frame, strlen(voice_hello_frame)) != DCC_OK) {
        fprintf(stderr, "voice hello frame handling failed\n");
        return 1;
    }
    session_info.size = sizeof(session_info);
    if (dcc_voice_client_session_info(voice_client, &session_info) != DCC_OK ||
        session_info.heartbeat_interval_ms != 41250 ||
        session_info.receive_sequence != 321 ||
        dcc_voice_client_handle_gateway_frame(voice_client, voice_resumed_frame, strlen(voice_resumed_frame)) !=
            DCC_OK ||
        !dcc_voice_client_session_active(voice_client) ||
        dcc_voice_client_handle_gateway_frame(
            voice_client,
            voice_dave_prepare_frame,
            strlen(voice_dave_prepare_frame)
        ) != DCC_OK ||
        !dcc_voice_client_dave_transition_pending(voice_client) ||
        dcc_voice_client_dave_transition_id(voice_client) != 7) {
        fprintf(stderr, "voice hello/resume/dave prepare state failed\n");
        return 1;
    }
    if (dcc_voice_client_build_dave_transition_ready_payload(7, voice_payload, sizeof(voice_payload), &voice_payload_len) !=
            DCC_OK ||
        voice_payload_len != strlen(voice_payload) ||
        strcmp(voice_payload, "{\"op\":23,\"d\":{\"transition_id\":7}}") != 0 ||
        dcc_voice_client_build_dave_invalid_commit_payload(9, voice_payload, sizeof(voice_payload), &voice_payload_len) !=
            DCC_OK ||
        voice_payload_len != strlen(voice_payload) ||
        strcmp(voice_payload, "{\"op\":31,\"d\":{\"transition_id\":9}}") != 0 ||
        dcc_voice_client_build_dave_mls_frame(
            DCC_VOICE_DAVE_MLS_KEY_PACKAGE,
            voice_dave_external_sender_frame + 3,
            sizeof(voice_dave_external_sender_frame) - 3U,
            (uint8_t *)voice_payload,
            sizeof(voice_payload),
            &voice_payload_len
        ) != DCC_OK ||
        voice_payload_len != 4U ||
        (uint8_t)voice_payload[0] != (uint8_t)DCC_VOICE_DAVE_MLS_KEY_PACKAGE ||
        (uint8_t)voice_payload[1] != 0x11 ||
        (uint8_t)voice_payload[2] != 0x22 ||
        (uint8_t)voice_payload[3] != 0x33 ||
        strcmp(dcc_voice_dave_mls_opcode_string(DCC_VOICE_DAVE_MLS_EXTERNAL_SENDER), "external_sender") != 0 ||
        strcmp(dcc_voice_dave_mls_opcode_string(DCC_VOICE_DAVE_MLS_KEY_PACKAGE), "key_package") != 0 ||
        strcmp(dcc_voice_dave_mls_opcode_string(DCC_VOICE_DAVE_MLS_COMMIT_WELCOME), "commit_welcome") != 0 ||
        strcmp(
            dcc_voice_dave_mls_opcode_string(DCC_VOICE_DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION),
            "announce_commit_transition"
        ) != 0 ||
        strcmp(dcc_voice_dave_mls_opcode_string((dcc_voice_dave_mls_opcode_t)255), "unknown") != 0 ||
        dcc_voice_parse_dave_mls_gateway_frame(
            voice_dave_announce_commit_frame,
            sizeof(voice_dave_announce_commit_frame),
            &dave_frame
        ) != DCC_OK ||
        dave_frame.sequence != 8 ||
        dave_frame.opcode != DCC_VOICE_DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION ||
        !dave_frame.has_transition_id ||
        dave_frame.transition_id != 55 ||
        dave_frame.payload_size != 2 ||
        dave_frame.payload[0] != 0xaa ||
        dcc_voice_build_dave_mls_gateway_frame(
            8,
            DCC_VOICE_DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION,
            55,
            dave_frame.payload,
            dave_frame.payload_size,
            built_gateway_frame,
            sizeof(built_gateway_frame),
            &built_gateway_frame_len
        ) != DCC_OK ||
        built_gateway_frame_len != sizeof(voice_dave_announce_commit_frame) ||
        memcmp(built_gateway_frame, voice_dave_announce_commit_frame, built_gateway_frame_len) != 0 ||
        dcc_voice_parse_dave_mls_gateway_frame(
            built_gateway_frame,
            built_gateway_frame_len,
            &dave_frame
        ) != DCC_OK ||
        dave_frame.sequence != 8 ||
        dave_frame.transition_id != 55 ||
        dave_frame.payload_size != 2 ||
        dcc_voice_client_set_dave_mls_handler(voice_client, voice_dave_mls_handler, &dave_mls_state) !=
            DCC_OK ||
        dcc_voice_client_handle_gateway_binary_frame(
            voice_client,
            voice_dave_external_sender_frame,
            sizeof(voice_dave_external_sender_frame)
        ) != DCC_OK ||
        !dave_mls_state.external_seen ||
        dcc_voice_client_receive_sequence(voice_client) != 7 ||
        dcc_voice_client_handle_gateway_binary_frame(
            voice_client,
            voice_dave_announce_commit_frame,
            sizeof(voice_dave_announce_commit_frame)
        ) != DCC_OK ||
        !dave_mls_state.announce_seen ||
        dave_mls_state.transition_id != 55 ||
        dave_mls_state.payload_size != 2 ||
        dave_mls_state.first_payload_byte != 0xaa ||
        dcc_voice_client_receive_sequence(voice_client) != 8 ||
        dcc_voice_client_dave_transition_id(voice_client) != 55 ||
        !dcc_voice_client_dave_transition_pending(voice_client)) {
        fprintf(stderr, "voice dave mls bridge failed\n");
        return 1;
    }

    int32_t dave_sequence_before_bad_frame = dcc_voice_client_receive_sequence(voice_client);
    if (dcc_voice_client_handle_gateway_binary_frame(
            voice_client,
            voice_dave_short_announce_frame,
            sizeof(voice_dave_short_announce_frame)
        ) != DCC_ERR_JSON ||
        dcc_voice_client_receive_sequence(voice_client) != dave_sequence_before_bad_frame ||
        dcc_voice_client_dave_transition_id(voice_client) != 55 ||
        !dcc_voice_client_dave_transition_pending(voice_client) ||
        dcc_voice_client_handle_gateway_binary_frame(
            voice_client,
            voice_dave_rejected_announce_frame,
            sizeof(voice_dave_rejected_announce_frame)
        ) != DCC_ERR_STATE ||
        dcc_voice_client_receive_sequence(voice_client) != dave_sequence_before_bad_frame ||
        dcc_voice_client_dave_transition_id(voice_client) != 55 ||
        !dcc_voice_client_dave_transition_pending(voice_client) ||
        dcc_voice_client_handle_gateway_frame(
            voice_client,
            voice_dave_execute_mismatch_frame,
            strlen(voice_dave_execute_mismatch_frame)
        ) != DCC_ERR_JSON ||
        dcc_voice_client_dave_transition_id(voice_client) != 55 ||
        !dcc_voice_client_dave_transition_pending(voice_client) ||
        dcc_voice_client_handle_gateway_frame(
            voice_client,
            voice_dave_invalid_mismatch_frame,
            strlen(voice_dave_invalid_mismatch_frame)
        ) != DCC_ERR_JSON ||
        dcc_voice_client_dave_transition_id(voice_client) != 55 ||
        !dcc_voice_client_dave_transition_pending(voice_client)) {
        fprintf(stderr, "voice dave malformed transition hardening failed\n");
        return 1;
    }
    if (dcc_voice_client_send_dave_transition_ready(voice_client, 7) != DCC_ERR_STATE ||
        dcc_voice_client_send_dave_invalid_commit_welcome(voice_client, 9) != DCC_ERR_STATE ||
        dcc_voice_client_send_dave_mls_key_package(
            voice_client,
            voice_dave_external_sender_frame + 3,
            sizeof(voice_dave_external_sender_frame) - 3U
        ) != DCC_ERR_STATE ||
        dcc_voice_client_send_dave_mls_commit_welcome(
            voice_client,
            voice_dave_announce_commit_frame + 5,
            sizeof(voice_dave_announce_commit_frame) - 5U
        ) != DCC_ERR_STATE) {
        fprintf(stderr, "voice dave send-without-ws state failed\n");
        return 1;
    }
    if (dcc_voice_client_handle_gateway_frame(
            voice_client,
            voice_dave_prepare_frame,
            strlen(voice_dave_prepare_frame)
        ) != DCC_OK) {
        fprintf(stderr, "voice dave prepare reset failed\n");
        return 1;
    }
    session_info.size = sizeof(session_info);
    if (dcc_voice_client_session_info(voice_client, &session_info) != DCC_OK ||
        session_info.dave_pending_version != DCC_VOICE_DAVE_NONE ||
        !session_info.dave_transition_ready ||
        dcc_voice_client_handle_gateway_frame(
            voice_client,
            voice_dave_execute_frame,
            strlen(voice_dave_execute_frame)
        ) != DCC_OK ||
        dcc_voice_client_dave_enabled(voice_client) ||
        dcc_voice_client_dave_version(voice_client) != DCC_VOICE_DAVE_NONE ||
        dcc_voice_client_dave_transition_pending(voice_client)) {
        fprintf(stderr, "voice dave execute state failed\n");
        return 1;
    }
    voice_stats.size = sizeof(voice_stats);
    if (dcc_voice_client_handle_gateway_frame(
            voice_client,
            voice_dave_epoch_frame,
            strlen(voice_dave_epoch_frame)
        ) != DCC_OK ||
        !dcc_voice_client_dave_enabled(voice_client) ||
        dcc_voice_client_dave_version(voice_client) != DCC_VOICE_DAVE_VERSION_1 ||
        dcc_voice_client_handle_gateway_frame(
            voice_client,
            voice_dave_invalid_frame,
            strlen(voice_dave_invalid_frame)
        ) != DCC_OK ||
        dcc_voice_client_dave_transition_pending(voice_client) ||
        dcc_voice_client_dave_transition_id(voice_client) != 9 ||
        dcc_voice_client_stats(voice_client, &voice_stats) != DCC_OK ||
        !voice_stats.dave_enabled ||
        voice_stats.dave_version != DCC_VOICE_DAVE_VERSION_1 ||
        voice_stats.dave_transition_pending ||
        voice_stats.dave_transition_id != 9 ||
        voice_stats.receive_sequence != dcc_voice_client_receive_sequence(voice_client)) {
        fprintf(stderr, "voice dave epoch/invalid state failed\n");
        return 1;
    }

    return 0;
}
