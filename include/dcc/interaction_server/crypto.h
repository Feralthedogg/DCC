#ifndef DCC_INTERACTION_SERVER_CRYPTO_H
#define DCC_INTERACTION_SERVER_CRYPTO_H

#include <dcc/interaction_server/base.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API int dcc_interaction_verify_signature(
    const char *public_key_hex,
    const char *timestamp,
    const void *body,
    size_t body_len,
    const char *signature_hex
);

#ifdef __cplusplus
}
#endif

#endif
