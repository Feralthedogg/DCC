#include "internal/rest/dcc_rest_sensitive_internal.h"

#include <openssl/evp.h>

dcc_status_t dcc_endpoint_sensitive_route_fingerprint(
    const char *encoded,
    size_t encoded_len,
    char out_hex[65]
) {
    static const unsigned char prefix[] = "dcc-rest-route-token-v1";
    if (encoded == NULL || encoded_len == 0U || out_hex == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    unsigned char digest[EVP_MAX_MD_SIZE] = {0};
    unsigned int digest_len = 0U;
    dcc_status_t status = DCC_OK;
    if (ctx == NULL || EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
        EVP_DigestUpdate(ctx, prefix, sizeof(prefix)) != 1 ||
        EVP_DigestUpdate(ctx, encoded, encoded_len) != 1 ||
        EVP_DigestFinal_ex(ctx, digest, &digest_len) != 1 ||
        digest_len != 32U) {
        status = DCC_ERR_RUNTIME;
    }
    EVP_MD_CTX_free(ctx);

    static const char hex[] = "0123456789abcdef";
    if (status == DCC_OK) {
        for (size_t i = 0U; i < 32U; ++i) {
            out_hex[i * 2U] = hex[digest[i] >> 4U];
            out_hex[i * 2U + 1U] = hex[digest[i] & 15U];
        }
        out_hex[64] = '\0';
    } else {
        out_hex[0] = '\0';
    }
    dcc_endpoint_secure_zero(digest, sizeof(digest));
    return status;
}
