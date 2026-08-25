#ifndef DCC_BOT_UI_H
#define DCC_BOT_UI_H

#include <dcc/component_v2.h>
#include <dcc/message.h>
#include <dcc/modal.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline dcc_component_v2_builder_t dcc_bot_ui_invalid(
    dcc_component_v2_builder_t value,
    dcc_status_t status
) {
    if (status != DCC_OK) {
        memset(&value, 0, sizeof(value));
    }
    return value;
}

/** Builds borrowed text with no allocation. */
static inline dcc_component_v2_builder_t DCC_UI_TEXT(const char *content) {
    return dcc_component_v2_text_display(content);
}

/** Builds an interactive button with explicit style. */
static inline dcc_component_v2_builder_t DCC_UI_BUTTON(
    dcc_button_style_t style, const char *label, const char *custom_id
) {
    return dcc_component_v2_button(style, label, custom_id);
}

/** Builds a primary button. */
static inline dcc_component_v2_builder_t DCC_UI_PRIMARY(
    const char *label, const char *custom_id
) {
    return DCC_UI_BUTTON(DCC_BUTTON_PRIMARY, label, custom_id);
}

/** Builds a secondary button. */
static inline dcc_component_v2_builder_t DCC_UI_SECONDARY(
    const char *label, const char *custom_id
) {
    return DCC_UI_BUTTON(DCC_BUTTON_SECONDARY, label, custom_id);
}

/** Builds a success button. */
static inline dcc_component_v2_builder_t DCC_UI_SUCCESS(
    const char *label, const char *custom_id
) {
    return DCC_UI_BUTTON(DCC_BUTTON_SUCCESS, label, custom_id);
}

/** Builds a danger button. */
static inline dcc_component_v2_builder_t DCC_UI_DANGER(
    const char *label, const char *custom_id
) {
    return DCC_UI_BUTTON(DCC_BUTTON_DANGER, label, custom_id);
}

/** Builds a link button borrowing its URL. */
static inline dcc_component_v2_builder_t DCC_UI_LINK(
    const char *label, const char *url
) {
    return dcc_component_v2_link_button(label, url);
}

/** Builds a premium SKU button. */
static inline dcc_component_v2_builder_t DCC_UI_PREMIUM(dcc_snowflake_t sku_id) {
    return dcc_component_v2_premium_button(sku_id);
}

/** Builds an action row borrowing an explicit child array. */
static inline dcc_component_v2_builder_t DCC_UI_ROW_ARRAY(
    const dcc_component_v2_builder_t *components, size_t component_count
) {
    return dcc_component_v2_action_row(components, component_count);
}

/** Builds a section borrowing children and one accessory. */
static inline dcc_component_v2_builder_t DCC_UI_SECTION_ARRAY(
    const dcc_component_v2_builder_t *components, size_t component_count,
    const dcc_component_v2_builder_t *accessory
) {
    return dcc_component_v2_section(components, component_count, accessory);
}

/** Builds a card borrowing an explicit child array. */
static inline dcc_component_v2_builder_t DCC_UI_CARD_ARRAY(
    const dcc_component_v2_builder_t *components, size_t component_count
) {
    return dcc_component_v2_container(components, component_count);
}

/** Builds an accented card or an invalid value when the setter rejects. */
static inline dcc_component_v2_builder_t DCC_UI_CARD_ACCENT_ARRAY(
    const dcc_component_v2_builder_t *components, size_t component_count,
    uint32_t accent_color
) {
    dcc_component_v2_builder_t value =
        dcc_component_v2_container(components, component_count);
    dcc_status_t status =
        dcc_component_v2_builder_set_accent_color(&value, accent_color);
    return dcc_bot_ui_invalid(value, status);
}

/** Builds a small visible separator. */
static inline dcc_component_v2_builder_t DCC_UI_SEPARATOR(void) {
    return dcc_component_v2_separator(1U, DCC_COMPONENT_V2_SPACING_SMALL);
}

/** Builds a large visible separator. */
static inline dcc_component_v2_builder_t DCC_UI_SEPARATOR_LARGE(void) {
    return dcc_component_v2_separator(1U, DCC_COMPONENT_V2_SPACING_LARGE);
}

/** Builds one non-spoiler gallery item borrowing its strings. */
static inline dcc_component_v2_media_gallery_item_t DCC_UI_MEDIA(
    const char *url, const char *description
) {
    dcc_component_v2_media_gallery_item_t value = {{url}, description, 0U, 0U};
    return value;
}

