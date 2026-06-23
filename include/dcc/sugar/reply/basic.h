#ifndef DCC_SUGAR_REPLY_BASIC_H
#define DCC_SUGAR_REPLY_BASIC_H

#include <dcc/sugar/reply/base.h>

#define DCC_REPLY(ctx_, message_) \
    dcc_ctx_reply((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)

#define DCC_REPLY_CB(ctx_, message_, cb_, user_data_) \
    dcc_ctx_reply((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_REPLY_TEXT(ctx_, content_) \
    dcc_ctx_reply_text((ctx_), (content_), NULL, NULL)

#define DCC_REPLY_TEXT_CB(ctx_, content_, cb_, user_data_) \
    dcc_ctx_reply_text((ctx_), (content_), (cb_), (user_data_))

#define DCC_REPLY_PUBLIC(ctx_, content_) \
    DCC_REPLY_TEXT((ctx_), (content_))

#define DCC_REPLY_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_TEXT_CB((ctx_), (content_), (cb_), (user_data_))

#define DCC_PUBLIC(ctx_, content_) \
    DCC_REPLY_TEXT((ctx_), (content_))

#define DCC_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_TEXT_CB((ctx_), (content_), (cb_), (user_data_))

#define DCC_REPLY_EPHEMERAL(ctx_, content_) \
    dcc_ctx_reply_ephemeral_text((ctx_), (content_), NULL, NULL)

#define DCC_REPLY_EPHEMERAL_CB(ctx_, content_, cb_, user_data_) \
    dcc_ctx_reply_ephemeral_text((ctx_), (content_), (cb_), (user_data_))

#define DCC_REPLY_PRIVATE(ctx_, content_) \
    DCC_REPLY_EPHEMERAL((ctx_), (content_))

#define DCC_REPLY_PRIVATE_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_EPHEMERAL_CB((ctx_), (content_), (cb_), (user_data_))

#define DCC_REPLY_SILENT(ctx_, content_) \
    DCC_REPLY((ctx_), DCC_MESSAGE_TEXT_SILENT((content_)))

#define DCC_REPLY_SILENT_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_CB((ctx_), DCC_MESSAGE_TEXT_SILENT((content_)), (cb_), (user_data_))

#define DCC_REPLY_NO_EMBEDS(ctx_, content_) \
    DCC_REPLY((ctx_), DCC_MESSAGE_TEXT_NO_EMBEDS((content_)))

#define DCC_REPLY_NO_EMBEDS_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_CB((ctx_), DCC_MESSAGE_TEXT_NO_EMBEDS((content_)), (cb_), (user_data_))

#define DCC_EPHEMERAL(ctx_, content_) \
    DCC_REPLY_EPHEMERAL((ctx_), (content_))

#define DCC_EPHEMERAL_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_EPHEMERAL_CB((ctx_), (content_), (cb_), (user_data_))

#define DCC_PRIVATE(ctx_, content_) \
    DCC_REPLY_PRIVATE((ctx_), (content_))

#define DCC_PRIVATE_CB(ctx_, content_, cb_, user_data_) \
    DCC_REPLY_PRIVATE_CB((ctx_), (content_), (cb_), (user_data_))


#endif /* DCC_SUGAR_REPLY_BASIC_H */
