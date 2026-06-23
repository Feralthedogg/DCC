#ifndef DCC_SUGAR_REPLY_RESPOND_BASIC_H
#define DCC_SUGAR_REPLY_RESPOND_BASIC_H

#include <dcc/sugar/reply/basic.h>

#define DCC_RESPOND(ctx_, message_) DCC_REPLY((ctx_), (message_))
#define DCC_RESPOND_CB(ctx_, message_, cb_, user_data_) \
    DCC_REPLY_CB((ctx_), (message_), (cb_), (user_data_))
#define DCC_RESPOND_TEXT(ctx_, content_) DCC_REPLY_TEXT((ctx_), (content_))
#define DCC_RESPOND_TEXT_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_TEXT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_PUBLIC(ctx_, content_) DCC_REPLY_PUBLIC((ctx_), (content_))
#define DCC_RESPOND_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_PUBLIC_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_EPHEMERAL(ctx_, content_) DCC_REPLY_EPHEMERAL((ctx_), (content_))
#define DCC_RESPOND_EPHEMERAL_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_EPHEMERAL_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_PRIVATE(ctx_, content_) DCC_REPLY_PRIVATE((ctx_), (content_))
#define DCC_RESPOND_PRIVATE_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_PRIVATE_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT(ctx_, message_) DCC_RESPOND((ctx_), (message_))
#define DCC_RESPOND_OR_EDIT_CB(ctx_, message_, cb_, user_data_) \
    DCC_RESPOND_CB((ctx_), (message_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_TEXT(ctx_, content_) DCC_RESPOND_TEXT((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_TEXT_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_TEXT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_PUBLIC(ctx_, content_) DCC_RESPOND_PUBLIC((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_PUBLIC_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_EPHEMERAL(ctx_, content_) DCC_RESPOND_EPHEMERAL((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_EPHEMERAL_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_PRIVATE(ctx_, content_) DCC_RESPOND_PRIVATE((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_PRIVATE_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_PRIVATE_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP(ctx_, message_) DCC_RESPOND_OR_EDIT((ctx_), (message_))
#define DCC_RESPOND_OR_FOLLOWUP_CB(ctx_, message_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_CB((ctx_), (message_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_TEXT(ctx_, content_) DCC_RESPOND_OR_EDIT_TEXT((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_TEXT_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_TEXT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_PUBLIC(ctx_, content_) DCC_RESPOND_OR_EDIT_PUBLIC((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_PUBLIC_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL(ctx_, content_) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE(ctx_, content_) \
    DCC_RESPOND_OR_EDIT_PRIVATE((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_PRIVATE_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_SILENT(ctx_, content_) DCC_REPLY_SILENT((ctx_), (content_))
#define DCC_RESPOND_SILENT_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_SILENT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_NO_EMBEDS(ctx_, content_) DCC_REPLY_NO_EMBEDS((ctx_), (content_))
#define DCC_RESPOND_NO_EMBEDS_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_NO_EMBEDS_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_SILENT(ctx_, content_) DCC_RESPOND_SILENT((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_SILENT_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_SILENT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_NO_EMBEDS(ctx_, content_) DCC_RESPOND_NO_EMBEDS((ctx_), (content_))
#define DCC_RESPOND_OR_EDIT_NO_EMBEDS_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_NO_EMBEDS_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_SILENT(ctx_, content_) DCC_RESPOND_OR_EDIT_SILENT((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_SILENT_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_SILENT_CB((ctx_), (content_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_NO_EMBEDS(ctx_, content_) \
    DCC_RESPOND_OR_EDIT_NO_EMBEDS((ctx_), (content_))
#define DCC_RESPOND_OR_FOLLOWUP_NO_EMBEDS_CB(ctx_, content_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_NO_EMBEDS_CB((ctx_), (content_), (cb_), (user_data_))

#endif /* DCC_SUGAR_REPLY_RESPOND_BASIC_H */
