#ifndef DCC_INTERACTION_CRYPTO_INTERNAL_H
#define DCC_INTERACTION_CRYPTO_INTERNAL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_interaction_hex_decode_exact(const char *hex, unsigned char *out, size_t out_len);

#ifdef __cplusplus
}
#endif

#endif
