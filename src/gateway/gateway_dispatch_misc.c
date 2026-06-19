#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_basic_internal.h"
#include "internal/gateway/dcc_gateway_emit_interactions_internal.h"
#include "internal/gateway/dcc_gateway_emit_resources_internal.h"
#include "internal/gateway/dcc_gateway_emit_runtime_internal.h"

uint8_t dcc_gateway_dispatch_misc_event(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len,
    dcc_status_t *out_status
) {
    if (type == DCC_EVENT_INTEGRATION_CREATE || type == DCC_EVENT_INTEGRATION_UPDATE ||
        type == DCC_EVENT_INTEGRATION_DELETE) {
        *out_status = dcc_gateway_emit_integration(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_PRESENCE_UPDATE) {
        *out_status = dcc_gateway_emit_presence(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_USER_UPDATE) {
        *out_status = dcc_gateway_emit_user(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_APPLICATION_COMMAND_PERMISSIONS_UPDATE) {
        *out_status = dcc_gateway_emit_application_command_permissions_update(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_AUDIT_LOG_ENTRY_CREATE) {
        *out_status = dcc_gateway_emit_guild_audit_log_entry(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_BAN_ADD || type == DCC_EVENT_GUILD_BAN_REMOVE) {
        *out_status = dcc_gateway_emit_guild_ban(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_EMOJIS_UPDATE || type == DCC_EVENT_GUILD_STICKERS_UPDATE ||
        type == DCC_EVENT_GUILD_SOUNDBOARD_SOUNDS_UPDATE || type == DCC_EVENT_SOUNDBOARD_SOUNDS) {
        *out_status = dcc_gateway_emit_guild_id_list_update(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_INTEGRATIONS_UPDATE) {
        *out_status = dcc_gateway_emit_guild_update_notice(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_SCHEDULED_EVENT_USER_ADD ||
        type == DCC_EVENT_GUILD_SCHEDULED_EVENT_USER_REMOVE) {
        *out_status = dcc_gateway_emit_scheduled_event_user_update(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_THREAD_LIST_SYNC) {
        *out_status = dcc_gateway_emit_thread_list_sync(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_VOICE_CHANNEL_START_TIME_UPDATE) {
        *out_status = dcc_gateway_emit_voice_channel_start_time_update(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_VOICE_CHANNEL_STATUS_UPDATE) {
        *out_status = dcc_gateway_emit_voice_channel_status_update(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_WEBHOOKS_UPDATE) {
        *out_status = dcc_gateway_emit_webhooks_update(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_JOIN_REQUEST_DELETE || type == DCC_EVENT_GUILD_JOIN_REQUEST_UPDATE) {
        *out_status = dcc_gateway_emit_guild_join_request_update(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_APPLICATION_COMMAND_COUNTS_UPDATE ||
        type == DCC_EVENT_GUILD_APPLICATION_COMMAND_INDEX_UPDATE) {
        *out_status = dcc_gateway_emit_guild_application_command_update(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_EMBEDDED_ACTIVITY_UPDATE) {
        *out_status = dcc_gateway_emit_embedded_activity_update(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_RATE_LIMITED) {
        *out_status = dcc_gateway_emit_rate_limited(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_INTERACTION_CREATE) {
        *out_status = dcc_gateway_emit_interaction(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_VOICE_STATE_UPDATE) {
        *out_status = dcc_gateway_emit_voice_state(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    return 0U;
}
