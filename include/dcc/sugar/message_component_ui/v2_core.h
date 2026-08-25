#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_CORE_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_CORE_H

#include <dcc/sugar/message_component_ui/legacy_options.h>

#define DCC_V2_TEXT(content_) dcc_component_v2_text_display((content_))
#define DCC_V2_BUTTON(style_, label_, custom_id_) \
    dcc_component_v2_button((style_), (label_), (custom_id_))
#define DCC_V2_BUTTON_NS(style_, label_, namespace_, action_) \
    DCC_V2_BUTTON((style_), (label_), DCC_COMPONENT_ID(namespace_, action_))
#define DCC_V2_BUTTON_PRIMARY(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_PRIMARY, (label_), (custom_id_))
#define DCC_V2_BUTTON_PRIMARY_NS(label_, namespace_, action_) \
    DCC_V2_BUTTON_NS(DCC_BUTTON_PRIMARY, (label_), namespace_, action_)
#define DCC_V2_BUTTON_SECONDARY(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_SECONDARY, (label_), (custom_id_))
#define DCC_V2_BUTTON_SECONDARY_NS(label_, namespace_, action_) \
    DCC_V2_BUTTON_NS(DCC_BUTTON_SECONDARY, (label_), namespace_, action_)
#define DCC_V2_BUTTON_SUCCESS(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_SUCCESS, (label_), (custom_id_))
#define DCC_V2_BUTTON_SUCCESS_NS(label_, namespace_, action_) \
    DCC_V2_BUTTON_NS(DCC_BUTTON_SUCCESS, (label_), namespace_, action_)
#define DCC_V2_BUTTON_DANGER(label_, custom_id_) \
    DCC_V2_BUTTON(DCC_BUTTON_DANGER, (label_), (custom_id_))
#define DCC_V2_BUTTON_DANGER_NS(label_, namespace_, action_) \
    DCC_V2_BUTTON_NS(DCC_BUTTON_DANGER, (label_), namespace_, action_)
#define DCC_V2_LINK_BUTTON(label_, url_) \
    dcc_component_v2_link_button((label_), (url_))
#define DCC_V2_PREMIUM_BUTTON(sku_id_) dcc_component_v2_premium_button((sku_id_))
#define DCC_V2_ACTION_ROW_ARRAY(children_, child_count_) \
    dcc_component_v2_action_row((children_), (child_count_))
#define DCC_V2_ACTION_ROW(...) \
    dcc_component_v2_action_row( \
        DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__))
#define DCC_V2_SECTION_ARRAY(children_, child_count_, accessory_) \
    dcc_component_v2_section((children_), (child_count_), (accessory_))
#define DCC_V2_SECTION(accessory_, ...) \
    dcc_component_v2_section( \
        DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        DCC_SUGAR_PTR(dcc_component_v2_builder_t, accessory_))
#define DCC_V2_CONTAINER_ARRAY(children_, child_count_) \
    dcc_component_v2_container((children_), (child_count_))
#define DCC_V2_CONTAINER(...) \
    dcc_component_v2_container( \
        DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__))
#define DCC_V2_CONTAINER_ACCENT_ARRAY(children_, child_count_, accent_color_) \
    ((dcc_component_v2_builder_t){ \
        .size = sizeof(dcc_component_v2_builder_t), \
        .version = DCC_COMPONENT_V2_BUILDER_VERSION, \
        .present = (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_COMPONENTS) | (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_ACCENT_COLOR), \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .as.layout.container = {(children_), (child_count_), (accent_color_), 0U} \
    })
#define DCC_V2_CONTAINER_ACCENT(accent_color_, ...) \
    ((dcc_component_v2_builder_t){ \
        .size = sizeof(dcc_component_v2_builder_t), \
        .version = DCC_COMPONENT_V2_BUILDER_VERSION, \
        .present = (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_COMPONENTS) | (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_ACCENT_COLOR), \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .as.layout.container = {DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), (accent_color_), 0U} \
    })
#define DCC_V2_SEPARATOR(divider_, spacing_) \
    dcc_component_v2_separator((divider_), (spacing_))
#define DCC_V2_SEPARATOR_SMALL() \
    DCC_V2_SEPARATOR(1U, DCC_COMPONENT_V2_SPACING_SMALL)
#define DCC_V2_SEPARATOR_LARGE() \
    DCC_V2_SEPARATOR(1U, DCC_COMPONENT_V2_SPACING_LARGE)

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_CORE_H */
