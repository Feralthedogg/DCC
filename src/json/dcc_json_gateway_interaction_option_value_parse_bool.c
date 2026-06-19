#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_bool_value(
    dcc_json_parser_t *parser,
    dcc_json_gateway_interaction_option_state_t *state
) {
    dcc_status_t status = dcc_json_parse_bool_direct(parser, &state->bool_value);
    if (status != DCC_OK) {
        return status;
    }
    state->has_bool = 1;
    state->has_string = 0;
    state->has_number = 0;
    return DCC_OK;
}
