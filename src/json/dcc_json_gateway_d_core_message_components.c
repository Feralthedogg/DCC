#include "internal/json/dcc_json_internal.h"

#include <stdint.h>
#include <string.h>

#define DCC_JSON_MESSAGE_COMPONENT_MAX_DEPTH 16U
#define DCC_JSON_MESSAGE_COMPONENT_NO_INDEX 0xFFU

static dcc_status_t dcc_json_gateway_parse_message_component_object(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    uint8_t parent_index,
    int has_parent,
    unsigned depth,
    uint8_t *out_index
);

static dcc_status_t dcc_json_gateway_component_string(
    dcc_json_parser_t *parser,
    char *storage,
    size_t storage_cap,
    const char **out_value
) {
    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        storage,
        storage_cap,
        &has_value
    );
    if (status == DCC_OK) {
        *out_value = has_value ? storage : NULL;
    }
    return status;
}

static dcc_status_t dcc_json_gateway_component_bool(
    dcc_json_parser_t *parser,
    uint8_t *value,
    uint8_t *has_value
) {
    int parsed = 0;
    dcc_status_t status = dcc_json_parse_bool_direct(parser, &parsed);
    if (status == DCC_OK) {
        *value = parsed ? 1U : 0U;
        *has_value = 1U;
    }
    return status;
}

static dcc_status_t dcc_json_gateway_component_u32(
    dcc_json_parser_t *parser,
    uint32_t *value,
    uint8_t *has_value
) {
    dcc_status_t status = dcc_json_parse_u32_direct(parser, value);
    if (status == DCC_OK && has_value != NULL) {
        *has_value = 1U;
    }
    return status;
}

static dcc_status_t dcc_json_gateway_component_snowflake(
    dcc_json_parser_t *parser,
    dcc_snowflake_t *value,
    uint8_t *has_value
) {
    uint64_t parsed = 0;
    int present = 0;
    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &parsed, &present);
    if (status == DCC_OK) {
        *value = present ? parsed : 0U;
        *has_value = present ? 1U : 0U;
    }
    return status;
}

static void dcc_json_gateway_component_add_child(
    dcc_component_v2_t *parent,
    uint8_t child_index
) {
    if (child_index == DCC_JSON_MESSAGE_COMPONENT_NO_INDEX ||
        parent->child_count >= DCC_COMPONENT_V2_MAX_COMPONENTS) {
        return;
    }
    parent->child_indices[parent->child_count++] = child_index;
}

static dcc_component_v2_t *dcc_json_gateway_component_alloc(
    dcc_json_gateway_payload_t *out,
    uint8_t parent_index,
    int has_parent,
    unsigned depth,
    uint8_t *out_index
) {
    if (out->message.components_count >= DCC_JSON_GATEWAY_MESSAGE_COMPONENT_CAP) {
        out->message.components_truncated = 1U;
        *out_index = DCC_JSON_MESSAGE_COMPONENT_NO_INDEX;
        return NULL;
    }

    size_t index = out->message.components_count++;
    dcc_component_v2_t *component = &out->message_components[index];
    memset(component, 0, sizeof(*component));
    component->parent_index = parent_index;
    component->has_parent = has_parent ? 1U : 0U;
    component->depth = depth > UINT8_MAX ? UINT8_MAX : (uint8_t)depth;
    out->message.components = out->message_components;
    *out_index = (uint8_t)index;

    if (!has_parent) {
        if (out->message.component_root_count < DCC_COMPONENT_V2_MAX_COMPONENTS) {
            out->message.component_root_indices[out->message.component_root_count++] = (uint8_t)index;
        } else {
            out->message.components_truncated = 1U;
        }
    }

    return component;
}

