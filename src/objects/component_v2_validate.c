#include "internal/objects/dcc_component_v2_internal.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint64_t validation_allowed(dcc_component_v2_type_t type);
static uint64_t validation_required(dcc_component_v2_type_t type);

static int dcc_component_v2_array_span_valid(
    const void *values,
    size_t count,
    size_t item_size
) {
    if (count == 0U) {
        return 1;
    }
    if (values == NULL || item_size == 0U || count > SIZE_MAX / item_size) {
        return 0;
    }
    size_t span = count * item_size;
    uintptr_t address = (uintptr_t)values;
    return address <= UINTPTR_MAX - (span - 1U);
}

static dcc_status_t dcc_component_v2_sequence_stride(
    const dcc_component_v2_builder_t *values,
    size_t count,
    size_t *out_stride) {
    *out_stride = 0U;
    if (count == 0U) return values == NULL || values != NULL ? DCC_OK : DCC_OK;
    if (values == NULL ||
        ((uintptr_t)values % _Alignof(dcc_component_v2_builder_t)) != 0U)
        return DCC_ERR_INVALID_ARG;
    size_t stride = 0U;
    memcpy(&stride, values, sizeof(stride));
    size_t prefix = offsetof(dcc_component_v2_builder_t, id) +
        sizeof(values->id);
    if (stride < prefix || stride % _Alignof(dcc_component_v2_builder_t) != 0U ||
        count > SIZE_MAX / stride ||
        (uintptr_t)values > UINTPTR_MAX - count * stride)
        return DCC_ERR_INVALID_ARG;
    *out_stride = stride;
    return DCC_OK;
}

static const dcc_component_v2_builder_t *dcc_component_v2_sequence_at(
    const dcc_component_v2_builder_t *values, size_t stride, size_t index) {
    return (const dcc_component_v2_builder_t *)
        ((const unsigned char *)values + stride * index);
}

