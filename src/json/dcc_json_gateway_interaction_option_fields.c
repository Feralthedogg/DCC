#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_interaction_option_t *option,
    dcc_json_key_id_t key_id,
    dcc_json_gateway_interaction_option_state_t *state,
    unsigned depth
) {
    if (parser == NULL || out == NULL || option == NULL || state == NULL) {
        return DCC_ERR_JSON;
    }

    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_interaction_option_meta_field(
        parser,
        option,
        key_id,
        state,
        &handled
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!handled) {
        status = dcc_json_gateway_parse_interaction_option_value_field(
            parser,
            key_id,
            state,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_interaction_option_child_field(
            parser,
            out,
            option,
            key_id,
            depth,
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
