#include "internal/objects/dcc_autocomplete_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

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

dcc_status_t dcc_autocomplete_choice_semantic_validate(
    const dcc_autocomplete_choice_t *choice,
    dcc_builder_abi_view_t *out_view
) {
    if (out_view == NULL ||
        dcc_autocomplete_choice_abi_validate(choice, out_view) != DCC_OK ||
        !dcc_builder_abi_view_has(out_view, DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME) ||
        choice->name == NULL ||
        !dcc_builder_abi_view_has(out_view, DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE) ||
        !dcc_autocomplete_choice_type_valid(choice->value_type)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (choice->value_type == DCC_AUTOCOMPLETE_CHOICE_STRING && choice->value_string == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (choice->value_type == DCC_AUTOCOMPLETE_CHOICE_NUMBER && !isfinite(choice->value_number)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_builder_abi_view_has(
            out_view, DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME_LOCALIZATIONS_JSON
        ) && choice->name_localizations_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_append_choice(
    const dcc_autocomplete_choice_t *choice,
    dcc_autocomplete_json_buffer_t *buffer
) {
    dcc_builder_abi_view_t view;
    dcc_status_t status = dcc_autocomplete_choice_semantic_validate(choice, &view);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_autocomplete_json_append_cstr(buffer, "{");
    if (status != DCC_OK) {
        return status;
    }

    int first = 1;
    status = dcc_autocomplete_json_append_string_member(buffer, &first, "name", choice->name);
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME_LOCALIZATIONS_JSON
        )) {
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
