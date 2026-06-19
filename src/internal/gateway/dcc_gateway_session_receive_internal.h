#ifndef DCC_GATEWAY_SESSION_RECEIVE_INTERNAL_H
#define DCC_GATEWAY_SESSION_RECEIVE_INTERNAL_H

#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_session_internal.h"
#include "internal/json/dcc_json.h"

#include <stddef.h>

dcc_status_t dcc_gateway_read_hello(dcc_gateway_session_t *session);
dcc_status_t dcc_gateway_session_handle_payload(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
);
dcc_status_t dcc_gateway_session_receive_loop(dcc_gateway_session_t *session);

#endif
