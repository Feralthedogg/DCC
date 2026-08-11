#include "internal/objects/dcc_modal_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_modal_builder_init(dcc_modal_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
        builder->size = sizeof(*builder);
        builder->version = DCC_MODAL_BUILDER_VERSION;
    }
}

static int dcc_modal_target(
    dcc_modal_builder_t *builder,
    size_t first_offset,
    size_t first_width,
    size_t second_offset,
    size_t second_width
) {
    dcc_builder_abi_view_t view;
    return dcc_modal_builder_abi_validate(builder, &view) == DCC_OK &&
        dcc_builder_abi_field_covered(view.size, first_offset, first_width) &&
        (second_width == 0U ||
            dcc_builder_abi_field_covered(view.size, second_offset, second_width));
}

static void dcc_modal_presence(dcc_modal_builder_t *builder, uint64_t bit, int present) {
    if (present) {
        builder->present |= bit;
    } else {
        builder->present &= ~bit;
    }
}

#define DCC_MODAL_SINGLE_TARGET(builder_, field_) \
    dcc_modal_target( \
        (builder_), offsetof(dcc_modal_builder_t, field_), \
        sizeof(((dcc_modal_builder_t *)0)->field_), 0U, 0U \
    )
#define DCC_MODAL_PAIR_TARGET(builder_, first_, second_) \
    dcc_modal_target( \
        (builder_), offsetof(dcc_modal_builder_t, first_), \
        sizeof(((dcc_modal_builder_t *)0)->first_), \
        offsetof(dcc_modal_builder_t, second_), \
        sizeof(((dcc_modal_builder_t *)0)->second_) \
    )

dcc_status_t dcc_modal_builder_set_custom_id(dcc_modal_builder_t *builder, const char *custom_id) {
    if (!DCC_MODAL_SINGLE_TARGET(builder, custom_id)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->custom_id = custom_id;
    dcc_modal_presence(builder, DCC_MODAL_BUILDER_PRESENT_CUSTOM_ID, custom_id != NULL);
    return DCC_OK;
}

dcc_status_t dcc_modal_builder_set_title(dcc_modal_builder_t *builder, const char *title) {
    if (!DCC_MODAL_SINGLE_TARGET(builder, title)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->title = title;
    dcc_modal_presence(builder, DCC_MODAL_BUILDER_PRESENT_TITLE, title != NULL);
    return DCC_OK;
}

dcc_status_t dcc_modal_builder_set_components(
    dcc_modal_builder_t *builder,
    const dcc_component_builder_t *components,
    size_t components_count
) {
    if (!DCC_MODAL_PAIR_TARGET(builder, components, components_count) ||
        (components_count != 0 && components == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components = components;
    builder->components_count = components_count;
    dcc_modal_presence(builder, DCC_MODAL_BUILDER_PRESENT_COMPONENTS, components_count != 0U);
    return DCC_OK;
}

dcc_status_t dcc_modal_builder_set_components_v2(
    dcc_modal_builder_t *builder,
    const dcc_component_v2_builder_t *components,
    size_t components_count
) {
    if (!DCC_MODAL_PAIR_TARGET(builder, components_v2, components_v2_count) ||
        (components_count != 0 && components == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components_v2 = components;
    builder->components_v2_count = components_count;
    dcc_modal_presence(builder, DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2, components_count != 0U);
    return DCC_OK;
}

dcc_status_t dcc_modal_builder_set_components_json(
    dcc_modal_builder_t *builder,
    const char *components_json
) {
    if (!DCC_MODAL_SINGLE_TARGET(builder, components_json)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components_json = components_json;
    dcc_modal_presence(
        builder, DCC_MODAL_BUILDER_PRESENT_COMPONENTS_JSON, components_json != NULL
    );
    return DCC_OK;
}

dcc_status_t dcc_modal_builder_set_components_v2_json(
    dcc_modal_builder_t *builder,
    const char *components_json
) {
    if (!DCC_MODAL_SINGLE_TARGET(builder, components_v2_json)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components_v2_json = components_json;
    dcc_modal_presence(
        builder, DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2_JSON, components_json != NULL
    );
    return DCC_OK;
}

void dcc_modal_builder_json_free(char *json) {
    free(json);
}
