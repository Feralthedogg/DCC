#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_data_object_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || out == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_VALUES) {
        dcc_status_t status = dcc_json_gateway_parse_interaction_values(parser, out);
        if (status != DCC_OK) {
            return status;
        }
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_COMPONENTS) {
        dcc_status_t status = dcc_json_gateway_parse_interaction_components(parser, out);
        if (status != DCC_OK) {
            return status;
        }
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_OPTIONS) {
        dcc_status_t status = dcc_json_gateway_parse_interaction_options(parser, out);
        if (status != DCC_OK) {
            return status;
        }
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_RESOLVED) {
        dcc_status_t status = dcc_json_gateway_parse_interaction_resolved(parser, out);
        if (status != DCC_OK) {
            return status;
        }
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
