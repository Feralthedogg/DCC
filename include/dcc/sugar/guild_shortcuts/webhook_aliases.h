#ifndef DCC_SUGAR_GUILD_SHORTCUTS_WEBHOOK_ALIASES_H
#define DCC_SUGAR_GUILD_SHORTCUTS_WEBHOOK_ALIASES_H

#include <dcc/sugar/guild_shortcuts/webhook_messages.h>

#define DCC_CHANNEL_WEBHOOKS_FETCH(app_, channel_id_) \
    DCC_APP_GET_CHANNEL_WEBHOOKS((app_), (channel_id_))
#define DCC_GUILD_WEBHOOKS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_WEBHOOKS((app_), (guild_id_))
#define DCC_WEBHOOK_FETCH(app_, webhook_id_) \
    DCC_APP_GET_WEBHOOK((app_), (webhook_id_))
#define DCC_WEBHOOK_FETCH_TOKEN(app_, webhook_id_, webhook_token_) \
    DCC_APP_GET_WEBHOOK_TOKEN((app_), (webhook_id_), (webhook_token_))
#define DCC_WEBHOOK_CREATE(app_, channel_id_, json_body_) \
    DCC_APP_CREATE_WEBHOOK((app_), (channel_id_), (json_body_))
#define DCC_WEBHOOK_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_WEBHOOK_PARAMS((app_), (params_))
#define DCC_WEBHOOK_EDIT(app_, webhook_id_, json_body_) \
    DCC_APP_EDIT_WEBHOOK((app_), (webhook_id_), (json_body_))
#define DCC_WEBHOOK_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_WEBHOOK_PARAMS((app_), (params_))
#define DCC_WEBHOOK_EDIT_TOKEN(app_, webhook_id_, webhook_token_, json_body_) \
    DCC_APP_EDIT_WEBHOOK_TOKEN((app_), (webhook_id_), (webhook_token_), (json_body_))
#define DCC_WEBHOOK_EDIT_TOKEN_PARAMS(app_, params_) \
    DCC_APP_EDIT_WEBHOOK_TOKEN_PARAMS((app_), (params_))
#define DCC_WEBHOOK_DELETE(app_, webhook_id_) \
    DCC_APP_DELETE_WEBHOOK((app_), (webhook_id_))
#define DCC_WEBHOOK_DELETE_TOKEN(app_, webhook_id_, webhook_token_) \
    DCC_APP_DELETE_WEBHOOK_TOKEN((app_), (webhook_id_), (webhook_token_))
#define DCC_WEBHOOK_SEND_JSON(app_, webhook_id_, webhook_token_, json_body_) \
    DCC_APP_EXECUTE_WEBHOOK((app_), (webhook_id_), (webhook_token_), NULL, (json_body_))
#define DCC_WEBHOOK_SEND_JSON_QUERY(app_, webhook_id_, webhook_token_, query_, json_body_) \
    DCC_APP_EXECUTE_WEBHOOK((app_), (webhook_id_), (webhook_token_), (query_), (json_body_))
#define DCC_WEBHOOK_SEND_OPTIONS(app_, webhook_id_, webhook_token_, wait_, thread_id_, with_components_, json_body_) \
    DCC_APP_EXECUTE_WEBHOOK_OPTIONS( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (wait_), \
        (thread_id_), \
        (with_components_), \
        (json_body_) \
    )
#define DCC_WEBHOOK_SEND(app_, webhook_id_, webhook_token_, message_) \
    DCC_APP_EXECUTE_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), 0U, 0U, 0U, (message_))
#define DCC_WEBHOOK_SEND_WAIT(app_, webhook_id_, webhook_token_, message_) \
    DCC_APP_EXECUTE_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), 1U, 0U, 0U, (message_))
#define DCC_WEBHOOK_SEND_THREAD(app_, webhook_id_, webhook_token_, thread_id_, message_) \
    DCC_APP_EXECUTE_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), 0U, (thread_id_), 0U, (message_))
#define DCC_WEBHOOK_SEND_TEXT(app_, webhook_id_, webhook_token_, content_) \
    DCC_APP_EXECUTE_WEBHOOK_TEXT((app_), (webhook_id_), (webhook_token_), 0U, 0U, (content_))
#define DCC_WEBHOOK_SEND_TEXT_WAIT(app_, webhook_id_, webhook_token_, content_) \
    DCC_APP_EXECUTE_WEBHOOK_TEXT((app_), (webhook_id_), (webhook_token_), 1U, 0U, (content_))
#define DCC_WEBHOOK_SEND_UI(app_, webhook_id_, webhook_token_, ...) \
    DCC_APP_EXECUTE_WEBHOOK_MESSAGE( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        0U, \
        0U, \
        1U, \
        DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__) \
    )
#define DCC_WEBHOOK_SEND_UI_WAIT(app_, webhook_id_, webhook_token_, ...) \
    DCC_APP_EXECUTE_WEBHOOK_MESSAGE( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        1U, \
        0U, \
        1U, \
        DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__) \
    )
#define DCC_WEBHOOK_MESSAGE_FETCH(app_, webhook_id_, webhook_token_, message_id_) \
    DCC_APP_GET_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), (message_id_))
#define DCC_WEBHOOK_MESSAGE_FETCH_THREAD(app_, webhook_id_, webhook_token_, message_id_, thread_id_) \
    DCC_APP_GET_WEBHOOK_MESSAGE_THREAD((app_), (webhook_id_), (webhook_token_), (message_id_), (thread_id_))
#define DCC_WEBHOOK_MESSAGE_EDIT_JSON(app_, webhook_id_, webhook_token_, message_id_, json_body_) \
    DCC_APP_EDIT_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), (message_id_), (json_body_))
#define DCC_WEBHOOK_MESSAGE_EDIT(app_, webhook_id_, webhook_token_, message_id_, message_) \
    DCC_APP_EDIT_WEBHOOK_MESSAGE_BUILDER((app_), (webhook_id_), (webhook_token_), (message_id_), (message_))
#define DCC_WEBHOOK_MESSAGE_EDIT_THREAD(app_, webhook_id_, webhook_token_, message_id_, thread_id_, message_) \
    DCC_APP_EDIT_WEBHOOK_MESSAGE_BUILDER_THREAD( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        (message_) \
    )
#define DCC_WEBHOOK_MESSAGE_DELETE(app_, webhook_id_, webhook_token_, message_id_) \
    DCC_APP_DELETE_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), (message_id_))
#define DCC_WEBHOOK_MESSAGE_DELETE_THREAD(app_, webhook_id_, webhook_token_, message_id_, thread_id_) \
    DCC_APP_DELETE_WEBHOOK_MESSAGE_THREAD((app_), (webhook_id_), (webhook_token_), (message_id_), (thread_id_))

#endif /* DCC_SUGAR_GUILD_SHORTCUTS_WEBHOOK_ALIASES_H */
