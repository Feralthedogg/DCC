#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_primary_message_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_MESSAGE_ID && key_id != DCC_JSON_KEY_LAST_MESSAGE_ID) {
        *handled = 0;
        return DCC_OK;
    }

    uint64_t value = 0;
    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }

    if (key_id == DCC_JSON_KEY_MESSAGE_ID) {
        out->gateway.message_id = value;
        out->interaction.message_id = value;
        out->message_reaction.message_id = value;
        out->poll_vote.message_id = value;
        out->auto_moderation_action.message_id = value;
        out->has_interaction = 1;
        out->has_message_reaction = 1;
        out->has_poll_vote = 1;
        out->has_auto_moderation_action = 1;
    } else {
        out->channel.last_message_id = value;
        out->has_channel = 1;
        out->cache_fields |= DCC_CACHE_FIELD_CHANNEL_LAST_MESSAGE_ID;
    }

    (void)has_value;
    *handled = 1;
    return DCC_OK;
}
