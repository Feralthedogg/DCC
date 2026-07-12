#include "internal/voice/dcc_voice_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_voice_client_process_rtp_packet(
    dcc_voice_client_t *voice_client,
    const uint8_t *packet,
    size_t packet_size
) {
    if (voice_client == NULL || packet == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_rtp_media_view_t view;
    dcc_status_t parse_status = dcc_voice_rtp_media_view_parse(
        &view,
        voice_client->encryption_mode,
        packet,
        packet_size
    );
    if (parse_status != DCC_OK) {
        return parse_status;
    }
    if (!view.should_decode || view.payload_size == 0) {
        return DCC_OK;
    }

    dcc_snowflake_t user_id = dcc_voice_client_user_for_ssrc(voice_client, view.ssrc);
    if (user_id == 0) {
        return DCC_ERR_STATE;
    }
    if (voice_client->decoder == NULL) {
        return DCC_ERR_STATE;
    }

    const uint8_t *opus_payload = view.payload;
    uint8_t decrypted[DCC_VOICE_PCM_BUFFER_CAP];
    size_t opus_payload_size = view.payload_size;
    if (voice_client->encryption_mode == DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE) {
        if (!voice_client->has_secret_key ||
            view.payload_size <= DCC_VOICE_AEAD_TAG_SIZE + DCC_VOICE_RTPSIZE_NONCE_SIZE) {
            return DCC_ERR_STATE;
        }

        uint8_t nonce[DCC_VOICE_XCHACHA20_NONCE_SIZE];
        memset(nonce, 0, sizeof(nonce));
        memcpy(
            nonce,
            view.packet + view.packet_size - DCC_VOICE_RTPSIZE_NONCE_SIZE,
            DCC_VOICE_RTPSIZE_NONCE_SIZE
        );

        size_t ciphertext_size = view.payload_size - DCC_VOICE_RTPSIZE_NONCE_SIZE;
        size_t decrypted_size = 0;
        dcc_status_t crypto_status = dcc_voice_xchacha20poly1305(
            0,
            decrypted,
            sizeof(decrypted),
            &decrypted_size,
            view.payload,
            ciphertext_size,
            view.packet,
            view.aad_size,
            nonce,
            voice_client->secret_key
        );
        if (crypto_status != DCC_OK) {
            return crypto_status;
        }
        if (view.encrypted_extension_size > decrypted_size) {
            return DCC_ERR_INVALID_ARG;
        }
        opus_payload = decrypted + view.encrypted_extension_size;
        opus_payload_size = decrypted_size - view.encrypted_extension_size;
    }
    if (opus_payload_size == 0) {
        return DCC_OK;
    }

    uint8_t *dave_plain = NULL;
    if (voice_client->dave_enabled) {
        dcc_status_t dave_status = dcc_voice_dave_backend_decrypt(
            voice_client,
            user_id,
            opus_payload,
            opus_payload_size,
            &dave_plain,
            &opus_payload_size
        );
        if (dave_status != DCC_OK) {
            return dave_status;
        }
        opus_payload = dave_plain;
    }

    union {
        uint8_t bytes[DCC_VOICE_PCM_BUFFER_CAP];
        int16_t align_i16;
    } pcm;
    size_t pcm_size = 0;
    dcc_status_t status = voice_client->decoder(
        voice_client->decoder_user_data,
        view.ssrc,
        opus_payload,
        opus_payload_size,
        pcm.bytes,
        sizeof(pcm.bytes),
        &pcm_size
    );
    free(dave_plain);
    if (status != DCC_OK) {
        return status;
    }
    if (pcm_size > sizeof(pcm.bytes)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (pcm_size == 0) {
        return DCC_OK;
    }

    return dcc_voice_client_receive_pcm(voice_client, view.ssrc, pcm.bytes, pcm_size);
}
