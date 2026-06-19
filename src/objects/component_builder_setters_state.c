#include "internal/objects/dcc_component_builder_internal.h"

dcc_status_t dcc_component_builder_set_children(
    dcc_component_builder_t *builder,
    const dcc_component_builder_t *children,
    size_t children_count
) {
    if (builder == NULL || (children_count != 0 && children == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->children = children;
    builder->children_count = children_count;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_disabled(dcc_component_builder_t *builder, uint8_t disabled) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->disabled = disabled ? 1U : 0U;
    builder->has_disabled = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_required(dcc_component_builder_t *builder, uint8_t required) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->required = required ? 1U : 0U;
    builder->has_required = 1;
    return DCC_OK;
}
