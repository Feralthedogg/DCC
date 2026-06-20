#include "internal/objects/dcc_component_v2_internal.h"
#include "internal/objects/dcc_component_serialize_internal.h"

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
    const dcc_component_v2_builder_t *builder,
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
    const dcc_component_v2_builder_t *builder,
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
    const dcc_component_v2_builder_t *builder,
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
    if (status == DCC_OK && builder->label != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "label", builder->label);
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
    const dcc_component_v2_builder_t *builder,
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
    const dcc_component_v2_builder_t *builder,
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
    const dcc_component_v2_builder_t *builder,
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
            break;
        case DCC_COMPONENT_V2_RADIO_GROUP:
        case DCC_COMPONENT_V2_CHECKBOX_GROUP:
            status = dcc_component_json_append_string_member(buffer, first, "custom_id", builder->custom_id);
            if (status == DCC_OK) {
                status = dcc_component_json_member_prefix(buffer, first, "options");
            }
            if (status == DCC_OK) {
                status = dcc_component_v2_append_options(builder->options, builder->options_count, buffer);
            }
            if (status == DCC_OK && builder->has_required) {
                status = dcc_component_json_append_bool_member(buffer, first, "required", builder->required);
            }
            break;
        case DCC_COMPONENT_V2_CHECKBOX:
            status = dcc_component_json_append_string_member(buffer, first, "custom_id", builder->custom_id);
            if (status == DCC_OK) {
                status = dcc_component_json_append_string_member(buffer, first, "label", builder->label);
            }
            if (status == DCC_OK && builder->value != NULL) {
                status = dcc_component_json_append_string_member(buffer, first, "value", builder->value);
            }
            if (status == DCC_OK && builder->description != NULL) {
                status = dcc_component_json_append_string_member(
                    buffer,
                    first,
                    "description",
                    builder->description
                );
            }
            if (status == DCC_OK && builder->has_checked) {
                status = dcc_component_json_append_bool_member(buffer, first, "default", builder->checked);
            }
            if (status == DCC_OK && builder->has_required) {
                status = dcc_component_json_append_bool_member(buffer, first, "required", builder->required);
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
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_component_json_append_u32_member(buffer, &first, "type", (uint32_t)builder->type);
    }
    if (status == DCC_OK && builder->has_id) {
        status = dcc_component_json_append_u32_member(buffer, &first, "id", builder->id);
    }
    if (status == DCC_OK) {
        status = dcc_component_v2_append_fields(builder, buffer, &first);
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
    dcc_status_t status = dcc_component_json_append_cstr(buffer, "[");
    for (size_t i = 0; status == DCC_OK && i < builder_count; ++i) {
        if (i != 0) {
            status = dcc_component_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_component_v2_append_json(&builders[i], buffer);
        }
    }
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "]");
    }
    return status;
}
