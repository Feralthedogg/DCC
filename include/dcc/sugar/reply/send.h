#ifndef DCC_SUGAR_REPLY_SEND_H
#define DCC_SUGAR_REPLY_SEND_H

#include <dcc/sugar/reply/base.h>

#define DCC_SEND(ctx_, message_) \
    dcc_ctx_send((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)

#define DCC_SEND_CB(ctx_, message_, cb_, user_data_) \
    dcc_ctx_send((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_SEND_WITH_ID(ctx_, message_, cb_, user_data_) \
    dcc_ctx_send_with_id((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))

#define DCC_SEND_ID(ctx_, message_, cb_, user_data_) \
    DCC_SEND_WITH_ID((ctx_), (message_), (cb_), (user_data_))

#define DCC_SEND_THREAD_PARAMS(ctx_, message_, thread_, cb_, user_data_) \
    dcc_ctx_send_with_thread((ctx_), &((dcc_message_builder_t[]){ (message_) })[0], (thread_), (cb_), (user_data_))

#define DCC_SEND_THREAD(ctx_, message_, thread_name_) \
    dcc_ctx_send_with_thread_name( \
        (ctx_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (thread_name_), \
        NULL, \
        NULL \
    )

#define DCC_SEND_THREAD_CB(ctx_, message_, thread_name_, cb_, user_data_) \
    dcc_ctx_send_with_thread_name( \
        (ctx_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (thread_name_), \
        (cb_), \
        (user_data_) \
    )

#define DCC_SEND_TEXT(ctx_, content_) \
    dcc_ctx_send_text((ctx_), (content_), NULL, NULL)

#define DCC_SEND_TEXT_WITH_ID(ctx_, content_, cb_, user_data_) \
    dcc_ctx_send_text_with_id((ctx_), (content_), (cb_), (user_data_))

#define DCC_SEND_TEXT_ID(ctx_, content_, cb_, user_data_) \
    DCC_SEND_TEXT_WITH_ID((ctx_), (content_), (cb_), (user_data_))

#define DCC_SEND_TEXT_THREAD(ctx_, content_, thread_name_) \
    dcc_ctx_send_text_with_thread((ctx_), (content_), (thread_name_), NULL, NULL)

#define DCC_SEND_TEXT_THREAD_CB(ctx_, content_, thread_name_, cb_, user_data_) \
    dcc_ctx_send_text_with_thread((ctx_), (content_), (thread_name_), (cb_), (user_data_))

#define DCC_SEND_V2_THREAD(ctx_, thread_name_, ...) \
    DCC_SEND_THREAD((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (thread_name_))

#define DCC_SEND_V2_THREAD_CB(ctx_, thread_name_, cb_, user_data_, ...) \
    DCC_SEND_THREAD_CB((ctx_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (thread_name_), (cb_), (user_data_))

#define DCC_SEND_UI_THREAD(ctx_, thread_name_, ...) \
    DCC_SEND_V2_THREAD((ctx_), (thread_name_), __VA_ARGS__)

#define DCC_SEND_UI_THREAD_CB(ctx_, thread_name_, cb_, user_data_, ...) \
    DCC_SEND_V2_THREAD_CB((ctx_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)

#define DCC_CTX_SEND_THREAD(ctx_, message_, thread_name_) \
    DCC_SEND_THREAD((ctx_), (message_), (thread_name_))

#define DCC_CTX_SEND_THREAD_CB(ctx_, message_, thread_name_, cb_, user_data_) \
    DCC_SEND_THREAD_CB((ctx_), (message_), (thread_name_), (cb_), (user_data_))

#define DCC_CTX_SEND_THREAD_PARAMS(ctx_, message_, thread_, cb_, user_data_) \
    DCC_SEND_THREAD_PARAMS((ctx_), (message_), (thread_), (cb_), (user_data_))

#define DCC_CTX_SEND_TEXT_THREAD(ctx_, content_, thread_name_) \
    DCC_SEND_TEXT_THREAD((ctx_), (content_), (thread_name_))

#define DCC_CTX_SEND_TEXT_THREAD_CB(ctx_, content_, thread_name_, cb_, user_data_) \
    DCC_SEND_TEXT_THREAD_CB((ctx_), (content_), (thread_name_), (cb_), (user_data_))

#define DCC_CTX_SEND_V2_THREAD(ctx_, thread_name_, ...) \
    DCC_SEND_V2_THREAD((ctx_), (thread_name_), __VA_ARGS__)

#define DCC_CTX_SEND_V2_THREAD_CB(ctx_, thread_name_, cb_, user_data_, ...) \
    DCC_SEND_V2_THREAD_CB((ctx_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)

#define DCC_CTX_SEND_UI_THREAD(ctx_, thread_name_, ...) \
    DCC_SEND_UI_THREAD((ctx_), (thread_name_), __VA_ARGS__)

#define DCC_CTX_SEND_UI_THREAD_CB(ctx_, thread_name_, cb_, user_data_, ...) \
    DCC_SEND_UI_THREAD_CB((ctx_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)

#endif /* DCC_SUGAR_REPLY_SEND_H */
