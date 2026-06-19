#ifndef DCC_GATEWAY_WS_CONTROL_INTERNAL_H
#define DCC_GATEWAY_WS_CONTROL_INTERNAL_H

#include "internal/gateway/dcc_gateway_session_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

void dcc_gateway_control_lock(dcc_client_t *client);
void dcc_gateway_control_unlock(dcc_client_t *client);
int dcc_gateway_should_stop(const dcc_client_t *client);
int dcc_gateway_reconnect_requested(const dcc_client_t *client);
void dcc_gateway_interrupt_ws(dcc_ws_t *ws, int graceful);
void dcc_gateway_bind_ws(dcc_client_t *client, dcc_ws_t *ws);
void dcc_gateway_clear_ws(dcc_client_t *client, dcc_ws_t *ws);
dcc_gateway_next_t dcc_gateway_take_reconnect_request(dcc_client_t *client);

#ifdef __cplusplus
}
#endif

#endif
