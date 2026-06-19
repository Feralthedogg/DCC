#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_entitlement_id_field(
    dcc_json_parser_t *parser,
    dcc_entitlement_t *entitlement,
    dcc_json_key_id_t key_id,
    int *handled
) {
    uint64_t value = 0;
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id != DCC_JSON_KEY_ID &&
        key_id != DCC_JSON_KEY_SKU_ID &&
        key_id != DCC_JSON_KEY_APPLICATION_ID &&
        key_id != DCC_JSON_KEY_SUBSCRIPTION_ID &&
        key_id != DCC_JSON_KEY_PROMOTION_ID &&
        key_id != DCC_JSON_KEY_USER_ID &&
        key_id != DCC_JSON_KEY_GUILD_ID) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }
    value = has_value ? value : 0;

    if (key_id == DCC_JSON_KEY_ID) {
        entitlement->id = value;
    } else if (key_id == DCC_JSON_KEY_SKU_ID) {
        entitlement->sku_id = value;
    } else if (key_id == DCC_JSON_KEY_APPLICATION_ID) {
        entitlement->application_id = value;
    } else if (key_id == DCC_JSON_KEY_SUBSCRIPTION_ID) {
        entitlement->subscription_id = value;
    } else if (key_id == DCC_JSON_KEY_PROMOTION_ID) {
        entitlement->promotion_id = value;
    } else if (key_id == DCC_JSON_KEY_USER_ID) {
        entitlement->user_id = value;
    } else {
        entitlement->guild_id = value;
    }

    *handled = 1;
    return DCC_OK;
}
