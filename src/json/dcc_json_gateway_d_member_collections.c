#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_member_collection_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    switch (key_id) {
        case DCC_JSON_KEY_MEMBERS:
        case DCC_JSON_KEY_ADDED_MEMBERS:
        case DCC_JSON_KEY_REMOVED_MEMBER_IDS:
        case DCC_JSON_KEY_NOT_FOUND:
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    dcc_status_t status = DCC_OK;
    if (key_id == DCC_JSON_KEY_MEMBERS ||
        key_id == DCC_JSON_KEY_ADDED_MEMBERS) {
        status = dcc_json_gateway_parse_d_member_array_collection(parser, out, key_id);
    } else if (key_id == DCC_JSON_KEY_REMOVED_MEMBER_IDS ||
               key_id == DCC_JSON_KEY_NOT_FOUND) {
        status = dcc_json_gateway_parse_d_member_id_collection(parser, out, key_id);
    } else {
        *handled = 0;
        return DCC_OK;
    }
    if (status != DCC_OK) {
        return status;
    }

    *handled = 1;
    return DCC_OK;
}
