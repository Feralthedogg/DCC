#ifndef DCC_VOICE_RECEIVE_INTERNAL_H
#define DCC_VOICE_RECEIVE_INTERNAL_H

#include "internal/voice/dcc_voice_receive_state_internal.h"

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_voice_rtp_media_view {
    const uint8_t *packet;
    size_t packet_size;
    const uint8_t *payload;
    size_t payload_size;
    size_t aad_size;
    size_t encrypted_extension_size;
    uint32_t ssrc;
    int should_decode;
} dcc_voice_rtp_media_view_t;

size_t dcc_voice_reorder_count(const dcc_voice_client_t *voice_client);
void dcc_voice_u32_saturating_inc(uint32_t *value);
int dcc_voice_sequence_before(uint16_t sequence, uint16_t expected);
uint16_t dcc_voice_sequence_distance(uint16_t expected, uint16_t sequence);
uint64_t dcc_voice_jitter_release_at(uint64_t now_ms, uint32_t delay_ms);
dcc_voice_reorder_slot_t *dcc_voice_reorder_find(dcc_voice_client_t *voice_client, uint16_t sequence);
dcc_voice_reorder_slot_t *dcc_voice_reorder_free_slot(dcc_voice_client_t *voice_client);
dcc_status_t dcc_voice_reorder_drain(dcc_voice_client_t *voice_client, int flush, uint64_t now_ms);
dcc_status_t dcc_voice_rtp_media_view_parse(
    dcc_voice_rtp_media_view_t *view,
    dcc_voice_encryption_mode_t encryption_mode,
    const uint8_t *packet,
    size_t packet_size
);
dcc_status_t dcc_voice_client_process_rtp_packet(
    dcc_voice_client_t *voice_client,
    const uint8_t *packet,
    size_t packet_size
);
void dcc_voice_receive_jitter_timer_task(void *arg);
void dcc_voice_receive_jitter_timer_cancel(dcc_voice_client_t *voice_client);

#ifdef __cplusplus
}
#endif

#endif
