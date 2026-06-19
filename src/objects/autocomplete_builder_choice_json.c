#include "internal/objects/dcc_autocomplete_builder_internal.h"

#include <math.h>

static int dcc_autocomplete_choice_type_valid(dcc_autocomplete_choice_value_type_t type) {
    switch (type) {
        case DCC_AUTOCOMPLETE_CHOICE_STRING:
        case DCC_AUTOCOMPLETE_CHOICE_INTEGER:
        case DCC_AUTOCOMPLETE_CHOICE_NUMBER:
            return 1;
        default:
            return 0;
    }
}

static dcc_status_t dcc_autocomplete_choice_validate(const dcc_autocomplete_choice_t *choice) {
    if (choice == NULL ||
        !choice->has_name ||
        choice->name == NULL ||
        !choice->has_value ||
        !dcc_autocomplete_choice_type_valid(choice->value_type)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (choice->value_type == DCC_AUTOCOMPLETE_CHOICE_STRING && choice->value_string == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (choice->value_type == DCC_AUTOCOMPLETE_CHOICE_NUMBER && !isfinite(choice->value_number)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_append_choice(
    const dcc_autocomplete_choice_t *choice,
    dcc_autocomplete_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_autocomplete_choice_validate(choice);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_autocomplete_json_append_cstr(buffer, "{");
    if (status != DCC_OK) {
        return status;
    }

    int first = 1;
    status = dcc_autocomplete_json_append_string_member(buffer, &first, "name", choice->name);
    if (status == DCC_OK && choice->name_localizations_json != NULL) {
        status = dcc_autocomplete_json_append_raw_member(
            buffer,
            &first,
            "name_localizations",
            choice->name_localizations_json
        );
    }
    if (status == DCC_OK) {
        status = dcc_autocomplete_json_member_prefix(buffer, &first, "value");
    }
    if (status == DCC_OK) {
        switch (choice->value_type) {
            case DCC_AUTOCOMPLETE_CHOICE_STRING:
                status = dcc_autocomplete_json_append_escaped_string(buffer, choice->value_string);
                break;
            case DCC_AUTOCOMPLETE_CHOICE_INTEGER:
                status = dcc_autocomplete_json_append_i64(buffer, choice->value_integer);
                break;
            case DCC_AUTOCOMPLETE_CHOICE_NUMBER:
                status = dcc_autocomplete_json_append_double(buffer, choice->value_number);
                break;
            default:
                status = DCC_ERR_INVALID_ARG;
                break;
        }
    }
    if (status == DCC_OK) {
        status = dcc_autocomplete_json_append_cstr(buffer, "}");
    }
    return status;
}
