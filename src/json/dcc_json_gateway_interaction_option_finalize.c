#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_finalize_interaction_option_state(
    dcc_interaction_option_t *option,
    const dcc_json_gateway_interaction_option_state_t *state
) {
    if (option == NULL || state == NULL) {
        return DCC_ERR_JSON;
    }

    return dcc_json_gateway_finalize_interaction_option_value(
        option,
        state->has_string,
        state->string_value,
        state->has_bool,
        state->bool_value,
        state->has_number,
        state->number_value,
        state->number_is_integer
    );
}
