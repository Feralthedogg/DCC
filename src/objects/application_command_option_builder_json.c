#include "internal/objects/dcc_application_command_builder_internal.h"
#include "internal/objects/dcc_autocomplete_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <stdlib.h>

static int dcc_command_choice_matches_option(
    const dcc_autocomplete_choice_t *choice,
    uint32_t option_type
) {
    switch (option_type) {
        case DCC_APPLICATION_COMMAND_OPTION_STRING:
            return choice->value_type == DCC_AUTOCOMPLETE_CHOICE_STRING;
        case DCC_APPLICATION_COMMAND_OPTION_INTEGER:
            return choice->value_type == DCC_AUTOCOMPLETE_CHOICE_INTEGER;
        case DCC_APPLICATION_COMMAND_OPTION_NUMBER:
            return choice->value_type == DCC_AUTOCOMPLETE_CHOICE_NUMBER;
        default:
            return 0;
    }
}

static dcc_status_t dcc_command_choice_validate(
    const dcc_autocomplete_choice_t *choice,
    uint32_t option_type
) {
    dcc_builder_abi_view_t view;
    if (dcc_autocomplete_choice_semantic_validate(choice, &view) != DCC_OK ||
        !dcc_command_choice_matches_option(choice, option_type)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static dcc_status_t dcc_command_option_append_choice(
    const dcc_autocomplete_choice_t *choice,
    uint32_t option_type,
    dcc_application_command_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_command_choice_validate(choice, option_type);
    if (status != DCC_OK) {
        return status;
    }
    dcc_builder_abi_view_t view;
    status = dcc_autocomplete_choice_abi_validate(choice, &view);

    status = dcc_command_json_append_cstr(buffer, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_command_json_append_string_member(buffer, &first, "name", choice->name);
    }
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME_LOCALIZATIONS_JSON
        )) {
        status = dcc_command_json_append_raw_member(
            buffer,
            &first,
            "name_localizations",
            choice->name_localizations_json
        );
    }
    if (status == DCC_OK) {
        status = dcc_command_json_member_prefix(buffer, &first, "value");
    }
    if (status == DCC_OK) {
        switch (choice->value_type) {
            case DCC_AUTOCOMPLETE_CHOICE_STRING:
                status = dcc_command_json_append_escaped_string(buffer, choice->value_string);
                break;
            case DCC_AUTOCOMPLETE_CHOICE_INTEGER:
                status = dcc_command_json_append_i64(buffer, choice->value_integer);
                break;
            case DCC_AUTOCOMPLETE_CHOICE_NUMBER:
                status = dcc_command_json_append_double(buffer, choice->value_number);
                break;
            default:
                status = DCC_ERR_INVALID_ARG;
                break;
        }
    }
    return status == DCC_OK ? dcc_command_json_append_cstr(buffer, "}") : status;
}

static dcc_status_t dcc_command_option_append_choices(
    const dcc_application_command_option_builder_t *option,
    dcc_application_command_json_buffer_t *buffer,
    int *first
) {
    dcc_builder_abi_view_t view;
    dcc_status_t status = dcc_application_command_option_builder_abi_validate(option, &view);
    if (status != DCC_OK) {
        return status;
    }
    if (dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES_JSON
        )) {
        if (option->choices_json == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        return dcc_command_json_append_raw_member(buffer, first, "choices", option->choices_json);
    }
    if (!dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES
        ) || option->choices_count == 0U) {
        return DCC_OK;
    }
    if (option->choices == NULL || option->choices_count > DCC_AUTOCOMPLETE_MAX_CHOICES) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_AUTOCOMPLETE
        ) && option->autocomplete) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t stride = 0U;
    status = dcc_autocomplete_choice_array_begin(
        option->choices, option->choices_count, &stride
    );
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_command_json_member_prefix(buffer, first, "choices");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_command_json_append_cstr(buffer, "[");
    for (size_t i = 0U; status == DCC_OK && i < option->choices_count; ++i) {
        if (i != 0U) {
            status = dcc_command_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            const dcc_autocomplete_choice_t *choice = (const dcc_autocomplete_choice_t *)
                dcc_builder_abi_array_at(option->choices, stride, i);
            dcc_builder_abi_view_t choice_view;
            status = dcc_autocomplete_choice_abi_validate(choice, &choice_view);
            if (status == DCC_OK && choice_view.size != stride) {
                status = DCC_ERR_INVALID_ARG;
            }
            if (status == DCC_OK) {
                status = dcc_command_option_append_choice(choice, option->type, buffer);
            }
        }
    }
    return status == DCC_OK ? dcc_command_json_append_cstr(buffer, "]") : status;
}

