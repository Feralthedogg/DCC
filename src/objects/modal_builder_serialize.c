#include "internal/objects/dcc_modal_builder_internal.h"
#include "internal/objects/dcc_component_validation_internal.h"
#include "internal/objects/dcc_component_v2_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <string.h>

#define DCC_MODAL_MAX_LEGACY_COMPONENTS 5U

dcc_status_t dcc_modal_builder_validate_for_json(
    const dcc_modal_builder_t *builder
) {
    dcc_builder_abi_view_t view;
    if (dcc_modal_builder_abi_validate(builder, &view) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
#define HAS(bit_) dcc_builder_abi_view_has(&view, (bit_))
    if (!HAS(DCC_MODAL_BUILDER_PRESENT_CUSTOM_ID) || builder->custom_id == NULL ||
        !HAS(DCC_MODAL_BUILDER_PRESENT_TITLE) || builder->title == NULL ||
        (HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS) &&
            (builder->components_count > DCC_MODAL_MAX_LEGACY_COMPONENTS ||
             (builder->components_count != 0U && builder->components == NULL))) ||
        (HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2) &&
            (builder->components_v2_count > DCC_COMPONENT_V2_MAX_COMPONENTS ||
             (builder->components_v2_count != 0U && builder->components_v2 == NULL))) ||
        (HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS_JSON) &&
            builder->components_json == NULL) ||
        (HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2_JSON) &&
            builder->components_v2_json == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t custom_id_length = strlen(builder->custom_id);
    size_t title_length = strlen(builder->title);
    if (custom_id_length == 0U || custom_id_length > 100U ||
        title_length == 0U || title_length > 45U) {
        return DCC_ERR_INVALID_ARG;
    }
    const int has_legacy_components =
        (HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS) &&
            builder->components_count != 0U) ||
        HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS_JSON);
    const int has_components_v2 =
        (HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2) &&
            builder->components_v2_count != 0U) ||
        HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2_JSON);
    if ((!has_legacy_components && !has_components_v2) ||
        (has_legacy_components && has_components_v2)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS)) {
        for (size_t i = 0U; i < builder->components_count; ++i) {
            if (dcc_component_validate(&builder->components[i]) != DCC_OK) {
                return DCC_ERR_INVALID_ARG;
            }
        }
    }
    if (HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2) &&
        dcc_component_v2_validate(
            builder->components_v2,
            builder->components_v2_count,
            DCC_COMPONENT_V2_CONTEXT_MODAL,
            NULL
        ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
#undef HAS
    return DCC_OK;
}

dcc_status_t dcc_modal_builder_build_json(const dcc_modal_builder_t *builder, char **out_json) {
    if (out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    dcc_status_t status = dcc_modal_builder_validate_for_json(builder);
    if (status != DCC_OK) {
        return status;
    }
    dcc_builder_abi_view_t view;
    (void)dcc_modal_builder_abi_validate(builder, &view);
#define HAS(bit_) dcc_builder_abi_view_has(&view, (bit_))
    dcc_modal_json_buffer_t buffer = {0};
    status = dcc_modal_json_append_cstr(&buffer, "{\"type\":9,\"data\":{");
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

    if (HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2) && builder->components_v2_count != 0U) {
        char *components_json = NULL;
        status = dcc_component_v2_validate(
            builder->components_v2,
            builder->components_v2_count,
            DCC_COMPONENT_V2_CONTEXT_MODAL,
            NULL
        );
        if (status == DCC_OK) {
            status = dcc_component_v2_builder_build_array_json(
                builder->components_v2,
                builder->components_v2_count,
                &components_json
            );
        }
        if (status != DCC_OK) {
            dcc_modal_json_buffer_deinit(&buffer);
            return status;
        }
        status = dcc_modal_json_append_raw_member(&buffer, &first, "components", components_json);
        dcc_component_v2_builder_json_free(components_json);
    } else if (HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2_JSON)) {
        status = dcc_modal_json_append_raw_member(&buffer, &first, "components", builder->components_v2_json);
    } else if (HAS(DCC_MODAL_BUILDER_PRESENT_COMPONENTS) && builder->components_count != 0U) {
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
    #undef HAS
    return DCC_OK;
}
