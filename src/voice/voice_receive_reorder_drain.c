#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

static dcc_voice_reorder_slot_t *dcc_voice_reorder_next_slot(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return NULL;
    }

    dcc_voice_reorder_slot_t *best = NULL;
    uint16_t best_distance = 0;
    for (size_t i = 0; i < DCC_VOICE_REORDER_QUEUE_CAP; ++i) {
        dcc_voice_reorder_slot_t *slot = &voice_client->receive_reorder[i];
        if (!slot->used || dcc_voice_sequence_before(slot->sequence, voice_client->receive_expected_sequence)) {
            continue;
        }
        uint16_t distance = dcc_voice_sequence_distance(voice_client->receive_expected_sequence, slot->sequence);
        if (best == NULL || distance < best_distance) {
            best = slot;
            best_distance = distance;
        }
    }
    return best;
}

dcc_status_t dcc_voice_reorder_drain(dcc_voice_client_t *voice_client, int flush, uint64_t now_ms) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    uint8_t packet[DCC_VOICE_REORDER_PACKET_CAP];
    for (;;) {
        dcc_voice_reorder_slot_t *slot =
            dcc_voice_reorder_find(voice_client, voice_client->receive_expected_sequence);
        if (slot != NULL) {
            if (!flush && voice_client->receive_jitter_delay_ms != 0 && now_ms < slot->release_at_ms) {
                return DCC_OK;
            }

            size_t packet_size = slot->size;
            memcpy(packet, slot->packet, packet_size);
            memset(slot, 0, sizeof(*slot));

            dcc_status_t status = dcc_voice_client_process_rtp_packet(voice_client, packet, packet_size);
            if (status != DCC_OK) {
                return status;
            }
            voice_client->receive_expected_sequence++;
            continue;
        }

        size_t count = dcc_voice_reorder_count(voice_client);
        if (count == 0) {
            return DCC_OK;
        }
        if (!flush) {
            dcc_voice_reorder_slot_t *next = dcc_voice_reorder_next_slot(voice_client);
            if (voice_client->receive_jitter_delay_ms != 0 &&
                next != NULL &&
                now_ms >= next->release_at_ms) {
                voice_client->receive_expected_sequence++;
                dcc_voice_u32_saturating_inc(&voice_client->receive_packets_lost);
                continue;
            }
            if (count <= (size_t)voice_client->receive_reorder_window) {
                return DCC_OK;
            }
        }

        voice_client->receive_expected_sequence++;
        dcc_voice_u32_saturating_inc(&voice_client->receive_packets_lost);
    }
}
