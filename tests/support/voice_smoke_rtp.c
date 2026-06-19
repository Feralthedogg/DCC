#include "voice_smoke_support.h"

#include <stdio.h>
#include <string.h>

int voice_rtp_reorder_jitter_smoke(
    dcc_voice_client_t *voice_client,
    voice_decode_state_t *decode_state,
    dcc_voice_client_stats_t *voice_stats,
    const char *voice_endpoint_frame,
    const char *voice_speaking_frame,
    const uint8_t *voice_rtp_packet,
    size_t voice_rtp_packet_size,
    const uint8_t *receive_audio
) {
    if (voice_rtp_packet == NULL || voice_rtp_packet_size < 13U || voice_rtp_packet_size > 256U) {
        fprintf(stderr, "voice rtp fixture size invalid\n");
        return 1;
    }

    uint8_t voice_reorder_a[256];
    uint8_t voice_reorder_b[256];
    uint8_t voice_reorder_c[256];
    uint8_t voice_loss_a[256];
    uint8_t voice_loss_c[256];
    memcpy(voice_reorder_a, voice_rtp_packet, voice_rtp_packet_size);
    memcpy(voice_reorder_b, voice_rtp_packet, voice_rtp_packet_size);
    memcpy(voice_reorder_c, voice_rtp_packet, voice_rtp_packet_size);
    memcpy(voice_loss_a, voice_rtp_packet, voice_rtp_packet_size);
    memcpy(voice_loss_c, voice_rtp_packet, voice_rtp_packet_size);
    voice_reorder_a[2] = 0x00;
    voice_reorder_a[3] = 0x2a;
    voice_reorder_a[12] = 0xa1;
    voice_reorder_b[2] = 0x00;
    voice_reorder_b[3] = 0x2b;
    voice_reorder_b[12] = 0xa2;
    voice_reorder_c[2] = 0x00;
    voice_reorder_c[3] = 0x2c;
    voice_reorder_c[12] = 0xa3;
    voice_loss_a[2] = 0x00;
    voice_loss_a[3] = 0x40;
    voice_loss_a[12] = 0xb0;
    voice_loss_c[2] = 0x00;
    voice_loss_c[3] = 0x42;
    voice_loss_c[12] = 0xb2;

    memset(decode_state, 0, sizeof(*decode_state));
    decode_state->expected_payload_size = 32;
    uint32_t dropped_before = dcc_voice_client_receive_packets_dropped(voice_client);
    if (dcc_voice_client_handle_gateway_frame(voice_client, voice_endpoint_frame, strlen(voice_endpoint_frame)) != DCC_OK ||
        dcc_voice_client_handle_gateway_frame(voice_client, voice_speaking_frame, strlen(voice_speaking_frame)) != DCC_OK ||
        dcc_voice_client_set_encryption_mode(voice_client, DCC_VOICE_ENCRYPTION_NONE) != DCC_OK ||
        dcc_voice_client_set_decoder(voice_client, voice_decode_pcm, decode_state) != DCC_OK ||
        dcc_voice_client_set_receive_reorder_window(voice_client, 2) != DCC_OK ||
        dcc_voice_client_receive_reorder_window(voice_client) != 2 ||
        dcc_voice_client_receive_rtp_packet(voice_client, voice_reorder_a, voice_rtp_packet_size) != DCC_OK ||
        decode_state->seen_count != 1 ||
        dcc_voice_client_receive_rtp_packet(voice_client, voice_reorder_c, voice_rtp_packet_size) != DCC_OK ||
        decode_state->seen_count != 1 ||
        dcc_voice_client_receive_rtp_packet(voice_client, voice_reorder_b, voice_rtp_packet_size) != DCC_OK ||
        decode_state->seen_count != 3 ||
        decode_state->seen_first_bytes[0] != 0xa1 ||
        decode_state->seen_first_bytes[1] != 0xa2 ||
        decode_state->seen_first_bytes[2] != 0xa3 ||
        dcc_voice_client_receive_rtp_packet(voice_client, voice_reorder_b, voice_rtp_packet_size) != DCC_OK ||
        dcc_voice_client_receive_packets_dropped(voice_client) != dropped_before + 1U ||
        dcc_voice_client_clear_receive_reorder(voice_client) != DCC_OK) {
        fprintf(stderr, "voice rtp reorder pipeline failed\n");
        return 1;
    }

    memset(decode_state, 0, sizeof(*decode_state));
    decode_state->expected_payload_size = 32;
    uint32_t lost_before = dcc_voice_client_receive_packets_lost(voice_client);
    if (dcc_voice_client_set_receive_reorder_window(voice_client, 1) != DCC_OK ||
        dcc_voice_client_receive_rtp_packet(voice_client, voice_loss_a, voice_rtp_packet_size) != DCC_OK ||
        decode_state->seen_count != 1 ||
        dcc_voice_client_receive_rtp_packet(voice_client, voice_loss_c, voice_rtp_packet_size) != DCC_OK ||
        decode_state->seen_count != 1 ||
        dcc_voice_client_flush_receive_reorder(voice_client) != DCC_OK ||
        decode_state->seen_count != 2 ||
        decode_state->seen_first_bytes[0] != 0xb0 ||
        decode_state->seen_first_bytes[1] != 0xb2 ||
        dcc_voice_client_receive_packets_lost(voice_client) != lost_before + 1U ||
        dcc_voice_client_set_receive_reorder_window(voice_client, 0) != DCC_OK ||
        dcc_voice_client_receive_reorder_window(voice_client) != 0) {
        fprintf(stderr, "voice rtp loss pipeline failed\n");
        return 1;
    }

    memset(decode_state, 0, sizeof(*decode_state));
    decode_state->expected_payload_size = 32;
    lost_before = dcc_voice_client_receive_packets_lost(voice_client);
    if (dcc_voice_client_set_receive_reorder_window(voice_client, 2) != DCC_OK ||
        dcc_voice_client_set_receive_jitter_delay(voice_client, 20) != DCC_OK ||
        dcc_voice_client_receive_jitter_delay(voice_client) != 20 ||
        dcc_voice_client_receive_rtp_packet_at(voice_client, voice_reorder_a, voice_rtp_packet_size, 1000) !=
            DCC_OK ||
        decode_state->seen_count != 0 ||
        dcc_voice_client_tick_receive_jitter(voice_client, 1019) != DCC_OK ||
        decode_state->seen_count != 0 ||
        dcc_voice_client_tick_receive_jitter(voice_client, 1020) != DCC_OK ||
        decode_state->seen_count != 1 ||
        decode_state->seen_first_bytes[0] != 0xa1 ||
        dcc_voice_client_receive_rtp_packet_at(voice_client, voice_reorder_c, voice_rtp_packet_size, 2000) !=
            DCC_OK ||
        decode_state->seen_count != 1 ||
        dcc_voice_client_tick_receive_jitter(voice_client, 2019) != DCC_OK ||
        decode_state->seen_count != 1 ||
        dcc_voice_client_tick_receive_jitter(voice_client, 2020) != DCC_OK ||
        decode_state->seen_count != 2 ||
        decode_state->seen_first_bytes[1] != 0xa3 ||
        dcc_voice_client_receive_packets_lost(voice_client) != lost_before + 1U ||
        dcc_voice_client_set_receive_jitter_delay(voice_client, 0) != DCC_OK ||
        dcc_voice_client_receive_jitter_delay(voice_client) != 0 ||
        dcc_voice_client_set_receive_reorder_window(voice_client, 0) != DCC_OK ||
        (voice_stats->size = sizeof(*voice_stats),
         dcc_voice_client_stats(voice_client, voice_stats)) != DCC_OK ||
        voice_stats->receive_packets_lost != dcc_voice_client_receive_packets_lost(voice_client) ||
        voice_stats->receive_packets_dropped != dcc_voice_client_receive_packets_dropped(voice_client) ||
        voice_stats->receive_reorder_window != 0 ||
        voice_stats->receive_reorder_buffered != 0 ||
        voice_stats->receive_jitter_delay_ms != 0) {
        fprintf(stderr, "voice timed jitter pipeline failed\n");
        return 1;
    }

#if !defined(_WIN32)
    if (voice_auto_jitter_timer_smoke(voice_reorder_a, voice_rtp_packet_size, receive_audio) != 0) {
        fprintf(stderr, "voice automatic jitter timer failed\n");
        return 1;
    }
#else
    (void)receive_audio;
#endif

    return 0;
}
