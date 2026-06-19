#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_basic_internal.h"
#include "internal/gateway/dcc_gateway_emit_resources_internal.h"

uint8_t dcc_gateway_dispatch_object_event(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len,
    dcc_status_t *out_status
) {
    if (type == DCC_EVENT_MESSAGE_CREATE || type == DCC_EVENT_MESSAGE_UPDATE ||
        type == DCC_EVENT_MESSAGE_DELETE) {
        *out_status = dcc_gateway_emit_message(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_CREATE || type == DCC_EVENT_GUILD_UPDATE ||
        type == DCC_EVENT_GUILD_DELETE) {
        *out_status = dcc_gateway_emit_guild(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_CHANNEL_CREATE || type == DCC_EVENT_CHANNEL_UPDATE ||
        type == DCC_EVENT_CHANNEL_DELETE || type == DCC_EVENT_CHANNEL_INFO ||
        type == DCC_EVENT_THREAD_CREATE || type == DCC_EVENT_THREAD_UPDATE ||
        type == DCC_EVENT_THREAD_DELETE || type == DCC_EVENT_CHANNEL_TOPIC_UPDATE) {
        *out_status = dcc_gateway_emit_channel(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_ROLE_CREATE || type == DCC_EVENT_GUILD_ROLE_UPDATE ||
        type == DCC_EVENT_GUILD_ROLE_DELETE) {
        *out_status = dcc_gateway_emit_role(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_MEMBER_ADD || type == DCC_EVENT_GUILD_MEMBER_REMOVE ||
        type == DCC_EVENT_GUILD_MEMBER_UPDATE) {
        *out_status = dcc_gateway_emit_member(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_TYPING_START) {
        *out_status = dcc_gateway_emit_typing_start(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    return 0U;
}
