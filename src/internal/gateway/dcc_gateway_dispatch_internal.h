#ifndef DCC_GATEWAY_DISPATCH_INTERNAL_H
#define DCC_GATEWAY_DISPATCH_INTERNAL_H

#include "internal/gateway/dcc_gateway_session_internal.h"
#include "internal/json/dcc_json.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_event_type_t dcc_gateway_event_type_from_name(const char *name);
void dcc_gateway_cache_apply_event(dcc_client_t *client, const dcc_event_t *event);
void dcc_gateway_init_event(
    dcc_event_t *event,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
);
dcc_status_t dcc_gateway_dispatch_event(dcc_gateway_session_t *session, const dcc_event_t *event);
dcc_status_t dcc_gateway_emit_raw_dispatch(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
);
uint8_t dcc_gateway_dispatch_session_event(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len,
    dcc_status_t *out_status
);
uint8_t dcc_gateway_dispatch_object_event(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len,
    dcc_status_t *out_status
);
uint8_t dcc_gateway_dispatch_resource_event(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len,
    dcc_status_t *out_status
);
uint8_t dcc_gateway_dispatch_runtime_event(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len,
    dcc_status_t *out_status
);
uint8_t dcc_gateway_dispatch_misc_event(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len,
    dcc_status_t *out_status
);
dcc_status_t dcc_gateway_handle_dispatch(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
);
dcc_status_t dcc_gateway_emit_socket_close(
    dcc_gateway_session_t *session,
    uint16_t code,
    const char *reason
);

#ifdef __cplusplus
}
#endif

#endif
