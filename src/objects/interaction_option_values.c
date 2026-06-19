#include <dcc/objects/clone/interactions.h>

const char *dcc_interaction_option_string(const dcc_interaction_t *interaction, const char *name) {
    const dcc_interaction_option_t *option = dcc_interaction_option_by_name(interaction, name);
    if (option == NULL || option->value_type != DCC_INTERACTION_OPTION_VALUE_STRING) {
        return NULL;
    }
    return option->string_value;
}

int dcc_interaction_option_integer(
    const dcc_interaction_t *interaction,
    const char *name,
    int64_t *out
) {
    const dcc_interaction_option_t *option = dcc_interaction_option_by_name(interaction, name);
    if (option == NULL || out == NULL || option->value_type != DCC_INTERACTION_OPTION_VALUE_INTEGER) {
        return 0;
    }
    *out = option->integer_value;
    return 1;
}

int dcc_interaction_option_number(
    const dcc_interaction_t *interaction,
    const char *name,
    double *out
) {
    const dcc_interaction_option_t *option = dcc_interaction_option_by_name(interaction, name);
    if (option == NULL || out == NULL || option->value_type != DCC_INTERACTION_OPTION_VALUE_NUMBER) {
        return 0;
    }
    *out = option->number_value;
    return 1;
}

int dcc_interaction_option_boolean(
    const dcc_interaction_t *interaction,
    const char *name,
    uint8_t *out
) {
    const dcc_interaction_option_t *option = dcc_interaction_option_by_name(interaction, name);
    if (option == NULL || out == NULL || option->value_type != DCC_INTERACTION_OPTION_VALUE_BOOLEAN) {
        return 0;
    }
    *out = option->boolean_value;
    return 1;
}

int dcc_interaction_option_snowflake(
    const dcc_interaction_t *interaction,
    const char *name,
    dcc_snowflake_t *out
) {
    const dcc_interaction_option_t *option = dcc_interaction_option_by_name(interaction, name);
    if (option == NULL || out == NULL || option->value_type != DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE) {
        return 0;
    }
    *out = option->snowflake_value;
    return 1;
}
