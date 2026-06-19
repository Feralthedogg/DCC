#ifndef DCC_VOICE_CRYPTO_INTERNAL_H
#define DCC_VOICE_CRYPTO_INTERNAL_H

#include "internal/voice/dcc_voice_protocol_internal.h"

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_voice_hchacha20(
    uint8_t out[DCC_VOICE_SECRET_KEY_SIZE],
    const uint8_t key[DCC_VOICE_SECRET_KEY_SIZE],
    const uint8_t nonce[16]
);

dcc_status_t dcc_voice_xchacha20poly1305(
    int encrypt,
    uint8_t *out,
    size_t out_cap,
    size_t *out_len,
    const uint8_t *input,
    size_t input_len,
    const uint8_t *aad,
    size_t aad_len,
    const uint8_t nonce[DCC_VOICE_XCHACHA20_NONCE_SIZE],
    const uint8_t key[DCC_VOICE_SECRET_KEY_SIZE]
);

#ifdef __cplusplus
}
#endif

#endif
