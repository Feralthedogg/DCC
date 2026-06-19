#include "internal/voice/dcc_voice_internal.h"

#include <openssl/evp.h>

#include <limits.h>
#include <string.h>

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
) {
    if (out == NULL || out_len == NULL || (input == NULL && input_len != 0) ||
        (aad == NULL && aad_len != 0) || nonce == NULL || key == NULL ||
        input_len > (size_t)INT_MAX || aad_len > (size_t)INT_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    if (encrypt) {
        if (input_len > SIZE_MAX - DCC_VOICE_AEAD_TAG_SIZE ||
            out_cap < input_len + DCC_VOICE_AEAD_TAG_SIZE) {
            return DCC_ERR_NOMEM;
        }
    } else {
        if (input_len < DCC_VOICE_AEAD_TAG_SIZE || out_cap < input_len - DCC_VOICE_AEAD_TAG_SIZE) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    uint8_t sub_key[DCC_VOICE_SECRET_KEY_SIZE];
    uint8_t chacha_nonce[DCC_VOICE_CHACHA20_NONCE_SIZE];
    memset(chacha_nonce, 0, sizeof(chacha_nonce));
    dcc_voice_hchacha20(sub_key, key, nonce);
    memcpy(chacha_nonce + 4U, nonce + 16U, 8U);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        memset(sub_key, 0, sizeof(sub_key));
        return DCC_ERR_NOMEM;
    }

    int len = 0;
    int total = 0;
    dcc_status_t status = DCC_ERR_STATE;
    if (encrypt) {
        if (EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), NULL, NULL, NULL) != 1 ||
            EVP_EncryptInit_ex(ctx, NULL, NULL, sub_key, chacha_nonce) != 1 ||
            EVP_EncryptUpdate(ctx, NULL, &len, aad, (int)aad_len) != 1 ||
            EVP_EncryptUpdate(ctx, out, &len, input, (int)input_len) != 1) {
            goto done;
        }
        total = len;
        if (EVP_EncryptFinal_ex(ctx, out + total, &len) != 1) {
            goto done;
        }
        total += len;
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, DCC_VOICE_AEAD_TAG_SIZE, out + total) != 1) {
            goto done;
        }
        total += (int)DCC_VOICE_AEAD_TAG_SIZE;
        *out_len = (size_t)total;
        status = DCC_OK;
    } else {
        size_t ciphertext_len = input_len - DCC_VOICE_AEAD_TAG_SIZE;
        if (ciphertext_len > (size_t)INT_MAX ||
            EVP_DecryptInit_ex(ctx, EVP_chacha20_poly1305(), NULL, NULL, NULL) != 1 ||
            EVP_DecryptInit_ex(ctx, NULL, NULL, sub_key, chacha_nonce) != 1 ||
            EVP_DecryptUpdate(ctx, NULL, &len, aad, (int)aad_len) != 1 ||
            EVP_DecryptUpdate(ctx, out, &len, input, (int)ciphertext_len) != 1) {
            goto done;
        }
        total = len;
        if (EVP_CIPHER_CTX_ctrl(
                ctx,
                EVP_CTRL_AEAD_SET_TAG,
                DCC_VOICE_AEAD_TAG_SIZE,
                (void *)(input + ciphertext_len)
            ) != 1 ||
            EVP_DecryptFinal_ex(ctx, out + total, &len) != 1) {
            goto done;
        }
        total += len;
        *out_len = (size_t)total;
        status = DCC_OK;
    }

done:
    EVP_CIPHER_CTX_free(ctx);
    memset(sub_key, 0, sizeof(sub_key));
    memset(chacha_nonce, 0, sizeof(chacha_nonce));
    return status;
}