static dcc_status_t dcc_command_option_append_channel_types(
    const dcc_application_command_option_builder_t *option,
    dcc_application_command_json_buffer_t *buffer,
    int *first
) {
    dcc_builder_abi_view_t view;
    dcc_status_t status = dcc_application_command_option_builder_abi_validate(option, &view);
    if (status != DCC_OK) {
        return status;
    }
    if (!dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHANNEL_TYPES
        ) || option->channel_types_count == 0U) {
        return DCC_OK;
    }
    if (option->channel_types == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    status = dcc_command_json_member_prefix(buffer, first, "channel_types");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_command_json_append_cstr(buffer, "[");
    for (size_t i = 0U; status == DCC_OK && i < option->channel_types_count; ++i) {
        if (i != 0U) {
            status = dcc_command_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_command_json_append_u32(buffer, option->channel_types[i]);
        }
    }
    return status == DCC_OK ? dcc_command_json_append_cstr(buffer, "]") : status;
}

static dcc_status_t dcc_command_option_append_nested(
    const dcc_application_command_option_builder_t *option,
    dcc_application_command_json_buffer_t *buffer,
    int *first
) {
    dcc_builder_abi_view_t view;
    dcc_status_t status = dcc_application_command_option_builder_abi_validate(option, &view);
    if (status != DCC_OK) {
        return status;
    }
    if (dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS_JSON
        )) {
        if (option->options_json == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        return dcc_command_json_append_raw_member(buffer, first, "options", option->options_json);
    }
    if (!dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS
        ) || option->options_count == 0U) {
        return DCC_OK;
    }
    if (option->options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    status = dcc_command_json_member_prefix(buffer, first, "options");
    if (status != DCC_OK) {
        return status;
    }
    return dcc_application_command_option_builder_append_array_json(
        option->options,
        option->options_count,
        buffer
    );
}

