#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_application_type_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_APPLICATION_ID) {
        dcc_status_t status = dcc_json_gateway_parse_d_application_id_key(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else if (key_id == DCC_JSON_KEY_TYPE) {
        dcc_status_t status = dcc_json_gateway_parse_d_type_key(parser, out);
        if (status != DCC_OK) {
            return status;
        }
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
