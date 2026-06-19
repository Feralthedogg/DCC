#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

dcc_status_t dcc_voice_client_set_rtp_state(
    dcc_voice_client_t *voice_client,
    uint16_t sequence,
    uint32_t timestamp
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    voice_client->rtp_sequence = sequence;
    voice_client->rtp_timestamp = timestamp;
    return DCC_OK;
}

uint16_t dcc_voice_client_rtp_sequence(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->rtp_sequence : 0;
}

uint32_t dcc_voice_client_rtp_timestamp(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->rtp_timestamp : 0;
}

dcc_status_t dcc_voice_client_set_encryption_mode(
    dcc_voice_client_t *voice_client,
    dcc_voice_encryption_mode_t mode
) {
    if (voice_client == NULL ||
        (mode != DCC_VOICE_ENCRYPTION_NONE &&
         mode != DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE)) {
        return DCC_ERR_INVALID_ARG;
    }
    voice_client->encryption_mode = mode;
    return DCC_OK;
}

dcc_voice_encryption_mode_t dcc_voice_client_encryption_mode(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->encryption_mode : DCC_VOICE_ENCRYPTION_NONE;
}

dcc_status_t dcc_voice_client_set_secret_key(
    dcc_voice_client_t *voice_client,
    const uint8_t *key,
    size_t key_len
) {
    if (voice_client == NULL || key == NULL || key_len != DCC_VOICE_SECRET_KEY_SIZE) {
        return DCC_ERR_INVALID_ARG;
    }
    memcpy(voice_client->secret_key, key, DCC_VOICE_SECRET_KEY_SIZE);
    voice_client->has_secret_key = 1;
    return DCC_OK;
}

void dcc_voice_client_clear_secret_key(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return;
    }
    memset(voice_client->secret_key, 0, sizeof(voice_client->secret_key));
    voice_client->has_secret_key = 0;
}

uint8_t dcc_voice_client_has_secret_key(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->has_secret_key : 0;
}

dcc_status_t dcc_voice_client_set_packet_nonce(dcc_voice_client_t *voice_client, uint32_t nonce) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    voice_client->packet_nonce = nonce;
    return DCC_OK;
}

uint32_t dcc_voice_client_packet_nonce(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->packet_nonce : 0;
}
