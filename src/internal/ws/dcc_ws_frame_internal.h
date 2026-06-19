#ifndef DCC_WS_FRAME_INTERNAL_H
#define DCC_WS_FRAME_INTERNAL_H

#include "internal/ws/dcc_ws_types_internal.h"

#include <dcc/error.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_ws_send(dcc_ws_t *ws, dcc_ws_opcode_t opcode, const void *data, size_t len);
dcc_status_t dcc_ws_send_text(dcc_ws_t *ws, const char *text, size_t len);
dcc_status_t dcc_ws_recv(dcc_ws_t *ws, dcc_ws_message_t *out);
void dcc_ws_message_deinit(dcc_ws_message_t *message);

#ifdef __cplusplus
}
#endif

#endif
