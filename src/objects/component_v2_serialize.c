#include "internal/objects/dcc_component_v2_internal.h"
#include "internal/objects/dcc_component_serialize_internal.h"

#include <string.h>

typedef dcc_component_v2_normalized_view_t dcc_component_v2_serial_view_t;

static int present(const dcc_component_v2_builder_t *builder,
                   dcc_component_v2_field_t field) {
    return (builder->present & dcc_component_v2_field_mask(field)) != 0U;
}

void dcc_component_v2_normalize_view(
    const dcc_component_v2_builder_t *builder,
    dcc_component_v2_normalized_view_t *out) {
    dcc_component_v2_builder_t local = {0};
    size_t copy_size = builder->size < sizeof(local) ? builder->size : sizeof(local);
    memcpy(&local, builder, copy_size);
    builder = &local;
    *out = (dcc_component_v2_normalized_view_t){0};
    out->type = builder->type;
    out->id = builder->id;
    out->has_id = present(builder, DCC_COMPONENT_V2_FIELD_ID);
    switch (builder->type) {
    case DCC_COMPONENT_V2_ACTION_ROW:
        out->children = builder->as.layout.action_row.components;
        out->children_count = builder->as.layout.action_row.component_count;
        break;
    case DCC_COMPONENT_V2_BUTTON:
        out->button_style = builder->as.button.style;
        out->label = builder->as.button.label;
        out->emoji = builder->as.button.emoji;
        out->disabled = builder->as.button.disabled;
        out->has_button_style = present(builder, DCC_COMPONENT_V2_FIELD_STYLE);
        out->has_emoji = present(builder, DCC_COMPONENT_V2_FIELD_EMOJI);
        out->has_disabled = present(builder, DCC_COMPONENT_V2_FIELD_DISABLED);
        out->has_sku_id = present(builder, DCC_COMPONENT_V2_FIELD_SKU_ID);
        if (present(builder, DCC_COMPONENT_V2_FIELD_CUSTOM_ID)) out->custom_id = builder->as.button.target.custom_id;
        if (present(builder, DCC_COMPONENT_V2_FIELD_URL)) out->url = builder->as.button.target.url;
        if (out->has_sku_id) out->sku_id = builder->as.button.target.sku_id;
        break;
    case DCC_COMPONENT_V2_STRING_SELECT:
    case DCC_COMPONENT_V2_USER_SELECT:
    case DCC_COMPONENT_V2_ROLE_SELECT:
    case DCC_COMPONENT_V2_MENTIONABLE_SELECT:
    case DCC_COMPONENT_V2_CHANNEL_SELECT:
        out->custom_id = builder->as.select.custom_id;
        out->placeholder = builder->as.select.placeholder;
        out->min_values = builder->as.select.min_values;
        out->max_values = builder->as.select.max_values;
        out->required = builder->as.select.required;
        out->disabled = builder->as.select.disabled;
        out->has_min_values = present(builder, DCC_COMPONENT_V2_FIELD_MIN_VALUES);
        out->has_max_values = present(builder, DCC_COMPONENT_V2_FIELD_MAX_VALUES);
        out->has_required = present(builder, DCC_COMPONENT_V2_FIELD_REQUIRED);
        out->has_disabled = present(builder, DCC_COMPONENT_V2_FIELD_DISABLED);
        if (builder->type == DCC_COMPONENT_V2_STRING_SELECT) {
            out->options = builder->as.select.data.string_select.options;
            out->options_count = builder->as.select.data.string_select.option_count;
        } else if (builder->type == DCC_COMPONENT_V2_CHANNEL_SELECT) {
            out->default_values = builder->as.select.data.channel_select.default_values;
            out->default_value_count = builder->as.select.data.channel_select.default_value_count;
            out->channel_types = builder->as.select.data.channel_select.channel_types;
            out->channel_type_count = builder->as.select.data.channel_select.channel_type_count;
        } else {
            out->default_values = builder->as.select.data.entity_select.default_values;
            out->default_value_count = builder->as.select.data.entity_select.default_value_count;
        }
        break;
    case DCC_COMPONENT_V2_TEXT_INPUT:
        out->custom_id = builder->as.text_input.custom_id;
        out->text_input_style = builder->as.text_input.style;
        out->placeholder = builder->as.text_input.placeholder;
        out->min_length = builder->as.text_input.min_length;
        out->max_length = builder->as.text_input.max_length;
        out->value = builder->as.text_input.value;
        out->required = builder->as.text_input.required;
        out->has_text_input_style = present(builder, DCC_COMPONENT_V2_FIELD_STYLE);
        out->has_min_length = present(builder, DCC_COMPONENT_V2_FIELD_MIN_LENGTH);
        out->has_max_length = present(builder, DCC_COMPONENT_V2_FIELD_MAX_LENGTH);
        out->has_required = present(builder, DCC_COMPONENT_V2_FIELD_REQUIRED);
        break;
    case DCC_COMPONENT_V2_SECTION:
        out->children = builder->as.layout.section.components;
        out->children_count = builder->as.layout.section.component_count;
        out->accessory = builder->as.layout.section.accessory;
        break;
    case DCC_COMPONENT_V2_TEXT_DISPLAY:
        out->content = builder->as.text_display.content;
        break;
    case DCC_COMPONENT_V2_THUMBNAIL:
        out->media_one.url = builder->as.media.thumbnail.media.url;
        out->media_one.description = builder->as.media.thumbnail.description;
        out->media_one.spoiler = builder->as.media.thumbnail.spoiler;
        out->media_one.has_spoiler = present(builder, DCC_COMPONENT_V2_FIELD_SPOILER);
        out->media = &out->media_one; out->media_count = 1U;
        break;
    case DCC_COMPONENT_V2_FILE:
        out->media_one.url = builder->as.media.file.file.url;
        out->media_one.spoiler = builder->as.media.file.spoiler;
        out->media_one.has_spoiler = present(builder, DCC_COMPONENT_V2_FIELD_SPOILER);
        out->media = &out->media_one; out->media_count = 1U;
        break;
    case DCC_COMPONENT_V2_MEDIA_GALLERY:
        out->media = (const void *)builder->as.media.gallery.items;
        out->media_count = builder->as.media.gallery.item_count;
        break;
    case DCC_COMPONENT_V2_SEPARATOR:
        out->divider = builder->as.separator.divider;
        out->spacing = builder->as.separator.spacing;
        out->has_divider = present(builder, DCC_COMPONENT_V2_FIELD_DIVIDER);
        out->has_spacing = present(builder, DCC_COMPONENT_V2_FIELD_SPACING);
        break;
    case DCC_COMPONENT_V2_CONTAINER:
        out->children = builder->as.layout.container.components;
        out->children_count = builder->as.layout.container.component_count;
        out->accent_color = builder->as.layout.container.accent_color;
        out->spoiler = builder->as.layout.container.spoiler;
        out->has_accent_color = present(builder, DCC_COMPONENT_V2_FIELD_ACCENT_COLOR);
        out->has_spoiler = present(builder, DCC_COMPONENT_V2_FIELD_SPOILER);
        break;
    case DCC_COMPONENT_V2_LABEL:
        out->label = builder->as.layout.label.label;
        out->description = builder->as.layout.label.description;
        out->children = builder->as.layout.label.component;
        out->children_count = out->children != NULL ? 1U : 0U;
        break;
    case DCC_COMPONENT_V2_FILE_UPLOAD:
        out->custom_id = builder->as.modal.file_upload.custom_id;
        out->min_values = builder->as.modal.file_upload.min_values;
        out->max_values = builder->as.modal.file_upload.max_values;
        out->required = builder->as.modal.file_upload.required;
        out->file_types = builder->as.modal.file_upload.file_types;
        out->file_type_count = builder->as.modal.file_upload.file_type_count;
        out->has_min_values = present(builder, DCC_COMPONENT_V2_FIELD_MIN_VALUES);
        out->has_max_values = present(builder, DCC_COMPONENT_V2_FIELD_MAX_VALUES);
        out->has_required = present(builder, DCC_COMPONENT_V2_FIELD_REQUIRED);
        break;
    case DCC_COMPONENT_V2_RADIO_GROUP:
        out->custom_id = builder->as.modal.radio_group.custom_id;
        out->choice_options = builder->as.modal.radio_group.options;
        out->options_count = builder->as.modal.radio_group.option_count;
        out->required = builder->as.modal.radio_group.required;
        out->has_required = present(builder, DCC_COMPONENT_V2_FIELD_REQUIRED);
        break;
    case DCC_COMPONENT_V2_CHECKBOX_GROUP:
        out->custom_id = builder->as.modal.checkbox_group.custom_id;
        out->choice_options = builder->as.modal.checkbox_group.options;
        out->options_count = builder->as.modal.checkbox_group.option_count;
        out->min_values = builder->as.modal.checkbox_group.min_values;
        out->max_values = builder->as.modal.checkbox_group.max_values;
        out->required = builder->as.modal.checkbox_group.required;
        out->has_min_values = present(builder, DCC_COMPONENT_V2_FIELD_MIN_VALUES);
        out->has_max_values = present(builder, DCC_COMPONENT_V2_FIELD_MAX_VALUES);
        out->has_required = present(builder, DCC_COMPONENT_V2_FIELD_REQUIRED);
        break;
    case DCC_COMPONENT_V2_CHECKBOX:
        out->custom_id = builder->as.modal.checkbox.custom_id;
        out->checked = builder->as.modal.checkbox.default_value;
        out->has_checked = present(builder, DCC_COMPONENT_V2_FIELD_DEFAULT);
        break;
    default: break;
    }
}

