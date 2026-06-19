#include "voice_smoke_support.h"

#include <stdio.h>
#include <string.h>

#if !defined(_WIN32)
#include <unistd.h>
#endif

static void voice_smoke_fail(voice_smoke_state_t *state, const char *message) {
    state->failed = 1;
    fprintf(stderr, "voice smoke callback failed: %s\n", message);
}

dcc_status_t voice_dave_mls_handler(
    void *user_data,
    dcc_voice_client_t *voice_client,
    dcc_voice_dave_mls_opcode_t opcode,
    uint16_t sequence,
    uint16_t transition_id,
    const uint8_t *payload,
    size_t payload_size
) {
    (void)voice_client;
    voice_dave_mls_state_t *state = (voice_dave_mls_state_t *)user_data;
    if (state == NULL || payload == NULL || payload_size == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    state->sequence = sequence;
    state->transition_id = transition_id;
    state->payload_size = payload_size;
    state->first_payload_byte = payload[0];
    if (opcode == DCC_VOICE_DAVE_MLS_EXTERNAL_SENDER &&
        sequence == 7 &&
        transition_id == 0 &&
        payload_size == 3 &&
        payload[0] == 0x11) {
        state->external_seen = 1;
        return DCC_OK;
    }
    if (opcode == DCC_VOICE_DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION &&
        sequence == 8 &&
        transition_id == 55 &&
        payload_size == 2 &&
        payload[0] == 0xaa) {
        state->announce_seen = 1;
        return DCC_OK;
    }
    return DCC_ERR_STATE;
}

dcc_status_t voice_decode_pcm(
    void *user_data,
    uint32_t ssrc,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *pcm_out,
    size_t pcm_out_cap,
    size_t *pcm_out_size
) {
    voice_decode_state_t *state = (voice_decode_state_t *)user_data;
    size_t expected_payload_size = state != NULL && state->expected_payload_size != 0
        ? state->expected_payload_size
        : 32U;
    uint8_t expected_first_byte = state != NULL && state->expected_payload_size != 0
        ? state->expected_first_byte
        : 0xaaU;
    int check_first_byte = state != NULL &&
        (state->expected_payload_size == 0 || state->expected_first_byte != 0);
    if (state == NULL ||
        ssrc != 123456 ||
        payload == NULL ||
        payload_size != expected_payload_size ||
        (check_first_byte && payload[0] != expected_first_byte) ||
        pcm_out == NULL ||
        pcm_out_cap < 4 ||
        pcm_out_size == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    state->seen = 1;
    if (state->seen_count >= 0 &&
        (size_t)state->seen_count < sizeof(state->seen_first_bytes) / sizeof(state->seen_first_bytes[0])) {
        state->seen_first_bytes[state->seen_count] = payload[0];
    }
    state->seen_count++;
    state->ssrc = ssrc;
    state->payload_size = payload_size;
    pcm_out[0] = 1;
    pcm_out[1] = 2;
    pcm_out[2] = 3;
    pcm_out[3] = 4;
    *pcm_out_size = 4;
    return DCC_OK;
}

dcc_status_t voice_encode_pcm(
    void *user_data,
    const int16_t *pcm,
    size_t pcm_sample_count,
    uint64_t duration_ms,
    uint8_t *opus_out,
    size_t opus_out_cap,
    size_t *opus_out_size
) {
    voice_encode_state_t *state = (voice_encode_state_t *)user_data;
    if (state == NULL ||
        pcm == NULL ||
        pcm_sample_count != 8 ||
        duration_ms != 20 ||
        opus_out == NULL ||
        opus_out_cap < 6 ||
        opus_out_size == NULL ||
        pcm[0] != 100 ||
        pcm[7] != 107) {
        return DCC_ERR_INVALID_ARG;
    }

    state->seen = 1;
    state->sample_count = pcm_sample_count;
    state->duration_ms = duration_ms;
    opus_out[0] = 0xf8;
    opus_out[1] = 0xff;
    opus_out[2] = 0xfe;
    opus_out[3] = 0x01;
    opus_out[4] = 0x02;
    opus_out[5] = 0x03;
    *opus_out_size = 6;
    return DCC_OK;
}

void voice_cancel_first(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    voice_cancel_state_t *state = (voice_cancel_state_t *)user_data;
    if (dcc_event_type(event) == DCC_EVENT_VOICE_READY && !dcc_event_is_cancelled(event)) {
        state->first_seen = 1;
        dcc_event_cancel(event);
        state->first_cancelled = dcc_event_is_cancelled(event) ? 1 : 0;
    }
}

void voice_cancel_second(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)event;
    voice_cancel_state_t *state = (voice_cancel_state_t *)user_data;
    state->second_seen = 1;
}

static int voice_smoke_common_ok(
    voice_smoke_state_t *state,
    dcc_client_t *client,
    const dcc_event_t *event,
    dcc_event_type_t type
) {
    const char *name = dcc_event_name(event);
    const char *type_name = dcc_event_type_name(type);
    if (client != state->client) {
        voice_smoke_fail(state, "client mismatch");
        return 0;
    }
    if (dcc_event_type(event) != type) {
        voice_smoke_fail(state, "event type mismatch");
        return 0;
    }
    if (dcc_event_shard_id(event) != state->shard_id) {
        voice_smoke_fail(state, "event shard mismatch");
        return 0;
    }
    if (dcc_event_raw(event) != NULL) {
        voice_smoke_fail(state, "voice internal event exposed raw gateway data");
        return 0;
    }
    if (dcc_event_data(event) != NULL) {
        voice_smoke_fail(state, "voice internal event exposed gateway data");
        return 0;
    }
    if (name == NULL || type_name == NULL || strcmp(name, type_name) != 0) {
        voice_smoke_fail(state, "event name mismatch");
        return 0;
    }
    return 1;
}

void voice_smoke_on_event(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    voice_smoke_state_t *state = (voice_smoke_state_t *)user_data;
    switch (dcc_event_type(event)) {
        case DCC_EVENT_VOICE_BUFFER_SEND: {
            const dcc_voice_buffer_send_event_t *voice = dcc_event_voice_buffer_send(event);
            if (voice_smoke_common_ok(state, client, event, DCC_EVENT_VOICE_BUFFER_SEND) &&
                voice != NULL &&
                voice->voice_client == state->voice_client) {
                if (voice->buffer_size == 4096 && voice->packets_left == 1) {
                    state->buffer_send_seen = 1;
                } else if (voice->buffer_size == 8 && voice->packets_left == 0) {
                    state->udp_buffer_send_seen = 1;
                } else {
                    voice_smoke_fail(state, "VOICE_BUFFER_SEND payload mismatch");
                }
            } else {
                voice_smoke_fail(state, "VOICE_BUFFER_SEND payload mismatch");
            }
            break;
        }
        case DCC_EVENT_VOICE_READY: {
            const dcc_voice_ready_event_t *voice = dcc_event_voice_ready(event);
            if (voice_smoke_common_ok(state, client, event, DCC_EVENT_VOICE_READY) &&
                voice != NULL &&
                voice->voice_client == state->voice_client &&
                voice->voice_channel_id == 222) {
                state->ready_seen = 1;
            } else {
                voice_smoke_fail(state, "VOICE_READY payload mismatch");
            }
            break;
        }
        case DCC_EVENT_VOICE_RECEIVE: {
            const dcc_voice_receive_event_t *voice = dcc_event_voice_receive(event);
            if (voice_smoke_common_ok(state, client, event, DCC_EVENT_VOICE_RECEIVE) &&
                voice != NULL &&
                voice->voice_client == state->voice_client &&
                voice->user_id == 444 &&
                voice->audio != NULL &&
                memcmp(voice->audio, state->receive_audio, 4) == 0 &&
                voice->audio_size == 4) {
                state->receive_seen = 1;
            } else {
                voice_smoke_fail(state, "VOICE_RECEIVE payload mismatch");
            }
            break;
        }
        case DCC_EVENT_VOICE_RECEIVE_COMBINED: {
            const dcc_voice_receive_event_t *voice = dcc_event_voice_receive(event);
            if (voice_smoke_common_ok(state, client, event, DCC_EVENT_VOICE_RECEIVE_COMBINED) &&
                voice != NULL &&
                voice->voice_client == state->voice_client &&
                voice->user_id == 0 &&
                voice->audio != NULL) {
                if (voice->audio_size == 2 && memcmp(voice->audio, state->combined_audio, 2) == 0) {
                    state->receive_combined_seen = 1;
                } else if (state->mixed_audio != NULL &&
                    voice->audio_size == state->mixed_audio_size &&
                    memcmp(voice->audio, state->mixed_audio, state->mixed_audio_size) == 0) {
                    state->mixed_combined_seen = 1;
                } else {
                    voice_smoke_fail(state, "VOICE_RECEIVE_COMBINED payload mismatch");
                }
            } else {
                voice_smoke_fail(state, "VOICE_RECEIVE_COMBINED payload mismatch");
            }
            break;
        }
        case DCC_EVENT_VOICE_TRACK_MARKER: {
            const dcc_voice_track_marker_event_t *voice = dcc_event_voice_track_marker(event);
            if (voice_smoke_common_ok(state, client, event, DCC_EVENT_VOICE_TRACK_MARKER) &&
                voice != NULL &&
                voice->voice_client == state->voice_client &&
                voice->track_meta != NULL &&
                strcmp(voice->track_meta, "marker-1") == 0) {
                state->track_marker_seen = 1;
            } else {
                voice_smoke_fail(state, "VOICE_TRACK_MARKER payload mismatch");
            }
            break;
        }
        case DCC_EVENT_VOICE_CLIENT_SPEAKING: {
            const dcc_voice_client_speaking_event_t *voice = dcc_event_voice_client_speaking(event);
            if (voice_smoke_common_ok(state, client, event, DCC_EVENT_VOICE_CLIENT_SPEAKING) &&
                voice != NULL &&
                voice->voice_client == state->voice_client &&
                voice->user_id == 444 &&
                voice->ssrc == 123456) {
                state->speaking_seen = 1;
            } else {
                voice_smoke_fail(state, "VOICE_CLIENT_SPEAKING payload mismatch");
            }
            break;
        }
        case DCC_EVENT_VOICE_CLIENT_DISCONNECT: {
            const dcc_voice_client_disconnect_event_t *voice = dcc_event_voice_client_disconnect(event);
            if (voice_smoke_common_ok(state, client, event, DCC_EVENT_VOICE_CLIENT_DISCONNECT) &&
                voice != NULL &&
                voice->voice_client == state->voice_client &&
                voice->user_id == 445) {
                state->disconnect_seen = 1;
            } else {
                voice_smoke_fail(state, "VOICE_CLIENT_DISCONNECT payload mismatch");
            }
            break;
        }
        case DCC_EVENT_VOICE_CLIENT_PLATFORM: {
            const dcc_voice_client_platform_event_t *voice = dcc_event_voice_client_platform(event);
            if (voice_smoke_common_ok(state, client, event, DCC_EVENT_VOICE_CLIENT_PLATFORM) &&
                voice != NULL &&
                voice->voice_client == state->voice_client &&
                voice->user_id == 446 &&
                voice->platform == DCC_VOICE_CLIENT_PLATFORM_MOBILE) {
                state->platform_seen = 1;
            } else {
                voice_smoke_fail(state, "VOICE_CLIENT_PLATFORM payload mismatch");
            }
            break;
        }
        default:
            voice_smoke_fail(state, "unexpected event");
            break;
    }
}

int voice_smoke_add_listener(
    dcc_client_t *client,
    dcc_event_type_t type,
    voice_smoke_state_t *state
) {
    dcc_status_t st = dcc_client_on(client, type, voice_smoke_on_event, state, NULL);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_on %s failed: %s\n", dcc_event_type_name(type), dcc_status_string(st));
        return 0;
    }
    return 1;
}

