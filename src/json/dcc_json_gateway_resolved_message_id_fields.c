#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_resolved_message_id_field(
    dcc_json_parser_t *parser,
    dcc_message_t *message,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || message == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    if (key_id == DCC_JSON_KEY_ID) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        message->id = value;
    } else if (key_id == DCC_JSON_KEY_CHANNEL_ID ||
               key_id == DCC_JSON_KEY_GUILD_ID ||
               key_id == DCC_JSON_KEY_WEBHOOK_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        if (key_id == DCC_JSON_KEY_CHANNEL_ID) {
            message->channel_id = has_value ? value : 0;
        } else if (key_id == DCC_JSON_KEY_GUILD_ID) {
            message->guild_id = has_value ? value : 0;
        } else {
            message->webhook_id = has_value ? value : 0;
        }
    } else {
        *handled = 0;
    }
    return DCC_OK;
}