static dcc_status_t dcc_component_v2_append_media_object(
    const dcc_component_v2_media_t *media,
    dcc_component_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "{\"url\":");
    if (status == DCC_OK) {
        status = dcc_component_json_append_escaped_string(buffer, media->url);
    }
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "}");
    }
    return status;
}

static dcc_status_t dcc_component_v2_append_media_item(
    const dcc_component_v2_media_t *media,
    dcc_component_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_component_json_member_prefix(buffer, &first, "media");
    }
    if (status == DCC_OK) {
        status = dcc_component_v2_append_media_object(media, buffer);
    }
    if (status == DCC_OK && media->description != NULL) {
        status = dcc_component_json_append_string_member(buffer, &first, "description", media->description);
    }
    if (status == DCC_OK && media->has_spoiler) {
        status = dcc_component_json_append_bool_member(buffer, &first, "spoiler", media->spoiler);
    }
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "}");
    }
    return status;
}

static dcc_status_t dcc_component_v2_append_options(
    const dcc_select_option_t *options,
    size_t options_count,
    dcc_component_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "[");
    for (size_t i = 0; status == DCC_OK && i < options_count; ++i) {
        const dcc_select_option_t *option = &options[i];
        if (i != 0) {
            status = dcc_component_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_component_json_append_cstr(buffer, "{");
        }
        int first = 1;
        if (status == DCC_OK) {
            status = dcc_component_json_append_string_member(buffer, &first, "label", option->label);
        }
        if (status == DCC_OK) {
            status = dcc_component_json_append_string_member(buffer, &first, "value", option->value);
        }
        if (status == DCC_OK && option->description != NULL) {
            status = dcc_component_json_append_string_member(buffer, &first, "description", option->description);
        }
        if (status == DCC_OK && option->has_emoji) {
            status = dcc_component_json_member_prefix(buffer, &first, "emoji");
            if (status == DCC_OK) {
                status = dcc_component_append_emoji_json(&option->emoji, buffer);
            }
        }
        if (status == DCC_OK && option->has_default) {
            status = dcc_component_json_append_bool_member(buffer, &first, "default", option->is_default);
        }
        if (status == DCC_OK) {
            status = dcc_component_json_append_cstr(buffer, "}");
        }
    }
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "]");
    }
    return status;
}

