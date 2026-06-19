#ifndef DCC_VOICE_SEND_QUEUE_INTERNAL_H
#define DCC_VOICE_SEND_QUEUE_INTERNAL_H

#include "internal/voice/dcc_voice_send_queue_state_internal.h"

#include <dcc/dcc.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t dcc_voice_send_queue_marker_count(const dcc_voice_client_t *voice_client);
dcc_status_t dcc_voice_send_queue_pop_front(dcc_voice_client_t *voice_client);
dcc_status_t dcc_voice_send_queue_push(dcc_voice_client_t *voice_client, dcc_voice_send_item_t *item);
dcc_status_t dcc_voice_client_send_queued_packet(
    dcc_voice_client_t *voice_client,
    const dcc_voice_send_item_t *item,
    uint16_t sequence,
    uint32_t timestamp
);

#ifdef __cplusplus
}
#endif

#endif
