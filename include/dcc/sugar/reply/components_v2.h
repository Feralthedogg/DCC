#ifndef DCC_SUGAR_REPLY_COMPONENTS_V2_H
#define DCC_SUGAR_REPLY_COMPONENTS_V2_H

#include <dcc/sugar/reply/basic.h>
#include <dcc/sugar/reply/defer_update.h>
#include <dcc/sugar/reply/followup.h>
#include <dcc/sugar/reply/send.h>

#define DCC_REPLY_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_REPLY_V2(ctx_, ...) DCC_REPLY_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_V2(ctx_, ...) DCC_RESPOND_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_REPLY_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_CB((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_REPLY_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_REPLY_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY((ctx_), DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(__VA_ARGS__))
#define DCC_REPLY_EPHEMERAL_V2(ctx_, ...) DCC_REPLY_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_REPLY_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_PRIVATE_V2(ctx_, ...) DCC_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_EDIT_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_REPLY_V2(ctx_, ...) DCC_REPLY_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_REPLY_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_EPHEMERAL_V2(ctx_, ...) DCC_REPLY_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_V2(ctx_, ...) DCC_RESPOND_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_V2(ctx_, ...) DCC_RESPOND_OR_EDIT_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_EDIT_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_V2(ctx_, ...) DCC_RESPOND_OR_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_RESPOND_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_PRIVATE_V2(ctx_, ...) DCC_RESPOND_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_EDIT_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_EDIT_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_OR_FOLLOWUP_PRIVATE_V2(ctx_, ...) \
    DCC_RESPOND_OR_FOLLOWUP_PRIVATE_V2((ctx_), __VA_ARGS__)
#define DCC_UPDATE_COMPONENTS_V2(ctx_, ...) \
    DCC_UPDATE((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_UPDATE_V2(ctx_, ...) DCC_UPDATE_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_UPDATE_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_UPDATE_CB((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_UPDATE_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_UPDATE_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_UPDATE_V2(ctx_, ...) DCC_UPDATE_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_UPDATE_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_UPDATE_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_FOLLOWUP_COMPONENTS_V2(ctx_, ...) \
    DCC_FOLLOWUP((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_FOLLOWUP_V2(ctx_, ...) DCC_FOLLOWUP_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_FOLLOWUP_COMPONENTS_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_FOLLOWUP_CB((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_FOLLOWUP_COMPONENTS_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_FOLLOWUP_EPHEMERAL_COMPONENTS_V2(ctx_, ...) \
    DCC_FOLLOWUP((ctx_), DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(__VA_ARGS__))
#define DCC_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_FOLLOWUP_EPHEMERAL_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_V2(ctx_, ...) DCC_FOLLOWUP_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_V2_CB(ctx_, cb_, user_data_, ...) \
    DCC_FOLLOWUP_V2_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_EPHEMERAL_V2(ctx_, ...) \
    DCC_FOLLOWUP_EPHEMERAL_V2((ctx_), __VA_ARGS__)
#define DCC_SEND_COMPONENTS_V2(ctx_, ...) \
    DCC_SEND((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_SEND_V2(ctx_, ...) DCC_SEND_COMPONENTS_V2((ctx_), __VA_ARGS__)
#define DCC_SEND_V2_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_WITH_ID((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_SEND_V2_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_V2(ctx_, ...) DCC_SEND_V2((ctx_), __VA_ARGS__)
#define DCC_CTX_SEND_V2_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_V2_ID(ctx_, cb_, user_data_, ...) \
    DCC_CTX_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_SEND_UI_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_SEND_UI_ID(ctx_, cb_, user_data_, ...) \
    DCC_SEND_UI_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_REPLY_UI(ctx_, ...) DCC_REPLY_V2((ctx_), __VA_ARGS__)
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
#define DCC_CTX_SEND_UI_WITH_ID(ctx_, cb_, user_data_, ...) \
    DCC_CTX_SEND_V2_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CTX_SEND_UI_ID(ctx_, cb_, user_data_, ...) \
    DCC_CTX_SEND_UI_WITH_ID((ctx_), (cb_), (user_data_), __VA_ARGS__)

#endif /* DCC_SUGAR_REPLY_COMPONENTS_V2_H */
