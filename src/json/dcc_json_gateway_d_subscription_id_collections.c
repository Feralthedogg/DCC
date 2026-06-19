#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_subscription_id_collection_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || out == NULL || handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id == DCC_JSON_KEY_SKU_IDS) {
        dcc_status_t status = dcc_json_parse_snowflake_array(
            parser,
            out->gateway_sku_ids,
            DCC_JSON_GATEWAY_ID_LIST_CAP,
            &out->gateway.sku_ids,
            &out->gateway.sku_ids_count,
            &out->gateway.sku_ids_truncated
        );
        if (status != DCC_OK) {
            return status;
        }
        out->subscription.sku_ids = out->gateway.sku_ids;
        out->subscription.sku_ids_count = out->gateway.sku_ids_count;
        out->subscription.sku_ids_truncated = out->gateway.sku_ids_truncated;
        out->has_subscription = 1;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_ENTITLEMENT_IDS) {
        dcc_status_t status = dcc_json_parse_snowflake_array(
            parser,
            out->gateway_entitlement_ids,
            DCC_JSON_GATEWAY_ID_LIST_CAP,
            &out->gateway.entitlement_ids,
            &out->gateway.entitlement_ids_count,
            &out->gateway.entitlement_ids_truncated
        );
        if (status != DCC_OK) {
            return status;
        }
        out->subscription.entitlement_ids = out->gateway.entitlement_ids;
        out->subscription.entitlement_ids_count = out->gateway.entitlement_ids_count;
        out->subscription.entitlement_ids_truncated = out->gateway.entitlement_ids_truncated;
        out->has_subscription = 1;
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
