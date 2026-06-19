#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_finalize_interaction_option_number_value(
    dcc_interaction_option_t *option,
    const char *number_value,
    int number_is_integer
) {
    if (option == NULL || number_value == NULL) {
        return DCC_ERR_JSON;
    }

    size_t len = strlen(number_value);
    if (option->type == 10U || !number_is_integer) {
        double parsed = 0.0;
        if (dcc_json_gateway_parse_double_ascii(number_value, len, &parsed) != 0) {
            return DCC_ERR_JSON;
        }
        option->number_value = parsed;
        option->value_type = DCC_INTERACTION_OPTION_VALUE_NUMBER;
        return DCC_OK;
    }

    if (dcc_json_gateway_interaction_option_type_is_snowflake(option->type)) {
        uint64_t parsed = 0;
        if (dcc_json_parse_u64_ascii(number_value, len, &parsed) != 0) {
            return DCC_ERR_JSON;
        }
        option->snowflake_value = parsed;
        option->value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE;
        return DCC_OK;
    }
    if (option->type == 5U) {
        return DCC_ERR_JSON;
    }

    int64_t parsed = 0;
    if (dcc_json_parse_i64_ascii(number_value, len, &parsed) != 0) {
        return DCC_ERR_JSON;
    }
    option->integer_value = parsed;
    option->value_type = DCC_INTERACTION_OPTION_VALUE_INTEGER;
    return DCC_OK;
}
