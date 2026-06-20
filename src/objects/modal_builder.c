#include "internal/objects/dcc_modal_builder_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_modal_builder_init(dcc_modal_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
    }
}

dcc_status_t dcc_modal_builder_set_custom_id(dcc_modal_builder_t *builder, const char *custom_id) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->custom_id = custom_id;
    builder->has_custom_id = custom_id != NULL;
    return DCC_OK;
}

dcc_status_t dcc_modal_builder_set_title(dcc_modal_builder_t *builder, const char *title) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->title = title;
    builder->has_title = title != NULL;
    return DCC_OK;
}

dcc_status_t dcc_modal_builder_set_components(
    dcc_modal_builder_t *builder,
    const dcc_component_builder_t *components,
    size_t components_count
) {
    if (builder == NULL || (components_count != 0 && components == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components = components;
    builder->components_count = components_count;
    return DCC_OK;
}

dcc_status_t dcc_modal_builder_set_components_v2(
    dcc_modal_builder_t *builder,
    const dcc_component_v2_builder_t *components,
    size_t components_count
) {
    if (builder == NULL || (components_count != 0 && components == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components_v2 = components;
    builder->components_v2_count = components_count;
    return DCC_OK;
}

dcc_status_t dcc_modal_builder_set_components_json(
    dcc_modal_builder_t *builder,
    const char *components_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components_json = components_json;
    return DCC_OK;
}

dcc_status_t dcc_modal_builder_set_components_v2_json(
    dcc_modal_builder_t *builder,
    const char *components_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components_v2_json = components_json;
    return DCC_OK;
}

void dcc_modal_builder_json_free(char *json) {
    free(json);
}
