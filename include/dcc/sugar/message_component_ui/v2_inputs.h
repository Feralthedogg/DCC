#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_INPUTS_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_INPUTS_H

#include <dcc/sugar/message_component_ui/v2_selects.h>

#define DCC_V2_TEXT_INPUT(custom_id_, label_, style_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .has_text_input_style = 1U \
    })
#define DCC_V2_TEXT_INPUT_NS(namespace_, action_, label_, style_) \
    DCC_V2_TEXT_INPUT(DCC_COMPONENT_ID(namespace_, action_), (label_), (style_))
#define DCC_V2_TEXT_INPUT_REQUIRED(custom_id_, label_, style_, required_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .required = (required_), \
        .has_text_input_style = 1U, \
        .has_required = 1U \
    })
#define DCC_V2_TEXT_INPUT_REQUIRED_NS(namespace_, action_, label_, style_, required_) \
    DCC_V2_TEXT_INPUT_REQUIRED(DCC_COMPONENT_ID(namespace_, action_), (label_), (style_), (required_))
#define DCC_V2_TEXT_INPUT_PLACEHOLDER(custom_id_, label_, style_, placeholder_, required_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .placeholder = (placeholder_), \
        .required = (required_), \
        .has_text_input_style = 1U, \
        .has_required = 1U \
    })
#define DCC_V2_TEXT_INPUT_PLACEHOLDER_NS(namespace_, action_, label_, style_, placeholder_, required_) \
    DCC_V2_TEXT_INPUT_PLACEHOLDER( \
        DCC_COMPONENT_ID(namespace_, action_), \
        (label_), \
        (style_), \
        (placeholder_), \
        (required_) \
    )
#define DCC_V2_LABEL_ARRAY(label_, component_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_LABEL, \
        .label = (label_), \
        .children = (component_), \
        .children_count = 1U \
    })
#define DCC_V2_LABEL(label_, component_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_LABEL, \
        .label = (label_), \
        .children = DCC_SUGAR_PTR(dcc_component_v2_builder_t, component_), \
        .children_count = 1U \
    })
#define DCC_V2_FILE_UPLOAD(custom_id_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_FILE_UPLOAD, \
        .custom_id = (custom_id_) \
    })
#define DCC_V2_FILE_UPLOAD_NS(namespace_, action_) \
    DCC_V2_FILE_UPLOAD(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_V2_RADIO_GROUP_ARRAY(custom_id_, options_, option_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_RADIO_GROUP, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_V2_RADIO_GROUP(custom_id_, ...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_RADIO_GROUP, \
        .custom_id = (custom_id_), \
        .options = DCC_SUGAR_ARRAY(dcc_select_option_t, __VA_ARGS__), \
        .options_count = DCC_SUGAR_ARRAY_LEN(dcc_select_option_t, __VA_ARGS__) \
    })
#define DCC_V2_RADIO_GROUP_NS(namespace_, action_, ...) \
    DCC_V2_RADIO_GROUP(DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)
#define DCC_V2_CHECKBOX_GROUP_ARRAY(custom_id_, options_, option_count_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHECKBOX_GROUP, \
        .custom_id = (custom_id_), \
        .options = (options_), \
        .options_count = (option_count_) \
    })
#define DCC_V2_CHECKBOX_GROUP(custom_id_, ...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHECKBOX_GROUP, \
        .custom_id = (custom_id_), \
        .options = DCC_SUGAR_ARRAY(dcc_select_option_t, __VA_ARGS__), \
        .options_count = DCC_SUGAR_ARRAY_LEN(dcc_select_option_t, __VA_ARGS__) \
    })
#define DCC_V2_CHECKBOX_GROUP_NS(namespace_, action_, ...) \
    DCC_V2_CHECKBOX_GROUP(DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)
#define DCC_V2_CHECKBOX(custom_id_, label_, checked_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CHECKBOX, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .checked = (checked_), \
        .has_checked = 1U \
    })
#define DCC_V2_CHECKBOX_NS(namespace_, action_, label_, checked_) \
    DCC_V2_CHECKBOX(DCC_COMPONENT_ID(namespace_, action_), (label_), (checked_))

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_INPUTS_H */
