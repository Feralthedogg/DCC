#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_CORE_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_CORE_H

#include <dcc/sugar/message_component_ui/legacy_options.h>

#define DCC_V2_TEXT(content_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_TEXT_DISPLAY, \
        .content = (content_) \
    })
#define DCC_V2_BUTTON(style_, label_, custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_BUTTON, \
        .button_style = (style_), \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .has_button_style = 1U \
    })
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
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_BUTTON, \
        .button_style = DCC_BUTTON_LINK, \
        .label = (label_), \
        .url = (url_), \
        .has_button_style = 1U \
    })
#define DCC_V2_PREMIUM_BUTTON(sku_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_BUTTON, \
        .button_style = DCC_BUTTON_PREMIUM, \
        .sku_id = (sku_id_), \
        .has_button_style = 1U, \
        .has_sku_id = 1U \
    })
#define DCC_V2_ACTION_ROW_ARRAY(children_, child_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_ACTION_ROW, \
        .children = (children_), \
        .children_count = (child_count_) \
    })
#define DCC_V2_ACTION_ROW(...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_ACTION_ROW, \
        .children = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__) \
    })
#define DCC_V2_SECTION_ARRAY(children_, child_count_, accessory_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_SECTION, \
        .children = (children_), \
        .children_count = (child_count_), \
        .accessory = (accessory_) \
    })
#define DCC_V2_SECTION(accessory_, ...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_SECTION, \
        .children = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .accessory = DCC_SUGAR_PTR(dcc_component_v2_builder_t, accessory_) \
    })
#define DCC_V2_CONTAINER_ARRAY(children_, child_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .children = (children_), \
        .children_count = (child_count_) \
    })
#define DCC_V2_CONTAINER(...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .children = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__) \
    })
#define DCC_V2_CONTAINER_ACCENT_ARRAY(children_, child_count_, accent_color_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .children = (children_), \
        .children_count = (child_count_), \
        .accent_color = (accent_color_), \
        .has_accent_color = 1U \
    })
#define DCC_V2_CONTAINER_ACCENT(accent_color_, ...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .children = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .accent_color = (accent_color_), \
        .has_accent_color = 1U \
    })
#define DCC_V2_SEPARATOR(divider_, spacing_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_SEPARATOR, \
        .divider = (divider_), \
        .spacing = (spacing_), \
        .has_divider = 1U, \
        .has_spacing = 1U \
    })
#define DCC_V2_SEPARATOR_SMALL() \
    DCC_V2_SEPARATOR(1U, DCC_COMPONENT_V2_SPACING_SMALL)
#define DCC_V2_SEPARATOR_LARGE() \
    DCC_V2_SEPARATOR(1U, DCC_COMPONENT_V2_SPACING_LARGE)

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_CORE_H */
