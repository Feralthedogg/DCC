#ifndef DCC_GATEWAY_HEARTBEAT_INTERNAL_H
#define DCC_GATEWAY_HEARTBEAT_INTERNAL_H

#include "internal/gateway/dcc_gateway_session_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

void dcc_gateway_heartbeat_task(void *arg);
void dcc_gateway_heartbeat_state_release(dcc_gateway_heartbeat_state_t *state);
dcc_status_t dcc_gateway_send_heartbeat(dcc_gateway_session_t *session);

#ifdef __cplusplus
}
#endif

#endif
