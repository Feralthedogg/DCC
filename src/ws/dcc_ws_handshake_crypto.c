#include "internal/ws/dcc_ws_handshake_internal.h"
#include "internal/ws/dcc_ws_handshake_helpers_internal.h"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

#include <stdio.h>

#define DCC_WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

int dcc_ws_generate_key(char key[32]) {
    unsigned char raw[16];
    if (RAND_bytes(raw, sizeof(raw)) != 1) {
        return -1;
    }
    int len = EVP_EncodeBlock((unsigned char *)key, raw, (int)sizeof(raw));
    if (len <= 0 || len >= 32) {
        return -1;
    }
    key[len] = '\0';
    return 0;
}

int dcc_ws_accept_for_key(const char *key, char out[64]) {
    unsigned char digest[SHA_DIGEST_LENGTH];
    char material[128];
    int material_len = snprintf(material, sizeof(material), "%s%s", key, DCC_WS_GUID);
    if (material_len <= 0 || (size_t)material_len >= sizeof(material)) {
        return -1;
    }
    SHA1((const unsigned char *)material, (size_t)material_len, digest);
    int encoded = EVP_EncodeBlock((unsigned char *)out, digest, SHA_DIGEST_LENGTH);
    if (encoded <= 0 || encoded >= 64) {
        return -1;
    }
    out[encoded] = '\0';
    return 0;
}
