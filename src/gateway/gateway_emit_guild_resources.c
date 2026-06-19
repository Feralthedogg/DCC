#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_guild_resources_internal.h"

dcc_status_t dcc_gateway_emit_application_command_permissions_update(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_APPLICATION_COMMAND_PERMISSIONS_UPDATE, payload, raw_json, raw_json_len);
    event.data.application_command_permissions_update = payload->application_command_permissions_update;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_guild_audit_log_entry(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_GUILD_AUDIT_LOG_ENTRY_CREATE, payload, raw_json, raw_json_len);
    event.data.guild_audit_log_entry = payload->guild_audit_log_entry;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_guild_ban(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, type, payload, raw_json, raw_json_len);
    event.data.guild_ban = payload->guild_ban;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_guild_id_list_update(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, type, payload, raw_json, raw_json_len);
    event.data.guild_id_list_update = payload->guild_id_list_update;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_guild_update_notice(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_GUILD_INTEGRATIONS_UPDATE, payload, raw_json, raw_json_len);
    event.data.guild_update_notice = payload->guild_update_notice;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_scheduled_event_user_update(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, type, payload, raw_json, raw_json_len);
    event.data.scheduled_event_user_update = payload->scheduled_event_user_update;
    return dcc_gateway_dispatch_event(session, &event);
}
