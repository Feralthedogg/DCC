#ifndef DCC_SUGAR_REPLY_RESPOND_MISC_H
#define DCC_SUGAR_REPLY_RESPOND_MISC_H

#include <dcc/sugar/reply/respond_status.h>
#include <dcc/sugar/reply/defer_update.h>
#include <dcc/sugar/reply/modal_autocomplete.h>

#define DCC_RESPOND_DEFER(ctx_) DCC_REPLY_DEFER((ctx_))
#define DCC_RESPOND_DEFER_EPHEMERAL(ctx_) DCC_REPLY_DEFER_EPHEMERAL((ctx_))
#define DCC_RESPOND_DEFER_PRIVATE(ctx_) DCC_REPLY_DEFER_PRIVATE((ctx_))
#define DCC_RESPOND_AUTOCOMPLETE(ctx_, autocomplete_) \
    DCC_REPLY_AUTOCOMPLETE((ctx_), (autocomplete_))
#define DCC_RESPOND_AUTOCOMPLETE_CB(ctx_, autocomplete_, cb_, user_data_) \
    DCC_REPLY_AUTOCOMPLETE_CB((ctx_), (autocomplete_), (cb_), (user_data_))
#define DCC_RESPOND_AUTOCOMPLETE_MATCHING(ctx_, ...) \
    DCC_REPLY_AUTOCOMPLETE_MATCHING((ctx_), __VA_ARGS__)
#define DCC_RESPOND_AUTOCOMPLETE_MATCHING_CB(ctx_, cb_, user_data_, ...) \
    DCC_REPLY_AUTOCOMPLETE_MATCHING_CB((ctx_), (cb_), (user_data_), __VA_ARGS__)

#endif /* DCC_SUGAR_REPLY_RESPOND_MISC_H */
