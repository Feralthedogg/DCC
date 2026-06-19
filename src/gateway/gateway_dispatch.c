#include "internal/gateway/dcc_gateway_dispatch_internal.h"

dcc_status_t dcc_gateway_handle_dispatch(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    if (!payload->has_event_name) {
        return DCC_OK;
    }

    dcc_event_type_t type = dcc_gateway_event_type_from_name(payload->event_name);
    dcc_status_t status = DCC_OK;
    if (dcc_gateway_dispatch_session_event(session, type, payload, raw_json, raw_json_len, &status) ||
        dcc_gateway_dispatch_object_event(session, type, payload, raw_json, raw_json_len, &status) ||
        dcc_gateway_dispatch_resource_event(session, type, payload, raw_json, raw_json_len, &status) ||
        dcc_gateway_dispatch_runtime_event(session, type, payload, raw_json, raw_json_len, &status) ||
        dcc_gateway_dispatch_misc_event(session, type, payload, raw_json, raw_json_len, &status)) {
        return status;
    }

    return dcc_gateway_emit_raw_dispatch(session, type, payload, raw_json, raw_json_len);
}
