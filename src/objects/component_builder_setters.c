#include "internal/objects/dcc_component_builder_internal.h"

#include <string.h>

dcc_status_t dcc_component_builder_set_style(dcc_component_builder_t *builder, dcc_button_style_t style) {
    if (builder == NULL || !dcc_button_style_valid(style)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->style = style;
    builder->has_style = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_custom_id(dcc_component_builder_t *builder, const char *custom_id) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->custom_id = custom_id;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_label(dcc_component_builder_t *builder, const char *label) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->label = label;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_url(dcc_component_builder_t *builder, const char *url) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->url = url;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_sku_id(dcc_component_builder_t *builder, dcc_snowflake_t sku_id) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->sku_id = sku_id;
    builder->has_sku_id = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_builder_set_emoji(
    dcc_component_builder_t *builder,
    const dcc_component_emoji_t *emoji
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (emoji == NULL) {
        memset(&builder->emoji, 0, sizeof(builder->emoji));
        builder->has_emoji = 0;
        return DCC_OK;
    }
    builder->emoji = *emoji;
    builder->has_emoji = 1;
    return DCC_OK;
}
