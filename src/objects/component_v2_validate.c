#include "internal/objects/dcc_component_v2_internal.h"

#include <string.h>

static int dcc_component_v2_type_valid(dcc_component_v2_type_t type) {
    switch (type) {
        case DCC_COMPONENT_V2_ACTION_ROW:
        case DCC_COMPONENT_V2_BUTTON:
        case DCC_COMPONENT_V2_STRING_SELECT:
        case DCC_COMPONENT_V2_TEXT_INPUT:
        case DCC_COMPONENT_V2_USER_SELECT:
        case DCC_COMPONENT_V2_ROLE_SELECT:
        case DCC_COMPONENT_V2_MENTIONABLE_SELECT:
        case DCC_COMPONENT_V2_CHANNEL_SELECT:
        case DCC_COMPONENT_V2_SECTION:
        case DCC_COMPONENT_V2_TEXT_DISPLAY:
        case DCC_COMPONENT_V2_THUMBNAIL:
        case DCC_COMPONENT_V2_MEDIA_GALLERY:
        case DCC_COMPONENT_V2_FILE:
        case DCC_COMPONENT_V2_SEPARATOR:
        case DCC_COMPONENT_V2_CONTAINER:
        case DCC_COMPONENT_V2_LABEL:
        case DCC_COMPONENT_V2_FILE_UPLOAD:
        case DCC_COMPONENT_V2_RADIO_GROUP:
        case DCC_COMPONENT_V2_CHECKBOX_GROUP:
        case DCC_COMPONENT_V2_CHECKBOX:
            return 1;
        default:
            return 0;
    }
}

static int dcc_component_v2_is_select(dcc_component_v2_type_t type) {
    switch (type) {
        case DCC_COMPONENT_V2_STRING_SELECT:
        case DCC_COMPONENT_V2_USER_SELECT:
        case DCC_COMPONENT_V2_ROLE_SELECT:
        case DCC_COMPONENT_V2_MENTIONABLE_SELECT:
        case DCC_COMPONENT_V2_CHANNEL_SELECT:
            return 1;
        default:
            return 0;
    }
}

static int dcc_component_v2_button_style_valid(dcc_button_style_t style) {
    switch (style) {
        case DCC_BUTTON_PRIMARY:
        case DCC_BUTTON_SECONDARY:
        case DCC_BUTTON_SUCCESS:
        case DCC_BUTTON_DANGER:
        case DCC_BUTTON_LINK:
        case DCC_BUTTON_PREMIUM:
            return 1;
        default:
            return 0;
    }
}

static int dcc_component_v2_text_input_style_valid(dcc_text_input_style_t style) {
    switch (style) {
        case DCC_TEXT_INPUT_SHORT:
        case DCC_TEXT_INPUT_PARAGRAPH:
            return 1;
        default:
            return 0;
    }
}

static int dcc_component_v2_spacing_valid(dcc_component_v2_separator_spacing_t spacing) {
    return spacing == DCC_COMPONENT_V2_SPACING_SMALL ||
        spacing == DCC_COMPONENT_V2_SPACING_LARGE;
}

