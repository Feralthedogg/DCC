#ifndef DCC_COMPONENT_V2_H
#define DCC_COMPONENT_V2_H

#include <dcc/component_types.h>
#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    DCC_COMPONENT_V2_MAX_COMPONENTS = 40,
    DCC_COMPONENT_V2_MAX_ACTION_ROW_CHILDREN = 5,
    DCC_COMPONENT_V2_MAX_SECTION_CHILDREN = 3,
    DCC_COMPONENT_V2_MAX_MEDIA_GALLERY_ITEMS = 10,
    DCC_COMPONENT_V2_MAX_RADIO_OPTIONS = 10,
    DCC_COMPONENT_V2_MAX_CHECKBOX_OPTIONS = 10,
    DCC_COMPONENT_V2_MAX_SELECT_DEFAULT_VALUES = 25,
    DCC_COMPONENT_V2_BUILDER_VERSION_1 = 1,
    DCC_COMPONENT_V2_BUILDER_VERSION = DCC_COMPONENT_V2_BUILDER_VERSION_1,
    DCC_COMPONENT_V2_VALIDATION_ERROR_VERSION_1 = 1,
    DCC_COMPONENT_V2_VALIDATION_ERROR_VERSION =
        DCC_COMPONENT_V2_VALIDATION_ERROR_VERSION_1,
};

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

typedef enum dcc_component_v2_field {
    DCC_COMPONENT_V2_FIELD_ID = 0,
    DCC_COMPONENT_V2_FIELD_COMPONENTS,
    DCC_COMPONENT_V2_FIELD_STYLE,
    DCC_COMPONENT_V2_FIELD_LABEL,
    DCC_COMPONENT_V2_FIELD_EMOJI,
    DCC_COMPONENT_V2_FIELD_CUSTOM_ID,
    DCC_COMPONENT_V2_FIELD_URL,
    DCC_COMPONENT_V2_FIELD_SKU_ID,
    DCC_COMPONENT_V2_FIELD_DISABLED,
    DCC_COMPONENT_V2_FIELD_OPTIONS,
    DCC_COMPONENT_V2_FIELD_PLACEHOLDER,
    DCC_COMPONENT_V2_FIELD_DEFAULT_VALUES,
    DCC_COMPONENT_V2_FIELD_CHANNEL_TYPES,
    DCC_COMPONENT_V2_FIELD_MIN_VALUES,
    DCC_COMPONENT_V2_FIELD_MAX_VALUES,
    DCC_COMPONENT_V2_FIELD_REQUIRED,
    DCC_COMPONENT_V2_FIELD_MIN_LENGTH,
    DCC_COMPONENT_V2_FIELD_MAX_LENGTH,
    DCC_COMPONENT_V2_FIELD_VALUE,
    DCC_COMPONENT_V2_FIELD_ACCESSORY,
    DCC_COMPONENT_V2_FIELD_CONTENT,
    DCC_COMPONENT_V2_FIELD_MEDIA,
    DCC_COMPONENT_V2_FIELD_DESCRIPTION,
    DCC_COMPONENT_V2_FIELD_SPOILER,
    DCC_COMPONENT_V2_FIELD_ITEMS,
    DCC_COMPONENT_V2_FIELD_DIVIDER,
    DCC_COMPONENT_V2_FIELD_SPACING,
    DCC_COMPONENT_V2_FIELD_ACCENT_COLOR,
    DCC_COMPONENT_V2_FIELD_COMPONENT,
    DCC_COMPONENT_V2_FIELD_FILE_TYPES,
    DCC_COMPONENT_V2_FIELD_DEFAULT,
    DCC_COMPONENT_V2_FIELD_COUNT,
} dcc_component_v2_field_t;

static inline uint64_t dcc_component_v2_field_mask(
    dcc_component_v2_field_t field) {
    return field >= 0 && field < DCC_COMPONENT_V2_FIELD_COUNT
               ? UINT64_C(1) << (unsigned)field
               : UINT64_C(0);
}

typedef enum dcc_component_v2_context {
    DCC_COMPONENT_V2_CONTEXT_ANY = 0,
    DCC_COMPONENT_V2_CONTEXT_MESSAGE_LEGACY,
    DCC_COMPONENT_V2_CONTEXT_MESSAGE_V2,
    DCC_COMPONENT_V2_CONTEXT_MODAL,
} dcc_component_v2_context_t;

