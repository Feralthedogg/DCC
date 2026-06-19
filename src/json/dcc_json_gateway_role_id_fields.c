#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_role_id_field(
    dcc_json_parser_t *parser,
    dcc_role_t *role,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || role == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_ID) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        role->id = value;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_GUILD_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        role->guild_id = has_value ? value : 0;
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