static dcc_status_t dcc_component_v2_validate_custom_id(
    dcc_component_v2_validation_t *ctx,
    const char *custom_id
) {
    if (custom_id == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t len = strlen(custom_id);
    if (len == 0 || len > 100U || ctx->custom_id_count >= DCC_COMPONENT_V2_MAX_COMPONENTS) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < ctx->custom_id_count; ++i) {
        if (strcmp(ctx->custom_ids[i], custom_id) == 0) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    ctx->custom_ids[ctx->custom_id_count++] = custom_id;
    return DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_id(
    dcc_component_v2_validation_t *ctx,
    uint32_t id
) {
    if (id == 0) {
        return DCC_OK;
    }
    if (ctx->id_count >= DCC_COMPONENT_V2_MAX_COMPONENTS) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < ctx->id_count; ++i) {
        if (ctx->ids[i] == id) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    ctx->ids[ctx->id_count++] = id;
    return DCC_OK;
}

static int dcc_component_v2_select_default_type_allowed(
    dcc_component_v2_type_t select_type,
    dcc_component_v2_select_default_type_t default_type
) {
    switch (select_type) {
        case DCC_COMPONENT_V2_USER_SELECT:
            return default_type == DCC_COMPONENT_V2_SELECT_DEFAULT_USER;
        case DCC_COMPONENT_V2_ROLE_SELECT:
            return default_type == DCC_COMPONENT_V2_SELECT_DEFAULT_ROLE;
        case DCC_COMPONENT_V2_MENTIONABLE_SELECT:
            return default_type == DCC_COMPONENT_V2_SELECT_DEFAULT_USER ||
                default_type == DCC_COMPONENT_V2_SELECT_DEFAULT_ROLE;
        case DCC_COMPONENT_V2_CHANNEL_SELECT:
            return default_type == DCC_COMPONENT_V2_SELECT_DEFAULT_CHANNEL;
        default:
            return 0;
    }
}

static dcc_status_t dcc_component_v2_validate_select_defaults(
    dcc_component_v2_type_t select_type,
    const dcc_component_v2_select_default_value_t *default_values,
    size_t default_value_count
) {
    if (default_value_count == 0) {
        return DCC_OK;
    }
    if (default_values == NULL ||
        default_value_count > DCC_COMPONENT_V2_MAX_SELECT_DEFAULT_VALUES ||
        select_type == DCC_COMPONENT_V2_STRING_SELECT) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < default_value_count; ++i) {
        if (default_values[i].id == 0 ||
            !dcc_component_v2_select_default_type_allowed(select_type, default_values[i].type)) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_channel_types(
    dcc_component_v2_type_t select_type,
    const uint32_t *channel_types,
    size_t channel_type_count
) {
    if (channel_type_count == 0) {
        return DCC_OK;
    }
    if (select_type != DCC_COMPONENT_V2_CHANNEL_SELECT ||
        channel_types == NULL ||
        channel_type_count > DCC_COMPONENT_V2_MAX_CHANNEL_TYPES) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_emoji(const dcc_component_emoji_t *emoji) {
    if (emoji == NULL || (!emoji->has_id && emoji->name == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_options(
    const dcc_select_option_t *options,
    size_t options_count,
    size_t min_count,
    size_t max_count
) {
    if (options_count < min_count || options_count > max_count || options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < options_count; ++i) {
        if (options[i].label == NULL || options[i].value == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        if (options[i].has_emoji && dcc_component_v2_validate_emoji(&options[i].emoji) != DCC_OK) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_media(
    const dcc_component_v2_media_t *media,
    size_t media_count,
    size_t min_count,
    size_t max_count
) {
    if (media_count < min_count || media_count > max_count || media == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < media_count; ++i) {
        if (media[i].url == NULL || media[i].url[0] == '\0') {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_one(
    const dcc_component_v2_builder_t *builder,
    dcc_component_v2_validation_t *ctx
);

static dcc_status_t dcc_component_v2_validate_children(
    const dcc_component_v2_builder_t *children,
    size_t children_count,
    dcc_component_v2_validation_t *ctx
) {
    if (children_count != 0 && children == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < children_count; ++i) {
        dcc_status_t status = dcc_component_v2_validate_one(&children[i], ctx);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_button(
    const dcc_component_v2_builder_t *builder,
    dcc_component_v2_validation_t *ctx
) {
    if (!builder->has_button_style || !dcc_component_v2_button_style_valid(builder->button_style)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->has_emoji && dcc_component_v2_validate_emoji(&builder->emoji) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->button_style == DCC_BUTTON_LINK) {
        return builder->url != NULL ? DCC_OK : DCC_ERR_INVALID_ARG;
    }
    if (builder->button_style == DCC_BUTTON_PREMIUM) {
        return builder->has_sku_id ? DCC_OK : DCC_ERR_INVALID_ARG;
    }
    if (builder->label == NULL && !builder->has_emoji) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_component_v2_validate_custom_id(ctx, builder->custom_id);
}

static dcc_status_t dcc_component_v2_validate_select(
    const dcc_component_v2_builder_t *builder,
    dcc_component_v2_validation_t *ctx
) {
    dcc_status_t status = dcc_component_v2_validate_custom_id(ctx, builder->custom_id);
    if (status != DCC_OK) {
        return status;
    }
    if (builder->type == DCC_COMPONENT_V2_STRING_SELECT) {
        status = dcc_component_v2_validate_options(builder->options, builder->options_count, 1U, 25U);
        if (status != DCC_OK) {
            return status;
        }
    } else if (builder->options_count != 0) {
        return DCC_ERR_INVALID_ARG;
    }
    status = dcc_component_v2_validate_select_defaults(
        builder->type,
        builder->default_values,
        builder->default_value_count
    );
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_component_v2_validate_channel_types(
        builder->type,
        builder->channel_types,
        builder->channel_type_count
    );
    if (status != DCC_OK) {
        return status;
    }
    if ((builder->has_min_values && builder->min_values > 25U) ||
        (builder->has_max_values && builder->max_values > 25U)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->has_min_values && builder->has_max_values &&
        builder->min_values > builder->max_values) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_action_row(
    const dcc_component_v2_builder_t *builder,
    dcc_component_v2_validation_t *ctx
) {
    if (builder->children_count == 0 ||
        builder->children_count > DCC_COMPONENT_V2_MAX_ACTION_ROW_CHILDREN ||
        builder->children == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    int has_select = 0;
    for (size_t i = 0; i < builder->children_count; ++i) {
        dcc_component_v2_type_t type = builder->children[i].type;
        if (type == DCC_COMPONENT_V2_BUTTON) {
            if (has_select) {
                return DCC_ERR_INVALID_ARG;
            }
        } else if (dcc_component_v2_is_select(type)) {
            if (builder->children_count != 1U) {
                return DCC_ERR_INVALID_ARG;
            }
            has_select = 1;
        } else {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return dcc_component_v2_validate_children(builder->children, builder->children_count, ctx);
}

static dcc_status_t dcc_component_v2_validate_section(
    const dcc_component_v2_builder_t *builder,
    dcc_component_v2_validation_t *ctx
) {
    if (builder->children_count == 0 ||
        builder->children_count > DCC_COMPONENT_V2_MAX_SECTION_CHILDREN ||
        builder->children == NULL ||
        builder->accessory == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < builder->children_count; ++i) {
        if (builder->children[i].type != DCC_COMPONENT_V2_TEXT_DISPLAY) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    if (builder->accessory->type != DCC_COMPONENT_V2_BUTTON &&
        builder->accessory->type != DCC_COMPONENT_V2_THUMBNAIL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_component_v2_validate_children(
        builder->children,
        builder->children_count,
        ctx
    );
    if (status == DCC_OK) {
        status = dcc_component_v2_validate_one(builder->accessory, ctx);
    }
    return status;
}

static dcc_status_t dcc_component_v2_validate_label(
    const dcc_component_v2_builder_t *builder,
    dcc_component_v2_validation_t *ctx
) {
    if (builder->label == NULL || builder->children_count != 1U || builder->children == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_type_t child_type = builder->children[0].type;
    if (child_type != DCC_COMPONENT_V2_TEXT_INPUT &&
        !dcc_component_v2_is_select(child_type) &&
        child_type != DCC_COMPONENT_V2_FILE_UPLOAD &&
        child_type != DCC_COMPONENT_V2_RADIO_GROUP &&
        child_type != DCC_COMPONENT_V2_CHECKBOX_GROUP &&
        child_type != DCC_COMPONENT_V2_CHECKBOX) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_component_v2_validate_children(builder->children, builder->children_count, ctx);
}

static dcc_status_t dcc_component_v2_validate_one(
    const dcc_component_v2_builder_t *builder,
    dcc_component_v2_validation_t *ctx
) {
    if (builder == NULL || !dcc_component_v2_type_valid(builder->type)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (ctx->total_count >= DCC_COMPONENT_V2_MAX_COMPONENTS) {
        return DCC_ERR_INVALID_ARG;
    }
    ++ctx->total_count;
    if (builder->has_id) {
        dcc_status_t status = dcc_component_v2_validate_id(ctx, builder->id);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_spacing && !dcc_component_v2_spacing_valid(builder->spacing)) {
        return DCC_ERR_INVALID_ARG;
    }
    switch (builder->type) {
        case DCC_COMPONENT_V2_ACTION_ROW:
            return dcc_component_v2_validate_action_row(builder, ctx);
        case DCC_COMPONENT_V2_BUTTON:
            return dcc_component_v2_validate_button(builder, ctx);
        case DCC_COMPONENT_V2_STRING_SELECT:
        case DCC_COMPONENT_V2_USER_SELECT:
        case DCC_COMPONENT_V2_ROLE_SELECT:
        case DCC_COMPONENT_V2_MENTIONABLE_SELECT:
        case DCC_COMPONENT_V2_CHANNEL_SELECT:
            return dcc_component_v2_validate_select(builder, ctx);
        case DCC_COMPONENT_V2_TEXT_INPUT:
            if (!builder->has_text_input_style ||
                !dcc_component_v2_text_input_style_valid(builder->text_input_style)) {
                return DCC_ERR_INVALID_ARG;
            }
            if (builder->has_min_length && builder->has_max_length &&
                builder->min_length > builder->max_length) {
                return DCC_ERR_INVALID_ARG;
            }
            return dcc_component_v2_validate_custom_id(ctx, builder->custom_id);
        case DCC_COMPONENT_V2_SECTION:
            return dcc_component_v2_validate_section(builder, ctx);
        case DCC_COMPONENT_V2_TEXT_DISPLAY:
            return builder->content != NULL ? DCC_OK : DCC_ERR_INVALID_ARG;
        case DCC_COMPONENT_V2_THUMBNAIL:
        case DCC_COMPONENT_V2_FILE:
            return dcc_component_v2_validate_media(builder->media, builder->media_count, 1U, 1U);
        case DCC_COMPONENT_V2_MEDIA_GALLERY:
            return dcc_component_v2_validate_media(
                builder->media,
                builder->media_count,
                1U,
                DCC_COMPONENT_V2_MAX_MEDIA_GALLERY_ITEMS
            );
        case DCC_COMPONENT_V2_SEPARATOR:
            return DCC_OK;
        case DCC_COMPONENT_V2_CONTAINER:
            return dcc_component_v2_validate_children(builder->children, builder->children_count, ctx);
        case DCC_COMPONENT_V2_LABEL:
            return dcc_component_v2_validate_label(builder, ctx);
        case DCC_COMPONENT_V2_FILE_UPLOAD:
            return dcc_component_v2_validate_custom_id(ctx, builder->custom_id);
        case DCC_COMPONENT_V2_RADIO_GROUP:
            if (dcc_component_v2_validate_custom_id(ctx, builder->custom_id) != DCC_OK) {
                return DCC_ERR_INVALID_ARG;
            }
            return dcc_component_v2_validate_options(
                builder->options,
                builder->options_count,
                2U,
                DCC_COMPONENT_V2_MAX_RADIO_OPTIONS
            );
        case DCC_COMPONENT_V2_CHECKBOX_GROUP:
            if (dcc_component_v2_validate_custom_id(ctx, builder->custom_id) != DCC_OK) {
                return DCC_ERR_INVALID_ARG;
            }
            return dcc_component_v2_validate_options(
                builder->options,
                builder->options_count,
                1U,
                DCC_COMPONENT_V2_MAX_CHECKBOX_OPTIONS
            );
        case DCC_COMPONENT_V2_CHECKBOX:
            if (builder->label == NULL) {
                return DCC_ERR_INVALID_ARG;
            }
            return dcc_component_v2_validate_custom_id(ctx, builder->custom_id);
        default:
            return DCC_ERR_INVALID_ARG;
    }
}

dcc_status_t dcc_component_v2_validate_array(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count
) {
    if (builder_count != 0 && builders == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_validation_t ctx = {0};
    for (size_t i = 0; i < builder_count; ++i) {
        dcc_status_t status = dcc_component_v2_validate_one(&builders[i], &ctx);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