static dcc_status_t dcc_component_v2_append_choice_options(
    const dcc_component_v2_choice_option_t *options,
    size_t option_count,
    dcc_component_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "[");
    for (size_t i = 0U; status == DCC_OK && i < option_count; ++i) {
        if (i != 0U) status = dcc_component_json_append_cstr(buffer, ",");
        if (status == DCC_OK) status = dcc_component_json_append_cstr(buffer, "{");
        int first = 1;
        if (status == DCC_OK) status = dcc_component_json_append_string_member(buffer, &first, "label", options[i].label);
        if (status == DCC_OK) status = dcc_component_json_append_string_member(buffer, &first, "value", options[i].value);
        if (status == DCC_OK && options[i].description != NULL)
            status = dcc_component_json_append_string_member(buffer, &first, "description", options[i].description);
        if (status == DCC_OK && options[i].has_default)
            status = dcc_component_json_append_bool_member(buffer, &first, "default", options[i].is_default);
        if (status == DCC_OK) status = dcc_component_json_append_cstr(buffer, "}");
    }
    return status == DCC_OK ? dcc_component_json_append_cstr(buffer, "]") : status;
}

static const char *dcc_component_v2_default_type_name(
    dcc_component_v2_select_default_type_t type
) {
    switch (type) {
        case DCC_COMPONENT_V2_SELECT_DEFAULT_USER:
            return "user";
        case DCC_COMPONENT_V2_SELECT_DEFAULT_ROLE:
            return "role";
        case DCC_COMPONENT_V2_SELECT_DEFAULT_CHANNEL:
            return "channel";
        default:
            return NULL;
    }
}

