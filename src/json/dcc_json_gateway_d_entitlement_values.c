#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_entitlement_value_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_ENTITLEMENT_ID) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.entitlement_id = value;
        out->entitlement.id = value;
        out->has_entitlement = 1;
    } else if (key_id == DCC_JSON_KEY_GIFT_CODE_FLAGS) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.gift_code_flags = value;
        out->entitlement.gift_code_flags = value;
        out->has_entitlement = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
