#include <dcc/component_v2.h>

dcc_status_t dcc_component_v2_builder_init_text_display(
    dcc_component_v2_builder_t *builder,
    const char *content
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_builder_init(builder, DCC_COMPONENT_V2_TEXT_DISPLAY);
    return dcc_component_v2_builder_set_content(builder, content);
}

dcc_status_t dcc_component_v2_builder_init_button(
    dcc_component_v2_builder_t *builder,
    dcc_button_style_t style,
    const char *label,
    const char *custom_id
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_builder_init(builder, DCC_COMPONENT_V2_BUTTON);
    dcc_status_t status = dcc_component_v2_builder_set_button_style(builder, style);
    if (status == DCC_OK) {
        status = dcc_component_v2_builder_set_label(builder, label);
    }
    if (status == DCC_OK) {
        status = dcc_component_v2_builder_set_custom_id(builder, custom_id);
    }
    return status;
}

dcc_status_t dcc_component_v2_builder_init_link_button(
    dcc_component_v2_builder_t *builder,
    const char *label,
    const char *url
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_builder_init(builder, DCC_COMPONENT_V2_BUTTON);
    dcc_status_t status = dcc_component_v2_builder_set_button_style(builder, DCC_BUTTON_LINK);
    if (status == DCC_OK) {
        status = dcc_component_v2_builder_set_label(builder, label);
    }
    if (status == DCC_OK) {
        status = dcc_component_v2_builder_set_url(builder, url);
    }
    return status;
}

dcc_status_t dcc_component_v2_builder_init_action_row(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *children,
    size_t children_count
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_builder_init(builder, DCC_COMPONENT_V2_ACTION_ROW);
    return dcc_component_v2_builder_set_children(builder, children, children_count);
}

dcc_status_t dcc_component_v2_builder_init_section(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *children,
    size_t children_count,
    const dcc_component_v2_builder_t *accessory
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_builder_init(builder, DCC_COMPONENT_V2_SECTION);
    dcc_status_t status = dcc_component_v2_builder_set_children(builder, children, children_count);
    if (status == DCC_OK) {
        status = dcc_component_v2_builder_set_accessory(builder, accessory);
    }
    return status;
}

dcc_status_t dcc_component_v2_builder_init_container(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *children,
    size_t children_count
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_builder_init(builder, DCC_COMPONENT_V2_CONTAINER);
    return dcc_component_v2_builder_set_children(builder, children, children_count);
}

dcc_status_t dcc_component_v2_builder_init_separator(
    dcc_component_v2_builder_t *builder,
    uint8_t divider,
    dcc_component_v2_separator_spacing_t spacing
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_builder_init(builder, DCC_COMPONENT_V2_SEPARATOR);
    dcc_status_t status = dcc_component_v2_builder_set_divider(builder, divider);
    if (status == DCC_OK) {
        status = dcc_component_v2_builder_set_separator_spacing(builder, spacing);
    }
    return status;
}

dcc_status_t dcc_component_v2_builder_init_label(
    dcc_component_v2_builder_t *builder,
    const char *label,
    const dcc_component_v2_builder_t *component
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_builder_init(builder, DCC_COMPONENT_V2_LABEL);
    dcc_status_t status = dcc_component_v2_builder_set_label(builder, label);
    if (status == DCC_OK) {
        status = dcc_component_v2_builder_set_children(builder, component, component != NULL ? 1U : 0U);
    }
    return status;
}

dcc_status_t dcc_component_v2_builder_init_media_gallery(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_media_t *media,
    size_t media_count
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_builder_init(builder, DCC_COMPONENT_V2_MEDIA_GALLERY);
    return dcc_component_v2_builder_set_media(builder, media, media_count);
}

dcc_status_t dcc_component_v2_builder_init_checkbox(
    dcc_component_v2_builder_t *builder,
    const char *custom_id,
    const char *label,
    uint8_t default_value
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_builder_init(builder, DCC_COMPONENT_V2_CHECKBOX);
    dcc_status_t status = dcc_component_v2_builder_set_custom_id(builder, custom_id);
    if (status == DCC_OK) {
        status = dcc_component_v2_builder_set_label(builder, label);
    }
    if (status == DCC_OK) {
        status = dcc_component_v2_builder_set_default(builder, default_value);
    }
    return status;
}
