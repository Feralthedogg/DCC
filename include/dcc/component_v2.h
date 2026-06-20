#ifndef DCC_COMPONENT_V2_H
#define DCC_COMPONENT_V2_H

#include <dcc/component.h>
#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_COMPONENT_V2_MAX_COMPONENTS 40U
#define DCC_COMPONENT_V2_MAX_ACTION_ROW_CHILDREN 5U
#define DCC_COMPONENT_V2_MAX_SECTION_CHILDREN 3U
#define DCC_COMPONENT_V2_MAX_MEDIA_GALLERY_ITEMS 10U
#define DCC_COMPONENT_V2_MAX_RADIO_OPTIONS 10U
#define DCC_COMPONENT_V2_MAX_CHECKBOX_OPTIONS 10U
#define DCC_COMPONENT_V2_MAX_SELECT_DEFAULT_VALUES 25U
#define DCC_COMPONENT_V2_MAX_CHANNEL_TYPES 16U

typedef enum dcc_component_v2_type {
    DCC_COMPONENT_V2_ACTION_ROW = 1,
    DCC_COMPONENT_V2_BUTTON = 2,
    DCC_COMPONENT_V2_STRING_SELECT = 3,
    DCC_COMPONENT_V2_TEXT_INPUT = 4,
    DCC_COMPONENT_V2_USER_SELECT = 5,
    DCC_COMPONENT_V2_ROLE_SELECT = 6,
    DCC_COMPONENT_V2_MENTIONABLE_SELECT = 7,
    DCC_COMPONENT_V2_CHANNEL_SELECT = 8,
    DCC_COMPONENT_V2_SECTION = 9,
    DCC_COMPONENT_V2_TEXT_DISPLAY = 10,
    DCC_COMPONENT_V2_THUMBNAIL = 11,
    DCC_COMPONENT_V2_MEDIA_GALLERY = 12,
    DCC_COMPONENT_V2_FILE = 13,
    DCC_COMPONENT_V2_SEPARATOR = 14,
    DCC_COMPONENT_V2_CONTAINER = 17,
    DCC_COMPONENT_V2_LABEL = 18,
    DCC_COMPONENT_V2_FILE_UPLOAD = 19,
    DCC_COMPONENT_V2_RADIO_GROUP = 21,
    DCC_COMPONENT_V2_CHECKBOX_GROUP = 22,
    DCC_COMPONENT_V2_CHECKBOX = 23,
} dcc_component_v2_type_t;

typedef enum dcc_component_v2_separator_spacing {
    DCC_COMPONENT_V2_SPACING_SMALL = 1,
    DCC_COMPONENT_V2_SPACING_LARGE = 2,
} dcc_component_v2_separator_spacing_t;

typedef struct dcc_component_v2_media {
    const char *url;
    const char *description;
    uint8_t spoiler;
    uint8_t has_spoiler;
} dcc_component_v2_media_t;

typedef enum dcc_component_v2_select_default_type {
    DCC_COMPONENT_V2_SELECT_DEFAULT_USER = 1,
    DCC_COMPONENT_V2_SELECT_DEFAULT_ROLE = 2,
    DCC_COMPONENT_V2_SELECT_DEFAULT_CHANNEL = 3,
} dcc_component_v2_select_default_type_t;

typedef struct dcc_component_v2_select_default_value {
    dcc_snowflake_t id;
    dcc_component_v2_select_default_type_t type;
} dcc_component_v2_select_default_value_t;

typedef struct dcc_component_v2 {
    dcc_component_v2_type_t type;
    uint32_t id;
    uint32_t style;
    dcc_component_v2_separator_spacing_t spacing;
    const char *custom_id;
    const char *label;
    const char *description;
    const char *placeholder;
    const char *content;
    const char *url;
    const char *value;
    dcc_snowflake_t sku_id;
    const dcc_select_option_t *options;
    size_t options_count;
    const dcc_component_v2_select_default_value_t *default_values;
    size_t default_value_count;
    const uint32_t *channel_types;
    size_t channel_type_count;
    const dcc_component_v2_media_t *media;
    size_t media_count;
    uint32_t min_values;
    uint32_t max_values;
    uint32_t min_length;
    uint32_t max_length;
    uint32_t accent_color;
    uint8_t child_indices[DCC_COMPONENT_V2_MAX_COMPONENTS];
    size_t child_count;
    uint8_t parent_index;
    uint8_t accessory_index;
    uint8_t component_index;
    uint8_t depth;
    uint8_t disabled;
    uint8_t required;
    uint8_t divider;
    uint8_t spoiler;
    uint8_t default_value;
    uint8_t has_id;
    uint8_t has_style;
    uint8_t has_spacing;
    uint8_t has_sku_id;
    uint8_t has_min_values;
    uint8_t has_max_values;
    uint8_t has_min_length;
    uint8_t has_max_length;
    uint8_t has_accent_color;
    uint8_t has_disabled;
    uint8_t has_required;
    uint8_t has_divider;
    uint8_t has_spoiler;
    uint8_t has_default_value;
    uint8_t has_parent;
    uint8_t has_accessory;
    uint8_t has_component;
    uint8_t options_truncated;
    uint8_t media_truncated;
    uint8_t default_values_truncated;
    uint8_t channel_types_truncated;
} dcc_component_v2_t;