static dcc_status_t dcc_component_v2_append_default_values(
    const dcc_component_v2_select_default_value_t *default_values,
    size_t default_value_count,
    dcc_component_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "[");
    for (size_t i = 0; status == DCC_OK && i < default_value_count; ++i) {
        const char *type_name = dcc_component_v2_default_type_name(default_values[i].type);
        if (type_name == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        if (i != 0) {
            status = dcc_component_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_component_json_append_cstr(buffer, "{");
        }
        int first = 1;
        if (status == DCC_OK) {
            status = dcc_component_json_append_snowflake_string_member(
                buffer,
                &first,
                "id",
                default_values[i].id
            );
        }
        if (status == DCC_OK) {
            status = dcc_component_json_append_string_member(buffer, &first, "type", type_name);
        }
        if (status == DCC_OK) {
            status = dcc_component_json_append_cstr(buffer, "}");
        }
    }
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "]");
    }
    return status;
}

static dcc_status_t dcc_component_v2_append_channel_types(
    const uint32_t *channel_types,
    size_t channel_type_count,
    dcc_component_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "[");
    for (size_t i = 0; status == DCC_OK && i < channel_type_count; ++i) {
        if (i != 0) {
            status = dcc_component_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_component_json_append_u32(buffer, channel_types[i]);
        }
    }
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "]");
    }
    return status;
}

static dcc_status_t dcc_component_v2_append_file_types(
    const char *const *file_types, size_t count,
    dcc_component_json_buffer_t *buffer) {
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "[");
    for (size_t i = 0U; status == DCC_OK && i < count; ++i) {
        if (i != 0U) status = dcc_component_json_append_cstr(buffer, ",");
        if (status == DCC_OK)
            status = dcc_component_json_append_escaped_string(buffer,
                                                               file_types[i]);
    }
    return status == DCC_OK ? dcc_component_json_append_cstr(buffer, "]")
                            : status;
}

static dcc_status_t dcc_component_v2_append_children_member(
    const dcc_component_v2_builder_t *children,
    size_t children_count,
    const char *name,
    dcc_component_json_buffer_t *buffer,
    int *first
) {
    if (children_count == 0) {
        return DCC_OK;
    }
    dcc_status_t status = dcc_component_json_member_prefix(buffer, first, name);
    if (status == DCC_OK) {
        status = dcc_component_v2_append_array_json(children, children_count, buffer);
    }
    return status;
}

static dcc_status_t dcc_component_v2_append_button_fields(
    const dcc_component_v2_serial_view_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = DCC_OK;
    if (builder->has_button_style) {
        status = dcc_component_json_append_u32_member(
            buffer,
            first,
            "style",
            (uint32_t)builder->button_style
        );
    }
    if (status == DCC_OK && builder->label != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "label", builder->label);
    }
    if (status == DCC_OK && builder->has_emoji) {
        status = dcc_component_json_member_prefix(buffer, first, "emoji");
        if (status == DCC_OK) {
            status = dcc_component_append_emoji_json(&builder->emoji, buffer);
        }
    }
    if (status == DCC_OK && builder->custom_id != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "custom_id", builder->custom_id);
    }
    if (status == DCC_OK && builder->url != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "url", builder->url);
    }
    if (status == DCC_OK && builder->has_sku_id) {
        status = dcc_component_json_append_snowflake_string_member(buffer, first, "sku_id", builder->sku_id);
    }
    if (status == DCC_OK && builder->has_disabled) {
        status = dcc_component_json_append_bool_member(buffer, first, "disabled", builder->disabled);
    }
    return status;
}

