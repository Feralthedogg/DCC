#include "internal/objects/dcc_autocomplete_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <math.h>
#include <string.h>

void dcc_autocomplete_choice_init(dcc_autocomplete_choice_t *choice, const char *name) {
    if (choice != NULL) {
        memset(choice, 0, sizeof(*choice));
        choice->size = sizeof(*choice);
        choice->version = DCC_AUTOCOMPLETE_CHOICE_VERSION;
        choice->name = name;
        if (name != NULL) {
            choice->present |= DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME;
        }
    }
}

static int dcc_autocomplete_choice_target(
    dcc_autocomplete_choice_t *choice,
    size_t offset,
    size_t width
) {
    dcc_builder_abi_view_t view;
    return dcc_autocomplete_choice_abi_validate(choice, &view) == DCC_OK &&
        dcc_builder_abi_field_covered(view.size, offset, width);
}

static int dcc_autocomplete_value_target(dcc_autocomplete_choice_t *choice) {
    dcc_builder_abi_view_t view;
    return dcc_autocomplete_choice_abi_validate(choice, &view) == DCC_OK &&
        DCC_BUILDER_ABI_FIELD(dcc_autocomplete_choice_t, &view, value_string) &&
        DCC_BUILDER_ABI_FIELD(dcc_autocomplete_choice_t, &view, value_integer) &&
        DCC_BUILDER_ABI_FIELD(dcc_autocomplete_choice_t, &view, value_number) &&
        DCC_BUILDER_ABI_FIELD(dcc_autocomplete_choice_t, &view, value_type);
}

static void dcc_autocomplete_choice_presence(
    dcc_autocomplete_choice_t *choice,
    uint64_t bit,
    int present
) {
    if (present) {
        choice->present |= bit;
    } else {
        choice->present &= ~bit;
    }
}

dcc_status_t dcc_autocomplete_choice_set_name(dcc_autocomplete_choice_t *choice, const char *name) {
    if (!dcc_autocomplete_choice_target(
            choice, offsetof(dcc_autocomplete_choice_t, name), sizeof(choice->name)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    choice->name = name;
    dcc_autocomplete_choice_presence(
        choice, DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME, name != NULL
    );
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_choice_set_name_localizations_json(
    dcc_autocomplete_choice_t *choice,
    const char *name_localizations_json
) {
    if (!dcc_autocomplete_choice_target(
            choice,
            offsetof(dcc_autocomplete_choice_t, name_localizations_json),
            sizeof(choice->name_localizations_json)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    choice->name_localizations_json = name_localizations_json;
    dcc_autocomplete_choice_presence(
        choice,
        DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME_LOCALIZATIONS_JSON,
        name_localizations_json != NULL
    );
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_choice_set_string_value(
    dcc_autocomplete_choice_t *choice,
    const char *value
) {
    if (!dcc_autocomplete_value_target(choice)) {
        return DCC_ERR_INVALID_ARG;
    }
    choice->value_string = value;
    choice->value_type = DCC_AUTOCOMPLETE_CHOICE_STRING;
    dcc_autocomplete_choice_presence(
        choice, DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE, value != NULL
    );
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_choice_set_integer_value(
    dcc_autocomplete_choice_t *choice,
    int64_t value
) {
    if (!dcc_autocomplete_value_target(choice)) {
        return DCC_ERR_INVALID_ARG;
    }
    choice->value_integer = value;
    choice->value_type = DCC_AUTOCOMPLETE_CHOICE_INTEGER;
    choice->present |= DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE;
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_choice_set_number_value(
    dcc_autocomplete_choice_t *choice,
    double value
) {
    if (!isfinite(value) || !dcc_autocomplete_value_target(choice)) {
        return DCC_ERR_INVALID_ARG;
    }
    choice->value_number = value;
    choice->value_type = DCC_AUTOCOMPLETE_CHOICE_NUMBER;
    choice->present |= DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE;
    return DCC_OK;
}

void dcc_autocomplete_builder_init(dcc_autocomplete_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
        builder->size = sizeof(*builder);
        builder->version = DCC_AUTOCOMPLETE_BUILDER_VERSION;
    }
}

dcc_status_t dcc_autocomplete_builder_set_choices(
    dcc_autocomplete_builder_t *builder,
    const dcc_autocomplete_choice_t *choices,
    size_t choices_count
) {
    dcc_builder_abi_view_t view;
    if (dcc_autocomplete_builder_abi_validate(builder, &view) != DCC_OK ||
        !DCC_BUILDER_ABI_FIELD(dcc_autocomplete_builder_t, &view, choices) ||
        !DCC_BUILDER_ABI_FIELD(dcc_autocomplete_builder_t, &view, choices_count) ||
        (choices_count != 0 && choices == NULL) ||
        choices_count > DCC_AUTOCOMPLETE_MAX_CHOICES) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->choices = choices;
    builder->choices_count = choices_count;
    if (choices_count != 0U) {
        builder->present |= DCC_AUTOCOMPLETE_BUILDER_PRESENT_CHOICES;
    } else {
        builder->present &= ~DCC_AUTOCOMPLETE_BUILDER_PRESENT_CHOICES;
    }
    return DCC_OK;
}
