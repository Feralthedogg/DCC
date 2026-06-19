#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_permission_collection_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_PERMISSIONS) {
        *handled = 0;
        return DCC_OK;
    }
    if (parser->cur >= parser->end || *parser->cur != '[') {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_gateway_parse_application_command_permissions_array(parser, out);
    if (status != DCC_OK) {
        return status;
    }

    *handled = 1;
    return DCC_OK;
}