/** Builds one spoiler gallery item borrowing its strings. */
static inline dcc_component_v2_media_gallery_item_t DCC_UI_MEDIA_SPOILER(
    const char *url, const char *description
) {
    dcc_component_v2_media_gallery_item_t value = {{url}, description, 1U, 1U};
    return value;
}

/** Builds a thumbnail borrowing its URL. */
static inline dcc_component_v2_builder_t DCC_UI_THUMBNAIL(const char *url) {
    dcc_component_v2_unfurled_media_t media = {url};
    return dcc_component_v2_thumbnail(media);
}

/** Builds an attachment-backed file component. */
static inline dcc_component_v2_builder_t DCC_UI_FILE(
    const char *attachment_url
) {
    dcc_component_v2_unfurled_media_t file = {attachment_url};
    return dcc_component_v2_file(file);
}

/** Builds a media gallery borrowing an explicit item array. */
static inline dcc_component_v2_builder_t DCC_UI_GALLERY_ARRAY(
    const dcc_component_v2_media_gallery_item_t *items, size_t item_count
) {
    return dcc_component_v2_media_gallery(items, item_count);
}

/** Builds a basic string-select option. */
static inline dcc_select_option_t DCC_UI_OPTION(
    const char *label, const char *value
) {
    dcc_select_option_t option = {label, value, NULL, {NULL, 0U, 0U, 0U, 0U},
                                  0U, 0U, 0U};
    return option;
}

/** Builds a described string-select option. */
static inline dcc_select_option_t DCC_UI_OPTION_DESCRIPTION(
    const char *label, const char *value, const char *description
) {
    dcc_select_option_t option = DCC_UI_OPTION(label, value);
    option.description = description;
    return option;
}

/** Builds a default-selected string-select option. */
static inline dcc_select_option_t DCC_UI_OPTION_DEFAULT(
    const char *label, const char *value
) {
    dcc_select_option_t option = DCC_UI_OPTION(label, value);
    option.is_default = 1U;
    option.has_default = 1U;
    return option;
}

/** Builds a described default-selected string-select option. */
static inline dcc_select_option_t DCC_UI_OPTION_DEFAULT_DESCRIPTION(
    const char *label, const char *value, const char *description
) {
    dcc_select_option_t option = DCC_UI_OPTION_DEFAULT(label, value);
    option.description = description;
    return option;
}

/** Builds a user-select default value. */
static inline dcc_component_v2_select_default_value_t DCC_UI_DEFAULT_USER(
    dcc_snowflake_t id
) {
    dcc_component_v2_select_default_value_t value = {
        id, DCC_COMPONENT_V2_SELECT_DEFAULT_USER};
    return value;
}

/** Builds a role-select default value. */
static inline dcc_component_v2_select_default_value_t DCC_UI_DEFAULT_ROLE(
    dcc_snowflake_t id
) {
    dcc_component_v2_select_default_value_t value = {
        id, DCC_COMPONENT_V2_SELECT_DEFAULT_ROLE};
    return value;
}

/** Builds a channel-select default value. */
static inline dcc_component_v2_select_default_value_t DCC_UI_DEFAULT_CHANNEL(
    dcc_snowflake_t id
) {
    dcc_component_v2_select_default_value_t value = {
        id, DCC_COMPONENT_V2_SELECT_DEFAULT_CHANNEL};
    return value;
}

/** Builds a basic modal choice. */
static inline dcc_component_v2_choice_option_t DCC_UI_CHOICE(
    const char *label, const char *value
) {
    dcc_component_v2_choice_option_t option = {label, value, NULL, 0U, 0U};
    return option;
}

/** Builds a described modal choice. */
static inline dcc_component_v2_choice_option_t DCC_UI_CHOICE_DESCRIPTION(
    const char *label, const char *value, const char *description
) {
    dcc_component_v2_choice_option_t option = DCC_UI_CHOICE(label, value);
    option.description = description;
    return option;
}

/** Builds a default-selected modal choice. */
static inline dcc_component_v2_choice_option_t DCC_UI_CHOICE_DEFAULT(
    const char *label, const char *value
) {
    dcc_component_v2_choice_option_t option = DCC_UI_CHOICE(label, value);
    option.is_default = 1U;
    option.has_default = 1U;
    return option;
}

/** Builds a described default-selected modal choice. */
static inline dcc_component_v2_choice_option_t DCC_UI_CHOICE_DEFAULT_DESCRIPTION(
    const char *label, const char *value, const char *description
) {
    dcc_component_v2_choice_option_t option = DCC_UI_CHOICE_DEFAULT(label, value);
    option.description = description;
    return option;
}