static dcc_status_t dcc_component_v2_append_select_fields(
    const dcc_component_v2_serial_view_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = DCC_OK;
    if (builder->custom_id != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "custom_id", builder->custom_id);
    }
    if (status == DCC_OK && builder->options_count != 0) {
        status = dcc_component_json_member_prefix(buffer, first, "options");
        if (status == DCC_OK) {
            status = dcc_component_v2_append_options(builder->options, builder->options_count, buffer);
        }
    }
    if (status == DCC_OK && builder->default_value_count != 0) {
        status = dcc_component_json_member_prefix(buffer, first, "default_values");
        if (status == DCC_OK) {
            status = dcc_component_v2_append_default_values(
                builder->default_values,
                builder->default_value_count,
                buffer
            );
        }
    }
    if (status == DCC_OK && builder->channel_type_count != 0) {
        status = dcc_component_json_member_prefix(buffer, first, "channel_types");
        if (status == DCC_OK) {
            status = dcc_component_v2_append_channel_types(
                builder->channel_types,
                builder->channel_type_count,
                buffer
            );
        }
    }
    if (status == DCC_OK && builder->placeholder != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "placeholder", builder->placeholder);
    }
    if (status == DCC_OK && builder->has_min_values) {
        status = dcc_component_json_append_u32_member(buffer, first, "min_values", builder->min_values);
    }
    if (status == DCC_OK && builder->has_max_values) {
        status = dcc_component_json_append_u32_member(buffer, first, "max_values", builder->max_values);
    }
    if (status == DCC_OK && builder->has_disabled) {
        status = dcc_component_json_append_bool_member(buffer, first, "disabled", builder->disabled);
    }
    if (status == DCC_OK && builder->has_required) {
        status = dcc_component_json_append_bool_member(buffer, first, "required", builder->required);
    }
    return status;
}

static dcc_status_t dcc_component_v2_append_text_input_fields(
    const dcc_component_v2_serial_view_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = DCC_OK;
    if (builder->custom_id != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "custom_id", builder->custom_id);
    }
    if (status == DCC_OK && builder->has_text_input_style) {
        status = dcc_component_json_append_u32_member(
            buffer,
            first,
            "style",
            (uint32_t)builder->text_input_style
        );
    }
    if (status == DCC_OK && builder->placeholder != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "placeholder", builder->placeholder);
    }
    if (status == DCC_OK && builder->has_min_length) {
        status = dcc_component_json_append_u32_member(buffer, first, "min_length", builder->min_length);
    }
    if (status == DCC_OK && builder->has_max_length) {
        status = dcc_component_json_append_u32_member(buffer, first, "max_length", builder->max_length);
    }
    if (status == DCC_OK && builder->value != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "value", builder->value);
    }
    if (status == DCC_OK && builder->has_required) {
        status = dcc_component_json_append_bool_member(buffer, first, "required", builder->required);
    }
    return status;
}

static dcc_status_t dcc_component_v2_append_media_fields(
    const dcc_component_v2_serial_view_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
) {
    const char *member = builder->type == DCC_COMPONENT_V2_FILE ? "file" : "media";
    dcc_status_t status = dcc_component_json_member_prefix(buffer, first, member);
    if (status == DCC_OK) {
        status = dcc_component_v2_append_media_object(&builder->media[0], buffer);
    }
    if (status == DCC_OK && builder->media[0].description != NULL) {
        status = dcc_component_json_append_string_member(
            buffer,
            first,
            "description",
            builder->media[0].description
        );
    }
    if (status == DCC_OK && builder->media[0].has_spoiler) {
        status = dcc_component_json_append_bool_member(buffer, first, "spoiler", builder->media[0].spoiler);
    }
    if (status == DCC_OK && builder->has_spoiler) {
        status = dcc_component_json_append_bool_member(buffer, first, "spoiler", builder->spoiler);
    }
    return status;
}

