#include "internal/voice/dcc_voice_opus_internal.h"

#if defined(DCC_HAVE_OPUS)
#include <limits.h>

dcc_status_t dcc_voice_opus_decode(
    void *user_data,
    uint32_t ssrc,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *pcm_out,
    size_t pcm_out_cap,
    size_t *pcm_out_size
) {
    (void)ssrc;
    dcc_voice_opus_backend_t *backend = (dcc_voice_opus_backend_t *)user_data;
    if (backend == NULL ||
        backend->decoder == NULL ||
        payload == NULL ||
        payload_size == 0 ||
        payload_size > (size_t)INT_MAX ||
        pcm_out == NULL ||
        pcm_out_size == NULL ||
        backend->channels == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t bytes_per_frame = sizeof(opus_int16) * (size_t)backend->channels;
    size_t frame_cap = bytes_per_frame != 0 ? pcm_out_cap / bytes_per_frame : 0;
    if (frame_cap == 0) {
        return DCC_ERR_NOMEM;
    }
    if (frame_cap > (size_t)INT_MAX) {
        frame_cap = (size_t)INT_MAX;
    }

    int decoded = opus_decode(
        backend->decoder,
        payload,
        (opus_int32)payload_size,
        (opus_int16 *)pcm_out,
        (int)frame_cap,
        0
    );
    if (decoded < 0) {
        return dcc_voice_status_from_opus(decoded);
    }
    if ((size_t)decoded > SIZE_MAX / (size_t)backend->channels) {
        return DCC_ERR_STATE;
    }

    size_t sample_count = (size_t)decoded * (size_t)backend->channels;
    if (sample_count > SIZE_MAX / sizeof(opus_int16)) {
        return DCC_ERR_STATE;
    }

    size_t byte_count = sample_count * sizeof(opus_int16);
    if (byte_count > pcm_out_cap) {
        return DCC_ERR_STATE;
    }

    *pcm_out_size = byte_count;
    return DCC_OK;
}
#endif
