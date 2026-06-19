#include "voice_smoke_support.h"

#include <stdio.h>
#include <string.h>

int voice_invalid_arg_smoke(
    voice_smoke_state_t *state,
    const uint8_t *secret_key,
    size_t secret_key_size,
    const uint8_t *queued_audio,
    size_t queued_audio_size,
    const int16_t *pcm_audio,
    size_t pcm_audio_count,
    const int16_t *mix_pcm_a,
    size_t mix_pcm_count,
    const uint8_t *receive_audio,
    size_t receive_audio_size,
    const char *voice_endpoint_frame,
    const char *voice_noop_frame,
    const uint8_t *voice_rtp_packet,
    size_t voice_rtp_packet_size,
    const uint8_t *voice_bad_rtp_packet,
    size_t voice_bad_rtp_packet_size
) {
    dcc_voice_session_info_t session_info = {
        .size = sizeof(session_info),
    };
    dcc_voice_client_stats_t voice_stats = {
        .size = sizeof(voice_stats),
    };
    dcc_voice_client_t *voice_client = state->voice_client;
    const char *voice_bad_json = "{\"op\":5";
    const char *voice_bad_speaking = "{\"op\":5,\"d\":{\"user_id\":\"444\"}}";
    const char *voice_bad_platform = "{\"op\":20,\"d\":{\"user_id\":\"446\",\"platform\":9}}";
    dcc_voice_session_info_t voice_bad_session_info = {0};
    dcc_voice_client_stats_t voice_bad_stats = {0};
    dcc_voice_dave_mls_frame_t voice_bad_mls_frame = {0};
    dcc_voice_dave_mls_frame_t voice_bad_mls_frame_sized = {
        .size = sizeof(voice_bad_mls_frame_sized),
    };
    dcc_voice_state_t voice_bad_state = {
        .guild_id = 1,
        .channel_id = 2,
        .user_id = 3,
        .session_id = NULL,
    };
    dcc_voice_server_update_t voice_bad_server = {
        .guild_id = 1,
        .token = "",
        .endpoint = "voice.example.com",
    };
    char voice_bad_payload[8];
    uint8_t voice_bad_discovery[8];
    uint8_t voice_bad_binary[2] = {0};
    const uint8_t voice_bad_payload_bytes[2] = {0x01, 0x02};
    char voice_bad_address[8];
    uint16_t voice_bad_port = 0;
    dcc_status_t voice_bad_status = DCC_OK;
    dcc_voice_opus_options_t voice_bad_opus_size = {0};
    dcc_voice_opus_options_t voice_bad_opus_rate = {
        .size = sizeof(voice_bad_opus_rate),
        .sample_rate = 44100,
    };
    if (dcc_voice_client_disconnect_session(state->voice_client) != DCC_OK ||
        dcc_voice_client_handle_gateway_frame(state->voice_client, voice_endpoint_frame, strlen(voice_endpoint_frame)) !=
            DCC_OK) {
        fprintf(stderr, "voice invalid argument reset failed\n");
        return 1;
    }
    if (dcc_voice_client_create(NULL, &voice_client) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_set_channel_id(NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_set_decoder(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_set_encoder(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_start_session(NULL, 1, 1, 0, 0, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_start_session(state->voice_client, 0, 1, 0, 0, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_start_session(state->voice_client, 1, 0, 0, 0, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_apply_voice_state(NULL, &voice_bad_state, 3) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_apply_voice_state(state->voice_client, NULL, 3) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_apply_voice_state(state->voice_client, &voice_bad_state, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_apply_voice_state(state->voice_client, &voice_bad_state, 3) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_apply_voice_server_update(NULL, &voice_bad_server) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_apply_voice_server_update(state->voice_client, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_apply_voice_server_update(state->voice_client, &voice_bad_server) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_session_info(NULL, &session_info) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_session_info(state->voice_client, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_session_info(state->voice_client, &voice_bad_session_info) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_stats(NULL, &voice_stats) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_stats(state->voice_client, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_stats(state->voice_client, &voice_bad_stats) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_session_state(NULL) != DCC_VOICE_SESSION_DISCONNECTED ||
        dcc_voice_client_session_ready(NULL) != 0 ||
        dcc_voice_client_session_active(NULL) != 0 ||
        dcc_voice_client_mark_session_active(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_disconnect_session(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_handle_disconnect(NULL, 1006, "closed") != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_reconnect_requested(NULL) != 0 ||
        dcc_voice_client_full_reconnect_requested(NULL) != 0 ||
        dcc_voice_client_clear_reconnect_request(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_set_dave_enabled(NULL, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_dave_enabled(NULL) != 0 ||
        dcc_voice_client_dave_version(NULL) != DCC_VOICE_DAVE_NONE ||
        dcc_voice_client_dave_transition_pending(NULL) != 0 ||
        dcc_voice_client_dave_transition_id(NULL) != 0 ||
        dcc_voice_client_set_dave_mls_handler(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_dave_mls_opcode_string((dcc_voice_dave_mls_opcode_t)0) == NULL ||
        dcc_voice_parse_dave_mls_gateway_frame(NULL, sizeof(voice_bad_binary), &voice_bad_mls_frame_sized) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_parse_dave_mls_gateway_frame(voice_bad_binary, sizeof(voice_bad_binary), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_parse_dave_mls_gateway_frame(voice_bad_binary, sizeof(voice_bad_binary), &voice_bad_mls_frame) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_parse_dave_mls_gateway_frame(
            voice_bad_binary,
            sizeof(voice_bad_binary),
            &voice_bad_mls_frame_sized
        ) != DCC_ERR_JSON ||
        dcc_voice_build_dave_mls_gateway_frame(
            1,
            DCC_VOICE_DAVE_MLS_KEY_PACKAGE,
            0,
            voice_bad_payload_bytes,
            sizeof(voice_bad_payload_bytes),
            (uint8_t *)voice_bad_payload,
            sizeof(voice_bad_payload),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_build_dave_mls_gateway_frame(
            1,
            DCC_VOICE_DAVE_MLS_EXTERNAL_SENDER,
            0,
            NULL,
            sizeof(voice_bad_payload_bytes),
            (uint8_t *)voice_bad_payload,
            sizeof(voice_bad_payload),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_build_dave_mls_gateway_frame(
            1,
            DCC_VOICE_DAVE_MLS_EXTERNAL_SENDER,
            0,
            voice_bad_payload_bytes,
            sizeof(voice_bad_payload_bytes),
            NULL,
            sizeof(voice_bad_payload),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_build_dave_mls_gateway_frame(
            1,
            DCC_VOICE_DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION,
            2,
            voice_bad_payload_bytes,
            sizeof(voice_bad_payload_bytes),
            (uint8_t *)voice_bad_payload,
            4,
            NULL
        ) != DCC_ERR_NOMEM ||
        dcc_voice_client_handle_gateway_binary_frame(NULL, voice_bad_binary, sizeof(voice_bad_binary)) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_handle_gateway_binary_frame(state->voice_client, NULL, sizeof(voice_bad_binary)) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_handle_gateway_binary_frame(state->voice_client, voice_bad_binary, sizeof(voice_bad_binary)) !=
            DCC_ERR_JSON ||
        dcc_voice_client_set_receive_sequence(NULL, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_sequence(NULL) != -1 ||
        dcc_voice_client_build_identify_payload(NULL, 1, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_identify_payload(state->voice_client, 0, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_identify_payload(state->voice_client, 1, NULL, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_identify_payload(state->voice_client, 1, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_STATE ||
        dcc_voice_client_build_resume_payload(NULL, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_resume_payload(state->voice_client, NULL, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_heartbeat_payload(NULL, 1, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_heartbeat_payload(state->voice_client, 1, NULL, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_heartbeat_payload(state->voice_client, 1, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_NOMEM ||
        dcc_voice_client_build_select_protocol_payload(
            NULL,
            1,
            DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE,
            voice_bad_payload,
            sizeof(voice_bad_payload),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_select_protocol_payload(
            "127.0.0.1",
            0,
            DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE,
            voice_bad_payload,
            sizeof(voice_bad_payload),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_select_protocol_payload(
            "127.0.0.1",
            1,
            DCC_VOICE_ENCRYPTION_NONE,
            voice_bad_payload,
            sizeof(voice_bad_payload),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_dave_transition_ready_payload(0, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_dave_transition_ready_payload(1, NULL, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_dave_transition_ready_payload(1, voice_bad_payload, 0, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_dave_transition_ready_payload(1, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_NOMEM ||
        dcc_voice_client_build_dave_invalid_commit_payload(0, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_dave_invalid_commit_payload(1, NULL, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_dave_invalid_commit_payload(1, voice_bad_payload, 0, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_dave_invalid_commit_payload(1, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_NOMEM ||
        dcc_voice_client_build_dave_mls_frame(
            DCC_VOICE_DAVE_MLS_EXTERNAL_SENDER,
            voice_bad_payload_bytes,
            sizeof(voice_bad_payload_bytes),
            (uint8_t *)voice_bad_payload,
            sizeof(voice_bad_payload),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_dave_mls_frame(
            DCC_VOICE_DAVE_MLS_KEY_PACKAGE,
            NULL,
            sizeof(voice_bad_payload_bytes),
            (uint8_t *)voice_bad_payload,
            sizeof(voice_bad_payload),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_dave_mls_frame(
            DCC_VOICE_DAVE_MLS_KEY_PACKAGE,
            voice_bad_payload_bytes,
            0,
            (uint8_t *)voice_bad_payload,
            sizeof(voice_bad_payload),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_dave_mls_frame(
            DCC_VOICE_DAVE_MLS_KEY_PACKAGE,
            voice_bad_payload_bytes,
            sizeof(voice_bad_payload_bytes),
            NULL,
            sizeof(voice_bad_payload),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_dave_mls_frame(
            DCC_VOICE_DAVE_MLS_KEY_PACKAGE,
            voice_bad_payload_bytes,
            sizeof(voice_bad_payload_bytes),
            (uint8_t *)voice_bad_payload,
            1,
            NULL
        ) != DCC_ERR_NOMEM ||
        dcc_voice_client_send_dave_transition_ready(NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_dave_transition_ready(state->voice_client, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_dave_mls_key_package(NULL, voice_bad_payload_bytes, sizeof(voice_bad_payload_bytes)) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_dave_mls_key_package(state->voice_client, NULL, sizeof(voice_bad_payload_bytes)) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_dave_mls_key_package(state->voice_client, voice_bad_payload_bytes, 0) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_dave_mls_commit_welcome(NULL, voice_bad_payload_bytes, sizeof(voice_bad_payload_bytes)) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_dave_mls_commit_welcome(state->voice_client, NULL, sizeof(voice_bad_payload_bytes)) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_dave_mls_commit_welcome(state->voice_client, voice_bad_payload_bytes, 0) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_dave_invalid_commit_welcome(NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_dave_invalid_commit_welcome(state->voice_client, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_speaking_payload(NULL, 1, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_speaking_payload(state->voice_client, 1, NULL, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_speaking_payload(state->voice_client, 1, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_NOMEM ||
        dcc_voice_client_build_websocket_url(NULL, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_websocket_url(state->voice_client, NULL, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_websocket_url(state->voice_client, voice_bad_payload, sizeof(voice_bad_payload), NULL) !=
            DCC_ERR_STATE ||
        dcc_voice_client_run_websocket_url(NULL, 1, "ws://127.0.0.1/", 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_run_websocket_url(state->voice_client, 0, "ws://127.0.0.1/", 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_run_websocket_url(state->voice_client, 1, NULL, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_run_websocket_url(state->voice_client, 1, "ws://127.0.0.1/", 0) != DCC_ERR_STATE ||
        dcc_voice_client_run_websocket(NULL, 1, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_run_websocket(state->voice_client, 1, 0) != DCC_ERR_STATE ||
        dcc_voice_client_run_websocket_loop_url(NULL, 1, "ws://127.0.0.1/", 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_run_websocket_loop_url(state->voice_client, 0, "ws://127.0.0.1/", 1) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_run_websocket_loop_url(state->voice_client, 1, NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_run_websocket_loop_url(state->voice_client, 1, "ws://127.0.0.1/", 1) !=
            DCC_ERR_STATE ||
        dcc_voice_client_run_websocket_loop(NULL, 1, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_run_websocket_loop(state->voice_client, 1, 1) != DCC_ERR_STATE ||
        dcc_voice_client_start_websocket_loop(NULL, 1, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_start_websocket_loop(state->voice_client, 0, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_start_websocket_loop(state->voice_client, 1, 1) != DCC_ERR_STATE ||
        dcc_voice_client_stop_websocket_loop(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_websocket_loop_running(NULL) != 0 ||
        dcc_voice_client_websocket_loop_status(NULL, &voice_bad_status) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_websocket_loop_status(state->voice_client, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_websocket_loop_status(state->voice_client, &voice_bad_status) != DCC_OK ||
        dcc_voice_client_enable_opus(NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_enable_opus(state->voice_client, &voice_bad_opus_size) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_enable_opus(state->voice_client, &voice_bad_opus_rate) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_disable_opus(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_connect_udp(NULL, "127.0.0.1", 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_connect_udp(state->voice_client, NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_connect_udp(state->voice_client, "", 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_connect_udp(state->voice_client, "127.0.0.1", 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_ip_discovery_packet(NULL, voice_bad_discovery, sizeof(voice_bad_discovery), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_ip_discovery_packet(state->voice_client, NULL, sizeof(voice_bad_discovery), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_build_ip_discovery_packet(
            state->voice_client,
            voice_bad_discovery,
            sizeof(voice_bad_discovery),
            NULL
        ) != DCC_ERR_NOMEM ||
        dcc_voice_parse_ip_discovery_packet(NULL, 0, voice_bad_address, sizeof(voice_bad_address), &voice_bad_port) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_parse_ip_discovery_packet(
            voice_bad_discovery,
            sizeof(voice_bad_discovery),
            voice_bad_address,
            sizeof(voice_bad_address),
            &voice_bad_port
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_discover_udp_ip(NULL, voice_bad_address, sizeof(voice_bad_address), &voice_bad_port, 1) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_discover_udp_ip(state->voice_client, NULL, sizeof(voice_bad_address), &voice_bad_port, 1) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_discover_udp_ip(state->voice_client, voice_bad_address, 0, &voice_bad_port, 1) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_discover_udp_ip(state->voice_client, voice_bad_address, sizeof(voice_bad_address), NULL, 1) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_discover_udp_ip(
            state->voice_client,
            voice_bad_address,
            sizeof(voice_bad_address),
            &voice_bad_port,
            1
        ) != DCC_ERR_STATE ||
        dcc_voice_client_udp_fd(NULL) != -1 ||
        dcc_voice_client_set_rtp_state(NULL, 0, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_rtp_sequence(NULL) != 0 ||
        dcc_voice_client_rtp_timestamp(NULL) != 0 ||
        dcc_voice_client_set_encryption_mode(NULL, DCC_VOICE_ENCRYPTION_NONE) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_set_encryption_mode(
            state->voice_client,
            (dcc_voice_encryption_mode_t)255
        ) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_encryption_mode(NULL) != DCC_VOICE_ENCRYPTION_NONE ||
        dcc_voice_client_set_secret_key(NULL, secret_key, secret_key_size) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_set_secret_key(state->voice_client, NULL, secret_key_size) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_set_secret_key(state->voice_client, secret_key, secret_key_size - 1U) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_has_secret_key(NULL) != 0 ||
        dcc_voice_client_set_packet_nonce(NULL, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_packet_nonce(NULL) != 0 ||
        dcc_voice_client_send_opus_packet(NULL, queued_audio, queued_audio_size, 20) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_opus_packet(state->voice_client, NULL, queued_audio_size, 20) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_opus_packet(state->voice_client, queued_audio, 0, 20) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_pcm(NULL, pcm_audio, pcm_audio_count, 20) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_pcm(state->voice_client, NULL, pcm_audio_count, 20) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_pcm(state->voice_client, pcm_audio, 0, 20) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_pcm(state->voice_client, pcm_audio, 7, 20) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_pcm(state->voice_client, pcm_audio, pcm_audio_count, 0) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_insert_marker(NULL, "") != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_insert_marker(state->voice_client, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_skip_to_next_marker(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_clear_send_queue(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_drain_send_queue(NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_send_queue_size(NULL) != 0 ||
        dcc_voice_client_tracks_remaining(NULL) != 0 ||
        dcc_voice_client_map_ssrc(NULL, 1, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_map_ssrc(state->voice_client, 0, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_map_ssrc(state->voice_client, 1, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_unmap_ssrc(NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_unmap_ssrc(state->voice_client, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_user_for_ssrc(NULL, 1) != 0 ||
        dcc_voice_client_handle_gateway_frame(NULL, voice_noop_frame, strlen(voice_noop_frame)) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_handle_gateway_frame(state->voice_client, NULL, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_handle_gateway_frame(state->voice_client, voice_bad_json, strlen(voice_bad_json)) !=
            DCC_ERR_JSON ||
        dcc_voice_client_handle_gateway_frame(state->voice_client, voice_bad_speaking, strlen(voice_bad_speaking)) !=
            DCC_ERR_JSON ||
        dcc_voice_client_handle_gateway_frame(state->voice_client, voice_bad_platform, strlen(voice_bad_platform)) !=
            DCC_ERR_JSON ||
        dcc_voice_client_receive_rtp_packet(NULL, voice_rtp_packet, voice_rtp_packet_size) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_rtp_packet(state->voice_client, NULL, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_rtp_packet_at(NULL, voice_rtp_packet, voice_rtp_packet_size, 0) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_rtp_packet_at(state->voice_client, NULL, 0, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_rtp_packet(state->voice_client, voice_bad_rtp_packet, voice_bad_rtp_packet_size) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_set_receive_reorder_window(NULL, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_set_receive_reorder_window(state->voice_client, 255) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_reorder_window(NULL) != 0 ||
        dcc_voice_client_set_receive_jitter_delay(NULL, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_jitter_delay(NULL) != 0 ||
        dcc_voice_client_tick_receive_jitter(NULL, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_start_receive_jitter_timer(NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_start_receive_jitter_timer(state->voice_client, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_start_receive_jitter_timer(state->voice_client, 1) != DCC_ERR_STATE ||
        dcc_voice_client_stop_receive_jitter_timer(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_jitter_timer_running(NULL) != 0 ||
        dcc_voice_client_receive_jitter_timer_interval(NULL) != 0 ||
        dcc_voice_client_flush_receive_reorder(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_clear_receive_reorder(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_packets_lost(NULL) != 0 ||
        dcc_voice_client_receive_packets_dropped(NULL) != 0 ||
        dcc_voice_client_mix_pcm(NULL, 1, mix_pcm_a, mix_pcm_count) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_mix_pcm(state->voice_client, 0, mix_pcm_a, mix_pcm_count) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_mix_pcm(state->voice_client, 1, NULL, mix_pcm_count) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_mix_pcm(state->voice_client, 1, mix_pcm_a, 0) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_mix_pcm(state->voice_client, 1, mix_pcm_a, 6000) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_mix_pcm(state->voice_client, 999999, mix_pcm_a, mix_pcm_count) !=
            DCC_ERR_STATE ||
        dcc_voice_client_flush_mixed_pcm(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_clear_mixed_pcm(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_mixed_source_count(NULL) != 0 ||
        dcc_voice_client_receive_pcm(NULL, 1, receive_audio, receive_audio_size) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_pcm(state->voice_client, 0, receive_audio, receive_audio_size) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_pcm(state->voice_client, 1, NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_receive_combined_pcm(state->voice_client, NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_notify_receive(state->voice_client, 1, NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_notify_receive_combined(state->voice_client, NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_notify_track_marker(state->voice_client, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_notify_platform(state->voice_client, 1, (dcc_voice_client_platform_t)255) !=
            DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "voice invalid argument checks failed\n");
        return 1;
    }

    return 0;
}
