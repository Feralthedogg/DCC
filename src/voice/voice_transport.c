#include "internal/voice/dcc_voice_internal.h"

static uint32_t dcc_voice_rtp_duration_samples(uint64_t duration_ms) {
    return (uint32_t)((duration_ms & UINT64_C(0xffffffff)) * DCC_VOICE_RTP_SAMPLE_RATE_PER_MS);
}

dcc_status_t dcc_voice_client_drain_send_queue(dcc_voice_client_t *voice_client, size_t max_packets) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t packets_sent = 0;
    while (voice_client->send_queue_len > 0 && (max_packets == 0 || packets_sent < max_packets)) {
        dcc_voice_send_item_t *item = &voice_client->send_queue[0];
        if (item->type == DCC_VOICE_SEND_ITEM_MARKER) {
            dcc_status_t status = dcc_voice_client_notify_track_marker(voice_client, item->track_meta);
            if (status != DCC_OK) {
                return status;
            }
            status = dcc_voice_send_queue_pop_front(voice_client);
            if (status != DCC_OK) {
                return status;
            }
            continue;
        }

        if (item->type != DCC_VOICE_SEND_ITEM_PACKET) {
            return DCC_ERR_STATE;
        }

        uint64_t buffer_size = (uint64_t)item->size;
        uint16_t next_sequence = (uint16_t)(voice_client->rtp_sequence + 1U);
        uint32_t next_timestamp =
            voice_client->rtp_timestamp + dcc_voice_rtp_duration_samples(item->duration_ms);
        dcc_status_t send_status = dcc_voice_client_send_queued_packet(
            voice_client,
            item,
            voice_client->rtp_sequence,
            voice_client->rtp_timestamp
        );
        if (send_status != DCC_OK) {
            return send_status;
        }

        dcc_status_t status = dcc_voice_send_queue_pop_front(voice_client);
        if (status != DCC_OK) {
            return status;
        }
        packets_sent++;
        voice_client->rtp_sequence = next_sequence;
        voice_client->rtp_timestamp = next_timestamp;
        if (voice_client->encryption_mode == DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE) {
            voice_client->packet_nonce++;
        }

        status = dcc_voice_client_notify_buffer_send(voice_client, buffer_size, voice_client->send_queue_len);
        if (status != DCC_OK) {
            return status;
        }
    }

    return DCC_OK;
}