static dcc_status_t dcc_component_v2_append_gallery_fields(
    const dcc_component_v2_serial_view_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = dcc_component_json_member_prefix(buffer, first, "items");
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "[");
    }
    for (size_t i = 0; status == DCC_OK && i < builder->media_count; ++i) {
        if (i != 0) {
            status = dcc_component_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_component_v2_append_media_item(&builder->media[i], buffer);
        }
    }
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "]");
    }
    return status;
}

static dcc_status_t dcc_component_v2_append_fields(
    const dcc_component_v2_serial_view_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = DCC_OK;
    switch (builder->type) {
        case DCC_COMPONENT_V2_ACTION_ROW:
        case DCC_COMPONENT_V2_CONTAINER:
            status = dcc_component_v2_append_children_member(
                builder->children,
                builder->children_count,
                "components",
                buffer,
                first
            );
            if (status == DCC_OK && builder->type == DCC_COMPONENT_V2_CONTAINER &&
                builder->has_accent_color) {
                status = dcc_component_json_append_u32_member(
                    buffer,
                    first,
                    "accent_color",
                    builder->accent_color
                );
            }
            if (status == DCC_OK && builder->type == DCC_COMPONENT_V2_CONTAINER &&
                builder->has_spoiler) {
                status = dcc_component_json_append_bool_member(buffer, first, "spoiler", builder->spoiler);
            }
            break;
        case DCC_COMPONENT_V2_BUTTON:
            status = dcc_component_v2_append_button_fields(builder, buffer, first);
            break;
        case DCC_COMPONENT_V2_STRING_SELECT:
        case DCC_COMPONENT_V2_USER_SELECT:
        case DCC_COMPONENT_V2_ROLE_SELECT:
        case DCC_COMPONENT_V2_MENTIONABLE_SELECT:
        case DCC_COMPONENT_V2_CHANNEL_SELECT:
            status = dcc_component_v2_append_select_fields(builder, buffer, first);
            break;
        case DCC_COMPONENT_V2_TEXT_INPUT:
            status = dcc_component_v2_append_text_input_fields(builder, buffer, first);
            break;
        case DCC_COMPONENT_V2_SECTION:
            status = dcc_component_v2_append_children_member(
                builder->children,
                builder->children_count,
                "components",
                buffer,
                first
            );
            if (status == DCC_OK) {
                status = dcc_component_json_member_prefix(buffer, first, "accessory");
            }
            if (status == DCC_OK) {
                status = dcc_component_v2_append_json(builder->accessory, buffer);
            }
            break;
        case DCC_COMPONENT_V2_TEXT_DISPLAY:
            status = dcc_component_json_append_string_member(buffer, first, "content", builder->content);
            break;
        case DCC_COMPONENT_V2_THUMBNAIL:
        case DCC_COMPONENT_V2_FILE:
            status = dcc_component_v2_append_media_fields(builder, buffer, first);
            break;
        case DCC_COMPONENT_V2_MEDIA_GALLERY:
            status = dcc_component_v2_append_gallery_fields(builder, buffer, first);
            break;
        case DCC_COMPONENT_V2_SEPARATOR:
            if (builder->has_divider) {
                status = dcc_component_json_append_bool_member(buffer, first, "divider", builder->divider);
            }
            if (status == DCC_OK && builder->has_spacing) {
                status = dcc_component_json_append_u32_member(
                    buffer,
                    first,
                    "spacing",
                    (uint32_t)builder->spacing
                );
            }
            break;
        case DCC_COMPONENT_V2_LABEL:
            status = dcc_component_json_append_string_member(buffer, first, "label", builder->label);
            if (status == DCC_OK && builder->description != NULL) {
                status = dcc_component_json_append_string_member(
                    buffer,
                    first,
                    "description",
                    builder->description
                );
            }
            if (status == DCC_OK) {
                status = dcc_component_json_member_prefix(buffer, first, "component");
            }
            if (status == DCC_OK) {
                status = dcc_component_v2_append_json(&builder->children[0], buffer);
            }
            break;
        case DCC_COMPONENT_V2_FILE_UPLOAD:
            status = dcc_component_json_append_string_member(buffer, first, "custom_id", builder->custom_id);
            if (status == DCC_OK && builder->has_min_values) {
                status = dcc_component_json_append_u32_member(buffer, first, "min_values", builder->min_values);
            }
            if (status == DCC_OK && builder->has_max_values) {
                status = dcc_component_json_append_u32_member(buffer, first, "max_values", builder->max_values);
            }
            if (status == DCC_OK && builder->has_required) {
                status = dcc_component_json_append_bool_member(buffer, first, "required", builder->required);
            }
            if (status == DCC_OK && builder->file_type_count != 0U) {
                status = dcc_component_json_member_prefix(
                    buffer, first, "file_types");
                if (status == DCC_OK)
                    status = dcc_component_v2_append_file_types(
                        builder->file_types, builder->file_type_count, buffer);
            }
            break;
        case DCC_COMPONENT_V2_RADIO_GROUP:
        case DCC_COMPONENT_V2_CHECKBOX_GROUP:
            status = dcc_component_json_append_string_member(buffer, first, "custom_id", builder->custom_id);
            if (status == DCC_OK) {
                status = dcc_component_json_member_prefix(buffer, first, "options");
            }
            if (status == DCC_OK) {
                status = dcc_component_v2_append_choice_options(
                    builder->choice_options, builder->options_count, buffer);
            }
            if (status == DCC_OK && builder->has_required) {
                status = dcc_component_json_append_bool_member(buffer, first, "required", builder->required);
            }
            if (status == DCC_OK && builder->type == DCC_COMPONENT_V2_CHECKBOX_GROUP &&
                builder->has_min_values) {
                status = dcc_component_json_append_u32_member(
                    buffer,
                    first,
                    "min_values",
                    builder->min_values
                );
            }
            if (status == DCC_OK && builder->type == DCC_COMPONENT_V2_CHECKBOX_GROUP &&
                builder->has_max_values) {
                status = dcc_component_json_append_u32_member(
                    buffer,
                    first,
                    "max_values",
                    builder->max_values
                );
            }
            break;
        case DCC_COMPONENT_V2_CHECKBOX:
            status = dcc_component_json_append_string_member(buffer, first, "custom_id", builder->custom_id);
            if (status == DCC_OK && builder->has_checked) {
                status = dcc_component_json_append_bool_member(buffer, first, "default", builder->checked);
            }
            break;
        default:
            status = DCC_ERR_INVALID_ARG;
            break;
    }
    return status;
}

