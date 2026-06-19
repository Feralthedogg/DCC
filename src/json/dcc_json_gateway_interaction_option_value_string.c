#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_finalize_interaction_option_string_value(
    dcc_interaction_option_t *option,
    const char *string_value
) {
    if (option == NULL || string_value == NULL) {
        return DCC_ERR_JSON;
    }

    size_t len = strlen(string_value);
    if (dcc_json_gateway_interaction_option_type_is_snowflake(option->type)) {
        uint64_t parsed = 0;
        if (dcc_json_parse_u64_ascii(string_value, len, &parsed) != 0) {
            return DCC_ERR_JSON;
        }
        option->snowflake_value = parsed;
        option->string_value = string_value;
        option->value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE;
        return DCC_OK;
    }
    if (option->type == 4U) {
        int64_t parsed = 0;
        if (dcc_json_parse_i64_ascii(string_value, len, &parsed) != 0) {
            return DCC_ERR_JSON;
        }
        option->integer_value = parsed;
        option->string_value = string_value;
        option->value_type = DCC_INTERACTION_OPTION_VALUE_INTEGER;
        return DCC_OK;
    }
    if (option->type == 5U) {
        return DCC_ERR_JSON;
    }
    if (option->type == 10U) {
        double parsed = 0.0;
        if (dcc_json_gateway_parse_double_ascii(string_value, len, &parsed) != 0) {
            return DCC_ERR_JSON;
        }
        option->number_value = parsed;
        option->string_value = string_value;
        option->value_type = DCC_INTERACTION_OPTION_VALUE_NUMBER;
        return DCC_OK;
    }

    option->string_value = string_value;
    option->value_type = DCC_INTERACTION_OPTION_VALUE_STRING;
    return DCC_OK;
}
