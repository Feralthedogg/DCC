#ifndef DCC_VOICE_TYPES_CODEC_H
#define DCC_VOICE_TYPES_CODEC_H

#include <dcc/voice/types/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_voice_opus_options {
    size_t size;
    uint32_t sample_rate;
    uint8_t channels;
    dcc_voice_opus_application_t application;
} dcc_voice_opus_options_t;

typedef dcc_status_t (*dcc_voice_decode_fn)(
    void *user_data,
    uint32_t ssrc,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *pcm_out,
    size_t pcm_out_cap,
    size_t *pcm_out_size
);

typedef dcc_status_t (*dcc_voice_encode_fn)(
    void *user_data,
    const int16_t *pcm,
    size_t pcm_sample_count,
    uint64_t duration_ms,
    uint8_t *opus_out,
    size_t opus_out_cap,
    size_t *opus_out_size
);

#ifdef __cplusplus
}
#endif

#endif
