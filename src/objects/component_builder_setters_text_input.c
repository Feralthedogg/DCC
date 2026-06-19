#include "internal/objects/dcc_component_builder_internal.h"

dcc_status_t dcc_component_builder_set_text_input_style(
    dcc_component_builder_t *builder,
    dcc_text_input_style_t style
) {
    if (builder == NULL || !dcc_text_input_style_valid(style)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->text_input_style = style;
    builder->has_text_input_style = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_min_length(dcc_component_builder_t *builder, uint32_t min_length) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->min_length = min_length;
    builder->has_min_length = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_max_length(dcc_component_builder_t *builder, uint32_t max_length) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->max_length = max_length;
    builder->has_max_length = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_value(dcc_component_builder_t *builder, const char *value) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->value = value;
    return DCC_OK;
}
