#ifndef DCC_VOICE_OPUS_INTERNAL_H
#define DCC_VOICE_OPUS_INTERNAL_H

#include "internal/voice/dcc_voice_internal.h"

typedef struct dcc_voice_opus_config {
    uint32_t sample_rate;
    uint8_t channels;
    dcc_voice_opus_application_t application;
} dcc_voice_opus_config_t;

dcc_status_t dcc_voice_opus_resolve_options(
    const dcc_voice_opus_options_t *options,
    dcc_voice_opus_config_t *out
);

#if defined(DCC_HAVE_OPUS)
#include <opus/opus.h>

typedef struct dcc_voice_opus_backend {
    OpusEncoder *encoder;
    OpusDecoder *decoder;
    uint32_t sample_rate;
    uint8_t channels;
    dcc_voice_opus_application_t application;
} dcc_voice_opus_backend_t;

dcc_status_t dcc_voice_status_from_opus(int status);
dcc_status_t dcc_voice_opus_encode(
    void *user_data,
    const int16_t *pcm,
    size_t pcm_sample_count,
    uint64_t duration_ms,
    uint8_t *opus_out,
    size_t opus_out_cap,
    size_t *opus_out_size
);
dcc_status_t dcc_voice_opus_decode(
    void *user_data,
    uint32_t ssrc,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *pcm_out,
    size_t pcm_out_cap,
    size_t *pcm_out_size
);
#endif

#endif
