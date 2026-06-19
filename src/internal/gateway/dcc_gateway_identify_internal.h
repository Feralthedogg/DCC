#ifndef DCC_GATEWAY_IDENTIFY_INTERNAL_H
#define DCC_GATEWAY_IDENTIFY_INTERNAL_H

#include "internal/gateway/dcc_gateway_session_internal.h"
#include "internal/json/dcc_json.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_gateway_wait_identify_slot(dcc_client_t *client);
void dcc_gateway_consume_identify(dcc_client_t *client);
dcc_gateway_next_t dcc_gateway_reconnect_next(const dcc_client_t *client);
dcc_gateway_next_t dcc_gateway_invalid_session_next(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload
);
dcc_status_t dcc_gateway_send_identify(dcc_gateway_session_t *session);
dcc_status_t dcc_gateway_send_resume(dcc_gateway_session_t *session);

#ifdef __cplusplus
}
#endif

#endif
