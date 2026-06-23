#ifndef DCC_SUGAR_REPLY_FOLLOWUP_H
#define DCC_SUGAR_REPLY_FOLLOWUP_H

#include <dcc/sugar/reply/base.h>

#define DCC_FOLLOWUP(ctx_, message_) \
    dcc_ctx_followup((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)

#define DCC_FOLLOWUP_CB(ctx_, message_, cb_, user_data_) \
    dcc_ctx_followup((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_FOLLOWUP_TEXT(ctx_, content_) \
    dcc_ctx_followup_text((ctx_), (content_), NULL, NULL)

#define DCC_FOLLOWUP_PUBLIC(ctx_, content_) \
    DCC_FOLLOWUP_TEXT((ctx_), (content_))

#define DCC_FOLLOWUP_PUBLIC_CB(ctx_, content_, cb_, user_data_) \
    dcc_ctx_followup_text((ctx_), (content_), (cb_), (user_data_))

#define DCC_FOLLOWUP_EPHEMERAL(ctx_, content_) \
    dcc_ctx_followup_ephemeral_text((ctx_), (content_), NULL, NULL)

#define DCC_FOLLOWUP_EPHEMERAL_TEXT(ctx_, content_) \
    DCC_FOLLOWUP_EPHEMERAL((ctx_), (content_))

#define DCC_FOLLOWUP_EPHEMERAL_TEXT_CB(ctx_, content_, cb_, user_data_) \
    dcc_ctx_followup_ephemeral_text((ctx_), (content_), (cb_), (user_data_))

#define DCC_FOLLOWUP_PRIVATE(ctx_, content_) \
    DCC_FOLLOWUP_EPHEMERAL((ctx_), (content_))

#define DCC_FOLLOWUP_PRIVATE_TEXT(ctx_, content_) \
    DCC_FOLLOWUP_PRIVATE((ctx_), (content_))

#define DCC_FOLLOWUP_PRIVATE_TEXT_CB(ctx_, content_, cb_, user_data_) \
    DCC_FOLLOWUP_EPHEMERAL_TEXT_CB((ctx_), (content_), (cb_), (user_data_))

#define DCC_FOLLOWUP_SILENT(ctx_, content_) \
    DCC_FOLLOWUP((ctx_), DCC_MESSAGE_TEXT_SILENT((content_)))

#define DCC_FOLLOWUP_SILENT_TEXT(ctx_, content_) \
    DCC_FOLLOWUP_SILENT((ctx_), (content_))

#define DCC_FOLLOWUP_SILENT_TEXT_CB(ctx_, content_, cb_, user_data_) \
    DCC_FOLLOWUP_CB((ctx_), DCC_MESSAGE_TEXT_SILENT((content_)), (cb_), (user_data_))

#endif /* DCC_SUGAR_REPLY_FOLLOWUP_H */
