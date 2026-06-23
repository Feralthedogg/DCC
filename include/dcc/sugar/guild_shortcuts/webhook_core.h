#ifndef DCC_SUGAR_GUILD_SHORTCUTS_WEBHOOK_CORE_H
#define DCC_SUGAR_GUILD_SHORTCUTS_WEBHOOK_CORE_H

#include <dcc/sugar/guild_shortcuts/base.h>

#define DCC_APP_GET_CHANNEL_WEBHOOKS(app_, channel_id_) \
    dcc_app_get_channel_webhooks((app_), (channel_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_WEBHOOKS(app_, guild_id_) \
    dcc_app_get_guild_webhooks((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_WEBHOOK(app_, webhook_id_) \
    dcc_app_get_webhook((app_), (webhook_id_), NULL, NULL)
#define DCC_APP_GET_WEBHOOK_TOKEN(app_, webhook_id_, webhook_token_) \
    dcc_app_get_webhook_with_token((app_), (webhook_id_), (webhook_token_), NULL, NULL)
#define DCC_APP_CREATE_WEBHOOK(app_, channel_id_, json_body_) \
    dcc_app_create_webhook((app_), (channel_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_WEBHOOK_PARAMS(app_, params_) \
    dcc_app_create_webhook_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_WEBHOOK(app_, webhook_id_, json_body_) \
    dcc_app_modify_webhook((app_), (webhook_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_WEBHOOK_PARAMS(app_, params_) \
    dcc_app_modify_webhook_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_WEBHOOK_TOKEN(app_, webhook_id_, webhook_token_, json_body_) \
    dcc_app_modify_webhook_with_token((app_), (webhook_id_), (webhook_token_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_WEBHOOK_TOKEN_PARAMS(app_, params_) \
    dcc_app_modify_webhook_with_token_params((app_), (params_), NULL, NULL)
#define DCC_APP_DELETE_WEBHOOK(app_, webhook_id_) \
    dcc_app_delete_webhook((app_), (webhook_id_), NULL, NULL)
#define DCC_APP_DELETE_WEBHOOK_TOKEN(app_, webhook_id_, webhook_token_) \
    dcc_app_delete_webhook_with_token((app_), (webhook_id_), (webhook_token_), NULL, NULL)
#define DCC_APP_EXECUTE_WEBHOOK(app_, webhook_id_, webhook_token_, query_, json_body_) \
    dcc_app_execute_webhook((app_), (webhook_id_), (webhook_token_), (query_), (json_body_), NULL, NULL)
#define DCC_APP_EXECUTE_WEBHOOK_OPTIONS( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    wait_, \
    thread_id_, \
    with_components_, \
    json_body_ \
) \
    dcc_app_execute_webhook_options( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (wait_), \
        (thread_id_), \
        (with_components_), \
        (json_body_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EXECUTE_WEBHOOK_MESSAGE( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    wait_, \
    thread_id_, \
    with_components_, \
    message_ \
) \
    dcc_app_execute_webhook_message( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (wait_), \
        (thread_id_), \
        (with_components_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        NULL, \
        NULL \
    )
#define DCC_APP_EXECUTE_WEBHOOK_TEXT(app_, webhook_id_, webhook_token_, wait_, thread_id_, content_) \
    dcc_app_execute_webhook_text( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (wait_), \
        (thread_id_), \
        (content_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EXECUTE_WEBHOOK_MULTIPART( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    query_, \
    payload_json_, \
    files_, \
    file_count_ \
) \
    dcc_app_execute_webhook_multipart( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (query_), \
        (payload_json_), \
        (files_), \
        (file_count_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EXECUTE_WEBHOOK_MULTIPART_OPTIONS( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    wait_, \
    thread_id_, \
    with_components_, \
    payload_json_, \
    files_, \
    file_count_ \
) \
    dcc_app_execute_webhook_multipart_options( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (wait_), \
        (thread_id_), \
        (with_components_), \
        (payload_json_), \
        (files_), \
        (file_count_), \
        NULL, \
        NULL \
    )

#endif /* DCC_SUGAR_GUILD_SHORTCUTS_WEBHOOK_CORE_H */