typedef enum dcc_component_v2_validation_reason {
    DCC_COMPONENT_V2_VALIDATION_NONE = 0,
    DCC_COMPONENT_V2_VALIDATION_ABI_SIZE,
    DCC_COMPONENT_V2_VALIDATION_ABI_VERSION,
    DCC_COMPONENT_V2_VALIDATION_PRESENCE,
    DCC_COMPONENT_V2_VALIDATION_TYPE,
    DCC_COMPONENT_V2_VALIDATION_FIELD_NOT_ALLOWED,
    DCC_COMPONENT_V2_VALIDATION_FIELD_NOT_COVERED,
    DCC_COMPONENT_V2_VALIDATION_REQUIRED_FIELD,
    DCC_COMPONENT_V2_VALIDATION_INVALID_VALUE,
    DCC_COMPONENT_V2_VALIDATION_LIMIT,
    DCC_COMPONENT_V2_VALIDATION_DUPLICATE,
    DCC_COMPONENT_V2_VALIDATION_PLACEMENT,
    DCC_COMPONENT_V2_VALIDATION_OVERFLOW,
} dcc_component_v2_validation_reason_t;

typedef struct dcc_component_v2_validation_error {
    size_t size;
    uint32_t version;
    dcc_status_t status;
    dcc_component_v2_validation_reason_t reason;
    char field_path[512];
} dcc_component_v2_validation_error_t;

#define DCC_COMPONENT_V2_VALIDATION_ERROR_INIT \
    {sizeof(dcc_component_v2_validation_error_t), \
     DCC_COMPONENT_V2_VALIDATION_ERROR_VERSION, DCC_OK, \
     DCC_COMPONENT_V2_VALIDATION_NONE, {0}}

typedef struct dcc_component_v2_media {
    const char *url;
    const char *description;
    uint8_t spoiler;
    uint8_t has_spoiler;
} dcc_component_v2_media_t;

typedef struct dcc_component_v2_unfurled_media {
    const char *url;
} dcc_component_v2_unfurled_media_t;

typedef struct dcc_component_v2_media_gallery_item {
    dcc_component_v2_unfurled_media_t media;
    const char *description;
    uint8_t spoiler;
    uint8_t has_spoiler;
} dcc_component_v2_media_gallery_item_t;

typedef struct dcc_component_v2_choice_option {
    const char *label;
    const char *value;
    const char *description;
    uint8_t is_default;
    uint8_t has_default;
} dcc_component_v2_choice_option_t;

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

typedef struct dcc_component_v2_button_value {
    dcc_button_style_t style;
    const char *label;
    dcc_component_emoji_t emoji;
    uint8_t disabled;
    union { const char *custom_id; const char *url; dcc_snowflake_t sku_id; } target;
} dcc_component_v2_button_value_t;

typedef struct dcc_component_v2_select_value {
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
} dcc_component_v2_select_value_t;

typedef union dcc_component_v2_layout_value {
    struct { const dcc_component_v2_builder_t *components; size_t component_count; } action_row;
    struct { const dcc_component_v2_builder_t *components; size_t component_count; const dcc_component_v2_builder_t *accessory; } section;
    struct { const dcc_component_v2_builder_t *components; size_t component_count; uint32_t accent_color; uint8_t spoiler; } container;
    struct { const char *label; const char *description; const dcc_component_v2_builder_t *component; } label;
} dcc_component_v2_layout_value_t;

typedef union dcc_component_v2_media_value {
    struct { dcc_component_v2_unfurled_media_t media; const char *description; uint8_t spoiler; } thumbnail;
    struct { const dcc_component_v2_media_gallery_item_t *items; size_t item_count; } gallery;
    struct { dcc_component_v2_unfurled_media_t file; uint8_t spoiler; } file;
} dcc_component_v2_media_value_t;

typedef struct dcc_component_v2_text_input_value {
    const char *custom_id; dcc_text_input_style_t style; const char *placeholder;
    uint32_t min_length; uint32_t max_length; const char *value; uint8_t required;
} dcc_component_v2_text_input_value_t;