typedef struct dcc_component_v2_builder dcc_component_v2_builder_t;

struct dcc_component_v2_builder {
    dcc_component_v2_type_t type;
    uint32_t id;
    dcc_button_style_t button_style;
    dcc_text_input_style_t text_input_style;
    dcc_component_v2_separator_spacing_t spacing;
    const char *content;
    const char *custom_id;
    const char *label;
    const char *description;
    const char *placeholder;
    const char *url;
    const char *value;
    dcc_snowflake_t sku_id;
    dcc_component_emoji_t emoji;
    const dcc_select_option_t *options;
    size_t options_count;
    const dcc_component_v2_select_default_value_t *default_values;
    size_t default_value_count;
    const uint32_t *channel_types;
    size_t channel_type_count;
    const dcc_component_v2_media_t *media;
    size_t media_count;
    const dcc_component_v2_builder_t *children;
    size_t children_count;
    const dcc_component_v2_builder_t *accessory;
    uint32_t min_values;
    uint32_t max_values;
    uint32_t min_length;
    uint32_t max_length;
    uint32_t accent_color;
    uint8_t disabled;
    uint8_t required;
    uint8_t divider;
    uint8_t spoiler;
    uint8_t checked;
    uint8_t has_id;
    uint8_t has_button_style;
    uint8_t has_text_input_style;
    uint8_t has_spacing;
    uint8_t has_sku_id;
    uint8_t has_emoji;
    uint8_t has_min_values;
    uint8_t has_max_values;
    uint8_t has_min_length;
    uint8_t has_max_length;
    uint8_t has_accent_color;
    uint8_t has_disabled;
    uint8_t has_required;
    uint8_t has_divider;
    uint8_t has_spoiler;
    uint8_t has_checked;
};