static dcc_status_t dcc_command_option_append_json(
    const dcc_application_command_option_builder_t *option,
    dcc_application_command_json_buffer_t *buffer
) {
    dcc_builder_abi_view_t view;
    if (buffer == NULL ||
        dcc_application_command_option_builder_abi_validate(option, &view) != DCC_OK ||
        !dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME) ||
        option->name == NULL ||
        !dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION) ||
        option->description == NULL ||
        !dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE) ||
        option->type < DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND ||
        option->type > DCC_APPLICATION_COMMAND_OPTION_ATTACHMENT) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_command_json_append_cstr(buffer, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_command_json_member_prefix(buffer, &first, "type");
    }
    if (status == DCC_OK) {
        status = dcc_command_json_append_u32(buffer, option->type);
    }
    if (status == DCC_OK) {
        status = dcc_command_json_append_string_member(buffer, &first, "name", option->name);
    }
    if (status == DCC_OK) {
        status = dcc_command_json_append_string_member(buffer, &first, "description", option->description);
    }
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON
        )) {
        status = dcc_command_json_append_raw_member(
            buffer,
            &first,
            "name_localizations",
            option->name_localizations_json
        );
    }
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON
        )) {
        status = dcc_command_json_append_raw_member(
            buffer,
            &first,
            "description_localizations",
            option->description_localizations_json
        );
    }
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_REQUIRED
        )) {
        status = dcc_command_json_member_prefix(buffer, &first, "required");
        if (status == DCC_OK) {
            status = dcc_command_json_append_cstr(buffer, option->required ? "true" : "false");
        }
    }
    if (status == DCC_OK) {
        status = dcc_command_option_append_choices(option, buffer, &first);
    }
    if (status == DCC_OK) {
        status = dcc_command_option_append_nested(option, buffer, &first);
    }
    if (status == DCC_OK) {
        status = dcc_command_option_append_channel_types(option, buffer, &first);
    }
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_AUTOCOMPLETE
        )) {
        status = dcc_command_json_member_prefix(buffer, &first, "autocomplete");
        if (status == DCC_OK) {
            status = dcc_command_json_append_cstr(buffer, option->autocomplete ? "true" : "false");
        }
    }
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_INTEGER_VALUE
        )) {
        status = dcc_command_json_member_prefix(buffer, &first, "min_value");
        if (status == DCC_OK) {
            status = dcc_command_json_append_i64(buffer, option->min_integer_value);
        }
    }
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_INTEGER_VALUE
        )) {
        status = dcc_command_json_member_prefix(buffer, &first, "max_value");
        if (status == DCC_OK) {
            status = dcc_command_json_append_i64(buffer, option->max_integer_value);
        }
    }
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_NUMBER_VALUE
        )) {
        status = dcc_command_json_member_prefix(buffer, &first, "min_value");
        if (status == DCC_OK) {
            status = dcc_command_json_append_double(buffer, option->min_number_value);
        }
    }
    if (status == DCC_OK && dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_NUMBER_VALUE
        )) {
        status = dcc_command_json_member_prefix(buffer, &first, "max_value");
        if (status == DCC_OK) {
            status = dcc_command_json_append_double(buffer, option->max_number_value);
        }
    }
    return status == DCC_OK ? dcc_command_json_append_cstr(buffer, "}") : status;
}

dcc_status_t dcc_application_command_option_builder_append_array_json(
    const dcc_application_command_option_builder_t *options,
    size_t option_count,
    dcc_application_command_json_buffer_t *buffer
) {
    if (buffer == NULL || (option_count != 0U && options == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t stride = 0U;
    dcc_status_t status = dcc_application_command_option_builder_array_begin(
        options, option_count, &stride
    );
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_command_json_append_cstr(buffer, "[");
    for (size_t i = 0U; status == DCC_OK && i < option_count; ++i) {
        if (i != 0U) {
            status = dcc_command_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            const dcc_application_command_option_builder_t *option =
                (const dcc_application_command_option_builder_t *)
                    dcc_builder_abi_array_at(options, stride, i);
            dcc_builder_abi_view_t view;
            status = dcc_application_command_option_builder_abi_validate(option, &view);
            if (status == DCC_OK && view.size != stride) {
                status = DCC_ERR_INVALID_ARG;
            }
            if (status == DCC_OK) {
                status = dcc_command_option_append_json(option, buffer);
            }
        }
    }
    return status == DCC_OK ? dcc_command_json_append_cstr(buffer, "]") : status;
}

dcc_status_t dcc_application_command_option_builder_build_array_json(
    const dcc_application_command_option_builder_t *options,
    size_t option_count,
    char **out_json
) {
    if (out_json == NULL || (option_count != 0U && options == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;

    dcc_application_command_json_buffer_t buffer = {0};
    dcc_status_t status = dcc_application_command_option_builder_append_array_json(
        options,
        option_count,
        &buffer
    );
    if (status != DCC_OK) {
        dcc_command_json_buffer_deinit(&buffer);
        return status;
    }
    *out_json = buffer.data;
    return DCC_OK;
}

void dcc_application_command_option_builder_json_free(char *json) {
    free(json);
}