typedef union dcc_component_v2_modal_value {
    struct { const char *custom_id; uint32_t min_values; uint32_t max_values; uint8_t required; const char *const *file_types; size_t file_type_count; } file_upload;
    struct { const char *custom_id; const dcc_component_v2_choice_option_t *options; size_t option_count; uint8_t required; } radio_group;
    struct { const char *custom_id; const dcc_component_v2_choice_option_t *options; size_t option_count; uint32_t min_values; uint32_t max_values; uint8_t required; } checkbox_group;
    struct { const char *custom_id; uint8_t default_value; } checkbox;
} dcc_component_v2_modal_value_t;

typedef struct dcc_component_v2_text_display_value { const char *content; } dcc_component_v2_text_display_value_t;
typedef struct dcc_component_v2_separator_value { uint8_t divider; dcc_component_v2_separator_spacing_t spacing; } dcc_component_v2_separator_value_t;

struct dcc_component_v2_builder {
    size_t size;
    uint32_t version;
    uint64_t present;
    dcc_component_v2_type_t type;
    uint32_t id;
    union {
        dcc_component_v2_button_value_t button;
        dcc_component_v2_select_value_t select;
        dcc_component_v2_layout_value_t layout;
        dcc_component_v2_media_value_t media;
        dcc_component_v2_text_input_value_t text_input;
        dcc_component_v2_modal_value_t modal;
        dcc_component_v2_text_display_value_t text_display;
        dcc_component_v2_separator_value_t separator;
    } as;
};

#define DCC_COMPONENT_V2_BUILDER_INIT(type_) \
    {sizeof(dcc_component_v2_builder_t), DCC_COMPONENT_V2_BUILDER_VERSION, \
     UINT64_C(0), (type_), 0U, \
     {{(dcc_button_style_t)0, NULL, {NULL, 0U, 0U, 0U, 0U}, 0U, {NULL}}}}

static inline dcc_component_v2_builder_t dcc_component_v2_value(
    dcc_component_v2_type_t type) {
    dcc_component_v2_builder_t value = DCC_COMPONENT_V2_BUILDER_INIT(type);
    return value;
}

