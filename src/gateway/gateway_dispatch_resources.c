#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_resources_internal.h"

uint8_t dcc_gateway_dispatch_resource_event(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len,
    dcc_status_t *out_status
) {
    if (type == DCC_EVENT_GUILD_SCHEDULED_EVENT_CREATE ||
        type == DCC_EVENT_GUILD_SCHEDULED_EVENT_UPDATE ||
        type == DCC_EVENT_GUILD_SCHEDULED_EVENT_DELETE) {
        *out_status = dcc_gateway_emit_scheduled_event(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_ENTITLEMENT_CREATE || type == DCC_EVENT_ENTITLEMENT_UPDATE ||
        type == DCC_EVENT_ENTITLEMENT_DELETE) {
        *out_status = dcc_gateway_emit_entitlement(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_SUBSCRIPTION_CREATE || type == DCC_EVENT_SUBSCRIPTION_UPDATE ||
        type == DCC_EVENT_SUBSCRIPTION_DELETE) {
        *out_status = dcc_gateway_emit_subscription(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_INVITE_CREATE || type == DCC_EVENT_INVITE_DELETE) {
        *out_status = dcc_gateway_emit_invite(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_STAGE_INSTANCE_CREATE || type == DCC_EVENT_STAGE_INSTANCE_UPDATE ||
        type == DCC_EVENT_STAGE_INSTANCE_DELETE) {
        *out_status = dcc_gateway_emit_stage_instance(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_SOUNDBOARD_SOUND_CREATE ||
        type == DCC_EVENT_GUILD_SOUNDBOARD_SOUND_UPDATE ||
        type == DCC_EVENT_GUILD_SOUNDBOARD_SOUND_DELETE) {
        *out_status = dcc_gateway_emit_soundboard_sound(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    return 0U;
}
