#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_entitlement_scalar_field(
    dcc_json_parser_t *parser,
    dcc_entitlement_t *entitlement,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_TYPE || key_id == DCC_JSON_KEY_GIFT_CODE_FLAGS) {
        uint32_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u32_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        if (key_id == DCC_JSON_KEY_TYPE) {
            entitlement->type = has_value ? value : 0U;
        } else {
            entitlement->gift_code_flags = has_value ? value : 0U;
        }
    } else if (key_id == DCC_JSON_KEY_FLAGS) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        entitlement->flags = has_value ? value : 0U;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
