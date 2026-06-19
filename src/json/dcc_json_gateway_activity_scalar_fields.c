#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_activity_scalar_field(
    dcc_json_parser_t *parser,
    dcc_gateway_activity_t *activity,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_TYPE) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        activity->type = value;
    } else if (key_id == DCC_JSON_KEY_APPLICATION_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        activity->application_id = has_value ? value : 0;
    } else if (key_id == DCC_JSON_KEY_CREATED_AT) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        activity->created_at = has_value ? value : 0;
    } else if (key_id == DCC_JSON_KEY_FLAGS) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        activity->flags = value;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
