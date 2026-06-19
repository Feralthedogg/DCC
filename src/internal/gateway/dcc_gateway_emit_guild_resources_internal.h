#ifndef DCC_GATEWAY_EMIT_GUILD_RESOURCES_INTERNAL_H
#define DCC_GATEWAY_EMIT_GUILD_RESOURCES_INTERNAL_H

#include "internal/gateway/dcc_gateway_session_internal.h"
#include "internal/json/dcc_json.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_gateway_emit_application_command_permissions_update(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
);
dcc_status_t dcc_gateway_emit_guild_audit_log_entry(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
);
dcc_status_t dcc_gateway_emit_guild_ban(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
);
dcc_status_t dcc_gateway_emit_guild_id_list_update(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
);
dcc_status_t dcc_gateway_emit_guild_update_notice(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
);
dcc_status_t dcc_gateway_emit_scheduled_event_user_update(
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
