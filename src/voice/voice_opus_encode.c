#include "internal/voice/dcc_voice_opus_internal.h"

#if defined(DCC_HAVE_OPUS)
#include <limits.h>

dcc_status_t dcc_voice_opus_encode(
    void *user_data,
    const int16_t *pcm,
    size_t pcm_sample_count,
    uint64_t duration_ms,
    uint8_t *opus_out,
    size_t opus_out_cap,
    size_t *opus_out_size
) {
    dcc_voice_opus_backend_t *backend = (dcc_voice_opus_backend_t *)user_data;
    if (backend == NULL ||
        backend->encoder == NULL ||
        pcm == NULL ||
        pcm_sample_count == 0 ||
        duration_ms == 0 ||
        opus_out == NULL ||
        opus_out_cap == 0 ||
        opus_out_size == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (backend->channels == 0 || pcm_sample_count % backend->channels != 0) {
        return DCC_ERR_INVALID_ARG;
    }

    uint64_t sample_rate = (uint64_t)backend->sample_rate;
    if (sample_rate == 0 || duration_ms > UINT64_MAX / sample_rate) {
        return DCC_ERR_INVALID_ARG;
    }

    uint64_t frame_samples_mul = sample_rate * duration_ms;
    if ((frame_samples_mul % 1000U) != 0) {
        return DCC_ERR_INVALID_ARG;
    }

    uint64_t frame_samples = frame_samples_mul / 1000U;
    if (frame_samples == 0 || frame_samples > (uint64_t)INT_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    if (pcm_sample_count / backend->channels != (size_t)frame_samples) {
        return DCC_ERR_INVALID_ARG;
    }

    int opus_cap = opus_out_cap > (size_t)INT_MAX ? INT_MAX : (int)opus_out_cap;
    opus_int32 encoded = opus_encode(
        backend->encoder,
        (const opus_int16 *)pcm,
        (int)frame_samples,
        opus_out,
        (opus_int32)opus_cap
    );
    if (encoded < 0) {
        return dcc_voice_status_from_opus((int)encoded);
    }
    if (encoded == 0) {
        return DCC_ERR_STATE;
    }

    *opus_out_size = (size_t)encoded;
    return DCC_OK;
}
#endif