static inline dcc_component_v2_builder_t dcc_component_v2_text_display(
    const char *content) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_TEXT_DISPLAY);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_CONTENT);
    v.as.text_display.content = content; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_button(
    dcc_button_style_t style, const char *label, const char *custom_id) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_BUTTON);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_STYLE) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_LABEL) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_CUSTOM_ID);
    v.as.button.style = style; v.as.button.label = label;
    v.as.button.target.custom_id = custom_id; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_link_button(
    const char *label, const char *url) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_BUTTON);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_STYLE) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_LABEL) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_URL);
    v.as.button.style = DCC_BUTTON_LINK; v.as.button.label = label;
    v.as.button.target.url = url; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_premium_button(
    dcc_snowflake_t sku_id) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_BUTTON);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_STYLE) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_SKU_ID);
    v.as.button.style = DCC_BUTTON_PREMIUM;
    v.as.button.target.sku_id = sku_id; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_action_row(
    const dcc_component_v2_builder_t *items, size_t count) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_ACTION_ROW);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_COMPONENTS);
    v.as.layout.action_row.components = items;
    v.as.layout.action_row.component_count = count; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_string_select(
    const char *custom_id, const dcc_select_option_t *options, size_t count) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_STRING_SELECT);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_OPTIONS);
    v.as.select.custom_id = custom_id;
    v.as.select.data.string_select.options = options;
    v.as.select.data.string_select.option_count = count; return v;
}
#define DCC_COMPONENT_V2_SIMPLE_SELECT(fn_, type_)                             \
static inline dcc_component_v2_builder_t fn_(const char *custom_id) {         \
    dcc_component_v2_builder_t v = dcc_component_v2_value(type_);             \
    v.present = dcc_component_v2_field_mask(                                  \
        DCC_COMPONENT_V2_FIELD_CUSTOM_ID);                                    \
    v.as.select.custom_id = custom_id; return v;                              \
}
DCC_COMPONENT_V2_SIMPLE_SELECT(dcc_component_v2_user_select, DCC_COMPONENT_V2_USER_SELECT)
DCC_COMPONENT_V2_SIMPLE_SELECT(dcc_component_v2_role_select, DCC_COMPONENT_V2_ROLE_SELECT)
DCC_COMPONENT_V2_SIMPLE_SELECT(dcc_component_v2_mentionable_select, DCC_COMPONENT_V2_MENTIONABLE_SELECT)
DCC_COMPONENT_V2_SIMPLE_SELECT(dcc_component_v2_channel_select, DCC_COMPONENT_V2_CHANNEL_SELECT)
#undef DCC_COMPONENT_V2_SIMPLE_SELECT
static inline dcc_component_v2_builder_t dcc_component_v2_text_input(
    const char *custom_id, dcc_text_input_style_t style) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_TEXT_INPUT);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_STYLE);
    v.as.text_input.custom_id = custom_id; v.as.text_input.style = style; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_section(
    const dcc_component_v2_builder_t *items, size_t count,
    const dcc_component_v2_builder_t *accessory) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_SECTION);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_COMPONENTS) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_ACCESSORY);
    v.as.layout.section.components = items; v.as.layout.section.component_count = count;
    v.as.layout.section.accessory = accessory; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_thumbnail(
    dcc_component_v2_unfurled_media_t media) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_THUMBNAIL);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_MEDIA);
    v.as.media.thumbnail.media = media; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_media_gallery(
    const dcc_component_v2_media_gallery_item_t *items, size_t count) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_MEDIA_GALLERY);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_ITEMS);
    v.as.media.gallery.items = items; v.as.media.gallery.item_count = count; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_file(
    dcc_component_v2_unfurled_media_t file) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_FILE);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_MEDIA);
    v.as.media.file.file = file; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_separator(
    uint8_t divider, dcc_component_v2_separator_spacing_t spacing) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_SEPARATOR);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_DIVIDER) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_SPACING);
    v.as.separator.divider = divider; v.as.separator.spacing = spacing; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_container(
    const dcc_component_v2_builder_t *items, size_t count) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_CONTAINER);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_COMPONENTS);
    v.as.layout.container.components = items;
    v.as.layout.container.component_count = count; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_label(
    const char *label, const dcc_component_v2_builder_t *component) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_LABEL);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_LABEL) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_COMPONENT);
    v.as.layout.label.label = label; v.as.layout.label.component = component; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_file_upload(
    const char *custom_id) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_FILE_UPLOAD);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_CUSTOM_ID);
    v.as.modal.file_upload.custom_id = custom_id; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_radio_group(
    const char *custom_id, const dcc_component_v2_choice_option_t *options,
    size_t count) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_RADIO_GROUP);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_OPTIONS);
    v.as.modal.radio_group.custom_id = custom_id;
    v.as.modal.radio_group.options = options; v.as.modal.radio_group.option_count = count; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_checkbox_group(
    const char *custom_id, const dcc_component_v2_choice_option_t *options,
    size_t count) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_CHECKBOX_GROUP);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_OPTIONS);
    v.as.modal.checkbox_group.custom_id = custom_id;
    v.as.modal.checkbox_group.options = options; v.as.modal.checkbox_group.option_count = count; return v;
}
static inline dcc_component_v2_builder_t dcc_component_v2_checkbox(
    const char *custom_id, uint8_t default_value) {
    dcc_component_v2_builder_t v = dcc_component_v2_value(DCC_COMPONENT_V2_CHECKBOX);
    v.present = dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_CUSTOM_ID) |
        dcc_component_v2_field_mask(DCC_COMPONENT_V2_FIELD_DEFAULT);
    v.as.modal.checkbox.custom_id = custom_id;
    v.as.modal.checkbox.default_value = default_value; return v;
}

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
DCC_API dcc_status_t dcc_component_v2_builder_set_choice_options(
    dcc_component_v2_builder_t *builder,
    const dcc_component_v2_choice_option_t *options,
    size_t option_count
);
DCC_API dcc_status_t dcc_component_v2_builder_set_file_types(
    dcc_component_v2_builder_t *builder,
    const char *const *file_types,
    size_t file_type_count
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
DCC_API dcc_status_t dcc_component_v2_validate(
    const dcc_component_v2_builder_t *components,
    size_t component_count,
    dcc_component_v2_context_t context,
    dcc_component_v2_validation_error_t *out_error
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
