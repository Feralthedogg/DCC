#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_moderation_runtime_internal.h"

dcc_status_t dcc_gateway_emit_auto_moderation_action(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_AUTO_MODERATION_ACTION_EXECUTION, payload, raw_json, raw_json_len);
    event.data.auto_moderation_action = payload->auto_moderation_action;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_auto_moderation_rule(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, type, payload, raw_json, raw_json_len);
    event.data.auto_moderation_rule = payload->auto_moderation_rule;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_guild_members_chunk(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_GUILD_MEMBERS_CHUNK, payload, raw_json, raw_json_len);
    event.data.guild_members_chunk = payload->guild_members_chunk;
    return dcc_gateway_dispatch_event(session, &event);
}
