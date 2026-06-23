#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_UI_ALIASES_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_UI_ALIASES_H

#include <dcc/sugar/message_component_ui/v2_inputs.h>

#define DCC_UI_TEXT(content_) DCC_V2_TEXT((content_))
#define DCC_UI_ROW(...) DCC_V2_ACTION_ROW(__VA_ARGS__)
#define DCC_UI_BUTTON(style_, label_, custom_id_) DCC_V2_BUTTON((style_), (label_), (custom_id_))
#define DCC_UI_BUTTON_NS(style_, label_, namespace_, action_) \
    DCC_V2_BUTTON_NS((style_), (label_), namespace_, action_)
#define DCC_UI_PRIMARY(label_, custom_id_) DCC_V2_BUTTON_PRIMARY((label_), (custom_id_))
#define DCC_UI_PRIMARY_NS(label_, namespace_, action_) DCC_V2_BUTTON_PRIMARY_NS((label_), namespace_, action_)
#define DCC_UI_SECONDARY(label_, custom_id_) DCC_V2_BUTTON_SECONDARY((label_), (custom_id_))
#define DCC_UI_SECONDARY_NS(label_, namespace_, action_) DCC_V2_BUTTON_SECONDARY_NS((label_), namespace_, action_)
#define DCC_UI_SUCCESS(label_, custom_id_) DCC_V2_BUTTON_SUCCESS((label_), (custom_id_))
#define DCC_UI_SUCCESS_NS(label_, namespace_, action_) DCC_V2_BUTTON_SUCCESS_NS((label_), namespace_, action_)
#define DCC_UI_DANGER(label_, custom_id_) DCC_V2_BUTTON_DANGER((label_), (custom_id_))
#define DCC_UI_DANGER_NS(label_, namespace_, action_) DCC_V2_BUTTON_DANGER_NS((label_), namespace_, action_)
#define DCC_UI_LINK(label_, url_) DCC_V2_LINK_BUTTON((label_), (url_))
#define DCC_UI_PREMIUM(sku_id_) DCC_V2_PREMIUM_BUTTON((sku_id_))
#define DCC_UI_SECTION(accessory_, ...) DCC_V2_SECTION((accessory_), __VA_ARGS__)
#define DCC_UI_CARD(...) DCC_V2_CONTAINER(__VA_ARGS__)
#define DCC_UI_CARD_ACCENT(accent_color_, ...) DCC_V2_CONTAINER_ACCENT((accent_color_), __VA_ARGS__)
#define DCC_UI_SEPARATOR() DCC_V2_SEPARATOR_SMALL()
#define DCC_UI_SEPARATOR_LARGE() DCC_V2_SEPARATOR_LARGE()
#define DCC_UI_MEDIA(url_, description_) DCC_V2_MEDIA((url_), (description_))
#define DCC_UI_GALLERY(...) DCC_V2_MEDIA_GALLERY(__VA_ARGS__)
#define DCC_UI_STRING_SELECT(custom_id_, ...) DCC_V2_STRING_SELECT((custom_id_), __VA_ARGS__)
#define DCC_UI_STRING_SELECT_NS(namespace_, action_, ...) \
    DCC_V2_STRING_SELECT_NS(namespace_, action_, __VA_ARGS__)
#define DCC_UI_USER_SELECT(custom_id_) DCC_V2_USER_SELECT((custom_id_))
#define DCC_UI_USER_SELECT_NS(namespace_, action_) DCC_V2_USER_SELECT_NS(namespace_, action_)
#define DCC_UI_ROLE_SELECT(custom_id_) DCC_V2_ROLE_SELECT((custom_id_))
#define DCC_UI_ROLE_SELECT_NS(namespace_, action_) DCC_V2_ROLE_SELECT_NS(namespace_, action_)
#define DCC_UI_MENTIONABLE_SELECT(custom_id_) DCC_V2_MENTIONABLE_SELECT((custom_id_))
#define DCC_UI_MENTIONABLE_SELECT_NS(namespace_, action_) DCC_V2_MENTIONABLE_SELECT_NS(namespace_, action_)
#define DCC_UI_CHANNEL_SELECT(custom_id_, default_values_, channel_types_) \
    DCC_V2_CHANNEL_SELECT((custom_id_), (default_values_), (channel_types_))
