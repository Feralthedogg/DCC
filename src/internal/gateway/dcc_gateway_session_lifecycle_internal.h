#ifndef DCC_GATEWAY_SESSION_LIFECYCLE_INTERNAL_H
#define DCC_GATEWAY_SESSION_LIFECYCLE_INTERNAL_H

#include "internal/gateway/dcc_gateway_session_internal.h"

#include <llam/runtime.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_gateway_session_init(dcc_gateway_session_t *session, dcc_client_t *client, bool resume);
void dcc_gateway_session_stop_heartbeat(dcc_gateway_session_t *session, llam_task_t *heartbeat);
void dcc_gateway_session_drop_ws(dcc_gateway_session_t *session);
void dcc_gateway_session_finish_ws(dcc_gateway_session_t *session);
dcc_status_t dcc_gateway_session_finalize_receive(
    dcc_gateway_session_t *session,
    dcc_status_t status
);

#ifdef __cplusplus
}
#endif

#endif
