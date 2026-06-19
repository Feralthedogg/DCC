#include "internal/voice/dcc_voice_internal.h"

size_t dcc_voice_reorder_count(const dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < DCC_VOICE_REORDER_QUEUE_CAP; ++i) {
        if (voice_client->receive_reorder[i].used) {
            count++;
        }
    }
    return count;
}

dcc_voice_reorder_slot_t *dcc_voice_reorder_find(dcc_voice_client_t *voice_client, uint16_t sequence) {
    if (voice_client == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < DCC_VOICE_REORDER_QUEUE_CAP; ++i) {
        dcc_voice_reorder_slot_t *slot = &voice_client->receive_reorder[i];
        if (slot->used && slot->sequence == sequence) {
            return slot;
        }
    }
    return NULL;
}

dcc_voice_reorder_slot_t *dcc_voice_reorder_free_slot(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < DCC_VOICE_REORDER_QUEUE_CAP; ++i) {
        dcc_voice_reorder_slot_t *slot = &voice_client->receive_reorder[i];
        if (!slot->used) {
            return slot;
        }
    }
    return NULL;
}
