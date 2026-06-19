#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_bulk_id_collection_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || out == NULL || handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_IDS) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_snowflake_array(
        parser,
        out->gateway_ids,
        DCC_JSON_GATEWAY_ID_LIST_CAP,
        &out->gateway.ids,
        &out->gateway.ids_count,
        &out->gateway.ids_truncated
    );
    if (status != DCC_OK) {
        return status;
    }
    out->message_delete_bulk.ids = out->gateway.ids;
    out->message_delete_bulk.ids_count = out->gateway.ids_count;
    out->message_delete_bulk.ids_truncated = out->gateway.ids_truncated;
    out->has_message_delete_bulk = 1;
    *handled = 1;
    return DCC_OK;
}
