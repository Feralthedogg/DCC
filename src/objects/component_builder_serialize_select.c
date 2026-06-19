#include "internal/objects/dcc_component_builder_internal.h"

static dcc_status_t dcc_component_append_select_options_json(
    const dcc_select_option_t *options,
    size_t options_count,
    dcc_component_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "[");
    for (size_t i = 0; status == DCC_OK && i < options_count; ++i) {
        const dcc_select_option_t *option = &options[i];
        if (i != 0) {
            status = dcc_component_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_component_json_append_cstr(buffer, "{");
        }
        int first = 1;
        if (status == DCC_OK) {
            status = dcc_component_json_append_string_member(buffer, &first, "label", option->label);
        }
        if (status == DCC_OK) {
            status = dcc_component_json_append_string_member(buffer, &first, "value", option->value);
        }
        if (status == DCC_OK && option->description != NULL) {
            status = dcc_component_json_append_string_member(buffer, &first, "description", option->description);
        }
        if (status == DCC_OK && option->has_emoji) {
            status = dcc_component_json_member_prefix(buffer, &first, "emoji");
            if (status == DCC_OK) {
                status = dcc_component_append_emoji_json(&option->emoji, buffer);
            }
        }
        if (status == DCC_OK && option->has_default) {
            status = dcc_component_json_append_bool_member(buffer, &first, "default", option->is_default);
        }
        if (status == DCC_OK) {
            status = dcc_component_json_append_cstr(buffer, "}");
        }
    }
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "]");
    }
    return status;
}

dcc_status_t dcc_component_append_select_json(
    const dcc_component_builder_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = DCC_OK;
    if (builder->custom_id != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "custom_id", builder->custom_id);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->options_count != 0) {
        status = dcc_component_json_member_prefix(buffer, first, "options");
        if (status == DCC_OK) {
            status = dcc_component_append_select_options_json(builder->options, builder->options_count, buffer);
        }
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->placeholder != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "placeholder", builder->placeholder);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_min_values) {
        status = dcc_component_json_append_u32_member(buffer, first, "min_values", builder->min_values);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_max_values) {
        status = dcc_component_json_append_u32_member(buffer, first, "max_values", builder->max_values);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_disabled) {
        status = dcc_component_json_append_bool_member(buffer, first, "disabled", builder->disabled);
        if (status != DCC_OK) {
            return status;
        }
    }

    return DCC_OK;
}
