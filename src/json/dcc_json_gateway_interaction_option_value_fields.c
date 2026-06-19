#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_value_field(
    dcc_json_parser_t *parser,
    dcc_json_key_id_t key_id,
    dcc_json_gateway_interaction_option_state_t *state,
    int *handled
) {
    if (parser == NULL || state == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id != DCC_JSON_KEY_VALUE) {
        *handled = 0;
        return DCC_OK;
    }

    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }

    dcc_status_t status = DCC_ERR_JSON;
    if (*parser->cur == 'n') {
        status = dcc_json_gateway_parse_interaction_option_null_value(parser, state);
    } else if (*parser->cur == '"') {
        status = dcc_json_gateway_parse_interaction_option_string_value(parser, state);
    } else if (*parser->cur == 't' || *parser->cur == 'f') {
        status = dcc_json_gateway_parse_interaction_option_bool_value(parser, state);
    } else if (*parser->cur == '-' || (*parser->cur >= '0' && *parser->cur <= '9')) {
        status = dcc_json_gateway_parse_interaction_option_number_value(parser, state);
    }
    if (status != DCC_OK) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    return DCC_OK;
}
