#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

static int dcc_voice_mixer_has_source(const dcc_voice_client_t *voice_client, uint32_t ssrc) {
    if (voice_client == NULL || ssrc == 0) {
        return 0;
    }
    for (size_t i = 0; i < voice_client->mix_source_count; ++i) {
        if (voice_client->mix_ssrcs[i] == ssrc) {
            return 1;
        }
    }
    return 0;
}

dcc_status_t dcc_voice_client_mix_pcm(
    dcc_voice_client_t *voice_client,
    uint32_t ssrc,
    const int16_t *pcm,
    size_t pcm_sample_count
) {
    if (voice_client == NULL || ssrc == 0 || pcm == NULL || pcm_sample_count == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (pcm_sample_count > DCC_VOICE_MIX_SAMPLE_CAP) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_voice_client_user_for_ssrc(voice_client, ssrc) == 0) {
        return DCC_ERR_STATE;
    }

    int new_source = !dcc_voice_mixer_has_source(voice_client, ssrc);
    if (new_source && voice_client->mix_source_count >= DCC_VOICE_MIX_SOURCE_CAP) {
        return DCC_ERR_NOMEM;
    }

    size_t old_sample_count = voice_client->mix_sample_count;
    if (old_sample_count == 0) {
        memset(
            voice_client->mix_accumulator,
            0,
            pcm_sample_count * sizeof(voice_client->mix_accumulator[0])
        );
        voice_client->mix_sample_count = pcm_sample_count;
    } else if (pcm_sample_count > old_sample_count) {
        memset(
            voice_client->mix_accumulator + old_sample_count,
            0,
            (pcm_sample_count - old_sample_count) * sizeof(voice_client->mix_accumulator[0])
        );
        voice_client->mix_sample_count = pcm_sample_count;
    }

    if (new_source) {
        voice_client->mix_ssrcs[voice_client->mix_source_count++] = ssrc;
    }

    for (size_t i = 0; i < pcm_sample_count; ++i) {
        voice_client->mix_accumulator[i] += (int32_t)pcm[i];
    }
    return DCC_OK;
}
