#ifndef DCC_SUGAR_REPLY_DEFER_UPDATE_H
#define DCC_SUGAR_REPLY_DEFER_UPDATE_H

#include <dcc/sugar/reply/base.h>

#define DCC_REPLY_DEFER(ctx_) \
    dcc_ctx_defer((ctx_), NULL, NULL)

#define DCC_DEFER(ctx_) \
    DCC_REPLY_DEFER((ctx_))

#define DCC_REPLY_DEFER_EPHEMERAL(ctx_) \
    dcc_ctx_defer_ephemeral((ctx_), NULL, NULL)

#define DCC_DEFER_EPHEMERAL(ctx_) \
    DCC_REPLY_DEFER_EPHEMERAL((ctx_))

#define DCC_REPLY_DEFER_PRIVATE(ctx_) \
    DCC_REPLY_DEFER_EPHEMERAL((ctx_))

#define DCC_DEFER_PRIVATE(ctx_) \
    DCC_REPLY_DEFER_PRIVATE((ctx_))

#define DCC_UPDATE_MESSAGE(ctx_, message_) \
    dcc_ctx_update_message((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)

#define DCC_UPDATE_MESSAGE_CB(ctx_, message_, cb_, user_data_) \
    dcc_ctx_update_message((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_UPDATE(ctx_, message_) \
    DCC_UPDATE_MESSAGE((ctx_), (message_))

#define DCC_UPDATE_CB(ctx_, message_, cb_, user_data_) \
    DCC_UPDATE_MESSAGE_CB((ctx_), (message_), (cb_), (user_data_))

#define DCC_EDIT_ORIGINAL(ctx_, message_) \
    dcc_ctx_edit_original((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)

#define DCC_EDIT_ORIGINAL_CB(ctx_, message_, cb_, user_data_) \
    dcc_ctx_edit_original((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_EDIT(ctx_, message_) \
    DCC_EDIT_ORIGINAL((ctx_), (message_))

#define DCC_EDIT_CB(ctx_, message_, cb_, user_data_) \
    DCC_EDIT_ORIGINAL_CB((ctx_), (message_), (cb_), (user_data_))

#define DCC_DELETE_ORIGINAL(ctx_) \
    dcc_ctx_delete_original((ctx_), NULL, NULL)

#define DCC_DELETE_ORIGINAL_CB(ctx_, cb_, user_data_) \
    dcc_ctx_delete_original((ctx_), (cb_), (user_data_))

#endif /* DCC_SUGAR_REPLY_DEFER_UPDATE_H */
