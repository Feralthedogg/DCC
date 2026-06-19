#ifndef DCC_VOICE_CODEC_STATE_INTERNAL_H
#define DCC_VOICE_CODEC_STATE_INTERNAL_H

#include <dcc/dcc.h>

#define DCC_VOICE_CODEC_STATE_FIELDS \
    dcc_voice_decode_fn decoder; \
    void *decoder_user_data; \
    dcc_voice_encode_fn encoder; \
    void *encoder_user_data; \
    void *opus_backend

#endif
