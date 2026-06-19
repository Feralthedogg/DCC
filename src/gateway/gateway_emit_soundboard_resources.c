#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_soundboard_resources_internal.h"

dcc_status_t dcc_gateway_emit_soundboard_sound(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, type, payload, raw_json, raw_json_len);
    event.data.soundboard_sound = payload->soundboard_sound;
    return dcc_gateway_dispatch_event(session, &event);
}
