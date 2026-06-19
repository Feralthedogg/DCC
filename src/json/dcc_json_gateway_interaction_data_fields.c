#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_data_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id
) {
    if (parser == NULL || out == NULL) {
        return DCC_ERR_JSON;
    }

    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_interaction_data_id_field(
        parser,
        out,
        key_id,
        &handled
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!handled) {
        status = dcc_json_gateway_parse_interaction_data_scalar_field(
            parser,
            out,
            key_id,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_interaction_data_text_field(
            parser,
            out,
            key_id,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_interaction_data_object_field(
            parser,
            out,
            key_id,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_skip_value(parser);
    }
    return status;
}
