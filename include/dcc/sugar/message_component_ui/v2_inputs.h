#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_INPUTS_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_INPUTS_H

#include <dcc/sugar/message_component_ui/v2_selects.h>

#define DCC_V2_TEXT_INPUT(custom_id_, label_, style_) \
    ((dcc_component_v2_builder_t){ \
        .size = sizeof(dcc_component_v2_builder_t), .version = DCC_COMPONENT_V2_BUILDER_VERSION, \
        .present = (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_CUSTOM_ID) | (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_STYLE), \
        .type = DCC_COMPONENT_V2_TEXT_INPUT, \
        .as.text_input = {(custom_id_), (style_), NULL, 0U, 0U, NULL, 0U} \
    })
#define DCC_V2_TEXT_INPUT_NS(namespace_, action_, label_, style_) \
    DCC_V2_TEXT_INPUT(DCC_COMPONENT_ID(namespace_, action_), (label_), (style_))
#define DCC_V2_TEXT_INPUT_REQUIRED(custom_id_, label_, style_, required_) \
    ((dcc_component_v2_builder_t){ \
        .size = sizeof(dcc_component_v2_builder_t), .version = DCC_COMPONENT_V2_BUILDER_VERSION, \
        .present = (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_CUSTOM_ID) | (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_STYLE) | (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_REQUIRED), \
        .type = DCC_COMPONENT_V2_TEXT_INPUT, \
        .as.text_input = {(custom_id_), (style_), NULL, 0U, 0U, NULL, (required_)} \
    })
#define DCC_V2_TEXT_INPUT_REQUIRED_NS(namespace_, action_, label_, style_, required_) \
    DCC_V2_TEXT_INPUT_REQUIRED(DCC_COMPONENT_ID(namespace_, action_), (label_), (style_), (required_))
#define DCC_V2_TEXT_INPUT_PLACEHOLDER(custom_id_, label_, style_, placeholder_, required_) \
    ((dcc_component_v2_builder_t){ \
        .size = sizeof(dcc_component_v2_builder_t), .version = DCC_COMPONENT_V2_BUILDER_VERSION, \
        .present = (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_CUSTOM_ID) | (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_STYLE) | (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_PLACEHOLDER) | (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_REQUIRED), \
        .type = DCC_COMPONENT_V2_TEXT_INPUT, \
        .as.text_input = {(custom_id_), (style_), (placeholder_), 0U, 0U, NULL, (required_)} \
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
    dcc_component_v2_label((label_), (component_))
#define DCC_V2_LABEL(label_, component_) \
    dcc_component_v2_label((label_), DCC_SUGAR_PTR(dcc_component_v2_builder_t, component_))
#define DCC_V2_FILE_UPLOAD(custom_id_) \
    dcc_component_v2_file_upload((custom_id_))
#define DCC_V2_FILE_UPLOAD_NS(namespace_, action_) \
    DCC_V2_FILE_UPLOAD(DCC_COMPONENT_ID(namespace_, action_))
#define DCC_V2_RADIO_GROUP_ARRAY(custom_id_, options_, option_count_) \
    dcc_component_v2_radio_group((custom_id_), (const dcc_component_v2_choice_option_t *)(options_), (option_count_))
#define DCC_V2_RADIO_GROUP(custom_id_, ...) \
    ((dcc_component_v2_builder_t){ \
        .size = sizeof(dcc_component_v2_builder_t), .version = DCC_COMPONENT_V2_BUILDER_VERSION, \
        .present = (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_CUSTOM_ID) | (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_OPTIONS), \
        .type = DCC_COMPONENT_V2_RADIO_GROUP, \
        .as.modal.radio_group = {(custom_id_), (const dcc_component_v2_choice_option_t *)(const void *)DCC_SUGAR_ARRAY(dcc_select_option_t, __VA_ARGS__), DCC_SUGAR_ARRAY_LEN(dcc_select_option_t, __VA_ARGS__), 0U} \
    })
#define DCC_V2_RADIO_GROUP_NS(namespace_, action_, ...) \
    DCC_V2_RADIO_GROUP(DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)
#define DCC_V2_CHECKBOX_GROUP_ARRAY(custom_id_, options_, option_count_) \
    dcc_component_v2_checkbox_group((custom_id_), (const dcc_component_v2_choice_option_t *)(options_), (option_count_))
#define DCC_V2_CHECKBOX_GROUP(custom_id_, ...) \
    ((dcc_component_v2_builder_t){ \
        .size = sizeof(dcc_component_v2_builder_t), .version = DCC_COMPONENT_V2_BUILDER_VERSION, \
        .present = (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_CUSTOM_ID) | (UINT64_C(1) << DCC_COMPONENT_V2_FIELD_OPTIONS), \
        .type = DCC_COMPONENT_V2_CHECKBOX_GROUP, \
        .as.modal.checkbox_group = {(custom_id_), (const dcc_component_v2_choice_option_t *)(const void *)DCC_SUGAR_ARRAY(dcc_select_option_t, __VA_ARGS__), DCC_SUGAR_ARRAY_LEN(dcc_select_option_t, __VA_ARGS__), 0U, 0U, 0U} \
    })
#define DCC_V2_CHECKBOX_GROUP_NS(namespace_, action_, ...) \
    DCC_V2_CHECKBOX_GROUP(DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)
#define DCC_V2_CHECKBOX(custom_id_, label_, checked_) \
    dcc_component_v2_checkbox((custom_id_), (checked_))
#define DCC_V2_CHECKBOX_NS(namespace_, action_, label_, checked_) \
    DCC_V2_CHECKBOX(DCC_COMPONENT_ID(namespace_, action_), (label_), (checked_))

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_V2_INPUTS_H */
