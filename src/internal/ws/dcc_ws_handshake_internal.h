#ifndef DCC_WS_HANDSHAKE_INTERNAL_H
#define DCC_WS_HANDSHAKE_INTERNAL_H

#include "internal/ws/dcc_ws_types_internal.h"

#include <dcc/error.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_ws_generate_key(char key[32]);
dcc_status_t dcc_ws_read_handshake(dcc_ws_t *ws, const char *key);

#ifdef __cplusplus
}
#endif

#endif
