#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_number_value(
    dcc_json_parser_t *parser,
    dcc_json_gateway_interaction_option_state_t *state
) {
    dcc_status_t status = dcc_json_gateway_parse_number_text(
        parser,
        state->number_value,
        sizeof(state->number_value),
        &state->number_is_integer
    );
    if (status != DCC_OK) {
        return status;
    }
    state->has_number = 1;
    state->has_string = 0;
    state->has_bool = 0;
    return DCC_OK;
}
