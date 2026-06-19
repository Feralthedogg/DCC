#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_finalize_interaction_option_bool_value(
    dcc_interaction_option_t *option,
    int bool_value
) {
    if (option == NULL || (option->type != 0U && option->type != 5U)) {
        return DCC_ERR_JSON;
    }

    option->boolean_value = bool_value ? 1U : 0U;
    option->value_type = DCC_INTERACTION_OPTION_VALUE_BOOLEAN;
    return DCC_OK;
}
