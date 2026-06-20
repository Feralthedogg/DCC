#include <dcc/component_v2.h>

#include <stdlib.h>
#include <string.h>

void dcc_component_v2_builder_init(
    dcc_component_v2_builder_t *builder,
    dcc_component_v2_type_t type
) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
        builder->type = type;
    }
}

dcc_status_t dcc_component_v2_builder_set_id(
    dcc_component_v2_builder_t *builder,
    uint32_t id
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->id = id;
    builder->has_id = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_content(
    dcc_component_v2_builder_t *builder,
    const char *content
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->content = content;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_custom_id(
    dcc_component_v2_builder_t *builder,
    const char *custom_id
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->custom_id = custom_id;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_label(
    dcc_component_v2_builder_t *builder,
    const char *label
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->label = label;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_description(
    dcc_component_v2_builder_t *builder,
    const char *description
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->description = description;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_placeholder(
    dcc_component_v2_builder_t *builder,
    const char *placeholder
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->placeholder = placeholder;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_url(
    dcc_component_v2_builder_t *builder,
    const char *url
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->url = url;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_value(
    dcc_component_v2_builder_t *builder,
    const char *value
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->value = value;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_button_style(
    dcc_component_v2_builder_t *builder,
    dcc_button_style_t style
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->button_style = style;
    builder->has_button_style = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_text_input_style(
    dcc_component_v2_builder_t *builder,
    dcc_text_input_style_t style
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->text_input_style = style;
    builder->has_text_input_style = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_separator_spacing(
    dcc_component_v2_builder_t *builder,
    dcc_component_v2_separator_spacing_t spacing
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->spacing = spacing;
    builder->has_spacing = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_sku_id(
    dcc_component_v2_builder_t *builder,
    dcc_snowflake_t sku_id
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->sku_id = sku_id;
    builder->has_sku_id = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_emoji(
    dcc_component_v2_builder_t *builder,
    const dcc_component_emoji_t *emoji
) {
    if (builder == NULL || emoji == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->emoji = *emoji;
    builder->has_emoji = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_options(
    dcc_component_v2_builder_t *builder,
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

dcc_status_t dcc_component_v2_builder_set_default_values(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_select_default_value_t *default_values,
    size_t default_value_count
) {
    if (builder == NULL || (default_value_count != 0 && default_values == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->default_values = default_values;
    builder->default_value_count = default_value_count;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_channel_types(
    dcc_component_v2_builder_t *builder,
    const uint32_t *channel_types,
    size_t channel_type_count
) {
    if (builder == NULL || (channel_type_count != 0 && channel_types == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->channel_types = channel_types;
    builder->channel_type_count = channel_type_count;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_media(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_media_t *media,
    size_t media_count
) {
    if (builder == NULL || (media_count != 0 && media == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->media = media;
    builder->media_count = media_count;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_children(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *children,
    size_t children_count
) {
    if (builder == NULL || (children_count != 0 && children == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->children = children;
    builder->children_count = children_count;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_accessory(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *accessory
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->accessory = accessory;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_min_values(
    dcc_component_v2_builder_t *builder,
    uint32_t min_values
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->min_values = min_values;
    builder->has_min_values = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_max_values(
    dcc_component_v2_builder_t *builder,
    uint32_t max_values
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->max_values = max_values;
    builder->has_max_values = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_min_length(
    dcc_component_v2_builder_t *builder,
    uint32_t min_length
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->min_length = min_length;
    builder->has_min_length = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_max_length(
    dcc_component_v2_builder_t *builder,
    uint32_t max_length
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->max_length = max_length;
    builder->has_max_length = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_accent_color(
    dcc_component_v2_builder_t *builder,
    uint32_t accent_color
) {
    if (builder == NULL || accent_color > 0xFFFFFFU) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->accent_color = accent_color;
    builder->has_accent_color = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_disabled(
    dcc_component_v2_builder_t *builder,
    uint8_t disabled
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->disabled = disabled ? 1U : 0U;
    builder->has_disabled = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_required(
    dcc_component_v2_builder_t *builder,
    uint8_t required
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->required = required ? 1U : 0U;
    builder->has_required = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_divider(
    dcc_component_v2_builder_t *builder,
    uint8_t divider
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->divider = divider ? 1U : 0U;
    builder->has_divider = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_spoiler(
    dcc_component_v2_builder_t *builder,
    uint8_t spoiler
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->spoiler = spoiler ? 1U : 0U;
    builder->has_spoiler = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_default(
    dcc_component_v2_builder_t *builder,
    uint8_t default_value
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->checked = default_value ? 1U : 0U;
    builder->has_checked = 1;
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_checked(
    dcc_component_v2_builder_t *builder,
    uint8_t checked
) {
    return dcc_component_v2_builder_set_default(builder, checked);
}

void dcc_component_v2_builder_json_free(char *json) {
    free(json);
}
