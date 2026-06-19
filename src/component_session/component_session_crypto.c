#include "internal/component_session/dcc_component_session_internal.h"

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

#include <limits.h>
#include <stdio.h>
#include <string.h>

static int dcc_component_session_token_char(int ch) {
    return (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z') ||
           (ch >= '0' && ch <= '9') ||
           ch == '_' ||
           ch == '-';
}

int dcc_component_session_token_valid(const char *token, size_t max_len) {
    if (token == NULL || token[0] == '\0') {
        return 0;
    }
    size_t len = 0;
    for (; token[len] != '\0'; ++len) {
        if (len >= max_len || !dcc_component_session_token_char((unsigned char)token[len])) {
            return 0;
        }
    }
    return len != 0U;
}

dcc_status_t dcc_component_session_make_random_hex(char *out, size_t hex_len) {
    static const char hex[] = "0123456789abcdef";
    if (out == NULL || hex_len == 0U || (hex_len % 2U) != 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    unsigned char raw[DCC_COMPONENT_SESSION_ID_MAX / 2U];
    size_t raw_len = hex_len / 2U;
    if (raw_len > sizeof(raw)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (RAND_bytes(raw, (int)raw_len) != 1) {
        return DCC_ERR_RUNTIME;
    }
    for (size_t i = 0; i < raw_len; ++i) {
        out[i * 2U] = hex[raw[i] >> 4U];
        out[i * 2U + 1U] = hex[raw[i] & 0x0FU];
    }
    out[hex_len] = '\0';
    return DCC_OK;
}

dcc_status_t dcc_component_session_sign(
    const dcc_component_session_state_t *state,
    const char *action,
    const char *nonce,
    char out_sig[DCC_COMPONENT_SESSION_SIG_HEX_LEN + 1U]
) {
    static const char hex[] = "0123456789abcdef";
    if (state == NULL || state->secret == NULL || state->secret_len == 0U ||
        action == NULL || nonce == NULL || out_sig == NULL || state->secret_len > (size_t)INT_MAX) {
        return DCC_ERR_INVALID_ARG;
    }

    char message[96];
    int message_len = snprintf(
        message,
        sizeof(message),
        "dcc:v1:%s:%s:%s",
        state->session_id,
        action,
        nonce
    );
    if (message_len <= 0 || (size_t)message_len >= sizeof(message)) {
        return DCC_ERR_INVALID_ARG;
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;
    if (HMAC(
            EVP_sha256(),
            state->secret,
            (int)state->secret_len,
            (const unsigned char *)message,
            (size_t)message_len,
            digest,
            &digest_len
        ) == NULL ||
        digest_len < DCC_COMPONENT_SESSION_SIG_HEX_LEN / 2U) {
        return DCC_ERR_RUNTIME;
    }

    for (size_t i = 0; i < DCC_COMPONENT_SESSION_SIG_HEX_LEN / 2U; ++i) {
        out_sig[i * 2U] = hex[digest[i] >> 4U];
        out_sig[i * 2U + 1U] = hex[digest[i] & 0x0FU];
    }
    out_sig[DCC_COMPONENT_SESSION_SIG_HEX_LEN] = '\0';
    return DCC_OK;
}

int dcc_component_session_sig_equal(const char *a, const char *b) {
    if (a == NULL || b == NULL) {
        return 0;
    }
    unsigned char diff = 0;
    for (size_t i = 0; i < DCC_COMPONENT_SESSION_SIG_HEX_LEN; ++i) {
        diff |= (unsigned char)(a[i] ^ b[i]);
    }
    return diff == 0U &&
        a[DCC_COMPONENT_SESSION_SIG_HEX_LEN] == '\0' &&
        b[DCC_COMPONENT_SESSION_SIG_HEX_LEN] == '\0';
}
