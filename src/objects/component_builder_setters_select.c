#include "internal/objects/dcc_component_builder_internal.h"

dcc_status_t dcc_component_builder_set_placeholder(
    dcc_component_builder_t *builder,
    const char *placeholder
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->placeholder = placeholder;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_options(
    dcc_component_builder_t *builder,
    const dcc_select_option_t *options,
    size_t options_count
) {
    if (builder == NULL || (options_count != 0 && options == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->options = options;
    builder->options_count = options_count;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_min_values(dcc_component_builder_t *builder, uint32_t min_values) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->min_values = min_values;
    builder->has_min_values = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_max_values(dcc_component_builder_t *builder, uint32_t max_values) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->max_values = max_values;
    builder->has_max_values = 1;
    return DCC_OK;
}
