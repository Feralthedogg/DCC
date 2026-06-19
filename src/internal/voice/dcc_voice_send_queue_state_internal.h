#ifndef DCC_VOICE_SEND_QUEUE_STATE_INTERNAL_H
#define DCC_VOICE_SEND_QUEUE_STATE_INTERNAL_H

#include "internal/voice/dcc_voice_limits_internal.h"

#include <stddef.h>
#include <stdint.h>

typedef enum dcc_voice_send_item_type {
    DCC_VOICE_SEND_ITEM_PACKET = 1,
    DCC_VOICE_SEND_ITEM_MARKER = 2
} dcc_voice_send_item_type_t;

typedef struct dcc_voice_send_item {
    dcc_voice_send_item_type_t type;
    uint8_t *data;
    size_t size;
    uint64_t duration_ms;
    char *track_meta;
} dcc_voice_send_item_t;

#define DCC_VOICE_SEND_QUEUE_STATE_FIELDS \
    size_t send_queue_len; \
    dcc_voice_send_item_t send_queue[DCC_VOICE_SEND_QUEUE_CAP]

#endif