#if !defined(_WIN32)
static void voice_auto_jitter_on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)event;
    voice_auto_jitter_state_t *state = (voice_auto_jitter_state_t *)user_data;
    dcc_voice_client_t *voice_client = state->event_state.voice_client;

    if (dcc_voice_client_start_receive_jitter_timer(voice_client, 1) != DCC_OK ||
        !dcc_voice_client_receive_jitter_timer_running(voice_client) ||
        dcc_voice_client_receive_jitter_timer_interval(voice_client) != 1 ||
        dcc_voice_client_receive_rtp_packet_at(voice_client, state->packet, state->packet_size, 1000) != DCC_OK ||
        state->decode_state.seen_count != 0) {
        state->setup_failed = 1;
        (void)dcc_client_stop(state->event_state.client);
    }
}

static void *voice_timer_stop_thread(void *arg) {
    voice_timer_stop_state_t *state = (voice_timer_stop_state_t *)arg;
    usleep(state->delay_us);
    if (state->voice_client != NULL) {
        (void)dcc_voice_client_stop_receive_jitter_timer(state->voice_client);
    }
    if (state->client != NULL) {
        (void)dcc_client_stop(state->client);
    }
    return NULL;
}

int voice_auto_jitter_timer_smoke(
    const uint8_t *packet,
    size_t packet_size,
    const uint8_t *receive_audio
) {
    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice_client = NULL;
    pthread_t stopper;
    int stopper_started = 0;
    int failed = 1;

    dcc_client_options_t opts = {
        .size = sizeof(opts),
    };
    dcc_status_t status = dcc_client_create(&opts, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "auto jitter client create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }

    status = dcc_voice_client_create(client, &voice_client);
    if (status != DCC_OK) {
        fprintf(stderr, "auto jitter voice create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }

    voice_auto_jitter_state_t auto_state;
    memset(&auto_state, 0, sizeof(auto_state));
    auto_state.event_state.client = client;
    auto_state.event_state.voice_client = voice_client;
    auto_state.event_state.receive_audio = receive_audio;
    auto_state.event_state.shard_id = 0;
    auto_state.decode_state.expected_payload_size = 32;
    auto_state.packet = packet;
    auto_state.packet_size = packet_size;

    if (dcc_client_on(client, DCC_EVENT_READY, voice_auto_jitter_on_ready, &auto_state, NULL) != DCC_OK ||
        dcc_client_on(client, DCC_EVENT_VOICE_RECEIVE, voice_smoke_on_event, &auto_state.event_state, NULL) !=
            DCC_OK ||
        dcc_voice_client_map_ssrc(voice_client, 123456, 444) != DCC_OK ||
        dcc_voice_client_set_encryption_mode(voice_client, DCC_VOICE_ENCRYPTION_NONE) != DCC_OK ||
        dcc_voice_client_set_decoder(voice_client, voice_decode_pcm, &auto_state.decode_state) != DCC_OK ||
        dcc_voice_client_set_receive_reorder_window(voice_client, 2) != DCC_OK ||
        dcc_voice_client_set_receive_jitter_delay(voice_client, 20) != DCC_OK ||
        dcc_client_start(client) != DCC_OK) {
        fprintf(stderr, "auto jitter setup failed\n");
        goto cleanup;
    }

    voice_timer_stop_state_t stop_state = {
        .client = client,
        .voice_client = voice_client,
        .delay_us = 200000U,
    };
    if (pthread_create(&stopper, NULL, voice_timer_stop_thread, &stop_state) != 0) {
        fprintf(stderr, "auto jitter stopper thread failed\n");
        goto cleanup;
    }
    stopper_started = 1;

    status = dcc_client_wait(client);
    pthread_join(stopper, NULL);
    stopper_started = 0;
    if (status != DCC_OK ||
        dcc_voice_client_stop_receive_jitter_timer(voice_client) != DCC_OK ||
        dcc_voice_client_receive_jitter_timer_running(voice_client) ||
        auto_state.setup_failed ||
        auto_state.event_state.failed ||
        !auto_state.event_state.receive_seen ||
        auto_state.decode_state.seen_count != 1) {
        fprintf(stderr, "auto jitter runtime failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }

    failed = 0;

cleanup:
    if (stopper_started) {
        (void)dcc_voice_client_stop_receive_jitter_timer(voice_client);
        (void)dcc_client_stop(client);
        pthread_join(stopper, NULL);
    }
    if (voice_client != NULL) {
        (void)dcc_voice_client_stop_receive_jitter_timer(voice_client);
        dcc_voice_client_destroy(voice_client);
    }
    if (client != NULL) {
        dcc_client_destroy(client);
    }
    return failed;
}
#endif
