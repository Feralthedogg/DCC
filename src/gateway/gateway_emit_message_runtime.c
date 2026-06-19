#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_message_runtime_internal.h"

dcc_status_t dcc_gateway_emit_message_reaction(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, type, payload, raw_json, raw_json_len);
    event.data.message_reaction = payload->message_reaction;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_poll_vote(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, type, payload, raw_json, raw_json_len);
    event.data.poll_vote = payload->poll_vote;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_message_delete_bulk(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_MESSAGE_DELETE_BULK, payload, raw_json, raw_json_len);
    event.data.message_delete_bulk = payload->message_delete_bulk;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_channel_pins_update(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_CHANNEL_PINS_UPDATE, payload, raw_json, raw_json_len);
    event.data.channel_pins_update = payload->channel_pins_update;
    return dcc_gateway_dispatch_event(session, &event);
}
