#include "internal/voice/dcc_voice_internal.h"

#include <stdlib.h>
#include <string.h>

#define DCC_VOICE_OPUS_PACKET_CAP 4096U

dcc_status_t dcc_voice_client_send_opus_packet(
    dcc_voice_client_t *voice_client,
    const uint8_t *packet,
    size_t packet_size,
    uint64_t duration_ms
) {
    if (voice_client == NULL || packet == NULL || packet_size == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    uint8_t *copy = (uint8_t *)malloc(packet_size);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(copy, packet, packet_size);

    dcc_voice_send_item_t item;
    memset(&item, 0, sizeof(item));
    item.type = DCC_VOICE_SEND_ITEM_PACKET;
    item.data = copy;
    item.size = packet_size;
    item.duration_ms = duration_ms;
    return dcc_voice_send_queue_push(voice_client, &item);
}

dcc_status_t dcc_voice_client_send_pcm(
    dcc_voice_client_t *voice_client,
    const int16_t *pcm,
    size_t pcm_sample_count,
    uint64_t duration_ms
) {
    if (voice_client == NULL || pcm == NULL || pcm_sample_count == 0 || duration_ms == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if ((pcm_sample_count % 2U) != 0 || pcm_sample_count > SIZE_MAX / sizeof(int16_t)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_client->encoder == NULL) {
        return DCC_ERR_STATE;
    }

    uint8_t opus_stack[DCC_VOICE_OPUS_PACKET_CAP];
    uint8_t *opus = opus_stack;
    uint8_t *opus_heap = NULL;
    size_t pcm_bytes = pcm_sample_count * sizeof(int16_t);
    size_t opus_cap = pcm_bytes > sizeof(opus_stack) ? pcm_bytes : sizeof(opus_stack);
    if (opus_cap > sizeof(opus_stack)) {
        opus_heap = (uint8_t *)malloc(opus_cap);
        if (opus_heap == NULL) {
            return DCC_ERR_NOMEM;
        }
        opus = opus_heap;
    }

    size_t opus_size = 0;
    dcc_status_t status = voice_client->encoder(
        voice_client->encoder_user_data,
        pcm,
        pcm_sample_count,
        duration_ms,
        opus,
        opus_cap,
        &opus_size
    );
    if (status != DCC_OK) {
        free(opus_heap);
        return status;
    }
    if (opus_size == 0 || opus_size > opus_cap) {
        free(opus_heap);
        return DCC_ERR_INVALID_ARG;
    }

    status = dcc_voice_client_send_opus_packet(voice_client, opus, opus_size, duration_ms);
    free(opus_heap);
    return status;
}
