#ifndef DCC_SUGAR_REPLY_RESPOND_EMBEDS_H
#define DCC_SUGAR_REPLY_RESPOND_EMBEDS_H

#include <dcc/sugar/reply/status.h>
#include <dcc/sugar/reply/respond_basic.h>

#define DCC_RESPOND_EMBED(ctx_, title_, description_, color_, ephemeral_) \
    DCC_REPLY_EMBED((ctx_), (title_), (description_), (color_), (ephemeral_))
#define DCC_RESPOND_EMBED_CB(ctx_, title_, description_, color_, ephemeral_, cb_, user_data_) \
    DCC_REPLY_EMBED_CB((ctx_), (title_), (description_), (color_), (ephemeral_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_EMBED(ctx_, title_, description_, color_, ephemeral_) \
    DCC_RESPOND_EMBED((ctx_), (title_), (description_), (color_), (ephemeral_))
#define DCC_RESPOND_OR_EDIT_EMBED_CB(ctx_, title_, description_, color_, ephemeral_, cb_, user_data_) \
    DCC_RESPOND_EMBED_CB((ctx_), (title_), (description_), (color_), (ephemeral_), (cb_), (user_data_))
#define DCC_RESPOND_PUBLIC_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_PUBLIC_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_PUBLIC_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_REPLY_PUBLIC_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_PUBLIC_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_PUBLIC_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_EDIT_PUBLIC_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_PUBLIC_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_EPHEMERAL_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_EPHEMERAL_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_EPHEMERAL_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_REPLY_EPHEMERAL_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_EPHEMERAL_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_EDIT_EPHEMERAL_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_EPHEMERAL_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_PRIVATE_EMBED(ctx_, title_, description_, color_) \
    DCC_REPLY_PRIVATE_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_PRIVATE_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_REPLY_PRIVATE_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_EDIT_PRIVATE_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_PRIVATE_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_EDIT_PRIVATE_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_PRIVATE_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_EMBED(ctx_, title_, description_, color_, ephemeral_) \
    DCC_RESPOND_OR_EDIT_EMBED((ctx_), (title_), (description_), (color_), (ephemeral_))
#define DCC_RESPOND_OR_FOLLOWUP_EMBED_CB(ctx_, title_, description_, color_, ephemeral_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_EMBED_CB((ctx_), (title_), (description_), (color_), (ephemeral_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_PUBLIC_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_OR_EDIT_PUBLIC_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_FOLLOWUP_PUBLIC_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_PUBLIC_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_FOLLOWUP_EPHEMERAL_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_EPHEMERAL_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_EMBED(ctx_, title_, description_, color_) \
    DCC_RESPOND_OR_EDIT_PRIVATE_EMBED((ctx_), (title_), (description_), (color_))
#define DCC_RESPOND_OR_FOLLOWUP_PRIVATE_EMBED_CB(ctx_, title_, description_, color_, cb_, user_data_) \
    DCC_RESPOND_OR_EDIT_PRIVATE_EMBED_CB((ctx_), (title_), (description_), (color_), (cb_), (user_data_))

#endif /* DCC_SUGAR_REPLY_RESPOND_EMBEDS_H */
