#include "internal/objects/dcc_component_builder_internal.h"

dcc_status_t dcc_component_append_text_input_json(
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
    if (builder->has_text_input_style) {
        status = dcc_component_json_append_u32_member(
            buffer,
            first,
            "style",
            (uint32_t)builder->text_input_style
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->label != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "label", builder->label);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_min_length) {
        status = dcc_component_json_append_u32_member(buffer, first, "min_length", builder->min_length);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_max_length) {
        status = dcc_component_json_append_u32_member(buffer, first, "max_length", builder->max_length);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_required) {
        status = dcc_component_json_append_bool_member(buffer, first, "required", builder->required);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->value != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "value", builder->value);
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

    return DCC_OK;
}
