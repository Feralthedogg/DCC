#include <dcc/dcc.h>

#include "support/voice_smoke_support.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice_client = NULL;
    const uint8_t receive_audio[] = {1, 2, 3, 4};
    const uint8_t combined_audio[] = {9, 8};
    const int16_t pcm_audio[] = {100, 101, 102, 103, 104, 105, 106, 107};
    const int16_t mix_pcm_a[] = {30000, -30000, 1000, -1000};
    const int16_t mix_pcm_b[] = {10000, -10000, -3000, 3000};
    const int16_t mixed_expected[] = {32767, -32768, -2000, 2000};
    int16_t opus_pcm[960 * 2];
    uint8_t queued_audio[4096];
    uint8_t secret_key[32];
    const dcc_event_type_t events[] = {
        DCC_EVENT_VOICE_BUFFER_SEND,
        DCC_EVENT_VOICE_READY,
        DCC_EVENT_VOICE_RECEIVE,
        DCC_EVENT_VOICE_RECEIVE_COMBINED,
        DCC_EVENT_VOICE_TRACK_MARKER,
        DCC_EVENT_VOICE_CLIENT_SPEAKING,
        DCC_EVENT_VOICE_CLIENT_DISCONNECT,
        DCC_EVENT_VOICE_CLIENT_PLATFORM,
    };
    memset(opus_pcm, 0, sizeof(opus_pcm));
    memset(queued_audio, 0x55, sizeof(queued_audio));
    for (size_t i = 0; i < sizeof(secret_key); ++i) {
        secret_key[i] = (uint8_t)(i + 1U);
    }
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .shard_id = 7,
        .shard_count = 8,
        .enable_cache = 1,
        .infer_guild_id_from_channel = 1,
    };

    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    st = dcc_voice_client_create(client, &voice_client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_voice_client_create failed: %s\n", dcc_status_string(st));
        dcc_client_destroy(client);
        return 1;
    }

    if (voice_session_state_smoke(client, voice_client) != 0) {
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }

    voice_smoke_state_t state = {
        .client = client,
        .voice_client = voice_client,
        .receive_audio = receive_audio,
        .combined_audio = combined_audio,
        .mixed_audio = (const uint8_t *)mixed_expected,
        .mixed_audio_size = sizeof(mixed_expected),
        .shard_id = 7,
    };
    voice_decode_state_t decode_state = {0};
    voice_encode_state_t encode_state = {0};

    dcc_voice_client_stats_t voice_stats = {
        .size = sizeof(voice_stats),
    };
    dcc_voice_health_snapshot_t voice_health = {
        .size = sizeof(voice_health),
    };
    char voice_health_json[1024];
    char voice_payload[1024];
    size_t voice_payload_len = 0;
    for (size_t i = 0; i < sizeof(events) / sizeof(events[0]); ++i) {
        if (!voice_smoke_add_listener(client, events[i], &state)) {
            dcc_voice_client_destroy(voice_client);
            dcc_client_destroy(client);
            return 1;
        }
    }

    if (dcc_voice_client_mixed_source_count(voice_client) != 0 ||
        dcc_voice_client_map_ssrc(voice_client, 1111, 7001) != DCC_OK ||
        dcc_voice_client_map_ssrc(voice_client, 2222, 7002) != DCC_OK ||
        dcc_voice_client_mix_pcm(voice_client, 1111, mix_pcm_a, sizeof(mix_pcm_a) / sizeof(mix_pcm_a[0])) !=
            DCC_OK ||
        dcc_voice_client_mixed_source_count(voice_client) != 1 ||
        dcc_voice_client_mix_pcm(voice_client, 2222, mix_pcm_b, sizeof(mix_pcm_b) / sizeof(mix_pcm_b[0])) !=
            DCC_OK ||
        dcc_voice_client_mixed_source_count(voice_client) != 2 ||
        dcc_voice_client_flush_mixed_pcm(voice_client) != DCC_OK ||
        dcc_voice_client_mixed_source_count(voice_client) != 0 ||
        !state.mixed_combined_seen ||
        dcc_voice_client_clear_mixed_pcm(voice_client) != DCC_OK) {
        fprintf(stderr, "voice receive mixer failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }

    dcc_voice_opus_options_t opus_options = {
        .size = sizeof(opus_options),
    };
    dcc_status_t opus_status = dcc_voice_client_enable_opus(voice_client, &opus_options);
    if (dcc_voice_opus_is_available()) {
        if (opus_status != DCC_OK ||
            dcc_voice_client_send_pcm(voice_client, opus_pcm, sizeof(opus_pcm) / sizeof(opus_pcm[0]), 20) !=
                DCC_OK ||
            dcc_voice_client_send_queue_size(voice_client) != 1 ||
            dcc_voice_client_clear_send_queue(voice_client) != DCC_OK ||
            dcc_voice_client_disable_opus(voice_client) != DCC_OK ||
            dcc_voice_client_send_pcm(voice_client, opus_pcm, sizeof(opus_pcm) / sizeof(opus_pcm[0]), 20) !=
                DCC_ERR_STATE) {
            fprintf(stderr, "voice default opus backend failed\n");
            dcc_voice_client_destroy(voice_client);
            dcc_client_destroy(client);
            return 1;
        }
    } else if (opus_status != DCC_ERR_STATE || dcc_voice_client_disable_opus(voice_client) != DCC_OK) {
        fprintf(stderr, "voice unavailable opus backend failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }

    if (dcc_voice_client_send_pcm(voice_client, pcm_audio, sizeof(pcm_audio) / sizeof(pcm_audio[0]), 20) !=
            DCC_ERR_STATE ||
        dcc_voice_client_set_encoder(voice_client, voice_encode_pcm, &encode_state) != DCC_OK ||
        dcc_voice_client_send_pcm(voice_client, pcm_audio, sizeof(pcm_audio) / sizeof(pcm_audio[0]), 20) != DCC_OK ||
        !encode_state.seen ||
        encode_state.sample_count != 8 ||
        encode_state.duration_ms != 20 ||
        dcc_voice_client_send_queue_size(voice_client) != 1 ||
        dcc_voice_client_tracks_remaining(voice_client) != 1 ||
        dcc_voice_client_clear_send_queue(voice_client) != DCC_OK ||
        dcc_voice_client_send_queue_size(voice_client) != 0) {
        fprintf(stderr, "voice pcm encode queue failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }

    if (dcc_voice_client_send_queue_size(voice_client) != 0 ||
        dcc_voice_client_tracks_remaining(voice_client) != 0 ||
        dcc_voice_client_send_opus_packet(voice_client, queued_audio, sizeof(queued_audio), 20) != DCC_OK ||
        dcc_voice_client_insert_marker(voice_client, "marker-1") != DCC_OK ||
        dcc_voice_client_send_queue_size(voice_client) != 2 ||
        dcc_voice_client_tracks_remaining(voice_client) != 2 ||
        dcc_voice_client_drain_send_queue(voice_client, 1) != DCC_OK ||
        dcc_voice_client_send_queue_size(voice_client) != 1 ||
        dcc_voice_client_tracks_remaining(voice_client) != 2 ||
        dcc_voice_client_drain_send_queue(voice_client, 1) != DCC_OK ||
        dcc_voice_client_send_queue_size(voice_client) != 0 ||
        dcc_voice_client_tracks_remaining(voice_client) != 0 ||
        dcc_voice_client_notify_ready(voice_client, 222) != DCC_OK ||
        dcc_voice_client_notify_receive(voice_client, 444, receive_audio, sizeof(receive_audio)) != DCC_OK ||
        dcc_voice_client_notify_receive_combined(voice_client, combined_audio, sizeof(combined_audio)) != DCC_OK ||
        dcc_voice_client_notify_speaking(voice_client, 444, 123456) != DCC_OK ||
        dcc_voice_client_notify_disconnect(voice_client, 445) != DCC_OK ||
        dcc_voice_client_notify_platform(voice_client, 446, DCC_VOICE_CLIENT_PLATFORM_MOBILE) != DCC_OK) {
        fprintf(stderr, "voice notify failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }

    if (dcc_voice_client_send_opus_packet(voice_client, queued_audio, 8, 20) != DCC_OK ||
        dcc_voice_client_insert_marker(voice_client, "skip-marker") != DCC_OK ||
        dcc_voice_client_send_opus_packet(voice_client, queued_audio, 9, 20) != DCC_OK ||
        dcc_voice_client_insert_marker(voice_client, "left-marker") != DCC_OK ||
        dcc_voice_client_send_queue_size(voice_client) != 4 ||
        dcc_voice_client_tracks_remaining(voice_client) != 3 ||
        (voice_stats.size = sizeof(voice_stats),
         dcc_voice_client_stats(voice_client, &voice_stats)) != DCC_OK ||
        voice_stats.send_queue_size != 4 ||
        voice_stats.tracks_remaining != 3 ||
        dcc_voice_client_skip_to_next_marker(voice_client) != DCC_OK ||
        dcc_voice_client_send_queue_size(voice_client) != 2 ||
        dcc_voice_client_tracks_remaining(voice_client) != 2 ||
        dcc_voice_client_clear_send_queue(voice_client) != DCC_OK ||
        dcc_voice_client_send_queue_size(voice_client) != 0 ||
        dcc_voice_client_tracks_remaining(voice_client) != 0) {
        fprintf(stderr, "voice send queue helpers failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }

#if !defined(_WIN32)
    if (voice_udp_smoke(
            voice_client,
            &state,
            &decode_state,
            queued_audio,
            secret_key,
            sizeof(secret_key)
        ) != 0) {
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }
#endif

    const char *voice_ready_frame =
        "{\"op\":4,\"d\":{\"secret_key\":["
        "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,"
        "17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32]}}";
    const char *voice_endpoint_frame =
        "{\"op\":2,\"d\":{\"ssrc\":123456,\"modes\":[\"aead_xchacha20_poly1305_rtpsize\"]}}";
    const char *voice_speaking_frame = "{\"op\":5,\"d\":{\"user_id\":\"444\",\"ssrc\":123456}}";
    const char *voice_disconnect_frame = "{\"op\":13,\"d\":{\"user_id\":\"445\"}}";
    const char *voice_platform_frame = "{\"op\":20,\"d\":{\"user_id\":\"446\",\"platform\":1}}";
    const char *voice_noop_frame = "{\"op\":6,\"d\":null}";
    const uint8_t voice_rtp_packet[] = {
        0x80, 0x78, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0xe2, 0x40,
        0xaa, 0xbb, 0xcc, 0xdd, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14,
        0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c
    };
    const uint8_t voice_short_rtp_packet[] = {
        0x80, 0x78, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0xe2, 0x40
    };
    const uint8_t voice_bad_rtp_packet[] = {0x80, 0x78, 0x00};

    if (voice_rtp_reorder_jitter_smoke(
            voice_client,
            &decode_state,
            &voice_stats,
            voice_endpoint_frame,
            voice_speaking_frame,
            voice_rtp_packet,
            sizeof(voice_rtp_packet),
            receive_audio
        ) != 0) {
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }

#if !defined(_WIN32)
    if (voice_websocket_smoke() != 0) {
        fprintf(stderr, "voice websocket smoke failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }
    if (voice_websocket_loop_smoke() != 0) {
        fprintf(stderr, "voice websocket loop smoke failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }
    if (voice_websocket_managed_loop_smoke() != 0) {
        fprintf(stderr, "voice managed websocket loop smoke failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }
    if (voice_websocket_managed_close_chaos_smoke() != 0) {
        fprintf(stderr, "voice managed websocket close chaos smoke failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }
    if (voice_websocket_full_reconnect_smoke() != 0) {
        fprintf(stderr, "voice websocket full reconnect smoke failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }
#endif

    if (voice_dave_gateway_smoke(voice_client) != 0) {
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }

    memset(&decode_state, 0, sizeof(decode_state));
    if (dcc_voice_client_handle_gateway_frame(voice_client, voice_ready_frame, strlen(voice_ready_frame)) != DCC_OK ||
        !dcc_voice_client_has_secret_key(voice_client) ||
        dcc_voice_client_handle_gateway_frame(voice_client, voice_endpoint_frame, strlen(voice_endpoint_frame)) != DCC_OK ||
        dcc_voice_client_encryption_mode(voice_client) !=
            DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE) {
        fprintf(stderr, "voice ready/endpoint frame handling failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }
    memset(&voice_health, 0, sizeof(voice_health));
    voice_health.size = sizeof(voice_health);
    voice_health_json[0] = '\0';
    dcc_status_t voice_health_status = dcc_voice_client_health_snapshot(voice_client, &voice_health);
    if (voice_health_status != DCC_OK ||
        voice_health.health != DCC_VOICE_HEALTH_ACTIVE ||
        voice_health.ok != 1U ||
        voice_health.media_ready != 1U ||
        voice_health.session.state != DCC_VOICE_SESSION_ACTIVE ||
        dcc_voice_client_health_snapshot_json(&voice_health, voice_health_json, sizeof(voice_health_json), NULL) !=
            DCC_OK ||
        strstr(voice_health_json, "\"health\":\"active\"") == NULL ||
        strstr(voice_health_json, "\"media_ready\":true") == NULL) {
        fprintf(
            stderr,
            "voice health active snapshot failed: status=%s health=%s state=%s current_state=%s json=%s\n",
            dcc_status_string(voice_health_status),
            dcc_voice_health_string(voice_health.health),
            dcc_voice_session_state_string(voice_health.session.state),
            dcc_voice_session_state_string(dcc_voice_client_session_state(voice_client)),
            voice_health_json
        );
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }
    if (dcc_voice_client_set_encryption_mode(voice_client, DCC_VOICE_ENCRYPTION_NONE) != DCC_OK ||
        dcc_voice_client_ssrc(voice_client) != 123456 ||
        dcc_voice_client_handle_gateway_frame(voice_client, voice_speaking_frame, strlen(voice_speaking_frame)) != DCC_OK ||
        dcc_voice_client_user_for_ssrc(voice_client, 123456) != 444 ||
        dcc_voice_client_set_decoder(voice_client, voice_decode_pcm, &decode_state) != DCC_OK ||
        dcc_voice_client_receive_rtp_packet(voice_client, voice_rtp_packet, sizeof(voice_rtp_packet)) != DCC_OK ||
        !decode_state.seen ||
        decode_state.ssrc != 123456 ||
        decode_state.payload_size != 32 ||
        dcc_voice_client_receive_rtp_packet(voice_client, voice_short_rtp_packet, sizeof(voice_short_rtp_packet)) !=
            DCC_OK ||
        dcc_voice_client_receive_pcm(voice_client, 123456, receive_audio, sizeof(receive_audio)) != DCC_OK ||
        dcc_voice_client_receive_combined_pcm(voice_client, combined_audio, sizeof(combined_audio)) != DCC_OK ||
        dcc_voice_client_map_ssrc(voice_client, 123456, 445) != DCC_OK ||
        dcc_voice_client_user_for_ssrc(voice_client, 123456) != 445 ||
        dcc_voice_client_handle_gateway_frame(voice_client, voice_disconnect_frame, strlen(voice_disconnect_frame)) != DCC_OK ||
        dcc_voice_client_user_for_ssrc(voice_client, 123456) != 0 ||
        dcc_voice_client_receive_pcm(voice_client, 123456, receive_audio, sizeof(receive_audio)) != DCC_ERR_STATE ||
        dcc_voice_client_handle_gateway_frame(voice_client, voice_platform_frame, strlen(voice_platform_frame)) != DCC_OK ||
        dcc_voice_client_handle_gateway_frame(voice_client, voice_noop_frame, strlen(voice_noop_frame)) != DCC_OK) {
        fprintf(stderr, "voice gateway frame handling failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }
    if (dcc_voice_client_build_speaking_payload(
            voice_client,
            1,
            voice_payload,
            sizeof(voice_payload),
            &voice_payload_len
        ) != DCC_OK ||
        voice_payload_len != strlen(voice_payload) ||
        strcmp(voice_payload, "{\"op\":5,\"d\":{\"speaking\":1,\"delay\":0,\"ssrc\":123456}}") != 0) {
        fprintf(stderr, "voice speaking payload build failed\n");
        dcc_voice_client_destroy(voice_client);
        dcc_client_destroy(client);
        return 1;
    }

    if (voice_invalid_arg_smoke(
            &state,
            secret_key,
            sizeof(secret_key),
            queued_audio,
            sizeof(queued_audio),
            pcm_audio,
            sizeof(pcm_audio) / sizeof(pcm_audio[0]),
            mix_pcm_a,
            sizeof(mix_pcm_a) / sizeof(mix_pcm_a[0]),
            receive_audio,
            sizeof(receive_audio),
            voice_endpoint_frame,
            voice_noop_frame,
            voice_rtp_packet,
            sizeof(voice_rtp_packet),
            voice_bad_rtp_packet,
            sizeof(voice_bad_rtp_packet)
        ) != 0) {
        dcc_voice_client_destroy(state.voice_client);
        dcc_client_destroy(client);
        return 1;
    }

    dcc_status_t voice_connect_null = dcc_voice_client_connect(NULL, 1, 1, 0, 0, 0);
    dcc_status_t voice_connect_state = dcc_voice_client_connect(state.voice_client, 1, 1, 0, 0, 0);
    dcc_channel_t inferred_channel = {
        .id = 222,
        .guild_id = 111,
        .name = "voice",
    };
    if (dcc_cache_put_channel(client, &inferred_channel) != DCC_OK) {
        fprintf(stderr, "voice inference channel cache put failed\n");
        dcc_voice_client_destroy(state.voice_client);
        dcc_client_destroy(client);
        return 1;
    }
    dcc_status_t voice_connect_inferred = dcc_voice_client_connect(state.voice_client, 0, 222, 0, 0, 0);
    dcc_status_t voice_leave_null = dcc_voice_client_leave(NULL);
    dcc_status_t voice_leave_state = dcc_voice_client_leave(state.voice_client);
    dcc_status_t gateway_voice_null = dcc_client_update_voice_state(NULL, 1, 1, 0, 0);
    dcc_status_t gateway_voice_state =
        dcc_client_update_voice_state(dcc_voice_client_owner(state.voice_client), 1, 1, 0, 0);
    dcc_status_t gateway_voice_inferred =
        dcc_client_update_voice_state(dcc_voice_client_owner(state.voice_client), 0, 222, 0, 0);
    if (voice_connect_null != DCC_ERR_INVALID_ARG ||
        voice_connect_state != DCC_ERR_STATE ||
        voice_connect_inferred != DCC_ERR_STATE ||
        voice_leave_null != DCC_ERR_INVALID_ARG ||
        voice_leave_state != DCC_ERR_STATE ||
        gateway_voice_null != DCC_ERR_INVALID_ARG ||
        gateway_voice_state != DCC_ERR_STATE ||
        gateway_voice_inferred != DCC_ERR_STATE) {
        fprintf(
            stderr,
            "voice gateway state checks failed: connect_null=%s connect=%s leave_null=%s leave=%s "
            "gateway_null=%s gateway=%s connect_inferred=%s gateway_inferred=%s\n",
            dcc_status_string(voice_connect_null),
            dcc_status_string(voice_connect_state),
            dcc_status_string(voice_leave_null),
            dcc_status_string(voice_leave_state),
            dcc_status_string(gateway_voice_null),
            dcc_status_string(gateway_voice_state),
            dcc_status_string(voice_connect_inferred),
            dcc_status_string(gateway_voice_inferred)
        );
        dcc_voice_client_destroy(state.voice_client);
        dcc_client_destroy(client);
        return 1;
    }

    if (state.failed ||
        !state.buffer_send_seen ||
        !state.ready_seen ||
        !state.receive_seen ||
        !state.receive_combined_seen ||
        !state.mixed_combined_seen ||
        !state.track_marker_seen ||
        !state.speaking_seen ||
        !state.disconnect_seen ||
        !state.platform_seen) {
        fprintf(stderr, "voice smoke did not observe all events\n");
        dcc_voice_client_destroy(state.voice_client);
        dcc_client_destroy(client);
        return 1;
    }

    dcc_voice_client_destroy(state.voice_client);
    dcc_client_destroy(client);
    return 0;
}
