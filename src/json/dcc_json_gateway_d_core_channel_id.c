#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_channel_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    uint64_t value = 0;
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_CHANNEL_ID) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }
    out->message.channel_id = value;
    out->interaction.channel_id = value;
    out->voice_state.channel_id = value;
    out->scheduled_event.channel_id = value;
    out->invite.channel_id = value;
    out->stage_instance.channel_id = value;
    out->message_reaction.channel_id = value;
    out->poll_vote.channel_id = value;
    out->voice_channel_effect.channel_id = value;
    out->auto_moderation_action.channel_id = value;
    out->message_delete_bulk.channel_id = value;
    out->channel_pins_update.channel_id = value;
    out->voice_channel_start_time_update.channel_id = value;
    out->voice_channel_status_update.channel_id = value;
    out->webhooks_update.channel_id = value;
    out->embedded_activity_update.channel_id = value;
    out->gateway.channel_id = value;
    out->has_message = 1;
    out->has_interaction = 1;
    out->has_voice_state = 1;
    out->has_scheduled_event = 1;
    out->has_invite = 1;
    out->has_stage_instance = 1;
    out->has_message_reaction = 1;
    out->has_poll_vote = 1;
    out->has_voice_channel_effect = 1;
    out->has_auto_moderation_action = 1;
    out->has_message_delete_bulk = 1;
    out->has_channel_pins_update = 1;
    out->has_voice_channel_start_time_update = 1;
    out->has_voice_channel_status_update = 1;
    out->has_webhooks_update = 1;
    out->has_embedded_activity_update = 1;
    out->cache_fields |= DCC_CACHE_FIELD_VOICE_CHANNEL_ID;
    (void)has_value;

    *handled = 1;
    return DCC_OK;
}
