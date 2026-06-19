#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_thread_list_id_field(
    dcc_json_parser_t *parser,
    dcc_channel_t *thread,
    dcc_json_key_id_t key_id,
    int *handled
) {
    uint64_t value = 0;
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_ID) {
        dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        thread->id = value;
    } else if (key_id == DCC_JSON_KEY_GUILD_ID ||
               key_id == DCC_JSON_KEY_PARENT_ID ||
               key_id == DCC_JSON_KEY_LAST_MESSAGE_ID) {
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        if (key_id == DCC_JSON_KEY_GUILD_ID) {
            thread->guild_id = has_value ? value : 0;
        } else if (key_id == DCC_JSON_KEY_PARENT_ID) {
            thread->parent_id = has_value ? value : 0;
        } else {
            thread->last_message_id = has_value ? value : 0;
        }
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