/** Builds a String Select borrowing an explicit option array. */
static inline dcc_component_v2_builder_t DCC_UI_STRING_SELECT_ARRAY(
    const char *custom_id, const dcc_select_option_t *options,
    size_t option_count
) {
    return dcc_component_v2_string_select(custom_id, options, option_count);
}

/** Builds a user select. */
static inline dcc_component_v2_builder_t DCC_UI_USER_SELECT(
    const char *custom_id
) {
    return dcc_component_v2_user_select(custom_id);
}

/** Builds a role select. */
static inline dcc_component_v2_builder_t DCC_UI_ROLE_SELECT(
    const char *custom_id
) {
    return dcc_component_v2_role_select(custom_id);
}

/** Builds a mentionable select. */
static inline dcc_component_v2_builder_t DCC_UI_MENTIONABLE_SELECT(
    const char *custom_id
) {
    return dcc_component_v2_mentionable_select(custom_id);
}

/** Builds a channel select borrowing defaults and channel-type arrays. */
static inline dcc_component_v2_builder_t DCC_UI_CHANNEL_SELECT_ARRAY(
    const char *custom_id,
    const dcc_component_v2_select_default_value_t *default_values,
    size_t default_value_count, const uint32_t *channel_types,
    size_t channel_type_count
) {
    dcc_component_v2_builder_t value = dcc_component_v2_channel_select(custom_id);
    dcc_status_t status = dcc_component_v2_builder_set_default_values(
        &value, default_values, default_value_count);
    if (status == DCC_OK) {
        status = dcc_component_v2_builder_set_channel_types(
            &value, channel_types, channel_type_count);
    }
    return dcc_bot_ui_invalid(value, status);
}

/** Builds a modal label borrowing one child. */
static inline dcc_component_v2_builder_t DCC_UI_LABEL(
    const char *label, const dcc_component_v2_builder_t *component
) {
    return dcc_component_v2_label(label, component);
}

/** Builds a text input. */
static inline dcc_component_v2_builder_t DCC_UI_INPUT(
    const char *custom_id, dcc_text_input_style_t style
) {
    return dcc_component_v2_text_input(custom_id, style);
}

/** Builds a text input with a borrowed placeholder. */
static inline dcc_component_v2_builder_t DCC_UI_INPUT_PLACEHOLDER(
    const char *custom_id, dcc_text_input_style_t style,
    const char *placeholder
) {
    dcc_component_v2_builder_t value =
        dcc_component_v2_text_input(custom_id, style);
    dcc_status_t status =
        dcc_component_v2_builder_set_placeholder(&value, placeholder);
    return dcc_bot_ui_invalid(value, status);
}

/** Builds a file upload. */
static inline dcc_component_v2_builder_t DCC_UI_FILE_UPLOAD(
    const char *custom_id
) {
    return dcc_component_v2_file_upload(custom_id);
}

/** Builds a file upload borrowing an allowed-type array. */
static inline dcc_component_v2_builder_t DCC_UI_FILE_UPLOAD_WITH_TYPES(
    const char *custom_id, const char *const *file_types,
    size_t file_type_count
) {
    dcc_component_v2_builder_t value = dcc_component_v2_file_upload(custom_id);
    dcc_status_t status = dcc_component_v2_builder_set_file_types(
        &value, file_types, file_type_count);
    return dcc_bot_ui_invalid(value, status);
}

/** Builds a radio group borrowing an explicit choice array. */
static inline dcc_component_v2_builder_t DCC_UI_RADIO_GROUP_ARRAY(
    const char *custom_id,
    const dcc_component_v2_choice_option_t *options, size_t option_count
) {
    return dcc_component_v2_radio_group(custom_id, options, option_count);
}

/** Builds a checkbox group borrowing an explicit choice array. */
static inline dcc_component_v2_builder_t DCC_UI_CHECKBOX_GROUP_ARRAY(
    const char *custom_id,
    const dcc_component_v2_choice_option_t *options, size_t option_count
) {
    return dcc_component_v2_checkbox_group(custom_id, options, option_count);
}

/** Builds one checkbox. */
static inline dcc_component_v2_builder_t DCC_UI_CHECKBOX(
    const char *custom_id, uint8_t default_value
) {
    return dcc_component_v2_checkbox(custom_id, default_value);
}

