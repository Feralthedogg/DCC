#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_LEGACY_COMPONENTS_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_LEGACY_COMPONENTS_H

#include <dcc/sugar/message_component_ui/legacy_options.h>

#define DCC_ACTION_ROW_ARRAY(children_, child_count_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ACTION_ROW, \
        .children = (children_), \
        .children_count = (child_count_) \
    })
#define DCC_ACTION_ROW(...) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ACTION_ROW, \
        .children = DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__) \
    })
#define DCC_BUTTON_BUILDER(style_, label_, custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_BUTTON, \
        .style = (style_), \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .has_style = 1U \
    })
#define DCC_BUTTON_BUILDER_NS(style_, label_, namespace_, action_) \
    DCC_BUTTON_BUILDER((style_), (label_), DCC_COMPONENT_ID(namespace_, action_))
#define DCC_BUTTON_PRIMARY_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_PRIMARY, (label_), (custom_id_))
#define DCC_BUTTON_PRIMARY_BUILDER_NS(label_, namespace_, action_) \
    DCC_BUTTON_BUILDER_NS(DCC_BUTTON_PRIMARY, (label_), namespace_, action_)
#define DCC_BUTTON_SECONDARY_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_SECONDARY, (label_), (custom_id_))
#define DCC_BUTTON_SECONDARY_BUILDER_NS(label_, namespace_, action_) \
    DCC_BUTTON_BUILDER_NS(DCC_BUTTON_SECONDARY, (label_), namespace_, action_)
#define DCC_BUTTON_SUCCESS_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_SUCCESS, (label_), (custom_id_))
#define DCC_BUTTON_SUCCESS_BUILDER_NS(label_, namespace_, action_) \
    DCC_BUTTON_BUILDER_NS(DCC_BUTTON_SUCCESS, (label_), namespace_, action_)
#define DCC_BUTTON_DANGER_BUILDER(label_, custom_id_) \
    DCC_BUTTON_BUILDER(DCC_BUTTON_DANGER, (label_), (custom_id_))
#define DCC_BUTTON_DANGER_BUILDER_NS(label_, namespace_, action_) \
    DCC_BUTTON_BUILDER_NS(DCC_BUTTON_DANGER, (label_), namespace_, action_)
#define DCC_LINK_BUTTON_BUILDER(label_, url_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_BUTTON, \
        .style = DCC_BUTTON_LINK, \
        .label = (label_), \
        .url = (url_), \
        .has_style = 1U \
    })
#define DCC_PREMIUM_BUTTON_BUILDER(sku_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_BUTTON, \
        .style = DCC_BUTTON_PREMIUM, \
        .sku_id = (sku_id_), \
        .has_style = 1U, \
        .has_sku_id = 1U \
    })
#define DCC_STRING_SELECT_BUILDER_ARRAY(custom_id_, options_, option_count_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_STRING_SELECT, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_STRING_SELECT_BUILDER(custom_id_, ...) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_STRING_SELECT, \
        .custom_id = (custom_id_), \
        .options = DCC_SUGAR_ARRAY(dcc_select_option_t, __VA_ARGS__), \
        .options_count = DCC_SUGAR_ARRAY_LEN(dcc_select_option_t, __VA_ARGS__) \
    })
#define DCC_STRING_SELECT_BUILDER_NS(namespace_, action_, ...) \
    DCC_STRING_SELECT_BUILDER(DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)
#define DCC_USER_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_USER_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_USER_SELECT_BUILDER_NS(namespace_, action_) \
    DCC_USER_SELECT_BUILDER(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_ROLE_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ROLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_ROLE_SELECT_BUILDER_NS(namespace_, action_) \
    DCC_ROLE_SELECT_BUILDER(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_MENTIONABLE_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_MENTIONABLE_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_MENTIONABLE_SELECT_BUILDER_NS(namespace_, action_) \
    DCC_MENTIONABLE_SELECT_BUILDER(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_CHANNEL_SELECT_BUILDER(custom_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_CHANNEL_SELECT, \
        .custom_id = (custom_id_) \
    })
#define DCC_CHANNEL_SELECT_BUILDER_NS(namespace_, action_) \
    DCC_CHANNEL_SELECT_BUILDER(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_TEXT_INPUT_BUILDER(custom_id_, label_, style_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .has_text_input_style = 1U \
    })
#define DCC_TEXT_INPUT_BUILDER_NS(namespace_, action_, label_, style_) \
    DCC_TEXT_INPUT_BUILDER(DCC_COMPONENT_ID(namespace_, action_), (label_), (style_))
#define DCC_TEXT_INPUT_REQUIRED(custom_id_, label_, style_, required_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .required = (required_), \
        .has_text_input_style = 1U, \
        .has_required = 1U \
    })
#define DCC_TEXT_INPUT_REQUIRED_NS(namespace_, action_, label_, style_, required_) \
    DCC_TEXT_INPUT_REQUIRED(DCC_COMPONENT_ID(namespace_, action_), (label_), (style_), (required_))
#define DCC_TEXT_INPUT_PLACEHOLDER(custom_id_, label_, style_, placeholder_, required_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .placeholder = (placeholder_), \
        .required = (required_), \
        .has_text_input_style = 1U, \
        .has_required = 1U \
    })
#define DCC_TEXT_INPUT_PLACEHOLDER_NS(namespace_, action_, label_, style_, placeholder_, required_) \
    DCC_TEXT_INPUT_PLACEHOLDER( \
        DCC_COMPONENT_ID(namespace_, action_), \
        (label_), \
        (style_), \
        (placeholder_), \
        (required_) \
    )

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_LEGACY_COMPONENTS_H */
