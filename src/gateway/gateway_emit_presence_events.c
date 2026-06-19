#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_presence_events_internal.h"

dcc_status_t dcc_gateway_emit_typing_start(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_TYPING_START, payload, raw_json, raw_json_len);
    event.data.typing_start.guild_id = payload->gateway.guild_id;
    event.data.typing_start.channel_id = payload->gateway.channel_id;
    event.data.typing_start.user_id = payload->gateway.user_id;
    event.data.typing_start.timestamp = payload->gateway.timestamp;
    event.data.typing_start.user = payload->gateway.user;
    event.data.typing_start.member = payload->member;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_presence(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_PRESENCE_UPDATE, payload, raw_json, raw_json_len);
    event.data.presence = payload->presence;
    return dcc_gateway_dispatch_event(session, &event);
}
