#ifndef DCC_GATEWAY_EMIT_INVITE_RESOURCES_INTERNAL_H
#define DCC_GATEWAY_EMIT_INVITE_RESOURCES_INTERNAL_H

#include "internal/gateway/dcc_gateway_session_internal.h"
#include "internal/json/dcc_json.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_gateway_emit_invite(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
);

#ifdef __cplusplus
}
#endif

#endif
