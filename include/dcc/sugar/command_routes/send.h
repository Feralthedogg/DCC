#ifndef DCC_SUGAR_COMMAND_ROUTES_SEND_H
#define DCC_SUGAR_COMMAND_ROUTES_SEND_H

#include <dcc/sugar/command_routes/base.h>

#define DCC_APP_SEND(app_, channel_id_, message_) \
    dcc_app_send((app_), (channel_id_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)
#define DCC_APP_SEND_CB(app_, channel_id_, message_, cb_, user_data_) \
    dcc_app_send((app_), (channel_id_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))
#define DCC_APP_SEND_WITH_ID(app_, channel_id_, message_, cb_, user_data_) \
    dcc_app_send_with_id((app_), (channel_id_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))
#define DCC_APP_SEND_ID(app_, channel_id_, message_, cb_, user_data_) \
    DCC_APP_SEND_WITH_ID((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_APP_SEND_THREAD_PARAMS(app_, channel_id_, message_, thread_, cb_, user_data_) \
    dcc_app_send_with_thread( \
        (app_), \
        (channel_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (thread_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_SEND_THREAD(app_, channel_id_, message_, thread_name_) \
    dcc_app_send_with_thread_name( \
        (app_), \
        (channel_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (thread_name_), \
        NULL, \
        NULL \
    )
#define DCC_APP_SEND_THREAD_CB(app_, channel_id_, message_, thread_name_, cb_, user_data_) \
    dcc_app_send_with_thread_name( \
        (app_), \
        (channel_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (thread_name_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_SEND_TEXT(app_, channel_id_, content_) \
    dcc_app_send_text((app_), (channel_id_), (content_), NULL, NULL)
#define DCC_APP_SEND_TEXT_CB(app_, channel_id_, content_, cb_, user_data_) \
    dcc_app_send_text((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_APP_SEND_TEXT_WITH_ID(app_, channel_id_, content_, cb_, user_data_) \
    dcc_app_send_text_with_id((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_APP_SEND_TEXT_ID(app_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_SEND_TEXT_WITH_ID((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_APP_SEND_TEXT_THREAD(app_, channel_id_, content_, thread_name_) \
    dcc_app_send_text_with_thread((app_), (channel_id_), (content_), (thread_name_), NULL, NULL)
#define DCC_APP_SEND_TEXT_THREAD_CB(app_, channel_id_, content_, thread_name_, cb_, user_data_) \
    dcc_app_send_text_with_thread((app_), (channel_id_), (content_), (thread_name_), (cb_), (user_data_))
#define DCC_APP_SEND_V2(app_, channel_id_, ...) \
    DCC_APP_SEND((app_), (channel_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_APP_SEND_V2_CB(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_CB((app_), (channel_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_APP_SEND_V2_WITH_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_WITH_ID((app_), (channel_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_APP_SEND_V2_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_APP_SEND_V2_THREAD(app_, channel_id_, thread_name_, ...) \
    DCC_APP_SEND_THREAD((app_), (channel_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (thread_name_))
#define DCC_APP_SEND_V2_THREAD_CB(app_, channel_id_, thread_name_, cb_, user_data_, ...) \
    DCC_APP_SEND_THREAD_CB( \
        (app_), \
        (channel_id_), \
        DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), \
        (thread_name_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_SEND_EPHEMERAL_V2(app_, channel_id_, ...) \
    DCC_APP_SEND((app_), (channel_id_), DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(__VA_ARGS__))
#define DCC_APP_SEND_UI(app_, channel_id_, ...) DCC_APP_SEND_V2((app_), (channel_id_), __VA_ARGS__)
#define DCC_APP_SEND_UI_WITH_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_APP_SEND_UI_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_UI_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_APP_SEND_UI_THREAD(app_, channel_id_, thread_name_, ...) \
    DCC_APP_SEND_V2_THREAD((app_), (channel_id_), (thread_name_), __VA_ARGS__)
#define DCC_APP_SEND_UI_THREAD_CB(app_, channel_id_, thread_name_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_THREAD_CB((app_), (channel_id_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_APP_SEND_EPHEMERAL_UI(app_, channel_id_, ...) \
    DCC_APP_SEND_EPHEMERAL_V2((app_), (channel_id_), __VA_ARGS__)

#endif /* DCC_SUGAR_COMMAND_ROUTES_SEND_H */
