#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_runtime_internal.h"

uint8_t dcc_gateway_dispatch_runtime_event(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len,
    dcc_status_t *out_status
) {
    if (type == DCC_EVENT_MESSAGE_REACTION_ADD || type == DCC_EVENT_MESSAGE_REACTION_REMOVE ||
        type == DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL ||
        type == DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI) {
        *out_status = dcc_gateway_emit_message_reaction(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_MESSAGE_POLL_VOTE_ADD || type == DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE) {
        *out_status = dcc_gateway_emit_poll_vote(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_VOICE_CHANNEL_EFFECT_SEND) {
        *out_status = dcc_gateway_emit_voice_channel_effect(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_AUTO_MODERATION_ACTION_EXECUTION) {
        *out_status = dcc_gateway_emit_auto_moderation_action(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_AUTO_MODERATION_RULE_CREATE ||
        type == DCC_EVENT_AUTO_MODERATION_RULE_UPDATE ||
        type == DCC_EVENT_AUTO_MODERATION_RULE_DELETE) {
        *out_status = dcc_gateway_emit_auto_moderation_rule(session, type, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_MESSAGE_DELETE_BULK) {
        *out_status = dcc_gateway_emit_message_delete_bulk(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_THREAD_MEMBER_UPDATE) {
        *out_status = dcc_gateway_emit_thread_member(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_THREAD_MEMBERS_UPDATE) {
        *out_status = dcc_gateway_emit_thread_members_update(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_GUILD_MEMBERS_CHUNK) {
        *out_status = dcc_gateway_emit_guild_members_chunk(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_CHANNEL_PINS_UPDATE) {
        *out_status = dcc_gateway_emit_channel_pins_update(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_VOICE_SERVER_UPDATE) {
        *out_status = dcc_gateway_emit_voice_server_update(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    return 0U;
}
