#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

dcc_status_t dcc_voice_client_clear_receive_reorder(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(voice_client->receive_reorder, 0, sizeof(voice_client->receive_reorder));
    voice_client->receive_reorder_started = 0;
    voice_client->receive_expected_sequence = 0;
    return DCC_OK;
}

dcc_status_t dcc_voice_client_set_receive_reorder_window(
    dcc_voice_client_t *voice_client,
    uint8_t window_packets
) {
    if (voice_client == NULL || window_packets >= DCC_VOICE_REORDER_QUEUE_CAP) {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_client->receive_reorder_window != window_packets) {
        dcc_status_t status = dcc_voice_client_clear_receive_reorder(voice_client);
        if (status != DCC_OK) {
            return status;
        }
    }
    voice_client->receive_reorder_window = window_packets;
    return DCC_OK;
}

uint8_t dcc_voice_client_receive_reorder_window(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->receive_reorder_window : 0;
}

dcc_status_t dcc_voice_client_set_receive_jitter_delay(
    dcc_voice_client_t *voice_client,
    uint32_t delay_ms
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    voice_client->receive_jitter_delay_ms = delay_ms;
    return DCC_OK;
}

uint32_t dcc_voice_client_receive_jitter_delay(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->receive_jitter_delay_ms : 0;
}

dcc_status_t dcc_voice_client_flush_receive_reorder(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!voice_client->receive_reorder_started) {
        return DCC_OK;
    }
    return dcc_voice_reorder_drain(voice_client, 1, 0);
}

dcc_status_t dcc_voice_client_tick_receive_jitter(dcc_voice_client_t *voice_client, uint64_t now_ms) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!voice_client->receive_reorder_started) {
        return DCC_OK;
    }
    return dcc_voice_reorder_drain(voice_client, 0, now_ms);
}
