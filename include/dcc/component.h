#ifndef DCC_COMPONENT_H
#define DCC_COMPONENT_H

#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_component_type {
    DCC_COMPONENT_ACTION_ROW = 1,
    DCC_COMPONENT_BUTTON = 2,
    DCC_COMPONENT_STRING_SELECT = 3,
    DCC_COMPONENT_TEXT_INPUT = 4,
    DCC_COMPONENT_USER_SELECT = 5,
    DCC_COMPONENT_ROLE_SELECT = 6,
    DCC_COMPONENT_MENTIONABLE_SELECT = 7,
    DCC_COMPONENT_CHANNEL_SELECT = 8,
} dcc_component_type_t;

typedef enum dcc_button_style {
    DCC_BUTTON_PRIMARY = 1,
    DCC_BUTTON_SECONDARY = 2,
    DCC_BUTTON_SUCCESS = 3,
    DCC_BUTTON_DANGER = 4,
    DCC_BUTTON_LINK = 5,
    DCC_BUTTON_PREMIUM = 6,
} dcc_button_style_t;

typedef enum dcc_text_input_style {
    DCC_TEXT_INPUT_SHORT = 1,
    DCC_TEXT_INPUT_PARAGRAPH = 2,
} dcc_text_input_style_t;

typedef struct dcc_component_emoji {
    const char *name;
    dcc_snowflake_t id;
    uint8_t animated;
    uint8_t has_id;
    uint8_t has_animated;
} dcc_component_emoji_t;

typedef struct dcc_select_option {
    const char *label;
    const char *value;
    const char *description;
    dcc_component_emoji_t emoji;
    uint8_t is_default;
    uint8_t has_default;
    uint8_t has_emoji;
} dcc_select_option_t;

typedef struct dcc_component_builder dcc_component_builder_t;

struct dcc_component_builder {
    dcc_component_type_t type;
    dcc_button_style_t style;
    dcc_text_input_style_t text_input_style;
    const char *custom_id;
    const char *label;
    const char *url;
    dcc_snowflake_t sku_id;
    dcc_component_emoji_t emoji;
    const char *placeholder;
    const dcc_select_option_t *options;
    size_t options_count;
    uint32_t min_values;
    uint32_t max_values;
    uint32_t min_length;
    uint32_t max_length;
    const char *value;
    const dcc_component_builder_t *children;
    size_t children_count;
    uint8_t disabled;
    uint8_t required;
    uint8_t has_style;
    uint8_t has_text_input_style;
    uint8_t has_sku_id;
    uint8_t has_emoji;
    uint8_t has_min_values;
    uint8_t has_max_values;
    uint8_t has_min_length;
    uint8_t has_max_length;
    uint8_t has_disabled;
    uint8_t has_required;
};

DCC_API void dcc_component_builder_init(dcc_component_builder_t *builder, dcc_component_type_t type);
DCC_API dcc_status_t dcc_component_builder_set_style(
    dcc_component_builder_t *builder,
    dcc_button_style_t style
);
DCC_API dcc_status_t dcc_component_builder_set_text_input_style(
    dcc_component_builder_t *builder,
    dcc_text_input_style_t style
);
DCC_API dcc_status_t dcc_component_builder_set_custom_id(dcc_component_builder_t *builder, const char *custom_id);
DCC_API dcc_status_t dcc_component_builder_set_label(dcc_component_builder_t *builder, const char *label);
DCC_API dcc_status_t dcc_component_builder_set_url(dcc_component_builder_t *builder, const char *url);
DCC_API dcc_status_t dcc_component_builder_set_sku_id(dcc_component_builder_t *builder, dcc_snowflake_t sku_id);
DCC_API dcc_status_t dcc_component_builder_set_emoji(
    dcc_component_builder_t *builder,
    const dcc_component_emoji_t *emoji
);
DCC_API dcc_status_t dcc_component_builder_set_placeholder(
    dcc_component_builder_t *builder,
    const char *placeholder
);
DCC_API dcc_status_t dcc_component_builder_set_options(
    dcc_component_builder_t *builder,
    const dcc_select_option_t *options,
    size_t options_count
);
DCC_API dcc_status_t dcc_component_builder_set_min_values(dcc_component_builder_t *builder, uint32_t min_values);
DCC_API dcc_status_t dcc_component_builder_set_max_values(dcc_component_builder_t *builder, uint32_t max_values);
DCC_API dcc_status_t dcc_component_builder_set_min_length(dcc_component_builder_t *builder, uint32_t min_length);
DCC_API dcc_status_t dcc_component_builder_set_max_length(dcc_component_builder_t *builder, uint32_t max_length);
DCC_API dcc_status_t dcc_component_builder_set_value(dcc_component_builder_t *builder, const char *value);
DCC_API dcc_status_t dcc_component_builder_set_children(
    dcc_component_builder_t *builder,
    const dcc_component_builder_t *children,
    size_t children_count
);
DCC_API dcc_status_t dcc_component_builder_set_disabled(dcc_component_builder_t *builder, uint8_t disabled);
DCC_API dcc_status_t dcc_component_builder_set_required(dcc_component_builder_t *builder, uint8_t required);
DCC_API dcc_status_t dcc_component_builder_build_json(const dcc_component_builder_t *builder, char **out_json);
DCC_API dcc_status_t dcc_component_builder_build_array_json(
    const dcc_component_builder_t *builders,
    size_t builder_count,
    char **out_json
);
DCC_API void dcc_component_builder_json_free(char *json);

#ifdef __cplusplus
}
#endif

#endif
