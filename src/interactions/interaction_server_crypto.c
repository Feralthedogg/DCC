#include "internal/interactions/dcc_interaction_server_internal.h"

#include <openssl/evp.h>

#include <stdlib.h>
#include <string.h>

static int dcc_interaction_hex_value(char ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return -1;
}

int dcc_interaction_hex_decode_exact(const char *hex, unsigned char *out, size_t out_len) {
    if (hex == NULL || out == NULL) {
        return -1;
    }
    size_t hex_len = strlen(hex);
    if (hex_len != out_len * 2U) {
        return -1;
    }
    for (size_t i = 0; i < out_len; ++i) {
        int hi = dcc_interaction_hex_value(hex[i * 2U]);
        int lo = dcc_interaction_hex_value(hex[i * 2U + 1U]);
        if (hi < 0 || lo < 0) {
            return -1;
        }
        out[i] = (unsigned char)((hi << 4) | lo);
    }
    return 0;
}

int dcc_interaction_verify_signature(
    const char *public_key_hex,
    const char *timestamp,
    const void *body,
    size_t body_len,
    const char *signature_hex
) {
    unsigned char public_key[32];
    unsigned char signature[64];
    EVP_PKEY *pkey = NULL;
    EVP_MD_CTX *ctx = NULL;
    unsigned char *message = NULL;
    int ok = 0;

    if (public_key_hex == NULL || timestamp == NULL || signature_hex == NULL ||
        (body == NULL && body_len > 0U)) {
        return 0;
    }
    if (dcc_interaction_hex_decode_exact(public_key_hex, public_key, sizeof(public_key)) != 0 ||
        dcc_interaction_hex_decode_exact(signature_hex, signature, sizeof(signature)) != 0) {
        return 0;
    }

    size_t timestamp_len = strlen(timestamp);
    if (timestamp_len > SIZE_MAX - body_len) {
        return 0;
    }
    size_t message_len = timestamp_len + body_len;
    message = (unsigned char *)malloc(message_len == 0U ? 1U : message_len);
    if (message == NULL) {
        return 0;
    }
    memcpy(message, timestamp, timestamp_len);
    if (body_len > 0U) {
        memcpy(message + timestamp_len, body, body_len);
    }

    pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, NULL, public_key, sizeof(public_key));
    ctx = EVP_MD_CTX_new();
    if (pkey != NULL && ctx != NULL &&
        EVP_DigestVerifyInit(ctx, NULL, NULL, NULL, pkey) == 1 &&
        EVP_DigestVerify(ctx, signature, sizeof(signature), message, message_len) == 1) {
        ok = 1;
    }

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    free(message);
    return ok;
}
