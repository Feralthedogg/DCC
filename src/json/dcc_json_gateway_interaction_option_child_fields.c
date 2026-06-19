#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_child_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_interaction_option_t *option,
    dcc_json_key_id_t key_id,
    unsigned depth,
    int *handled
) {
    if (parser == NULL || out == NULL || option == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id != DCC_JSON_KEY_OPTIONS) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_gateway_parse_interaction_option_array(
        parser,
        out,
        &option->options,
        &option->options_count,
        &option->options_truncated,
        depth + 1U
    );
    if (status != DCC_OK) {
        return status;
    }

    *handled = 1;
    return DCC_OK;
}
