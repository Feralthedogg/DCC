#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_finalize_interaction_option_value(
    dcc_interaction_option_t *option,
    int has_string,
    const char *string_value,
    int has_bool,
    int bool_value,
    int has_number,
    const char *number_value,
    int number_is_integer
) {
    if (option == NULL) {
        return DCC_ERR_JSON;
    }

    if (has_string) {
        return dcc_json_gateway_finalize_interaction_option_string_value(option, string_value);
    }

    if (has_bool) {
        return dcc_json_gateway_finalize_interaction_option_bool_value(option, bool_value);
    }

    if (has_number) {
        return dcc_json_gateway_finalize_interaction_option_number_value(
            option,
            number_value,
            number_is_integer
        );
    }

    option->value_type = DCC_INTERACTION_OPTION_VALUE_NONE;
    return DCC_OK;
}
