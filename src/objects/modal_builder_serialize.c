#include "internal/objects/dcc_modal_builder_internal.h"

dcc_status_t dcc_modal_builder_build_json(const dcc_modal_builder_t *builder, char **out_json) {
    if (builder == NULL ||
        out_json == NULL ||
        (builder->has_custom_id && builder->custom_id == NULL) ||
        (builder->has_title && builder->title == NULL) ||
        (builder->components_count != 0 && builder->components == NULL) ||
        (builder->components_v2_count != 0 && builder->components_v2 == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!builder->has_custom_id || !builder->has_title) {
        return DCC_ERR_INVALID_ARG;
    }
    const int has_legacy_components = builder->components_count != 0 ||
        builder->components_json != NULL;
    const int has_components_v2 = builder->components_v2_count != 0 ||
        builder->components_v2_json != NULL;
    if (!has_legacy_components && !has_components_v2) {
        return DCC_ERR_INVALID_ARG;
    }
    if (has_legacy_components && has_components_v2) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;

    dcc_modal_json_buffer_t buffer = {0};
    dcc_status_t status = dcc_modal_json_append_cstr(&buffer, "{\"type\":9,\"data\":{");
    if (status != DCC_OK) {
        dcc_modal_json_buffer_deinit(&buffer);
        return status;
    }

    int first = 1;
    status = dcc_modal_json_append_string_member(&buffer, &first, "custom_id", builder->custom_id);
    if (status == DCC_OK) {
        status = dcc_modal_json_append_string_member(&buffer, &first, "title", builder->title);
    }
    if (status != DCC_OK) {
        dcc_modal_json_buffer_deinit(&buffer);
        return status;
    }

    if (builder->components_v2_count != 0) {
        char *components_json = NULL;
        status = dcc_component_v2_builder_build_array_json(
            builder->components_v2,
            builder->components_v2_count,
            &components_json
        );
        if (status != DCC_OK) {
            dcc_modal_json_buffer_deinit(&buffer);
            return status;
        }
        status = dcc_modal_json_append_raw_member(&buffer, &first, "components", components_json);
        dcc_component_v2_builder_json_free(components_json);
    } else if (builder->components_v2_json != NULL) {
        status = dcc_modal_json_append_raw_member(&buffer, &first, "components", builder->components_v2_json);
    } else if (builder->components_count != 0) {
        char *components_json = NULL;
        status = dcc_component_builder_build_array_json(
            builder->components,
            builder->components_count,
            &components_json
        );
        if (status != DCC_OK) {
            dcc_modal_json_buffer_deinit(&buffer);
            return status;
        }
        status = dcc_modal_json_append_raw_member(&buffer, &first, "components", components_json);
        dcc_component_builder_json_free(components_json);
    } else {
        status = dcc_modal_json_append_raw_member(&buffer, &first, "components", builder->components_json);
    }
    if (status != DCC_OK) {
        dcc_modal_json_buffer_deinit(&buffer);
        return status;
    }

    status = dcc_modal_json_append_cstr(&buffer, "}}");
    if (status != DCC_OK) {
        dcc_modal_json_buffer_deinit(&buffer);
        return status;
    }

    *out_json = buffer.data;
    return DCC_OK;
}
