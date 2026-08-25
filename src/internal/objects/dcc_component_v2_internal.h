#ifndef DCC_COMPONENT_V2_INTERNAL_H
#define DCC_COMPONENT_V2_INTERNAL_H

#include "internal/objects/dcc_component_json_members_internal.h"

#include <dcc/component_v2.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_component_v2_validation {
    const char *custom_ids[DCC_COMPONENT_V2_MAX_COMPONENTS];
    uint32_t ids[DCC_COMPONENT_V2_MAX_COMPONENTS];
    size_t custom_id_count;
    size_t id_count;
    size_t total_count;
} dcc_component_v2_validation_t;

typedef struct dcc_component_v2_normalized_view {
    dcc_component_v2_type_t type;
    uint32_t id;
    dcc_button_style_t button_style;
    dcc_text_input_style_t text_input_style;
    dcc_component_v2_separator_spacing_t spacing;
    const char *content, *custom_id, *label, *description, *placeholder;
    const char *url, *value;
    dcc_snowflake_t sku_id;
    dcc_component_emoji_t emoji;
    const dcc_select_option_t *options;
    size_t options_count;
    const dcc_component_v2_choice_option_t *choice_options;
    const dcc_component_v2_select_default_value_t *default_values;
    size_t default_value_count;
    const uint32_t *channel_types;
    size_t channel_type_count;
    const dcc_component_v2_media_t *media;
    size_t media_count;
    dcc_component_v2_media_t media_one;
    const dcc_component_v2_builder_t *children;
    size_t children_count;
    const dcc_component_v2_builder_t *accessory;
    const char *const *file_types;
    size_t file_type_count;
    uint32_t min_values, max_values, min_length, max_length, accent_color;
    uint8_t disabled, required, divider, spoiler, checked;
    uint8_t has_id, has_button_style, has_text_input_style, has_spacing;
    uint8_t has_sku_id, has_emoji, has_min_values, has_max_values;
    uint8_t has_min_length, has_max_length, has_accent_color;
    uint8_t has_disabled, has_required, has_divider, has_spoiler, has_checked;
} dcc_component_v2_normalized_view_t;

void dcc_component_v2_normalize_view(
    const dcc_component_v2_builder_t *builder,
    dcc_component_v2_normalized_view_t *out);
size_t dcc_component_v2_field_end_internal(
    dcc_component_v2_type_t type,
    dcc_component_v2_field_t field);

dcc_status_t dcc_component_v2_validate_array(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count
);
dcc_status_t dcc_component_v2_validate_array_context(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count,
    dcc_component_v2_context_t context
);
dcc_status_t dcc_component_v2_append_json(
    const dcc_component_v2_builder_t *builder,
    dcc_component_json_buffer_t *buffer
);
dcc_status_t dcc_component_v2_append_array_json(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count,
    dcc_component_json_buffer_t *buffer
);
dcc_status_t dcc_component_v2_builder_measure_array_json(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count,
    size_t *out_json_len
);

#ifdef __cplusplus
}
#endif

#endif
