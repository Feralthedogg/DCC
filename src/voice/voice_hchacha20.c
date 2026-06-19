#include "internal/voice/dcc_voice_internal.h"

static uint32_t dcc_voice_rotl32(uint32_t value, unsigned shift) {
    return (value << shift) | (value >> (32U - shift));
}

static uint32_t dcc_voice_load_le32(const uint8_t *data) {
    return (uint32_t)data[0] |
        ((uint32_t)data[1] << 8U) |
        ((uint32_t)data[2] << 16U) |
        ((uint32_t)data[3] << 24U);
}

static void dcc_voice_store_le32(uint8_t *data, uint32_t value) {
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8U);
    data[2] = (uint8_t)(value >> 16U);
    data[3] = (uint8_t)(value >> 24U);
}

static void dcc_voice_quarter_round(uint32_t *x, size_t a, size_t b, size_t c, size_t d) {
    x[a] += x[b];
    x[d] = dcc_voice_rotl32(x[d] ^ x[a], 16U);
    x[c] += x[d];
    x[b] = dcc_voice_rotl32(x[b] ^ x[c], 12U);
    x[a] += x[b];
    x[d] = dcc_voice_rotl32(x[d] ^ x[a], 8U);
    x[c] += x[d];
    x[b] = dcc_voice_rotl32(x[b] ^ x[c], 7U);
}

void dcc_voice_hchacha20(
    uint8_t out[DCC_VOICE_SECRET_KEY_SIZE],
    const uint8_t key[DCC_VOICE_SECRET_KEY_SIZE],
    const uint8_t nonce[16]
) {
    uint32_t x[16];
    x[0] = UINT32_C(0x61707865);
    x[1] = UINT32_C(0x3320646e);
    x[2] = UINT32_C(0x79622d32);
    x[3] = UINT32_C(0x6b206574);
    for (size_t i = 0; i < 8U; ++i) {
        x[4U + i] = dcc_voice_load_le32(key + i * 4U);
    }
    for (size_t i = 0; i < 4U; ++i) {
        x[12U + i] = dcc_voice_load_le32(nonce + i * 4U);
    }

    for (size_t i = 0; i < 10U; ++i) {
        dcc_voice_quarter_round(x, 0, 4, 8, 12);
        dcc_voice_quarter_round(x, 1, 5, 9, 13);
        dcc_voice_quarter_round(x, 2, 6, 10, 14);
        dcc_voice_quarter_round(x, 3, 7, 11, 15);
        dcc_voice_quarter_round(x, 0, 5, 10, 15);
        dcc_voice_quarter_round(x, 1, 6, 11, 12);
        dcc_voice_quarter_round(x, 2, 7, 8, 13);
        dcc_voice_quarter_round(x, 3, 4, 9, 14);
    }

    dcc_voice_store_le32(out + 0U, x[0]);
    dcc_voice_store_le32(out + 4U, x[1]);
    dcc_voice_store_le32(out + 8U, x[2]);
    dcc_voice_store_le32(out + 12U, x[3]);
    dcc_voice_store_le32(out + 16U, x[12]);
    dcc_voice_store_le32(out + 20U, x[13]);
    dcc_voice_store_le32(out + 24U, x[14]);
    dcc_voice_store_le32(out + 28U, x[15]);
}