#define DCC_UI_CHANNEL_SELECT_NS(namespace_, action_, default_values_, channel_types_) \
    DCC_V2_CHANNEL_SELECT_NS(namespace_, action_, (default_values_), (channel_types_))
#define DCC_UI_LABEL(label_, component_) DCC_V2_LABEL((label_), (component_))
#define DCC_UI_INPUT(custom_id_, label_, required_) \
    DCC_MODAL_V2_TEXT_INPUT((custom_id_), (label_), (required_))
#define DCC_UI_INPUT_NS(namespace_, action_, label_, required_) \
    DCC_MODAL_V2_TEXT_INPUT(DCC_COMPONENT_ID(namespace_, action_), (label_), (required_))
#define DCC_UI_INPUT_PLACEHOLDER(custom_id_, label_, placeholder_, required_) \
    DCC_MODAL_V2_TEXT_INPUT_PLACEHOLDER((custom_id_), (label_), (placeholder_), (required_))
#define DCC_UI_INPUT_PLACEHOLDER_NS(namespace_, action_, label_, placeholder_, required_) \
    DCC_MODAL_V2_TEXT_INPUT_PLACEHOLDER( \
        DCC_COMPONENT_ID(namespace_, action_), \
        (label_), \
        (placeholder_), \
        (required_) \
    )
#define DCC_UI_CHECKBOX(custom_id_, label_, checked_) DCC_MODAL_V2_CHECKBOX((custom_id_), (label_), (checked_))
#define DCC_UI_CHECKBOX_NS(namespace_, action_, label_, checked_) \
    DCC_MODAL_V2_CHECKBOX(DCC_COMPONENT_ID(namespace_, action_), (label_), (checked_))
#define DCC_UI_MESSAGE(...) DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__)
#define DCC_UI_EPHEMERAL(...) DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(__VA_ARGS__)
#define DCC_RESPOND_UI(ctx_, ...) DCC_RESPOND_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_UI(ctx_, ...) DCC_RESPOND_OR_EDIT_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_UI(ctx_, ...) DCC_RESPOND_OR_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_EPHEMERAL_UI(ctx_, ...) DCC_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_PRIVATE_UI(ctx_, ...) DCC_RESPOND_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_UI(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_PRIVATE_UI(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_UI(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_UI(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_REPLY_UI(ctx_, ...) DCC_CTX_REPLY_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_UI(ctx_, ...) DCC_CTX_RESPOND_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_CTX_RESPOND_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_UI(ctx_, ...) DCC_CTX_RESPOND_OR_EDIT_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_CTX_RESPOND_OR_EDIT_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_UI_CB(ctx_, cb_, user_data_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_EPHEMERAL_UI(ctx_, ...) DCC_CTX_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_EPHEMERAL_UI(ctx_, ...) \
    DCC_CTX_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_PRIVATE_UI(ctx_, ...) DCC_CTX_RESPOND_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_EPHEMERAL_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_PRIVATE_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_EDIT_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_EPHEMERAL_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_PRIVATE_UI(ctx_, ...) \
    DCC_CTX_RESPOND_OR_FOLLOWUP_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_UPDATE_UI(ctx_, ...) DCC_CTX_UPDATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_UI(ctx_, ...) DCC_CTX_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_SEND_UI(ctx_, ...) DCC_CTX_SEND_V2((ctx_), __VA_ARGS__)
#define DCC_SEND_UI_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_SEND_UI_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_UI_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_UI_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_CTX_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_UI_ID(ctx_, cb_, user_data_, ...) \
    DCC_CTX_SEND_UI_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_UI_ALIASES_H */
