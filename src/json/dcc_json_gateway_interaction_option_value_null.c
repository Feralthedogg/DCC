#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_null_value(
    dcc_json_parser_t *parser,
    dcc_json_gateway_interaction_option_state_t *state
) {
    if (dcc_json_match_literal(parser, "null") != 0) {
        return DCC_ERR_JSON;
    }
    state->has_string = 0;
    state->has_bool = 0;
    state->has_number = 0;
    state->string_value[0] = '\0';
    state->number_value[0] = '\0';
    return DCC_OK;
}
