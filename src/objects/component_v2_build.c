#include "internal/objects/dcc_component_v2_internal.h"

dcc_status_t dcc_component_v2_builder_build_json(
    const dcc_component_v2_builder_t *builder,
    char **out_json
) {
    if (builder == NULL || out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;

    dcc_status_t status = dcc_component_v2_validate_array(builder, 1U);
    if (status != DCC_OK) {
        return status;
    }

    dcc_component_json_buffer_t buffer = {0};
    status = dcc_component_v2_append_json(builder, &buffer);
    if (status != DCC_OK) {
        dcc_component_json_buffer_deinit(&buffer);
        return status;
    }

    *out_json = buffer.data;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_build_array_json(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count,
    char **out_json
) {
    if (out_json == NULL || (builder_count != 0 && builders == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;

    dcc_status_t status = dcc_component_v2_validate_array(builders, builder_count);
    if (status != DCC_OK) {
        return status;
    }

    dcc_component_json_buffer_t buffer = {0};
    status = dcc_component_v2_append_array_json(builders, builder_count, &buffer);
    if (status != DCC_OK) {
        dcc_component_json_buffer_deinit(&buffer);
        return status;
    }

    *out_json = buffer.data;
    return DCC_OK;
}
