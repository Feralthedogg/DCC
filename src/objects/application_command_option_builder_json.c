#include "internal/objects/dcc_application_command_builder_internal.h"

#include <math.h>
#include <stdlib.h>

static int dcc_command_choice_type_valid(dcc_autocomplete_choice_value_type_t type) {
    switch (type) {
        case DCC_AUTOCOMPLETE_CHOICE_STRING:
        case DCC_AUTOCOMPLETE_CHOICE_INTEGER:
        case DCC_AUTOCOMPLETE_CHOICE_NUMBER:
            return 1;
        default:
            return 0;
    }
}

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
    if (choice == NULL ||
        !choice->has_name ||
        choice->name == NULL ||
        !choice->has_value ||
        !dcc_command_choice_type_valid(choice->value_type) ||
        !dcc_command_choice_matches_option(choice, option_type)) {
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

static dcc_status_t dcc_command_option_append_choice(
    const dcc_autocomplete_choice_t *choice,
    uint32_t option_type,
    dcc_application_command_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_command_choice_validate(choice, option_type);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_command_json_append_cstr(buffer, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_command_json_append_string_member(buffer, &first, "name", choice->name);
    }
    if (status == DCC_OK && choice->name_localizations_json != NULL) {
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
    if (option->choices_json != NULL) {
        return dcc_command_json_append_raw_member(buffer, first, "choices", option->choices_json);
    }
    if (option->choices_count == 0U) {
        return DCC_OK;
    }
    if (option->choices == NULL || option->choices_count > DCC_AUTOCOMPLETE_MAX_CHOICES) {
        return DCC_ERR_INVALID_ARG;
    }
    if (option->has_autocomplete && option->autocomplete) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_command_json_member_prefix(buffer, first, "choices");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_command_json_append_cstr(buffer, "[");
    for (size_t i = 0U; status == DCC_OK && i < option->choices_count; ++i) {
        if (i != 0U) {
            status = dcc_command_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_command_option_append_choice(&option->choices[i], option->type, buffer);
        }
    }
    return status == DCC_OK ? dcc_command_json_append_cstr(buffer, "]") : status;
}

static dcc_status_t dcc_command_option_append_channel_types(
    const dcc_application_command_option_builder_t *option,
    dcc_application_command_json_buffer_t *buffer,
    int *first
) {
    if (option->channel_types_count == 0U) {
        return DCC_OK;
    }
    if (option->channel_types == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_command_json_member_prefix(buffer, first, "channel_types");
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
    if (option->options_json != NULL) {
        return dcc_command_json_append_raw_member(buffer, first, "options", option->options_json);
    }
    if (option->options_count == 0U) {
        return DCC_OK;
    }
    if (option->options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_command_json_member_prefix(buffer, first, "options");
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
    if (option == NULL ||
        buffer == NULL ||
        option->name == NULL ||
        option->description == NULL ||
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
    if (status == DCC_OK && option->name_localizations_json != NULL) {
        status = dcc_command_json_append_raw_member(
            buffer,
            &first,
            "name_localizations",
            option->name_localizations_json
        );
    }
    if (status == DCC_OK && option->description_localizations_json != NULL) {
        status = dcc_command_json_append_raw_member(
            buffer,
            &first,
            "description_localizations",
            option->description_localizations_json
        );
    }
    if (status == DCC_OK && option->has_required) {
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
    if (status == DCC_OK && option->has_autocomplete) {
        status = dcc_command_json_member_prefix(buffer, &first, "autocomplete");
        if (status == DCC_OK) {
            status = dcc_command_json_append_cstr(buffer, option->autocomplete ? "true" : "false");
        }
    }
    if (status == DCC_OK && option->has_min_integer_value) {
        status = dcc_command_json_member_prefix(buffer, &first, "min_value");
        if (status == DCC_OK) {
            status = dcc_command_json_append_i64(buffer, option->min_integer_value);
        }
    }
    if (status == DCC_OK && option->has_max_integer_value) {
        status = dcc_command_json_member_prefix(buffer, &first, "max_value");
        if (status == DCC_OK) {
            status = dcc_command_json_append_i64(buffer, option->max_integer_value);
        }
    }
    if (status == DCC_OK && option->has_min_number_value) {
        status = dcc_command_json_member_prefix(buffer, &first, "min_value");
        if (status == DCC_OK) {
            status = dcc_command_json_append_double(buffer, option->min_number_value);
        }
    }
    if (status == DCC_OK && option->has_max_number_value) {
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

    dcc_status_t status = dcc_command_json_append_cstr(buffer, "[");
    for (size_t i = 0U; status == DCC_OK && i < option_count; ++i) {
        if (i != 0U) {
            status = dcc_command_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_command_option_append_json(&options[i], buffer);
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
