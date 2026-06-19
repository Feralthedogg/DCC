#include "internal/objects/dcc_autocomplete_builder_internal.h"

#include <math.h>
#include <string.h>

void dcc_autocomplete_choice_init(dcc_autocomplete_choice_t *choice, const char *name) {
    if (choice != NULL) {
        memset(choice, 0, sizeof(*choice));
        choice->name = name;
        choice->has_name = name != NULL;
    }
}

dcc_status_t dcc_autocomplete_choice_set_name(dcc_autocomplete_choice_t *choice, const char *name) {
    if (choice == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    choice->name = name;
    choice->has_name = name != NULL;
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_choice_set_name_localizations_json(
    dcc_autocomplete_choice_t *choice,
    const char *name_localizations_json
) {
    if (choice == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    choice->name_localizations_json = name_localizations_json;
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_choice_set_string_value(
    dcc_autocomplete_choice_t *choice,
    const char *value
) {
    if (choice == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    choice->value_string = value;
    choice->value_type = DCC_AUTOCOMPLETE_CHOICE_STRING;
    choice->has_value = value != NULL;
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_choice_set_integer_value(
    dcc_autocomplete_choice_t *choice,
    int64_t value
) {
    if (choice == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    choice->value_integer = value;
    choice->value_type = DCC_AUTOCOMPLETE_CHOICE_INTEGER;
    choice->has_value = 1;
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_choice_set_number_value(
    dcc_autocomplete_choice_t *choice,
    double value
) {
    if (choice == NULL || !isfinite(value)) {
        return DCC_ERR_INVALID_ARG;
    }
    choice->value_number = value;
    choice->value_type = DCC_AUTOCOMPLETE_CHOICE_NUMBER;
    choice->has_value = 1;
    return DCC_OK;
}

void dcc_autocomplete_builder_init(dcc_autocomplete_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
    }
}

dcc_status_t dcc_autocomplete_builder_set_choices(
    dcc_autocomplete_builder_t *builder,
    const dcc_autocomplete_choice_t *choices,
    size_t choices_count
) {
    if (builder == NULL ||
        (choices_count != 0 && choices == NULL) ||
        choices_count > DCC_AUTOCOMPLETE_MAX_CHOICES) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->choices = choices;
    builder->choices_count = choices_count;
    return DCC_OK;
}
