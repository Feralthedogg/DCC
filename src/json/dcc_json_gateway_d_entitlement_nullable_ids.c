#include "internal/json/dcc_json_internal.h"

static void dcc_json_gateway_apply_entitlement_nullable_id(
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    uint64_t value
) {
    if (key_id == DCC_JSON_KEY_SKU_ID) {
        out->gateway.sku_id = value;
        out->entitlement.sku_id = value;
    } else if (key_id == DCC_JSON_KEY_SUBSCRIPTION_ID) {
        out->gateway.subscription_id = value;
        out->entitlement.subscription_id = value;
    } else if (key_id == DCC_JSON_KEY_PROMOTION_ID) {
        out->gateway.promotion_id = value;
        out->entitlement.promotion_id = value;
    }
}

dcc_status_t dcc_json_gateway_parse_d_entitlement_nullable_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    uint64_t value = 0;
    int has_value = 0;
    dcc_status_t status = DCC_OK;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id != DCC_JSON_KEY_SKU_ID &&
        key_id != DCC_JSON_KEY_SUBSCRIPTION_ID &&
        key_id != DCC_JSON_KEY_PROMOTION_ID) {
        *handled = 0;
        return DCC_OK;
    }

    status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }
    dcc_json_gateway_apply_entitlement_nullable_id(out, key_id, value);
    out->has_entitlement = 1;
    *handled = 1;
    (void)has_value;
    return DCC_OK;
}
