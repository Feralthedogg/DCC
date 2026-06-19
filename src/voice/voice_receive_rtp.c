#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

dcc_status_t dcc_voice_client_receive_rtp_packet_at(
    dcc_voice_client_t *voice_client,
    const uint8_t *packet,
    size_t packet_size,
    uint64_t now_ms
) {
    if (voice_client == NULL || packet == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_client->receive_reorder_window == 0) {
        return dcc_voice_client_process_rtp_packet(voice_client, packet, packet_size);
    }
    if (packet_size < DCC_VOICE_RTP_HEADER_SIZE || (packet[0] >> 6U) != 2U) {
        return DCC_ERR_INVALID_ARG;
    }

    uint8_t payload_type = (uint8_t)(packet[1] & 0x7FU);
    if (payload_type >= 72U && payload_type <= 76U) {
        return DCC_OK;
    }
    if (packet_size > DCC_VOICE_REORDER_PACKET_CAP) {
        return DCC_ERR_INVALID_ARG;
    }

    uint16_t sequence = dcc_voice_read_be16(packet + 2U);
    if (!voice_client->receive_reorder_started) {
        voice_client->receive_expected_sequence = sequence;
        voice_client->receive_reorder_started = 1U;
    } else if (dcc_voice_sequence_before(sequence, voice_client->receive_expected_sequence)) {
        dcc_voice_u32_saturating_inc(&voice_client->receive_packets_dropped);
        return DCC_OK;
    }

    if (dcc_voice_reorder_find(voice_client, sequence) != NULL) {
        dcc_voice_u32_saturating_inc(&voice_client->receive_packets_dropped);
        return DCC_OK;
    }

    uint16_t distance = dcc_voice_sequence_distance(voice_client->receive_expected_sequence, sequence);
    while (distance > voice_client->receive_reorder_window) {
        voice_client->receive_expected_sequence++;
        dcc_voice_u32_saturating_inc(&voice_client->receive_packets_lost);
        distance--;
    }

    dcc_voice_reorder_slot_t *slot = dcc_voice_reorder_free_slot(voice_client);
    if (slot == NULL) {
        dcc_voice_u32_saturating_inc(&voice_client->receive_packets_dropped);
        return DCC_ERR_NOMEM;
    }

    slot->used = 1U;
    slot->sequence = sequence;
    slot->release_at_ms = dcc_voice_jitter_release_at(now_ms, voice_client->receive_jitter_delay_ms);
    slot->size = packet_size;
    memcpy(slot->packet, packet, packet_size);
    return dcc_voice_reorder_drain(voice_client, 0, now_ms);
}

dcc_status_t dcc_voice_client_receive_rtp_packet(
    dcc_voice_client_t *voice_client,
    const uint8_t *packet,
    size_t packet_size
) {
    return dcc_voice_client_receive_rtp_packet_at(voice_client, packet, packet_size, dcc_voice_now_ms());
}
