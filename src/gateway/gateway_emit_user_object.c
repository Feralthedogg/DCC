#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_user_object_internal.h"

dcc_status_t dcc_gateway_emit_user(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_USER_UPDATE, payload, raw_json, raw_json_len);
    event.data.user = payload->user;
    return dcc_gateway_dispatch_event(session, &event);
}
