#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_member_list_scalar_field(
    dcc_json_parser_t *parser,
    dcc_member_t *member,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_member_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || member == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_PENDING) {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        member->pending = value ? 1U : 0U;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_FLAGS) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        member->flags = value;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_PERMISSIONS) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        if (buffers->permissions != NULL) {
            *buffers->permissions = has_value ? (dcc_permission_t)value : 0;
        }
        if (buffers->has_permissions != NULL) {
            *buffers->has_permissions = has_value ? 1 : 0;
        }
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