static dcc_status_t dcc_json_gateway_parse_component_media_object(
    dcc_json_parser_t *parser,
    char *url_storage,
    size_t url_storage_cap,
    const char **out_url
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        dcc_json_key_id_t key_id = DCC_JSON_KEY_UNKNOWN;
        if (dcc_json_parse_gateway_key(parser, &key_id) != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end || *parser->cur != ':') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);

        dcc_status_t status = DCC_OK;
        if (key_id == DCC_JSON_KEY_URL) {
            status = dcc_json_gateway_component_string(
                parser,
                url_storage,
                url_storage_cap,
                out_url
            );
        } else {
            status = dcc_json_skip_value(parser);
        }
        if (status != DCC_OK) {
            return status;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == '}') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

static dcc_component_v2_media_t *dcc_json_gateway_component_media_alloc(
    dcc_json_gateway_payload_t *out,
    dcc_component_v2_t *component
) {
    if (out->message_component_media_count >= DCC_JSON_GATEWAY_MESSAGE_COMPONENT_MEDIA_CAP) {
        component->media_truncated = 1U;
        out->message.components_truncated = 1U;
        return NULL;
    }
    size_t index = out->message_component_media_count++;
    dcc_component_v2_media_t *media = &out->message_component_media[index];
    memset(media, 0, sizeof(*media));
    if (component->media_count == 0) {
        component->media = media;
    }
    component->media_count++;
    return media;
}