dcc_status_t dcc_component_v2_append_json(
    const dcc_component_v2_builder_t *builder,
    dcc_component_json_buffer_t *buffer
) {
    dcc_component_v2_serial_view_t normalized;
    dcc_component_v2_normalize_view(builder, &normalized);
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_component_json_append_u32_member(buffer, &first, "type", (uint32_t)normalized.type);
    }
    if (status == DCC_OK && normalized.has_id) {
        status = dcc_component_json_append_u32_member(buffer, &first, "id", normalized.id);
    }
    if (status == DCC_OK) {
        status = dcc_component_v2_append_fields(&normalized, buffer, &first);
    }
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "}");
    }
    return status;
}

dcc_status_t dcc_component_v2_append_array_json(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count,
    dcc_component_json_buffer_t *buffer
) {
    size_t stride = 0U;
    if (builder_count != 0U) {
        if (builders == NULL ||
            ((uintptr_t)builders % _Alignof(dcc_component_v2_builder_t)) != 0U)
            return DCC_ERR_INVALID_ARG;
        memcpy(&stride, builders, sizeof(stride));
        size_t prefix = offsetof(dcc_component_v2_builder_t, id) +
            sizeof(builders->id);
        if (stride < prefix ||
            stride % _Alignof(dcc_component_v2_builder_t) != 0U ||
            builder_count > SIZE_MAX / stride)
            return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "[");
    for (size_t i = 0; status == DCC_OK && i < builder_count; ++i) {
        if (i != 0) {
            status = dcc_component_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            const dcc_component_v2_builder_t *item =
                (const dcc_component_v2_builder_t *)
                    ((const unsigned char *)builders + stride * i);
            size_t item_size = 0U;
            memcpy(&item_size, item, sizeof(item_size));
            status = item_size == stride
                ? dcc_component_v2_append_json(item, buffer)
                : DCC_ERR_INVALID_ARG;
        }
    }
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "]");
    }
    return status;
}