static int dcc_component_v2_string_length_between(
    const char *value,
    size_t min_length,
    size_t max_length
) {
    if (value == NULL) {
        return min_length == 0U;
    }
    size_t length = strlen(value);
    return length >= min_length && length <= max_length;
}

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
    if (!dcc_component_v2_array_span_valid(
            default_values,
            default_value_count,
            sizeof(*default_values)
        ) ||
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
        !dcc_component_v2_array_span_valid(
            channel_types,
            channel_type_count,
            sizeof(*channel_types)
        )) {
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
    if (options_count < min_count || options_count > max_count ||
        !dcc_component_v2_array_span_valid(
            options,
            options_count,
            sizeof(*options)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < options_count; ++i) {
        if (!dcc_component_v2_string_length_between(options[i].label, 1U, 100U) ||
            !dcc_component_v2_string_length_between(options[i].value, 1U, 100U) ||
            !dcc_component_v2_string_length_between(options[i].description, 0U, 100U)) {
            return DCC_ERR_INVALID_ARG;
        }
        if (options[i].has_emoji && dcc_component_v2_validate_emoji(&options[i].emoji) != DCC_OK) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_choices(
    const dcc_component_v2_choice_option_t *options,
    size_t option_count,
    size_t min_count,
    size_t max_count,
    int radio
) {
    if (option_count < min_count || option_count > max_count ||
        !dcc_component_v2_array_span_valid(
            options, option_count, sizeof(*options)))
        return DCC_ERR_INVALID_ARG;
    size_t defaults = 0U;
    for (size_t i = 0U; i < option_count; ++i) {
        if (!dcc_component_v2_string_length_between(options[i].label, 1U, 100U) ||
            !dcc_component_v2_string_length_between(options[i].value, 1U, 100U) ||
            !dcc_component_v2_string_length_between(options[i].description, 0U, 100U) ||
            options[i].has_default > 1U || options[i].is_default > 1U)
            return DCC_ERR_INVALID_ARG;
        if (options[i].has_default && options[i].is_default)
            ++defaults;
    }
    return radio && defaults > 1U ? DCC_ERR_INVALID_ARG : DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_media(
    const dcc_component_v2_media_t *media,
    size_t media_count,
    size_t min_count,
    size_t max_count
) {
    if (media_count < min_count || media_count > max_count ||
        !dcc_component_v2_array_span_valid(
            media,
            media_count,
            sizeof(*media)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < media_count; ++i) {
        if (!dcc_component_v2_string_length_between(media[i].url, 1U, 2048U) ||
            !dcc_component_v2_string_length_between(media[i].description, 0U, 1024U)) {
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
    size_t stride = 0U;
    if (children_count > DCC_COMPONENT_V2_MAX_COMPONENTS ||
        dcc_component_v2_sequence_stride(children, children_count, &stride) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < children_count; ++i) {
        const dcc_component_v2_builder_t *child =
            dcc_component_v2_sequence_at(children, stride, i);
        size_t child_size = 0U;
        memcpy(&child_size, child, sizeof(child_size));
        if (child_size != stride) return DCC_ERR_INVALID_ARG;
        dcc_status_t status = dcc_component_v2_validate_one(child, ctx);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_button(
    const dcc_component_v2_normalized_view_t *builder,
    dcc_component_v2_validation_t *ctx
) {
    if (!builder->has_button_style || !dcc_component_v2_button_style_valid(builder->button_style)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!dcc_component_v2_string_length_between(builder->label, 0U, 80U) ||
        !dcc_component_v2_string_length_between(builder->url, 0U, 512U)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->has_emoji && dcc_component_v2_validate_emoji(&builder->emoji) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->button_style == DCC_BUTTON_LINK) {
        return builder->url != NULL && builder->url[0] != '\0' &&
                builder->custom_id == NULL && !builder->has_sku_id &&
                (builder->label != NULL || builder->has_emoji)
            ? DCC_OK
            : DCC_ERR_INVALID_ARG;
    }
    if (builder->button_style == DCC_BUTTON_PREMIUM) {
        return builder->has_sku_id && builder->sku_id != 0U &&
                builder->custom_id == NULL && builder->url == NULL &&
                builder->label == NULL && !builder->has_emoji
            ? DCC_OK
            : DCC_ERR_INVALID_ARG;
    }
    if (builder->url != NULL || builder->has_sku_id) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->label == NULL && !builder->has_emoji) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_component_v2_validate_custom_id(ctx, builder->custom_id);
}

static dcc_status_t dcc_component_v2_validate_select(
    const dcc_component_v2_normalized_view_t *builder,
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
    if (!dcc_component_v2_string_length_between(builder->placeholder, 0U, 150U)) {
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
    const dcc_component_v2_normalized_view_t *builder,
    dcc_component_v2_validation_t *ctx
) {
    if (builder->children_count == 0 ||
        builder->children_count > DCC_COMPONENT_V2_MAX_ACTION_ROW_CHILDREN ||
        builder->children == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    int has_select = 0;
    size_t stride = 0U;
    if (dcc_component_v2_sequence_stride(
            builder->children, builder->children_count, &stride) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    for (size_t i = 0; i < builder->children_count; ++i) {
        dcc_component_v2_type_t type =
            dcc_component_v2_sequence_at(builder->children, stride, i)->type;
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
    const dcc_component_v2_normalized_view_t *builder,
    dcc_component_v2_validation_t *ctx
) {
    if (builder->children_count == 0 ||
        builder->children_count > DCC_COMPONENT_V2_MAX_SECTION_CHILDREN ||
        builder->children == NULL ||
        builder->accessory == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t stride = 0U;
    if (dcc_component_v2_sequence_stride(
            builder->children, builder->children_count, &stride) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    for (size_t i = 0; i < builder->children_count; ++i) {
        if (dcc_component_v2_sequence_at(builder->children, stride, i)->type !=
            DCC_COMPONENT_V2_TEXT_DISPLAY) {
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
    const dcc_component_v2_normalized_view_t *builder,
    dcc_component_v2_validation_t *ctx
) {
    if (!dcc_component_v2_string_length_between(builder->label, 1U, 45U) ||
        !dcc_component_v2_string_length_between(builder->description, 0U, 100U) ||
        builder->children_count != 1U || builder->children == NULL) {
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
    const size_t prefix = offsetof(dcc_component_v2_builder_t, id) +
        sizeof(builder->id);
    if (builder == NULL || builder->size < prefix ||
        builder->version != DCC_COMPONENT_V2_BUILDER_VERSION ||
        !dcc_component_v2_type_valid(builder->type)) {
        return DCC_ERR_INVALID_ARG;
    }
    uint64_t known = (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_COUNT) - 1U;
    uint64_t allowed_mask = validation_allowed(builder->type);
    uint64_t required_mask = validation_required(builder->type);
    if ((builder->present & ~known) != 0U ||
        (builder->present & ~allowed_mask) != 0U ||
        (builder->present & required_mask) != required_mask)
        return DCC_ERR_INVALID_ARG;
    for (int i = 0; i < DCC_COMPONENT_V2_FIELD_COUNT; ++i) {
        dcc_component_v2_field_t field = (dcc_component_v2_field_t)i;
        if ((builder->present & dcc_component_v2_field_mask(field)) != 0U &&
            dcc_component_v2_field_end_internal(builder->type, field) >
                builder->size)
            return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_normalized_view_t normalized;
    dcc_component_v2_normalize_view(builder, &normalized);
#define builder (&normalized)
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
    if ((builder->has_disabled && builder->disabled > 1U) ||
        (builder->has_required && builder->required > 1U) ||
        (builder->has_divider && builder->divider > 1U) ||
        (builder->has_spoiler && builder->spoiler > 1U) ||
        (builder->has_checked && builder->checked > 1U))
        return DCC_ERR_INVALID_ARG;
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
            if (!dcc_component_v2_string_length_between(builder->placeholder, 0U, 100U) ||
                !dcc_component_v2_string_length_between(builder->value, 0U, 4000U) ||
                (builder->has_min_length && builder->min_length > 4000U) ||
                (builder->has_max_length &&
                    (builder->max_length == 0U || builder->max_length > 4000U))) {
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
            return dcc_component_v2_string_length_between(builder->content, 1U, 4000U)
                ? DCC_OK
                : DCC_ERR_INVALID_ARG;
        case DCC_COMPONENT_V2_THUMBNAIL:
            return dcc_component_v2_validate_media(builder->media, builder->media_count, 1U, 1U);
        case DCC_COMPONENT_V2_FILE:
            if (dcc_component_v2_validate_media(
                    builder->media, builder->media_count, 1U, 1U) != DCC_OK ||
                strncmp(builder->media[0].url, "attachment://", 13U) != 0 ||
                builder->media[0].url[13] == '\0')
                return DCC_ERR_INVALID_ARG;
            return DCC_OK;
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
            if ((builder->has_min_values && builder->min_values > 10U) ||
                (builder->has_max_values &&
                    (builder->max_values == 0U || builder->max_values > 10U)) ||
                (builder->has_min_values && builder->has_max_values &&
                    builder->min_values > builder->max_values)) {
                return DCC_ERR_INVALID_ARG;
            }
            if (builder->file_type_count > 10U ||
                (builder->file_type_count != 0U && builder->file_types == NULL))
                return DCC_ERR_INVALID_ARG;
            for (size_t i = 0U; i < builder->file_type_count; ++i) {
                const char *type = builder->file_types[i];
                if (type == NULL || type[0] == '\0' ||
                    (strcmp(type, "image") != 0 && strcmp(type, "video") != 0 &&
                     strcmp(type, "audio") != 0 &&
                     !(type[0] == '.' && type[1] != '\0')))
                    return DCC_ERR_INVALID_ARG;
            }
            return dcc_component_v2_validate_custom_id(ctx, builder->custom_id);
        case DCC_COMPONENT_V2_RADIO_GROUP:
            if (dcc_component_v2_validate_custom_id(ctx, builder->custom_id) != DCC_OK) {
                return DCC_ERR_INVALID_ARG;
            }
            return dcc_component_v2_validate_choices(
                builder->choice_options,
                builder->options_count,
                2U,
                DCC_COMPONENT_V2_MAX_RADIO_OPTIONS,
                1
            );
        case DCC_COMPONENT_V2_CHECKBOX_GROUP:
            if (dcc_component_v2_validate_custom_id(ctx, builder->custom_id) != DCC_OK) {
                return DCC_ERR_INVALID_ARG;
            }
            if ((builder->has_min_values && builder->min_values > 10U) ||
                (builder->has_max_values &&
                    (builder->max_values == 0U || builder->max_values > 10U)) ||
                (builder->has_min_values && builder->has_max_values &&
                    builder->min_values > builder->max_values) ||
                ((!builder->has_required || builder->required) && builder->has_min_values &&
                    builder->min_values == 0U)) {
                return DCC_ERR_INVALID_ARG;
            }
            return dcc_component_v2_validate_choices(
                builder->choice_options,
                builder->options_count,
                1U,
                DCC_COMPONENT_V2_MAX_CHECKBOX_OPTIONS,
                0
            );
        case DCC_COMPONENT_V2_CHECKBOX:
            return dcc_component_v2_validate_custom_id(ctx, builder->custom_id);
        default:
            return DCC_ERR_INVALID_ARG;
    }
#undef builder
}

dcc_status_t dcc_component_v2_validate_array(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count
) {
    size_t stride = 0U;
    if (builder_count > DCC_COMPONENT_V2_MAX_COMPONENTS ||
        dcc_component_v2_sequence_stride(builders, builder_count, &stride) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_validation_t ctx = {0};
    for (size_t i = 0; i < builder_count; ++i) {
        const dcc_component_v2_builder_t *item =
            dcc_component_v2_sequence_at(builders, stride, i);
        size_t item_size = 0U;
        memcpy(&item_size, item, sizeof(item_size));
        if (item_size != stride) return DCC_ERR_INVALID_ARG;
        dcc_status_t status = dcc_component_v2_validate_one(item, &ctx);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

static int dcc_component_v2_message_root_allowed(dcc_component_v2_type_t type) {
    switch (type) {
        case DCC_COMPONENT_V2_ACTION_ROW:
        case DCC_COMPONENT_V2_SECTION:
        case DCC_COMPONENT_V2_TEXT_DISPLAY:
        case DCC_COMPONENT_V2_MEDIA_GALLERY:
        case DCC_COMPONENT_V2_FILE:
        case DCC_COMPONENT_V2_SEPARATOR:
        case DCC_COMPONENT_V2_CONTAINER:
            return 1;
        default:
            return 0;
    }
}

static int dcc_component_v2_container_child_allowed(dcc_component_v2_type_t type) {
    switch (type) {
        case DCC_COMPONENT_V2_ACTION_ROW:
        case DCC_COMPONENT_V2_TEXT_DISPLAY:
        case DCC_COMPONENT_V2_SECTION:
        case DCC_COMPONENT_V2_MEDIA_GALLERY:
        case DCC_COMPONENT_V2_SEPARATOR:
        case DCC_COMPONENT_V2_FILE:
            return 1;
        default:
            return 0;
    }
}

static dcc_status_t dcc_component_v2_validate_message_layout(
    const dcc_component_v2_builder_t *builder,
    dcc_component_v2_type_t parent,
    int is_root
) {
    dcc_component_v2_normalized_view_t view;
    dcc_component_v2_normalize_view(builder, &view);
    if (is_root && !dcc_component_v2_message_root_allowed(builder->type)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!is_root && parent == DCC_COMPONENT_V2_CONTAINER &&
        !dcc_component_v2_container_child_allowed(builder->type)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->type == DCC_COMPONENT_V2_CONTAINER && view.children_count == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t child_stride = 0U;
    if (dcc_component_v2_sequence_stride(
            view.children, view.children_count, &child_stride) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    for (size_t i = 0; i < view.children_count; ++i) {
        dcc_status_t status = dcc_component_v2_validate_message_layout(
            dcc_component_v2_sequence_at(view.children, child_stride, i),
            builder->type,
            0
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (view.accessory != NULL) {
        return dcc_component_v2_validate_message_layout(
            view.accessory,
            builder->type,
            0
        );
    }
    return DCC_OK;
}

static dcc_status_t dcc_component_v2_validate_context_fields(
    const dcc_component_v2_builder_t *builder,
    dcc_component_v2_context_t context) {
    dcc_component_v2_normalized_view_t view;
    dcc_component_v2_normalize_view(builder, &view);
    if (dcc_component_v2_is_select(builder->type)) {
        if ((context == DCC_COMPONENT_V2_CONTEXT_MESSAGE_LEGACY ||
             context == DCC_COMPONENT_V2_CONTEXT_MESSAGE_V2) &&
            view.has_required)
            return DCC_ERR_INVALID_ARG;
        if (context == DCC_COMPONENT_V2_CONTEXT_MODAL && view.has_disabled)
            return DCC_ERR_INVALID_ARG;
    }
    size_t stride = 0U;
    if (dcc_component_v2_sequence_stride(
            view.children, view.children_count, &stride) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    for (size_t i = 0U; i < view.children_count; ++i) {
        dcc_status_t status = dcc_component_v2_validate_context_fields(
            dcc_component_v2_sequence_at(view.children, stride, i), context);
        if (status != DCC_OK) return status;
    }
    return view.accessory != NULL
        ? dcc_component_v2_validate_context_fields(view.accessory, context)
        : DCC_OK;
}

dcc_status_t dcc_component_v2_validate_array_context(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count,
    dcc_component_v2_context_t context
) {
    dcc_status_t status = dcc_component_v2_validate_array(builders, builder_count);
    if (status != DCC_OK || context == DCC_COMPONENT_V2_CONTEXT_ANY) {
        return status;
    }
    size_t stride = 0U;
    if (dcc_component_v2_sequence_stride(builders, builder_count, &stride) !=
        DCC_OK)
        return DCC_ERR_INVALID_ARG;
    if (context == DCC_COMPONENT_V2_CONTEXT_MODAL) {
        if (builder_count == 0U || builder_count > 5U) {
            return DCC_ERR_INVALID_ARG;
        }
        for (size_t i = 0; i < builder_count; ++i) {
            const dcc_component_v2_builder_t *item =
                dcc_component_v2_sequence_at(builders, stride, i);
            dcc_component_v2_type_t type = item->type;
            if (type != DCC_COMPONENT_V2_LABEL &&
                type != DCC_COMPONENT_V2_TEXT_DISPLAY) {
                return DCC_ERR_INVALID_ARG;
            }
            if (dcc_component_v2_validate_context_fields(item, context) != DCC_OK)
                return DCC_ERR_INVALID_ARG;
        }
        return DCC_OK;
    }
    if (context == DCC_COMPONENT_V2_CONTEXT_MESSAGE_LEGACY) {
        if (builder_count > 5U) return DCC_ERR_INVALID_ARG;
        for (size_t i = 0U; i < builder_count; ++i) {
            const dcc_component_v2_builder_t *item =
                dcc_component_v2_sequence_at(builders, stride, i);
            if (item->type != DCC_COMPONENT_V2_ACTION_ROW)
                return DCC_ERR_INVALID_ARG;
            if (dcc_component_v2_validate_context_fields(item, context) != DCC_OK)
                return DCC_ERR_INVALID_ARG;
        }
        return DCC_OK;
    }
    if (context != DCC_COMPONENT_V2_CONTEXT_MESSAGE_V2) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < builder_count; ++i) {
        const dcc_component_v2_builder_t *item =
            dcc_component_v2_sequence_at(builders, stride, i);
        status = dcc_component_v2_validate_message_layout(
            item, item->type, 1);
        if (status == DCC_OK)
            status = dcc_component_v2_validate_context_fields(item, context);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

static uint64_t validation_allowed(dcc_component_v2_type_t type) {
    uint64_t id = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_ID);
    uint64_t select = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_PLACEHOLDER) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_MIN_VALUES) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_MAX_VALUES) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_REQUIRED) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_DISABLED);
#define F(name_) dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_##name_)
    switch (type) {
    case DCC_COMPONENT_V2_ACTION_ROW: return id | F(COMPONENTS);
    case DCC_COMPONENT_V2_BUTTON: return id | F(STYLE) | F(LABEL) | F(EMOJI) | F(CUSTOM_ID) | F(URL) | F(SKU_ID) | F(DISABLED);
    case DCC_COMPONENT_V2_STRING_SELECT: return id | select | F(OPTIONS);
    case DCC_COMPONENT_V2_USER_SELECT:
    case DCC_COMPONENT_V2_ROLE_SELECT:
    case DCC_COMPONENT_V2_MENTIONABLE_SELECT: return id | select | F(DEFAULT_VALUES);
    case DCC_COMPONENT_V2_CHANNEL_SELECT: return id | select | F(DEFAULT_VALUES) | F(CHANNEL_TYPES);
    case DCC_COMPONENT_V2_TEXT_INPUT: return id | F(CUSTOM_ID) | F(STYLE) | F(PLACEHOLDER) | F(MIN_LENGTH) | F(MAX_LENGTH) | F(VALUE) | F(REQUIRED);
    case DCC_COMPONENT_V2_SECTION: return id | F(COMPONENTS) | F(ACCESSORY);
    case DCC_COMPONENT_V2_TEXT_DISPLAY: return id | F(CONTENT);
    case DCC_COMPONENT_V2_THUMBNAIL: return id | F(MEDIA) | F(DESCRIPTION) | F(SPOILER);
    case DCC_COMPONENT_V2_MEDIA_GALLERY: return id | F(ITEMS);
    case DCC_COMPONENT_V2_FILE: return id | F(MEDIA) | F(SPOILER);
    case DCC_COMPONENT_V2_SEPARATOR: return id | F(DIVIDER) | F(SPACING);
    case DCC_COMPONENT_V2_CONTAINER: return id | F(COMPONENTS) | F(ACCENT_COLOR) | F(SPOILER);
    case DCC_COMPONENT_V2_LABEL: return id | F(LABEL) | F(DESCRIPTION) | F(COMPONENT);
    case DCC_COMPONENT_V2_FILE_UPLOAD: return id | F(CUSTOM_ID) | F(MIN_VALUES) | F(MAX_VALUES) | F(REQUIRED) | F(FILE_TYPES);
    case DCC_COMPONENT_V2_RADIO_GROUP: return id | F(CUSTOM_ID) | F(OPTIONS) | F(REQUIRED);
    case DCC_COMPONENT_V2_CHECKBOX_GROUP: return id | F(CUSTOM_ID) | F(OPTIONS) | F(MIN_VALUES) | F(MAX_VALUES) | F(REQUIRED);
    case DCC_COMPONENT_V2_CHECKBOX: return id | F(CUSTOM_ID) | F(DEFAULT);
    default: return 0U;
    }
#undef F
}

static uint64_t validation_required(dcc_component_v2_type_t type) {
#define F(name_) dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_##name_)
    switch (type) {
    case DCC_COMPONENT_V2_ACTION_ROW: return F(COMPONENTS);
    case DCC_COMPONENT_V2_BUTTON: return F(STYLE);
    case DCC_COMPONENT_V2_STRING_SELECT: return F(CUSTOM_ID) | F(OPTIONS);
    case DCC_COMPONENT_V2_USER_SELECT:
    case DCC_COMPONENT_V2_ROLE_SELECT:
    case DCC_COMPONENT_V2_MENTIONABLE_SELECT:
    case DCC_COMPONENT_V2_CHANNEL_SELECT:
    case DCC_COMPONENT_V2_FILE_UPLOAD:
    case DCC_COMPONENT_V2_CHECKBOX: return F(CUSTOM_ID);
    case DCC_COMPONENT_V2_TEXT_INPUT: return F(CUSTOM_ID) | F(STYLE);
    case DCC_COMPONENT_V2_SECTION: return F(COMPONENTS) | F(ACCESSORY);
    case DCC_COMPONENT_V2_TEXT_DISPLAY: return F(CONTENT);
    case DCC_COMPONENT_V2_THUMBNAIL:
    case DCC_COMPONENT_V2_FILE: return F(MEDIA);
    case DCC_COMPONENT_V2_MEDIA_GALLERY: return F(ITEMS);
    case DCC_COMPONENT_V2_CONTAINER: return F(COMPONENTS);
    case DCC_COMPONENT_V2_LABEL: return F(LABEL) | F(COMPONENT);
    case DCC_COMPONENT_V2_RADIO_GROUP:
    case DCC_COMPONENT_V2_CHECKBOX_GROUP: return F(CUSTOM_ID) | F(OPTIONS);
    case DCC_COMPONENT_V2_SEPARATOR: return 0U;
    default: return UINT64_MAX;
    }
#undef F
}

static const char *validation_field_name(dcc_component_v2_field_t field) {
    static const char *const names[] = {
        "id", "components", "style", "label", "emoji", "custom_id",
        "url", "sku_id", "disabled", "options", "placeholder",
        "default_values", "channel_types", "min_values", "max_values",
        "required", "min_length", "max_length", "value", "accessory",
        "content", "media", "description", "spoiler", "items", "divider",
        "spacing", "accent_color", "component", "file_types", "default"
    };
    return field >= 0 && field < DCC_COMPONENT_V2_FIELD_COUNT
        ? names[field] : "present";
}

#define FIELD_END(member_) (offsetof(dcc_component_v2_builder_t, member_) + \
                            sizeof(((dcc_component_v2_builder_t *)0)->member_))
size_t dcc_component_v2_field_end_internal(
    dcc_component_v2_type_t type, dcc_component_v2_field_t field) {
    if (field == DCC_COMPONENT_V2_FIELD_ID) return FIELD_END(id);
    switch (type) {
    case DCC_COMPONENT_V2_BUTTON:
        switch (field) {
        case DCC_COMPONENT_V2_FIELD_STYLE: return FIELD_END(as.button.style);
        case DCC_COMPONENT_V2_FIELD_LABEL: return FIELD_END(as.button.label);
        case DCC_COMPONENT_V2_FIELD_EMOJI: return FIELD_END(as.button.emoji);
        case DCC_COMPONENT_V2_FIELD_DISABLED: return FIELD_END(as.button.disabled);
        default: return FIELD_END(as.button.target);
        }
    case DCC_COMPONENT_V2_STRING_SELECT:
    case DCC_COMPONENT_V2_USER_SELECT:
    case DCC_COMPONENT_V2_ROLE_SELECT:
    case DCC_COMPONENT_V2_MENTIONABLE_SELECT:
    case DCC_COMPONENT_V2_CHANNEL_SELECT:
        switch (field) {
        case DCC_COMPONENT_V2_FIELD_CUSTOM_ID: return FIELD_END(as.select.custom_id);
        case DCC_COMPONENT_V2_FIELD_PLACEHOLDER: return FIELD_END(as.select.placeholder);
        case DCC_COMPONENT_V2_FIELD_MIN_VALUES: return FIELD_END(as.select.min_values);
        case DCC_COMPONENT_V2_FIELD_MAX_VALUES: return FIELD_END(as.select.max_values);
        case DCC_COMPONENT_V2_FIELD_REQUIRED: return FIELD_END(as.select.required);
        case DCC_COMPONENT_V2_FIELD_DISABLED: return FIELD_END(as.select.disabled);
        default: return FIELD_END(as.select.data);
        }
    case DCC_COMPONENT_V2_TEXT_INPUT:
        switch (field) {
        case DCC_COMPONENT_V2_FIELD_CUSTOM_ID: return FIELD_END(as.text_input.custom_id);
        case DCC_COMPONENT_V2_FIELD_STYLE: return FIELD_END(as.text_input.style);
        case DCC_COMPONENT_V2_FIELD_PLACEHOLDER: return FIELD_END(as.text_input.placeholder);
        case DCC_COMPONENT_V2_FIELD_MIN_LENGTH: return FIELD_END(as.text_input.min_length);
        case DCC_COMPONENT_V2_FIELD_MAX_LENGTH: return FIELD_END(as.text_input.max_length);
        case DCC_COMPONENT_V2_FIELD_VALUE: return FIELD_END(as.text_input.value);
        default: return FIELD_END(as.text_input.required);
        }
    case DCC_COMPONENT_V2_ACTION_ROW:
    case DCC_COMPONENT_V2_SECTION:
    case DCC_COMPONENT_V2_CONTAINER:
    case DCC_COMPONENT_V2_LABEL: return FIELD_END(as.layout);
    case DCC_COMPONENT_V2_THUMBNAIL:
    case DCC_COMPONENT_V2_MEDIA_GALLERY:
    case DCC_COMPONENT_V2_FILE: return FIELD_END(as.media);
    case DCC_COMPONENT_V2_SEPARATOR: return FIELD_END(as.separator);
    case DCC_COMPONENT_V2_TEXT_DISPLAY: return FIELD_END(as.text_display.content);
    case DCC_COMPONENT_V2_FILE_UPLOAD:
    case DCC_COMPONENT_V2_RADIO_GROUP:
    case DCC_COMPONENT_V2_CHECKBOX_GROUP:
    case DCC_COMPONENT_V2_CHECKBOX: return FIELD_END(as.modal);
    default: return SIZE_MAX;
    }
}
#undef FIELD_END

static void validation_error_set(
    dcc_component_v2_validation_error_t *error,
    dcc_status_t status,
    dcc_component_v2_validation_reason_t reason,
    const char *path) {
    if (error == NULL) return;
    error->status = status;
    error->reason = reason;
    snprintf(error->field_path, sizeof(error->field_path), "%s", path);
}

static dcc_status_t validation_prefix_node(
    const dcc_component_v2_builder_t *builder,
    const char *base,
    dcc_component_v2_validation_error_t *error) {
    char path[512];
    size_t size = 0U;
    memcpy(&size, builder, sizeof(size));
    size_t prefix = offsetof(dcc_component_v2_builder_t, id) + sizeof(builder->id);
    if (size < prefix) {
        snprintf(path, sizeof(path), "%s.size", base);
        validation_error_set(error, DCC_ERR_INVALID_ARG,
                             DCC_COMPONENT_V2_VALIDATION_ABI_SIZE, path);
        return DCC_ERR_INVALID_ARG;
    }
    uint32_t version = 0U;
    memcpy(&version, (const unsigned char *)builder +
           offsetof(dcc_component_v2_builder_t, version), sizeof(version));
    if (version != DCC_COMPONENT_V2_BUILDER_VERSION) {
        snprintf(path, sizeof(path), "%s.version", base);
        validation_error_set(error, DCC_ERR_INVALID_ARG,
                             DCC_COMPONENT_V2_VALIDATION_ABI_VERSION, path);
        return DCC_ERR_INVALID_ARG;
    }
    uint64_t present_mask = 0U;
    memcpy(&present_mask, (const unsigned char *)builder +
           offsetof(dcc_component_v2_builder_t, present), sizeof(present_mask));
    uint64_t known = (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_COUNT) - 1U;
    if ((present_mask & ~known) != 0U) {
        snprintf(path, sizeof(path), "%s.present", base);
        validation_error_set(error, DCC_ERR_INVALID_ARG,
                             DCC_COMPONENT_V2_VALIDATION_PRESENCE, path);
        return DCC_ERR_INVALID_ARG;
    }
    dcc_component_v2_type_t type = 0;
    memcpy(&type, (const unsigned char *)builder +
           offsetof(dcc_component_v2_builder_t, type), sizeof(type));
    uint64_t allowed_mask = validation_allowed(type);
    uint64_t required_mask = validation_required(type);
    if (allowed_mask == 0U || required_mask == UINT64_MAX) {
        snprintf(path, sizeof(path), "%s.type", base);
        validation_error_set(error, DCC_ERR_INVALID_ARG,
                             DCC_COMPONENT_V2_VALIDATION_TYPE, path);
        return DCC_ERR_INVALID_ARG;
    }
    for (int i = 0; i < DCC_COMPONENT_V2_FIELD_COUNT; ++i) {
        dcc_component_v2_field_t field = (dcc_component_v2_field_t)i;
        uint64_t mask = dcc_component_v2_field_mask(field);
        if ((present_mask & mask) != 0U && (allowed_mask & mask) == 0U) {
            snprintf(path, sizeof(path), "%s.%s", base, validation_field_name(field));
            validation_error_set(error, DCC_ERR_INVALID_ARG,
                                 DCC_COMPONENT_V2_VALIDATION_FIELD_NOT_ALLOWED, path);
            return DCC_ERR_INVALID_ARG;
        }
        if ((present_mask & mask) != 0U &&
            dcc_component_v2_field_end_internal(type, field) > size) {
            snprintf(path, sizeof(path), "%s.%s", base, validation_field_name(field));
            validation_error_set(error, DCC_ERR_INVALID_ARG,
                                 DCC_COMPONENT_V2_VALIDATION_FIELD_NOT_COVERED, path);
            return DCC_ERR_INVALID_ARG;
        }
        if ((required_mask & mask) != 0U && (present_mask & mask) == 0U) {
            snprintf(path, sizeof(path), "%s.%s", base, validation_field_name(field));
            validation_error_set(error, DCC_ERR_INVALID_ARG,
                                 DCC_COMPONENT_V2_VALIDATION_REQUIRED_FIELD, path);
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}

static dcc_status_t validation_prefix_tree(
    const dcc_component_v2_builder_t *builder,
    const char *base,
    dcc_component_v2_validation_error_t *error) {
    dcc_status_t status = validation_prefix_node(builder, base, error);
    if (status != DCC_OK) return status;
    dcc_component_v2_normalized_view_t view;
    dcc_component_v2_normalize_view(builder, &view);
    if (builder->type == DCC_COMPONENT_V2_LABEL && view.children != NULL) {
        char path[512]; snprintf(path, sizeof(path), "%s.component", base);
        status = validation_prefix_tree(view.children, path, error);
        if (status != DCC_OK) return status;
    } else if (view.children_count != 0U) {
        size_t stride = 0U;
        if (dcc_component_v2_sequence_stride(
                view.children, view.children_count, &stride) != DCC_OK) {
            char path[512]; snprintf(path, sizeof(path), "%s.components", base);
            validation_error_set(error, DCC_ERR_INVALID_ARG,
                                 DCC_COMPONENT_V2_VALIDATION_OVERFLOW, path);
            return DCC_ERR_INVALID_ARG;
        }
        for (size_t i = 0U; i < view.children_count; ++i) {
            const dcc_component_v2_builder_t *child =
                dcc_component_v2_sequence_at(view.children, stride, i);
            char path[512];
            snprintf(path, sizeof(path), "%s.components[%zu]", base, i);
            status = validation_prefix_tree(child, path, error);
            if (status != DCC_OK) return status;
        }
    }
    if (view.accessory != NULL) {
        char path[512]; snprintf(path, sizeof(path), "%s.accessory", base);
        status = validation_prefix_tree(view.accessory, path, error);
    }
    return status;
}

dcc_status_t dcc_component_v2_validate(
    const dcc_component_v2_builder_t *components,
    size_t component_count,
    dcc_component_v2_context_t context,
    dcc_component_v2_validation_error_t *out_error) {
    if (out_error != NULL &&
        (out_error->size < sizeof(*out_error) ||
         out_error->version != DCC_COMPONENT_V2_VALIDATION_ERROR_VERSION))
        return DCC_ERR_INVALID_ARG;
    if (out_error != NULL) {
        out_error->status = DCC_OK;
        out_error->reason = DCC_COMPONENT_V2_VALIDATION_NONE;
        out_error->field_path[0] = '\0';
    }
    if (context < DCC_COMPONENT_V2_CONTEXT_ANY ||
        context > DCC_COMPONENT_V2_CONTEXT_MODAL ||
        (component_count != 0U && components == NULL))
        return DCC_ERR_INVALID_ARG;
    if (component_count == 0U)
        return dcc_component_v2_validate_array_context(
            components, component_count, context);
    if (((uintptr_t)components % _Alignof(dcc_component_v2_builder_t)) != 0U) {
        validation_error_set(out_error, DCC_ERR_INVALID_ARG,
                             DCC_COMPONENT_V2_VALIDATION_ABI_SIZE,
                             "components[0].size");
        return DCC_ERR_INVALID_ARG;
    }
    size_t stride = 0U;
    memcpy(&stride, components, sizeof(stride));
    const size_t common_prefix = offsetof(dcc_component_v2_builder_t, id) +
        sizeof(components->id);
    if (stride < common_prefix) {
        validation_error_set(out_error, DCC_ERR_INVALID_ARG,
                             DCC_COMPONENT_V2_VALIDATION_ABI_SIZE,
                             "components[0].size");
        return DCC_ERR_INVALID_ARG;
    }
    if (stride > SIZE_MAX / component_count ||
        stride % _Alignof(dcc_component_v2_builder_t) != 0U) {
        validation_error_set(out_error, DCC_ERR_INVALID_ARG,
                             DCC_COMPONENT_V2_VALIDATION_OVERFLOW,
                             "components[0].size");
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0U; i < component_count; ++i) {
        const dcc_component_v2_builder_t *item =
            (const dcc_component_v2_builder_t *)
                ((const unsigned char *)components + i * stride);
        size_t item_size = 0U;
        memcpy(&item_size, item, sizeof(item_size));
        char base[64];
        snprintf(base, sizeof(base), "components[%zu]", i);
        if (item_size != stride) {
            char path[96]; snprintf(path, sizeof(path), "%s.size", base);
            validation_error_set(out_error, DCC_ERR_INVALID_ARG,
                                 DCC_COMPONENT_V2_VALIDATION_ABI_SIZE, path);
            return DCC_ERR_INVALID_ARG;
        }
        dcc_status_t status = validation_prefix_tree(item, base, out_error);
        if (status != DCC_OK) return status;
    }
    dcc_status_t status = dcc_component_v2_validate_array_context(
        components, component_count, context);
    if (status != DCC_OK)
        validation_error_set(out_error, status,
                             DCC_COMPONENT_V2_VALIDATION_INVALID_VALUE,
                             "components[0]");
    return status;
}