DCC_API void dcc_component_v2_builder_init(
    dcc_component_v2_builder_t *builder,
    dcc_component_v2_type_t type
);
DCC_API dcc_status_t dcc_component_v2_builder_init_text_display(
    dcc_component_v2_builder_t *builder,
    const char *content
);
DCC_API dcc_status_t dcc_component_v2_builder_init_button(
    dcc_component_v2_builder_t *builder,
    dcc_button_style_t style,
    const char *label,
    const char *custom_id
);
DCC_API dcc_status_t dcc_component_v2_builder_init_link_button(
    dcc_component_v2_builder_t *builder,
    const char *label,
    const char *url
);
DCC_API dcc_status_t dcc_component_v2_builder_init_action_row(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *children,
    size_t children_count
);
DCC_API dcc_status_t dcc_component_v2_builder_init_section(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *children,
    size_t children_count,
    const dcc_component_v2_builder_t *accessory
);
DCC_API dcc_status_t dcc_component_v2_builder_init_container(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *children,
    size_t children_count
);
DCC_API dcc_status_t dcc_component_v2_builder_init_separator(
    dcc_component_v2_builder_t *builder,
    uint8_t divider,
    dcc_component_v2_separator_spacing_t spacing
);
DCC_API dcc_status_t dcc_component_v2_builder_init_label(
    dcc_component_v2_builder_t *builder,
    const char *label,
    const dcc_component_v2_builder_t *component
);
DCC_API dcc_status_t dcc_component_v2_builder_init_media_gallery(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_media_t *media,
    size_t media_count
);
DCC_API dcc_status_t dcc_component_v2_builder_init_checkbox(
    dcc_component_v2_builder_t *builder,
    const char *custom_id,
    const char *label,
    uint8_t default_value
);
DCC_API dcc_status_t dcc_component_v2_builder_set_id(
    dcc_component_v2_builder_t *builder,
    uint32_t id
);
DCC_API dcc_status_t dcc_component_v2_builder_set_content(
    dcc_component_v2_builder_t *builder,
    const char *content
);
DCC_API dcc_status_t dcc_component_v2_builder_set_custom_id(
    dcc_component_v2_builder_t *builder,
    const char *custom_id
);
DCC_API dcc_status_t dcc_component_v2_builder_set_label(
    dcc_component_v2_builder_t *builder,
    const char *label
);
DCC_API dcc_status_t dcc_component_v2_builder_set_description(
    dcc_component_v2_builder_t *builder,
    const char *description
);
DCC_API dcc_status_t dcc_component_v2_builder_set_placeholder(
    dcc_component_v2_builder_t *builder,
    const char *placeholder
);
DCC_API dcc_status_t dcc_component_v2_builder_set_url(
    dcc_component_v2_builder_t *builder,
    const char *url
);
DCC_API dcc_status_t dcc_component_v2_builder_set_value(
    dcc_component_v2_builder_t *builder,
    const char *value
);
DCC_API dcc_status_t dcc_component_v2_builder_set_button_style(
    dcc_component_v2_builder_t *builder,
    dcc_button_style_t style
);
DCC_API dcc_status_t dcc_component_v2_builder_set_text_input_style(
    dcc_component_v2_builder_t *builder,
    dcc_text_input_style_t style
);
DCC_API dcc_status_t dcc_component_v2_builder_set_separator_spacing(
    dcc_component_v2_builder_t *builder,
    dcc_component_v2_separator_spacing_t spacing
);
DCC_API dcc_status_t dcc_component_v2_builder_set_sku_id(
    dcc_component_v2_builder_t *builder,
    dcc_snowflake_t sku_id
);
DCC_API dcc_status_t dcc_component_v2_builder_set_emoji(
    dcc_component_v2_builder_t *builder,
    const dcc_component_emoji_t *emoji
);
DCC_API dcc_status_t dcc_component_v2_builder_set_options(
    dcc_component_v2_builder_t *builder,
    const dcc_select_option_t *options,
    size_t options_count
);
DCC_API dcc_status_t dcc_component_v2_builder_set_default_values(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_select_default_value_t *default_values,
    size_t default_value_count
);
DCC_API dcc_status_t dcc_component_v2_builder_set_channel_types(
    dcc_component_v2_builder_t *builder,
    const uint32_t *channel_types,
    size_t channel_type_count
);
DCC_API dcc_status_t dcc_component_v2_builder_set_media(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_media_t *media,
    size_t media_count
);
DCC_API dcc_status_t dcc_component_v2_builder_set_children(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *children,
    size_t children_count
);
DCC_API dcc_status_t dcc_component_v2_builder_set_accessory(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_builder_t *accessory
);
DCC_API dcc_status_t dcc_component_v2_builder_set_min_values(
    dcc_component_v2_builder_t *builder,
    uint32_t min_values
);
DCC_API dcc_status_t dcc_component_v2_builder_set_max_values(
    dcc_component_v2_builder_t *builder,
    uint32_t max_values
);
DCC_API dcc_status_t dcc_component_v2_builder_set_min_length(
    dcc_component_v2_builder_t *builder,
    uint32_t min_length
);
DCC_API dcc_status_t dcc_component_v2_builder_set_max_length(
    dcc_component_v2_builder_t *builder,
    uint32_t max_length
);
DCC_API dcc_status_t dcc_component_v2_builder_set_accent_color(
    dcc_component_v2_builder_t *builder,
    uint32_t accent_color
);
DCC_API dcc_status_t dcc_component_v2_builder_set_disabled(
    dcc_component_v2_builder_t *builder,
    uint8_t disabled
);
DCC_API dcc_status_t dcc_component_v2_builder_set_required(
    dcc_component_v2_builder_t *builder,
    uint8_t required
);
DCC_API dcc_status_t dcc_component_v2_builder_set_divider(
    dcc_component_v2_builder_t *builder,
    uint8_t divider
);
DCC_API dcc_status_t dcc_component_v2_builder_set_spoiler(
    dcc_component_v2_builder_t *builder,
    uint8_t spoiler
);
DCC_API dcc_status_t dcc_component_v2_builder_set_default(
    dcc_component_v2_builder_t *builder,
    uint8_t default_value
);
DCC_API dcc_status_t dcc_component_v2_builder_set_checked(
    dcc_component_v2_builder_t *builder,
    uint8_t checked
);
DCC_API dcc_status_t dcc_component_v2_builder_build_json(
    const dcc_component_v2_builder_t *builder,
    char **out_json
);
DCC_API dcc_status_t dcc_component_v2_builder_build_array_json(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count,
    char **out_json
);
DCC_API void dcc_component_v2_builder_json_free(char *json);

struct dcc_message;

DCC_API const dcc_component_v2_t *dcc_message_component_at(
    const struct dcc_message *message,
    size_t index
);
DCC_API const dcc_component_v2_t *dcc_message_component_root(
    const struct dcc_message *message,
    size_t index
);
DCC_API const dcc_component_v2_t *dcc_message_component_by_custom_id(
    const struct dcc_message *message,
    const char *custom_id
);
DCC_API const dcc_component_v2_t *dcc_component_v2_child(
    const struct dcc_message *message,
    const dcc_component_v2_t *component,
    size_t index
);
DCC_API const dcc_component_v2_t *dcc_component_v2_accessory(
    const struct dcc_message *message,
    const dcc_component_v2_t *component
);
DCC_API const dcc_component_v2_t *dcc_component_v2_labeled_component(
    const struct dcc_message *message,
    const dcc_component_v2_t *label
);

#ifdef __cplusplus
}
#endif

#endif
