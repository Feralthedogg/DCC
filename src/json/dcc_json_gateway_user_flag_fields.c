#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_user_flag_field(
    dcc_json_parser_t *parser,
    dcc_user_t *user,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || user == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    if (key_id == DCC_JSON_KEY_BOT) {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        user->bot = value ? 1U : 0U;
    } else if (key_id == DCC_JSON_KEY_FLAGS ||
               key_id == DCC_JSON_KEY_PUBLIC_FLAGS) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        user->flags |= value;
    } else {
        *handled = 0;
    }
    return DCC_OK;
}