/** Builds a modal borrowing an explicit tagged component array. */
static inline dcc_modal_builder_t DCC_UI_MODAL_ARRAY(
    const char *custom_id, const char *title,
    const dcc_component_v2_builder_t *components, size_t component_count
) {
    dcc_modal_builder_t modal;
    dcc_modal_builder_init(&modal);
    dcc_status_t status = dcc_modal_builder_set_custom_id(&modal, custom_id);
    if (status == DCC_OK) {
        status = dcc_modal_builder_set_title(&modal, title);
    }
    if (status == DCC_OK) {
        status = dcc_modal_builder_set_components_v2(
            &modal, components, component_count);
    }
    if (status != DCC_OK) {
        memset(&modal, 0, sizeof(modal));
    }
    return modal;
}

#if !defined(__cplusplus)
/** Builds a row from one or more block-lifetime component values. */
#define DCC_UI_ROW(...) \
    DCC_UI_ROW_ARRAY((dcc_component_v2_builder_t[]){__VA_ARGS__}, \
        sizeof((dcc_component_v2_builder_t[]){__VA_ARGS__}) / \
            sizeof(dcc_component_v2_builder_t))
/** Builds a section from one accessory and one or more children. */
#define DCC_UI_SECTION(accessory_, ...) \
    DCC_UI_SECTION_ARRAY((dcc_component_v2_builder_t[]){__VA_ARGS__}, \
        sizeof((dcc_component_v2_builder_t[]){__VA_ARGS__}) / \
            sizeof(dcc_component_v2_builder_t), \
        &((dcc_component_v2_builder_t[]){(accessory_)})[0])
/** Builds a card from one or more block-lifetime children. */
#define DCC_UI_CARD(...) \
    DCC_UI_CARD_ARRAY((dcc_component_v2_builder_t[]){__VA_ARGS__}, \
        sizeof((dcc_component_v2_builder_t[]){__VA_ARGS__}) / \
            sizeof(dcc_component_v2_builder_t))
/** Builds an accented card from one or more children. */
#define DCC_UI_CARD_ACCENT(accent_color_, ...) \
    DCC_UI_CARD_ACCENT_ARRAY((dcc_component_v2_builder_t[]){__VA_ARGS__}, \
        sizeof((dcc_component_v2_builder_t[]){__VA_ARGS__}) / \
            sizeof(dcc_component_v2_builder_t), (accent_color_))
/** Builds a gallery from one or more block-lifetime media items. */
#define DCC_UI_GALLERY(...) \
    DCC_UI_GALLERY_ARRAY((dcc_component_v2_media_gallery_item_t[]){__VA_ARGS__}, \
        sizeof((dcc_component_v2_media_gallery_item_t[]){__VA_ARGS__}) / \
            sizeof(dcc_component_v2_media_gallery_item_t))
/** Builds a String Select from one or more block-lifetime options. */
#define DCC_UI_STRING_SELECT(custom_id_, ...) \
    DCC_UI_STRING_SELECT_ARRAY((custom_id_), \
        (dcc_select_option_t[]){__VA_ARGS__}, \
        sizeof((dcc_select_option_t[]){__VA_ARGS__}) / \
            sizeof(dcc_select_option_t))
/** Builds a radio group from one or more block-lifetime choices. */
#define DCC_UI_RADIO_GROUP(custom_id_, ...) \
    DCC_UI_RADIO_GROUP_ARRAY((custom_id_), \
        (dcc_component_v2_choice_option_t[]){__VA_ARGS__}, \
        sizeof((dcc_component_v2_choice_option_t[]){__VA_ARGS__}) / \
            sizeof(dcc_component_v2_choice_option_t))
/** Builds a checkbox group from one or more block-lifetime choices. */
#define DCC_UI_CHECKBOX_GROUP(custom_id_, ...) \
    DCC_UI_CHECKBOX_GROUP_ARRAY((custom_id_), \
        (dcc_component_v2_choice_option_t[]){__VA_ARGS__}, \
        sizeof((dcc_component_v2_choice_option_t[]){__VA_ARGS__}) / \
            sizeof(dcc_component_v2_choice_option_t))
/** Builds a modal from one or more block-lifetime tagged children. */
#define DCC_UI_MODAL(custom_id_, title_, ...) \
    DCC_UI_MODAL_ARRAY((custom_id_), (title_), \
        (dcc_component_v2_builder_t[]){__VA_ARGS__}, \
        sizeof((dcc_component_v2_builder_t[]){__VA_ARGS__}) / \
            sizeof(dcc_component_v2_builder_t))
#endif

#ifdef __cplusplus
}
#endif

#endif
