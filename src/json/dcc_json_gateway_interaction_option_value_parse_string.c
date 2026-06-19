#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_string_value(
    dcc_json_parser_t *parser,
    dcc_json_gateway_interaction_option_state_t *state
) {
    if (dcc_json_parse_string_buffer(
            parser,
            state->string_value,
            DCC_JSON_GATEWAY_INTERACTION_OPTION_STRING_CAP
        ) != 0) {
        return DCC_ERR_JSON;
    }
    state->has_string = 1;
    state->has_bool = 0;
    state->has_number = 0;
    return DCC_OK;
}
