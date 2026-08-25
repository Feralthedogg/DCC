#ifndef DCC_TEST_COMPONENT_V2_EXPECTED_LAYOUT_H
#define DCC_TEST_COMPONENT_V2_EXPECTED_LAYOUT_H

#include <dcc/component_v2.h>

typedef struct dcc_test_component_v2_builder dcc_test_component_v2_builder_t;

typedef struct dcc_test_component_v2_button_value {
  dcc_button_style_t style;
  const char *label;
  dcc_component_emoji_t emoji;
  uint8_t disabled;
  union { const char *custom_id; const char *url; dcc_snowflake_t sku_id; } target;
} dcc_test_component_v2_button_value_t;

typedef struct dcc_test_component_v2_select_value {
  const char *custom_id;
  const char *placeholder;
  uint32_t min_values;
  uint32_t max_values;
  uint8_t required;
  uint8_t disabled;
  union {
    struct { const dcc_select_option_t *options; size_t option_count; } string_select;
    struct { const dcc_component_v2_select_default_value_t *default_values; size_t default_value_count; } entity_select;
    struct { const dcc_component_v2_select_default_value_t *default_values; size_t default_value_count; const uint32_t *channel_types; size_t channel_type_count; } channel_select;
  } data;
} dcc_test_component_v2_select_value_t;

typedef union dcc_test_component_v2_layout_value {
  struct { const dcc_test_component_v2_builder_t *components; size_t component_count; } action_row;
  struct { const dcc_test_component_v2_builder_t *components; size_t component_count; const dcc_test_component_v2_builder_t *accessory; } section;
  struct { const dcc_test_component_v2_builder_t *components; size_t component_count; uint32_t accent_color; uint8_t spoiler; } container;
  struct { const char *label; const char *description; const dcc_test_component_v2_builder_t *component; } label;
} dcc_test_component_v2_layout_value_t;

typedef union dcc_test_component_v2_media_value {
  struct { dcc_component_v2_unfurled_media_t media; const char *description; uint8_t spoiler; } thumbnail;
  struct { const dcc_component_v2_media_gallery_item_t *items; size_t item_count; } gallery;
  struct { dcc_component_v2_unfurled_media_t file; uint8_t spoiler; } file;
} dcc_test_component_v2_media_value_t;

typedef struct dcc_test_component_v2_text_input_value {
  const char *custom_id;
  dcc_text_input_style_t style;
  const char *placeholder;
  uint32_t min_length;
  uint32_t max_length;
  const char *value;
  uint8_t required;
} dcc_test_component_v2_text_input_value_t;

typedef union dcc_test_component_v2_modal_value {
  struct { const char *custom_id; uint32_t min_values; uint32_t max_values; uint8_t required; const char *const *file_types; size_t file_type_count; } file_upload;
  struct { const char *custom_id; const dcc_component_v2_choice_option_t *options; size_t option_count; uint8_t required; } radio_group;
  struct { const char *custom_id; const dcc_component_v2_choice_option_t *options; size_t option_count; uint32_t min_values; uint32_t max_values; uint8_t required; } checkbox_group;
  struct { const char *custom_id; uint8_t default_value; } checkbox;
} dcc_test_component_v2_modal_value_t;

typedef struct dcc_test_component_v2_text_display_value { const char *content; } dcc_test_component_v2_text_display_value_t;
typedef struct dcc_test_component_v2_separator_value { uint8_t divider; dcc_component_v2_separator_spacing_t spacing; } dcc_test_component_v2_separator_value_t;

struct dcc_test_component_v2_builder {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_component_v2_type_t type;
  uint32_t id;
  union {
    dcc_test_component_v2_button_value_t button;
    dcc_test_component_v2_select_value_t select;
    dcc_test_component_v2_layout_value_t layout;
    dcc_test_component_v2_media_value_t media;
    dcc_test_component_v2_text_input_value_t text_input;
    dcc_test_component_v2_modal_value_t modal;
    dcc_test_component_v2_text_display_value_t text_display;
    dcc_test_component_v2_separator_value_t separator;
  } as;
};

#endif
