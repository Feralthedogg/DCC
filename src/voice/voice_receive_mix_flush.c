#include "internal/voice/dcc_voice_internal.h"

#include <limits.h>
#include <string.h>

static int16_t dcc_voice_mix_clamp_i16(int32_t sample) {
    if (sample > INT16_MAX) {
        return INT16_MAX;
    }
    if (sample < INT16_MIN) {
        return INT16_MIN;
    }
    return (int16_t)sample;
}

dcc_status_t dcc_voice_client_clear_mixed_pcm(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_client->mix_sample_count > 0) {
        memset(
            voice_client->mix_accumulator,
            0,
            voice_client->mix_sample_count * sizeof(voice_client->mix_accumulator[0])
        );
    }
    if (voice_client->mix_source_count > 0) {
        memset(
            voice_client->mix_ssrcs,
            0,
            voice_client->mix_source_count * sizeof(voice_client->mix_ssrcs[0])
        );
    }
    voice_client->mix_sample_count = 0;
    voice_client->mix_source_count = 0;
    return DCC_OK;
}

dcc_status_t dcc_voice_client_flush_mixed_pcm(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_client->mix_sample_count == 0) {
        return DCC_OK;
    }

    union {
        int16_t samples[DCC_VOICE_MIX_SAMPLE_CAP];
        uint8_t bytes[DCC_VOICE_MIX_SAMPLE_CAP * sizeof(int16_t)];
    } mixed;

    size_t sample_count = voice_client->mix_sample_count;
    for (size_t i = 0; i < sample_count; ++i) {
        mixed.samples[i] = dcc_voice_mix_clamp_i16(voice_client->mix_accumulator[i]);
    }

    dcc_status_t status = dcc_voice_client_notify_receive_combined(
        voice_client,
        mixed.bytes,
        sample_count * sizeof(int16_t)
    );
    if (status == DCC_OK) {
        status = dcc_voice_client_clear_mixed_pcm(voice_client);
    }
    return status;
}

size_t dcc_voice_client_mixed_source_count(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->mix_source_count : 0;
}
