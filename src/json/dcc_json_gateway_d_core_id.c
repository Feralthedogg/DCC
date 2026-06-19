#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    uint64_t value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_ID) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    out->message.id = value;
    out->guild.id = value;
    out->channel.id = value;
    out->interaction.id = value;
    out->scheduled_event.id = value;
    out->entitlement.id = value;
    out->subscription.id = value;
    out->stage_instance.id = value;
    out->soundboard_sound.id = value;
    out->auto_moderation_rule.id = value;
    out->integration.id = value;
    out->thread_member.id = value;
    out->thread_members_update.id = value;
    out->user.id = value;
    out->application_command_permissions_update.id = value;
    out->guild_audit_log_entry.id = value;
    out->gateway.id = value;
    out->has_message = 1;
    out->has_guild = 1;
    out->has_channel = 1;
    out->has_interaction = 1;
    out->has_scheduled_event = 1;
    out->has_entitlement = 1;
    out->has_subscription = 1;
    out->has_stage_instance = 1;
    out->has_soundboard_sound = 1;
    out->has_auto_moderation_rule = 1;
    out->has_integration = 1;
    out->has_thread_member = 1;
    out->has_thread_members_update = 1;
    out->has_user = 1;
    out->has_application_command_permissions_update = 1;
    out->has_guild_audit_log_entry = 1;

    *handled = 1;
    return DCC_OK;
}
