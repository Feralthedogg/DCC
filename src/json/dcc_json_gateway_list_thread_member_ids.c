#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_thread_member_id_field(
    dcc_json_parser_t *parser,
    dcc_thread_member_t *member,
    dcc_json_key_id_t key_id,
    int *handled
) {
    uint64_t value = 0;
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_ID ||
        key_id == DCC_JSON_KEY_USER_ID ||
        key_id == DCC_JSON_KEY_GUILD_ID) {
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        if (key_id == DCC_JSON_KEY_ID) {
            member->id = has_value ? value : 0U;
        } else if (key_id == DCC_JSON_KEY_USER_ID) {
            member->user_id = has_value ? value : 0U;
        } else {
            member->guild_id = has_value ? value : 0U;
        }
    } else if (key_id == DCC_JSON_KEY_USER) {
        dcc_status_t status = dcc_json_parse_nested_user_id(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        if (has_value) {
            member->user_id = value;
        }
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
