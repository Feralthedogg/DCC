#ifndef DCC_WS_LIFECYCLE_INTERNAL_H
#define DCC_WS_LIFECYCLE_INTERNAL_H

#include "internal/ws/dcc_ws_types_internal.h"

#include <dcc/error.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_ws_connect(dcc_ws_t **out, const char *url, int timeout_ms);
dcc_status_t dcc_ws_retain(dcc_ws_t *ws);
intptr_t dcc_ws_fd(const dcc_ws_t *ws);
uint16_t dcc_ws_last_close_code(const dcc_ws_t *ws);
const char *dcc_ws_last_close_reason(const dcc_ws_t *ws);
void dcc_ws_abort(dcc_ws_t *ws);
void dcc_ws_close(dcc_ws_t *ws, uint16_t code, const char *reason);
void dcc_ws_destroy(dcc_ws_t *ws);

#ifdef __cplusplus
}
#endif

#endif
