#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_receive_pcm(
    dcc_voice_client_t *voice_client,
    uint32_t ssrc,
    const uint8_t *audio,
    size_t audio_size
) {
    if (voice_client == NULL || ssrc == 0 || (audio == NULL && audio_size != 0)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_snowflake_t user_id = dcc_voice_client_user_for_ssrc(voice_client, ssrc);
    if (user_id == 0) {
        return DCC_ERR_STATE;
    }
    return dcc_voice_client_notify_receive(voice_client, user_id, audio, audio_size);
}

dcc_status_t dcc_voice_client_receive_combined_pcm(
    dcc_voice_client_t *voice_client,
    const uint8_t *audio,
    size_t audio_size
) {
    return dcc_voice_client_notify_receive_combined(voice_client, audio, audio_size);
}

dcc_status_t dcc_voice_client_set_decoder(
    dcc_voice_client_t *voice_client,
    dcc_voice_decode_fn decoder,
    void *decoder_user_data
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    voice_client->decoder = decoder;
    voice_client->decoder_user_data = decoder_user_data;
    return DCC_OK;
}

dcc_status_t dcc_voice_client_set_encoder(
    dcc_voice_client_t *voice_client,
    dcc_voice_encode_fn encoder,
    void *encoder_user_data
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    voice_client->encoder = encoder;
    voice_client->encoder_user_data = encoder_user_data;
    return DCC_OK;
}