static dcc_status_t dcc_json_gateway_parse_component_media_item_fields(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_component_v2_t *component,
    dcc_component_v2_media_t *media,
    size_t media_index
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || *parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        dcc_json_key_id_t key_id = DCC_JSON_KEY_UNKNOWN;
        if (dcc_json_parse_gateway_key(parser, &key_id) != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end || *parser->cur != ':') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);

        dcc_status_t status = DCC_OK;
        switch (key_id) {
            case DCC_JSON_KEY_MEDIA:
            case DCC_JSON_KEY_FILE:
                status = dcc_json_gateway_parse_component_media_object(
                    parser,
                    out->message_component_media_urls[media_index],
                    sizeof(out->message_component_media_urls[media_index]),
                    &media->url
                );
                break;
            case DCC_JSON_KEY_DESCRIPTION:
                status = dcc_json_gateway_component_string(
                    parser,
                    out->message_component_media_descriptions[media_index],
                    sizeof(out->message_component_media_descriptions[media_index]),
                    &media->description
                );
                break;
            case DCC_JSON_KEY_SPOILER:
                status = dcc_json_gateway_component_bool(parser, &media->spoiler, &media->has_spoiler);
                break;
            default:
                status = dcc_json_skip_value(parser);
                break;
        }
        if (status != DCC_OK) {
            return status;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == '}') {
            parser->cur++;
            (void)component;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

static dcc_status_t dcc_json_gateway_parse_component_single_media(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_component_v2_t *component,
    int nested_object
) {
    dcc_component_v2_media_t *media = dcc_json_gateway_component_media_alloc(out, component);
    if (media == NULL) {
        return dcc_json_skip_value(parser);
    }
    size_t media_index = (size_t)(media - out->message_component_media);
    if (nested_object) {
        return dcc_json_gateway_parse_component_media_object(
            parser,
            out->message_component_media_urls[media_index],
            sizeof(out->message_component_media_urls[media_index]),
            &media->url
        );
    }
    return dcc_json_gateway_parse_component_media_item_fields(
        parser,
        out,
        component,
        media,
        media_index
    );
}

static dcc_status_t dcc_json_gateway_parse_component_media_items(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_component_v2_t *component
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '[') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        dcc_component_v2_media_t *media = dcc_json_gateway_component_media_alloc(out, component);
        dcc_status_t status = DCC_OK;
        if (media == NULL) {
            status = dcc_json_skip_value(parser);
        } else {
            size_t media_index = (size_t)(media - out->message_component_media);
            status = dcc_json_gateway_parse_component_media_item_fields(
                parser,
                out,
                component,
                media,
                media_index
            );
        }
        if (status != DCC_OK) {
            return status;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

static dcc_status_t dcc_json_gateway_parse_component_option(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_select_option_t *option,
    size_t option_index
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || *parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        dcc_json_key_id_t key_id = DCC_JSON_KEY_UNKNOWN;
        if (dcc_json_parse_gateway_key(parser, &key_id) != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end || *parser->cur != ':') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);

        dcc_status_t status = DCC_OK;
        switch (key_id) {
            case DCC_JSON_KEY_LABEL:
                status = dcc_json_gateway_component_string(
                    parser,
                    out->message_component_option_labels[option_index],
                    sizeof(out->message_component_option_labels[option_index]),
                    &option->label
                );
                break;
            case DCC_JSON_KEY_VALUE:
                status = dcc_json_gateway_component_string(
                    parser,
                    out->message_component_option_values[option_index],
                    sizeof(out->message_component_option_values[option_index]),
                    &option->value
                );
                break;
            case DCC_JSON_KEY_DESCRIPTION:
                status = dcc_json_gateway_component_string(
                    parser,
                    out->message_component_option_descriptions[option_index],
                    sizeof(out->message_component_option_descriptions[option_index]),
                    &option->description
                );
                break;
            case DCC_JSON_KEY_DEFAULT:
                status = dcc_json_gateway_component_bool(parser, &option->is_default, &option->has_default);
                break;
            default:
                status = dcc_json_skip_value(parser);
                break;
        }
        if (status != DCC_OK) {
            return status;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == '}') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

static dcc_status_t dcc_json_gateway_parse_component_options(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_component_v2_t *component
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '[') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        if (out->message_component_option_count >= DCC_JSON_GATEWAY_MESSAGE_COMPONENT_OPTION_CAP) {
            component->options_truncated = 1U;
            out->message.components_truncated = 1U;
            dcc_status_t status = dcc_json_skip_value(parser);
            if (status != DCC_OK) {
                return status;
            }
        } else {
            size_t option_index = out->message_component_option_count++;
            dcc_select_option_t *option = &out->message_component_options[option_index];
            memset(option, 0, sizeof(*option));
            if (component->options_count == 0) {
                component->options = option;
            }
            component->options_count++;
            dcc_status_t status = dcc_json_gateway_parse_component_option(
                parser,
                out,
                option,
                option_index
            );
            if (status != DCC_OK) {
                return status;
            }
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

static dcc_component_v2_select_default_type_t dcc_json_gateway_component_default_type(
    const char *type
) {
    if (type == NULL) {
        return 0;
    }
    if (strcmp(type, "user") == 0) {
        return DCC_COMPONENT_V2_SELECT_DEFAULT_USER;
    }
    if (strcmp(type, "role") == 0) {
        return DCC_COMPONENT_V2_SELECT_DEFAULT_ROLE;
    }
    if (strcmp(type, "channel") == 0) {
        return DCC_COMPONENT_V2_SELECT_DEFAULT_CHANNEL;
    }
    return 0;
}

static dcc_status_t dcc_json_gateway_parse_component_default_value(
    dcc_json_parser_t *parser,
    dcc_component_v2_select_default_value_t *value
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || *parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        dcc_json_key_id_t key_id = DCC_JSON_KEY_UNKNOWN;
        if (dcc_json_parse_gateway_key(parser, &key_id) != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end || *parser->cur != ':') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);

        dcc_status_t status = DCC_OK;
        if (key_id == DCC_JSON_KEY_ID) {
            uint8_t has_value = 0;
            status = dcc_json_gateway_component_snowflake(parser, &value->id, &has_value);
        } else if (key_id == DCC_JSON_KEY_TYPE) {
            char type_storage[32];
            const char *type = NULL;
            status = dcc_json_gateway_component_string(
                parser,
                type_storage,
                sizeof(type_storage),
                &type
            );
            if (status == DCC_OK) {
                value->type = dcc_json_gateway_component_default_type(type);
            }
        } else {
            status = dcc_json_skip_value(parser);
        }
        if (status != DCC_OK) {
            return status;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == '}') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

static dcc_status_t dcc_json_gateway_parse_component_default_values(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_component_v2_t *component
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '[') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        if (out->message_component_default_value_count >=
            DCC_JSON_GATEWAY_MESSAGE_COMPONENT_DEFAULT_VALUE_CAP) {
            component->default_values_truncated = 1U;
            out->message.components_truncated = 1U;
            dcc_status_t status = dcc_json_skip_value(parser);
            if (status != DCC_OK) {
                return status;
            }
        } else {
            size_t value_index = out->message_component_default_value_count++;
            dcc_component_v2_select_default_value_t *value =
                &out->message_component_default_values[value_index];
            memset(value, 0, sizeof(*value));
            if (component->default_value_count == 0) {
                component->default_values = value;
            }
            component->default_value_count++;
            dcc_status_t status = dcc_json_gateway_parse_component_default_value(parser, value);
            if (status != DCC_OK) {
                return status;
            }
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

static dcc_status_t dcc_json_gateway_parse_component_channel_types(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_component_v2_t *component
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '[') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        if (out->message_component_channel_type_count >=
            DCC_JSON_GATEWAY_MESSAGE_COMPONENT_CHANNEL_TYPE_CAP) {
            component->channel_types_truncated = 1U;
            out->message.components_truncated = 1U;
            dcc_status_t status = dcc_json_skip_value(parser);
            if (status != DCC_OK) {
                return status;
            }
        } else {
            size_t channel_type_index = out->message_component_channel_type_count++;
            if (component->channel_type_count == 0) {
                component->channel_types =
                    &out->message_component_channel_types[channel_type_index];
            }
            component->channel_type_count++;
            dcc_status_t status = dcc_json_parse_u32_direct(
                parser,
                &out->message_component_channel_types[channel_type_index]
            );
            if (status != DCC_OK) {
                return status;
            }
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

static dcc_status_t dcc_json_gateway_parse_message_component_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    uint8_t parent_index,
    int has_parent,
    unsigned depth
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '[') {
        return dcc_json_skip_value(parser);
    }
    if (depth > DCC_JSON_MESSAGE_COMPONENT_MAX_DEPTH) {
        return DCC_ERR_JSON;
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        uint8_t child_index = DCC_JSON_MESSAGE_COMPONENT_NO_INDEX;
        dcc_status_t status = dcc_json_gateway_parse_message_component_object(
            parser,
            out,
            parent_index,
            has_parent,
            depth + 1U,
            &child_index
        );
        if (status != DCC_OK) {
            return status;
        }
        if (has_parent && parent_index != DCC_JSON_MESSAGE_COMPONENT_NO_INDEX) {
            dcc_json_gateway_component_add_child(&out->message_components[parent_index], child_index);
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

static dcc_status_t dcc_json_gateway_parse_component_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_component_v2_t *component,
    uint8_t component_index,
    dcc_json_key_id_t key_id,
    unsigned depth
) {
    switch (key_id) {
        case DCC_JSON_KEY_TYPE: {
            uint32_t type = 0;
            dcc_status_t status = dcc_json_parse_u32_direct(parser, &type);
            if (status == DCC_OK) {
                component->type = (dcc_component_v2_type_t)type;
            }
            return status;
        }
        case DCC_JSON_KEY_ID:
            return dcc_json_gateway_component_u32(parser, &component->id, &component->has_id);
        case DCC_JSON_KEY_STYLE:
            return dcc_json_gateway_component_u32(parser, &component->style, &component->has_style);
        case DCC_JSON_KEY_CUSTOM_ID:
            return dcc_json_gateway_component_string(
                parser,
                out->message_component_custom_ids[component_index],
                sizeof(out->message_component_custom_ids[component_index]),
                &component->custom_id
            );
        case DCC_JSON_KEY_LABEL:
            return dcc_json_gateway_component_string(
                parser,
                out->message_component_labels[component_index],
                sizeof(out->message_component_labels[component_index]),
                &component->label
            );
        case DCC_JSON_KEY_DESCRIPTION:
            return dcc_json_gateway_component_string(
                parser,
                out->message_component_descriptions[component_index],
                sizeof(out->message_component_descriptions[component_index]),
                &component->description
            );
        case DCC_JSON_KEY_PLACEHOLDER:
            return dcc_json_gateway_component_string(
                parser,
                out->message_component_placeholders[component_index],
                sizeof(out->message_component_placeholders[component_index]),
                &component->placeholder
            );
        case DCC_JSON_KEY_CONTENT:
            return dcc_json_gateway_component_string(
                parser,
                out->message_component_contents[component_index],
                sizeof(out->message_component_contents[component_index]),
                &component->content
            );
        case DCC_JSON_KEY_URL:
            return dcc_json_gateway_component_string(
                parser,
                out->message_component_urls[component_index],
                sizeof(out->message_component_urls[component_index]),
                &component->url
            );
        case DCC_JSON_KEY_VALUE:
            return dcc_json_gateway_component_string(
                parser,
                out->message_component_values[component_index],
                sizeof(out->message_component_values[component_index]),
                &component->value
            );
        case DCC_JSON_KEY_SKU_ID:
            return dcc_json_gateway_component_snowflake(parser, &component->sku_id, &component->has_sku_id);
        case DCC_JSON_KEY_OPTIONS:
            return dcc_json_gateway_parse_component_options(parser, out, component);
        case DCC_JSON_KEY_DEFAULT_VALUES:
            return dcc_json_gateway_parse_component_default_values(parser, out, component);
        case DCC_JSON_KEY_CHANNEL_TYPES:
            return dcc_json_gateway_parse_component_channel_types(parser, out, component);
        case DCC_JSON_KEY_MEDIA:
        case DCC_JSON_KEY_FILE:
            return dcc_json_gateway_parse_component_single_media(parser, out, component, 1);
        case DCC_JSON_KEY_ITEMS:
            return dcc_json_gateway_parse_component_media_items(parser, out, component);
        case DCC_JSON_KEY_COMPONENTS:
            return dcc_json_gateway_parse_message_component_array(
                parser,
                out,
                component_index,
                1,
                depth + 1U
            );
        case DCC_JSON_KEY_ACCESSORY: {
            uint8_t child_index = DCC_JSON_MESSAGE_COMPONENT_NO_INDEX;
            dcc_status_t status = dcc_json_gateway_parse_message_component_object(
                parser,
                out,
                component_index,
                1,
                depth + 1U,
                &child_index
            );
            if (status == DCC_OK && child_index != DCC_JSON_MESSAGE_COMPONENT_NO_INDEX) {
                component->accessory_index = child_index;
                component->has_accessory = 1U;
            }
            return status;
        }
        case DCC_JSON_KEY_COMPONENT: {
            uint8_t child_index = DCC_JSON_MESSAGE_COMPONENT_NO_INDEX;
            dcc_status_t status = dcc_json_gateway_parse_message_component_object(
                parser,
                out,
                component_index,
                1,
                depth + 1U,
                &child_index
            );
            if (status == DCC_OK && child_index != DCC_JSON_MESSAGE_COMPONENT_NO_INDEX) {
                component->component_index = child_index;
                component->has_component = 1U;
            }
            return status;
        }
        case DCC_JSON_KEY_MIN_VALUES:
            return dcc_json_gateway_component_u32(parser, &component->min_values, &component->has_min_values);
        case DCC_JSON_KEY_MAX_VALUES:
            return dcc_json_gateway_component_u32(parser, &component->max_values, &component->has_max_values);
        case DCC_JSON_KEY_MIN_LENGTH:
            return dcc_json_gateway_component_u32(parser, &component->min_length, &component->has_min_length);
        case DCC_JSON_KEY_MAX_LENGTH:
            return dcc_json_gateway_component_u32(parser, &component->max_length, &component->has_max_length);
        case DCC_JSON_KEY_ACCENT_COLOR:
            return dcc_json_gateway_component_u32(
                parser,
                &component->accent_color,
                &component->has_accent_color
            );
        case DCC_JSON_KEY_DISABLED:
            return dcc_json_gateway_component_bool(parser, &component->disabled, &component->has_disabled);
        case DCC_JSON_KEY_REQUIRED:
            return dcc_json_gateway_component_bool(parser, &component->required, &component->has_required);
        case DCC_JSON_KEY_DIVIDER:
            return dcc_json_gateway_component_bool(parser, &component->divider, &component->has_divider);
        case DCC_JSON_KEY_SPOILER:
            return dcc_json_gateway_component_bool(parser, &component->spoiler, &component->has_spoiler);
        case DCC_JSON_KEY_DEFAULT:
            return dcc_json_gateway_component_bool(
                parser,
                &component->default_value,
                &component->has_default_value
            );
        case DCC_JSON_KEY_SPACING: {
            uint32_t spacing = 0;
            dcc_status_t status = dcc_json_parse_u32_direct(parser, &spacing);
            if (status == DCC_OK) {
                component->spacing = (dcc_component_v2_separator_spacing_t)spacing;
                component->has_spacing = 1U;
            }
            return status;
        }
        default:
            return dcc_json_skip_value(parser);
    }
}

static dcc_status_t dcc_json_gateway_parse_message_component_object(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    uint8_t parent_index,
    int has_parent,
    unsigned depth,
    uint8_t *out_index
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        *out_index = DCC_JSON_MESSAGE_COMPONENT_NO_INDEX;
        return dcc_json_skip_value(parser);
    }
    if (depth > DCC_JSON_MESSAGE_COMPONENT_MAX_DEPTH) {
        return DCC_ERR_JSON;
    }

    dcc_component_v2_t *component = dcc_json_gateway_component_alloc(
        out,
        parent_index,
        has_parent,
        depth,
        out_index
    );
    if (component == NULL) {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        dcc_json_key_id_t key_id = DCC_JSON_KEY_UNKNOWN;
        if (dcc_json_parse_gateway_key(parser, &key_id) != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end || *parser->cur != ':') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);

        dcc_status_t status = dcc_json_gateway_parse_component_field(
            parser,
            out,
            component,
            *out_index,
            key_id,
            depth
        );
        if (status != DCC_OK) {
            return status;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == '}') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

dcc_status_t dcc_json_gateway_parse_d_core_message_components_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_COMPONENTS) {
        *handled = 0;
        return DCC_OK;
    }

    const char *start = parser->cur;
    dcc_status_t status = dcc_json_skip_value(parser);
    if (status != DCC_OK) {
        return status;
    }

    const char *end = parser->cur;
    size_t len = (size_t)(end - start);
    size_t copy_len = len;
    if (copy_len >= sizeof(out->message_components_json)) {
        copy_len = sizeof(out->message_components_json) - 1U;
        out->message.components_json_truncated = 1U;
    } else {
        out->message.components_json_truncated = 0U;
    }
    memcpy(out->message_components_json, start, copy_len);
    out->message_components_json[copy_len] = '\0';
    out->message.components_json = out->message_components_json;

    out->message.components = out->message_components;
    out->message.components_count = 0;
    out->message.component_root_count = 0;
    out->message.components_truncated = 0U;
    out->message_component_option_count = 0;
    out->message_component_media_count = 0;
    out->message_component_default_value_count = 0;
    out->message_component_channel_type_count = 0;

    dcc_json_parser_t component_parser = {
        start,
        end,
        parser->doc
    };
    status = dcc_json_gateway_parse_message_component_array(
        &component_parser,
        out,
        DCC_JSON_MESSAGE_COMPONENT_NO_INDEX,
        0,
        0
    );
    if (status != DCC_OK) {
        return status;
    }

    out->has_message = 1;
    out->cache_fields |= DCC_CACHE_FIELD_MESSAGE_COMPONENTS;
    *handled = 1;
    return DCC_OK;
}
