#ifndef DCC_SUGAR_GUILD_SHORTCUTS_WEBHOOK_MESSAGES_H
#define DCC_SUGAR_GUILD_SHORTCUTS_WEBHOOK_MESSAGES_H

#include <dcc/sugar/guild_shortcuts/webhook_core.h>

#define DCC_APP_GET_WEBHOOK_MESSAGE(app_, webhook_id_, webhook_token_, message_id_) \
    dcc_app_get_webhook_message((app_), (webhook_id_), (webhook_token_), (message_id_), NULL, NULL, NULL)
#define DCC_APP_GET_WEBHOOK_MESSAGE_QUERY(app_, webhook_id_, webhook_token_, message_id_, query_) \
    dcc_app_get_webhook_message((app_), (webhook_id_), (webhook_token_), (message_id_), (query_), NULL, NULL)
#define DCC_APP_GET_WEBHOOK_MESSAGE_THREAD(app_, webhook_id_, webhook_token_, message_id_, thread_id_) \
    dcc_app_get_webhook_message_thread( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE(app_, webhook_id_, webhook_token_, message_id_, json_body_) \
    dcc_app_modify_webhook_message( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        NULL, \
        (json_body_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_QUERY(app_, webhook_id_, webhook_token_, message_id_, query_, json_body_) \
    dcc_app_modify_webhook_message( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (query_), \
        (json_body_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_THREAD( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    message_id_, \
    thread_id_, \
    json_body_ \
) \
    dcc_app_modify_webhook_message_thread( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        (json_body_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_BUILDER(app_, webhook_id_, webhook_token_, message_id_, message_) \
    dcc_app_modify_webhook_message_builder( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        NULL, \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_BUILDER_THREAD( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    message_id_, \
    thread_id_, \
    message_ \
) \
    dcc_app_modify_webhook_message_builder_thread( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_MULTIPART( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    message_id_, \
    query_, \
    payload_json_, \
    files_, \
    file_count_ \
) \
    dcc_app_modify_webhook_message_multipart( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (query_), \
        (payload_json_), \
        (files_), \
        (file_count_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_MULTIPART_THREAD( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    message_id_, \
    thread_id_, \
    payload_json_, \
    files_, \
    file_count_ \
) \
    dcc_app_modify_webhook_message_multipart_thread( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        (payload_json_), \
        (files_), \
        (file_count_), \
        NULL, \
        NULL \
    )
#define DCC_APP_DELETE_WEBHOOK_MESSAGE(app_, webhook_id_, webhook_token_, message_id_) \
    dcc_app_delete_webhook_message((app_), (webhook_id_), (webhook_token_), (message_id_), NULL, NULL, NULL)
#define DCC_APP_DELETE_WEBHOOK_MESSAGE_QUERY(app_, webhook_id_, webhook_token_, message_id_, query_) \
    dcc_app_delete_webhook_message((app_), (webhook_id_), (webhook_token_), (message_id_), (query_), NULL, NULL)
#define DCC_APP_DELETE_WEBHOOK_MESSAGE_THREAD(app_, webhook_id_, webhook_token_, message_id_, thread_id_) \
    dcc_app_delete_webhook_message_thread( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        NULL, \
        NULL \
    )

#endif /* DCC_SUGAR_GUILD_SHORTCUTS_WEBHOOK_MESSAGES_H */
