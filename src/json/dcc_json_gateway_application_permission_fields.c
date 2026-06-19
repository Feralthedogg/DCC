#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_application_command_permission_field(
    dcc_json_parser_t *parser,
    dcc_application_command_permission_t *permission,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (parser == NULL || permission == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        permission->id = has_value ? value : 0;
    } else if (key_id == DCC_JSON_KEY_TYPE) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        permission->type = value;
    } else if (key_id == DCC_JSON_KEY_PERMISSION) {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        permission->permission = value ? 1U : 0U;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
