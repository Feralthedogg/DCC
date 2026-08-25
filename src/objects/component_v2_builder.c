#include "internal/objects/dcc_component_v2_internal.h"

#include <stdlib.h>

static uint64_t bit(dcc_component_v2_field_t field) {
    return dcc_component_v2_field_mask(field);
}

static uint64_t allowed(dcc_component_v2_type_t type) {
    uint64_t id = bit(DCC_COMPONENT_V2_FIELD_ID);
    uint64_t select = bit(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
        bit(DCC_COMPONENT_V2_FIELD_PLACEHOLDER) |
        bit(DCC_COMPONENT_V2_FIELD_MIN_VALUES) |
        bit(DCC_COMPONENT_V2_FIELD_MAX_VALUES) |
        bit(DCC_COMPONENT_V2_FIELD_REQUIRED) |
        bit(DCC_COMPONENT_V2_FIELD_DISABLED);
    switch (type) {
    case DCC_COMPONENT_V2_ACTION_ROW:
        return id | bit(DCC_COMPONENT_V2_FIELD_COMPONENTS);
    case DCC_COMPONENT_V2_BUTTON:
        return id | bit(DCC_COMPONENT_V2_FIELD_STYLE) |
            bit(DCC_COMPONENT_V2_FIELD_LABEL) |
            bit(DCC_COMPONENT_V2_FIELD_EMOJI) |
            bit(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
            bit(DCC_COMPONENT_V2_FIELD_URL) |
            bit(DCC_COMPONENT_V2_FIELD_SKU_ID) |
            bit(DCC_COMPONENT_V2_FIELD_DISABLED);
    case DCC_COMPONENT_V2_STRING_SELECT:
        return id | select | bit(DCC_COMPONENT_V2_FIELD_OPTIONS);
    case DCC_COMPONENT_V2_USER_SELECT:
    case DCC_COMPONENT_V2_ROLE_SELECT:
    case DCC_COMPONENT_V2_MENTIONABLE_SELECT:
        return id | select | bit(DCC_COMPONENT_V2_FIELD_DEFAULT_VALUES);
    case DCC_COMPONENT_V2_CHANNEL_SELECT:
        return id | select | bit(DCC_COMPONENT_V2_FIELD_DEFAULT_VALUES) |
            bit(DCC_COMPONENT_V2_FIELD_CHANNEL_TYPES);
    case DCC_COMPONENT_V2_TEXT_INPUT:
        return id | bit(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
            bit(DCC_COMPONENT_V2_FIELD_STYLE) |
            bit(DCC_COMPONENT_V2_FIELD_PLACEHOLDER) |
            bit(DCC_COMPONENT_V2_FIELD_MIN_LENGTH) |
            bit(DCC_COMPONENT_V2_FIELD_MAX_LENGTH) |
            bit(DCC_COMPONENT_V2_FIELD_VALUE) |
            bit(DCC_COMPONENT_V2_FIELD_REQUIRED);
    case DCC_COMPONENT_V2_SECTION:
        return id | bit(DCC_COMPONENT_V2_FIELD_COMPONENTS) |
            bit(DCC_COMPONENT_V2_FIELD_ACCESSORY);
    case DCC_COMPONENT_V2_TEXT_DISPLAY:
        return id | bit(DCC_COMPONENT_V2_FIELD_CONTENT);
    case DCC_COMPONENT_V2_THUMBNAIL:
        return id | bit(DCC_COMPONENT_V2_FIELD_MEDIA) |
            bit(DCC_COMPONENT_V2_FIELD_DESCRIPTION) |
            bit(DCC_COMPONENT_V2_FIELD_SPOILER);
    case DCC_COMPONENT_V2_MEDIA_GALLERY:
        return id | bit(DCC_COMPONENT_V2_FIELD_ITEMS);
    case DCC_COMPONENT_V2_FILE:
        return id | bit(DCC_COMPONENT_V2_FIELD_MEDIA) |
            bit(DCC_COMPONENT_V2_FIELD_SPOILER);
    case DCC_COMPONENT_V2_SEPARATOR:
        return id | bit(DCC_COMPONENT_V2_FIELD_DIVIDER) |
            bit(DCC_COMPONENT_V2_FIELD_SPACING);
    case DCC_COMPONENT_V2_CONTAINER:
        return id | bit(DCC_COMPONENT_V2_FIELD_COMPONENTS) |
            bit(DCC_COMPONENT_V2_FIELD_ACCENT_COLOR) |
            bit(DCC_COMPONENT_V2_FIELD_SPOILER);
    case DCC_COMPONENT_V2_LABEL:
        return id | bit(DCC_COMPONENT_V2_FIELD_LABEL) |
            bit(DCC_COMPONENT_V2_FIELD_DESCRIPTION) |
            bit(DCC_COMPONENT_V2_FIELD_COMPONENT);
    case DCC_COMPONENT_V2_FILE_UPLOAD:
        return id | bit(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
            bit(DCC_COMPONENT_V2_FIELD_MIN_VALUES) |
            bit(DCC_COMPONENT_V2_FIELD_MAX_VALUES) |
            bit(DCC_COMPONENT_V2_FIELD_REQUIRED) |
            bit(DCC_COMPONENT_V2_FIELD_FILE_TYPES);
    case DCC_COMPONENT_V2_RADIO_GROUP:
        return id | bit(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
            bit(DCC_COMPONENT_V2_FIELD_OPTIONS) |
            bit(DCC_COMPONENT_V2_FIELD_REQUIRED);
    case DCC_COMPONENT_V2_CHECKBOX_GROUP:
        return id | bit(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
            bit(DCC_COMPONENT_V2_FIELD_OPTIONS) |
            bit(DCC_COMPONENT_V2_FIELD_MIN_VALUES) |
            bit(DCC_COMPONENT_V2_FIELD_MAX_VALUES) |
            bit(DCC_COMPONENT_V2_FIELD_REQUIRED);
    case DCC_COMPONENT_V2_CHECKBOX:
        return id | bit(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
            bit(DCC_COMPONENT_V2_FIELD_DEFAULT);
    default:
        return 0U;
    }
}

static dcc_status_t can_set(dcc_component_v2_builder_t *builder,
                            dcc_component_v2_field_t field) {
    uint64_t known = (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_COUNT) - 1U;
    size_t prefix = offsetof(dcc_component_v2_builder_t, id) +
        sizeof(builder->id);
    if (builder == NULL || builder->size < prefix ||
        builder->version != DCC_COMPONENT_V2_BUILDER_VERSION ||
        (builder->present & ~known) != 0U ||
        (allowed(builder->type) & bit(field)) == 0U ||
        builder->size < dcc_component_v2_field_end_internal(
                            builder->type, field)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static void mark(dcc_component_v2_builder_t *builder,
                 dcc_component_v2_field_t field) {
    builder->present |= bit(field);
}

void dcc_component_v2_builder_init(dcc_component_v2_builder_t *builder,
                                   dcc_component_v2_type_t type) {
    if (builder != NULL)
        *builder = (dcc_component_v2_builder_t)
            DCC_COMPONENT_V2_BUILDER_INIT(type);
}

#define SET_SCALAR(fn_, field_, member_, type_)                                \
    dcc_status_t fn_(dcc_component_v2_builder_t *builder, type_ value) {       \
        if (can_set(builder, field_) != DCC_OK) return DCC_ERR_INVALID_ARG;    \
        builder->member_ = value; mark(builder, field_); return DCC_OK;        \
    }

SET_SCALAR(dcc_component_v2_builder_set_id, DCC_COMPONENT_V2_FIELD_ID,
           id, uint32_t)
SET_SCALAR(dcc_component_v2_builder_set_content,
           DCC_COMPONENT_V2_FIELD_CONTENT, as.text_display.content,
           const char *)
SET_SCALAR(dcc_component_v2_builder_set_value, DCC_COMPONENT_V2_FIELD_VALUE,
           as.text_input.value, const char *)
SET_SCALAR(dcc_component_v2_builder_set_text_input_style,
           DCC_COMPONENT_V2_FIELD_STYLE, as.text_input.style,
           dcc_text_input_style_t)
SET_SCALAR(dcc_component_v2_builder_set_separator_spacing,
           DCC_COMPONENT_V2_FIELD_SPACING, as.separator.spacing,
           dcc_component_v2_separator_spacing_t)
SET_SCALAR(dcc_component_v2_builder_set_sku_id,
           DCC_COMPONENT_V2_FIELD_SKU_ID, as.button.target.sku_id,
           dcc_snowflake_t)
SET_SCALAR(dcc_component_v2_builder_set_min_length,
           DCC_COMPONENT_V2_FIELD_MIN_LENGTH, as.text_input.min_length,
           uint32_t)
SET_SCALAR(dcc_component_v2_builder_set_max_length,
           DCC_COMPONENT_V2_FIELD_MAX_LENGTH, as.text_input.max_length,
           uint32_t)

dcc_status_t dcc_component_v2_builder_set_custom_id(
    dcc_component_v2_builder_t *builder, const char *value) {
    if (can_set(builder, DCC_COMPONENT_V2_FIELD_CUSTOM_ID) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    switch (builder->type) {
    case DCC_COMPONENT_V2_BUTTON: builder->as.button.target.custom_id = value; break;
    case DCC_COMPONENT_V2_STRING_SELECT:
    case DCC_COMPONENT_V2_USER_SELECT:
    case DCC_COMPONENT_V2_ROLE_SELECT:
    case DCC_COMPONENT_V2_MENTIONABLE_SELECT:
    case DCC_COMPONENT_V2_CHANNEL_SELECT: builder->as.select.custom_id = value; break;
    case DCC_COMPONENT_V2_TEXT_INPUT: builder->as.text_input.custom_id = value; break;
    case DCC_COMPONENT_V2_FILE_UPLOAD: builder->as.modal.file_upload.custom_id = value; break;
    case DCC_COMPONENT_V2_RADIO_GROUP: builder->as.modal.radio_group.custom_id = value; break;
    case DCC_COMPONENT_V2_CHECKBOX_GROUP: builder->as.modal.checkbox_group.custom_id = value; break;
    case DCC_COMPONENT_V2_CHECKBOX: builder->as.modal.checkbox.custom_id = value; break;
    default: return DCC_ERR_INVALID_ARG;
    }
    mark(builder, DCC_COMPONENT_V2_FIELD_CUSTOM_ID);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_label(
    dcc_component_v2_builder_t *builder, const char *value) {
    if (can_set(builder, DCC_COMPONENT_V2_FIELD_LABEL) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    if (builder->type == DCC_COMPONENT_V2_BUTTON) builder->as.button.label = value;
    else builder->as.layout.label.label = value;
    mark(builder, DCC_COMPONENT_V2_FIELD_LABEL);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_description(
    dcc_component_v2_builder_t *builder, const char *value) {
    if (can_set(builder, DCC_COMPONENT_V2_FIELD_DESCRIPTION) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    if (builder->type == DCC_COMPONENT_V2_THUMBNAIL)
        builder->as.media.thumbnail.description = value;
    else builder->as.layout.label.description = value;
    mark(builder, DCC_COMPONENT_V2_FIELD_DESCRIPTION);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_placeholder(
    dcc_component_v2_builder_t *builder, const char *value) {
    if (can_set(builder, DCC_COMPONENT_V2_FIELD_PLACEHOLDER) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    if (builder->type == DCC_COMPONENT_V2_TEXT_INPUT)
        builder->as.text_input.placeholder = value;
    else builder->as.select.placeholder = value;
    mark(builder, DCC_COMPONENT_V2_FIELD_PLACEHOLDER);
    return DCC_OK;
}

SET_SCALAR(dcc_component_v2_builder_set_url, DCC_COMPONENT_V2_FIELD_URL,
           as.button.target.url, const char *)
SET_SCALAR(dcc_component_v2_builder_set_button_style,
           DCC_COMPONENT_V2_FIELD_STYLE, as.button.style, dcc_button_style_t)

dcc_status_t dcc_component_v2_builder_set_emoji(
    dcc_component_v2_builder_t *builder, const dcc_component_emoji_t *value) {
    if (value == NULL || can_set(builder, DCC_COMPONENT_V2_FIELD_EMOJI) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    builder->as.button.emoji = *value;
    mark(builder, DCC_COMPONENT_V2_FIELD_EMOJI);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_options(
    dcc_component_v2_builder_t *builder, const dcc_select_option_t *values,
    size_t count) {
    if ((count != 0U && values == NULL) ||
        can_set(builder, DCC_COMPONENT_V2_FIELD_OPTIONS) != DCC_OK ||
        builder->type != DCC_COMPONENT_V2_STRING_SELECT)
        return DCC_ERR_INVALID_ARG;
    builder->as.select.data.string_select.options = values;
    builder->as.select.data.string_select.option_count = count;
    mark(builder, DCC_COMPONENT_V2_FIELD_OPTIONS);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_choice_options(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_choice_option_t *values, size_t count) {
    if ((count != 0U && values == NULL) ||
        can_set(builder, DCC_COMPONENT_V2_FIELD_OPTIONS) != DCC_OK ||
        (builder->type != DCC_COMPONENT_V2_RADIO_GROUP &&
         builder->type != DCC_COMPONENT_V2_CHECKBOX_GROUP))
        return DCC_ERR_INVALID_ARG;
    if (builder->type == DCC_COMPONENT_V2_RADIO_GROUP) {
        builder->as.modal.radio_group.options = values;
        builder->as.modal.radio_group.option_count = count;
    } else {
        builder->as.modal.checkbox_group.options = values;
        builder->as.modal.checkbox_group.option_count = count;
    }
    mark(builder, DCC_COMPONENT_V2_FIELD_OPTIONS);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_file_types(
    dcc_component_v2_builder_t *builder, const char *const *values,
    size_t count) {
    if ((count != 0U && values == NULL) ||
        can_set(builder, DCC_COMPONENT_V2_FIELD_FILE_TYPES) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    builder->as.modal.file_upload.file_types = values;
    builder->as.modal.file_upload.file_type_count = count;
    mark(builder, DCC_COMPONENT_V2_FIELD_FILE_TYPES);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_default_values(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_select_default_value_t *values, size_t count) {
    if ((count != 0U && values == NULL) ||
        can_set(builder, DCC_COMPONENT_V2_FIELD_DEFAULT_VALUES) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    if (builder->type == DCC_COMPONENT_V2_CHANNEL_SELECT) {
        builder->as.select.data.channel_select.default_values = values;
        builder->as.select.data.channel_select.default_value_count = count;
    } else {
        builder->as.select.data.entity_select.default_values = values;
        builder->as.select.data.entity_select.default_value_count = count;
    }
    mark(builder, DCC_COMPONENT_V2_FIELD_DEFAULT_VALUES);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_channel_types(
    dcc_component_v2_builder_t *builder, const uint32_t *values,
    size_t count) {
    if ((count != 0U && values == NULL) ||
        can_set(builder, DCC_COMPONENT_V2_FIELD_CHANNEL_TYPES) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    builder->as.select.data.channel_select.channel_types = values;
    builder->as.select.data.channel_select.channel_type_count = count;
    mark(builder, DCC_COMPONENT_V2_FIELD_CHANNEL_TYPES);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_media(
    dcc_component_v2_builder_t *builder, const dcc_component_v2_media_t *values,
    size_t count) {
    if (builder == NULL || (count != 0U && values == NULL)) return DCC_ERR_INVALID_ARG;
    if (builder->type == DCC_COMPONENT_V2_MEDIA_GALLERY) {
        if (can_set(builder, DCC_COMPONENT_V2_FIELD_ITEMS) != DCC_OK) return DCC_ERR_INVALID_ARG;
        builder->as.media.gallery.items = (const void *)values;
        builder->as.media.gallery.item_count = count;
        mark(builder, DCC_COMPONENT_V2_FIELD_ITEMS);
        return DCC_OK;
    }
    if (count != 1U || can_set(builder, DCC_COMPONENT_V2_FIELD_MEDIA) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    if (builder->type == DCC_COMPONENT_V2_THUMBNAIL) {
        builder->as.media.thumbnail.media.url = values[0].url;
        builder->as.media.thumbnail.description = values[0].description;
        builder->as.media.thumbnail.spoiler = values[0].spoiler;
    } else {
        builder->as.media.file.file.url = values[0].url;
        builder->as.media.file.spoiler = values[0].spoiler;
    }
    mark(builder, DCC_COMPONENT_V2_FIELD_MEDIA);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_children(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *values, size_t count) {
    if (builder == NULL || (count != 0U && values == NULL)) return DCC_ERR_INVALID_ARG;
    if (builder->type == DCC_COMPONENT_V2_LABEL) {
        if (count != 1U || can_set(builder, DCC_COMPONENT_V2_FIELD_COMPONENT) != DCC_OK)
            return DCC_ERR_INVALID_ARG;
        builder->as.layout.label.component = values;
        mark(builder, DCC_COMPONENT_V2_FIELD_COMPONENT);
        return DCC_OK;
    }
    if (can_set(builder, DCC_COMPONENT_V2_FIELD_COMPONENTS) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    if (builder->type == DCC_COMPONENT_V2_ACTION_ROW) {
        builder->as.layout.action_row.components = values;
        builder->as.layout.action_row.component_count = count;
    } else if (builder->type == DCC_COMPONENT_V2_SECTION) {
        builder->as.layout.section.components = values;
        builder->as.layout.section.component_count = count;
    } else {
        builder->as.layout.container.components = values;
        builder->as.layout.container.component_count = count;
    }
    mark(builder, DCC_COMPONENT_V2_FIELD_COMPONENTS);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_accessory(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *value) {
    if (value == NULL || can_set(builder, DCC_COMPONENT_V2_FIELD_ACCESSORY) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    builder->as.layout.section.accessory = value;
    mark(builder, DCC_COMPONENT_V2_FIELD_ACCESSORY);
    return DCC_OK;
}

static uint32_t *values_slot(dcc_component_v2_builder_t *builder, int maximum) {
    switch (builder->type) {
    case DCC_COMPONENT_V2_STRING_SELECT:
    case DCC_COMPONENT_V2_USER_SELECT:
    case DCC_COMPONENT_V2_ROLE_SELECT:
    case DCC_COMPONENT_V2_MENTIONABLE_SELECT:
    case DCC_COMPONENT_V2_CHANNEL_SELECT:
        return maximum ? &builder->as.select.max_values : &builder->as.select.min_values;
    case DCC_COMPONENT_V2_FILE_UPLOAD:
        return maximum ? &builder->as.modal.file_upload.max_values : &builder->as.modal.file_upload.min_values;
    case DCC_COMPONENT_V2_CHECKBOX_GROUP:
        return maximum ? &builder->as.modal.checkbox_group.max_values : &builder->as.modal.checkbox_group.min_values;
    default: return NULL;
    }
}

static dcc_status_t set_values(dcc_component_v2_builder_t *builder,
                               uint32_t value, int maximum) {
    dcc_component_v2_field_t field = maximum ? DCC_COMPONENT_V2_FIELD_MAX_VALUES
                                              : DCC_COMPONENT_V2_FIELD_MIN_VALUES;
    if (can_set(builder, field) != DCC_OK) return DCC_ERR_INVALID_ARG;
    uint32_t *slot = values_slot(builder, maximum);
    if (slot == NULL) return DCC_ERR_INVALID_ARG;
    *slot = value; mark(builder, field); return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_min_values(
    dcc_component_v2_builder_t *builder, uint32_t value) {
    return set_values(builder, value, 0);
}
dcc_status_t dcc_component_v2_builder_set_max_values(
    dcc_component_v2_builder_t *builder, uint32_t value) {
    return set_values(builder, value, 1);
}

dcc_status_t dcc_component_v2_builder_set_accent_color(
    dcc_component_v2_builder_t *builder, uint32_t value) {
    if (value > 0xFFFFFFU || can_set(builder, DCC_COMPONENT_V2_FIELD_ACCENT_COLOR) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    builder->as.layout.container.accent_color = value;
    mark(builder, DCC_COMPONENT_V2_FIELD_ACCENT_COLOR);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_disabled(
    dcc_component_v2_builder_t *builder, uint8_t value) {
    if (value > 1U || can_set(builder, DCC_COMPONENT_V2_FIELD_DISABLED) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    if (builder->type == DCC_COMPONENT_V2_BUTTON) builder->as.button.disabled = value;
    else builder->as.select.disabled = value;
    mark(builder, DCC_COMPONENT_V2_FIELD_DISABLED); return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_required(
    dcc_component_v2_builder_t *builder, uint8_t value) {
    if (value > 1U || can_set(builder, DCC_COMPONENT_V2_FIELD_REQUIRED) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    switch (builder->type) {
    case DCC_COMPONENT_V2_TEXT_INPUT: builder->as.text_input.required = value; break;
    case DCC_COMPONENT_V2_FILE_UPLOAD: builder->as.modal.file_upload.required = value; break;
    case DCC_COMPONENT_V2_RADIO_GROUP: builder->as.modal.radio_group.required = value; break;
    case DCC_COMPONENT_V2_CHECKBOX_GROUP: builder->as.modal.checkbox_group.required = value; break;
    default: builder->as.select.required = value; break;
    }
    mark(builder, DCC_COMPONENT_V2_FIELD_REQUIRED); return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_divider(
    dcc_component_v2_builder_t *builder, uint8_t value) {
    if (value > 1U || can_set(builder, DCC_COMPONENT_V2_FIELD_DIVIDER) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    builder->as.separator.divider = value;
    mark(builder, DCC_COMPONENT_V2_FIELD_DIVIDER);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_spoiler(
    dcc_component_v2_builder_t *builder, uint8_t value) {
    if (value > 1U || can_set(builder, DCC_COMPONENT_V2_FIELD_SPOILER) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    if (builder->type == DCC_COMPONENT_V2_THUMBNAIL) builder->as.media.thumbnail.spoiler = value;
    else if (builder->type == DCC_COMPONENT_V2_FILE) builder->as.media.file.spoiler = value;
    else builder->as.layout.container.spoiler = value;
    mark(builder, DCC_COMPONENT_V2_FIELD_SPOILER); return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_default(
    dcc_component_v2_builder_t *builder, uint8_t value) {
    if (value > 1U || can_set(builder, DCC_COMPONENT_V2_FIELD_DEFAULT) != DCC_OK)
        return DCC_ERR_INVALID_ARG;
    builder->as.modal.checkbox.default_value = value;
    mark(builder, DCC_COMPONENT_V2_FIELD_DEFAULT);
    return DCC_OK;
}

dcc_status_t dcc_component_v2_builder_set_checked(
    dcc_component_v2_builder_t *builder, uint8_t value) {
    return dcc_component_v2_builder_set_default(builder, value);
}

void dcc_component_v2_builder_json_free(char *json) { free(json); }
