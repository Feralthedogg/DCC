#include <dcc/app.h>

#include <dcc/rest/emojis_stickers/guild_emojis.h>
#include <dcc/rest/emojis_stickers/guild_stickers.h>
#include <dcc/rest/emojis_stickers/message_search.h>
#include <dcc/rest/emojis_stickers/stickers.h>
#include <dcc/rest/channels/actions.h>
#include <dcc/rest/channels/core.h>
#include <dcc/rest/channels/invites.h>
#include <dcc/rest/channels/permissions.h>
#include <dcc/rest/channels/positions.h>
#include <dcc/rest/guilds/audit_log.h>
#include <dcc/rest/guilds/auto_moderation.h>
#include <dcc/rest/guilds/bans.h>
#include <dcc/rest/guilds/current_member.h>
#include <dcc/rest/guilds/integrations.h>
#include <dcc/rest/guilds/onboarding.h>
#include <dcc/rest/guilds/prune.h>
#include <dcc/rest/guilds/scheduled_events.h>
#include <dcc/rest/guilds/welcome_screen.h>
#include <dcc/rest/guilds/widgets.h>
#include <dcc/rest/guild_members/lifecycle.h>
#include <dcc/rest/guild_members/query.h>
#include <dcc/rest/guild_members/roles.h>
#include <dcc/rest/invites.h>
#include <dcc/rest/messages/edit.h>
#include <dcc/rest/messages/pins.h>
#include <dcc/rest/messages/reactions.h>
#include <dcc/rest/resources/direct_messages.h>
#include <dcc/rest/resources/entitlements.h>
#include <dcc/rest/resources/gateway.h>
#include <dcc/rest/resources/group_dms.h>
#include <dcc/rest/resources/invites.h>
#include <dcc/rest/resources/stage_instances.h>
#include <dcc/rest/resources/templates.h>
#include <dcc/rest/resources/users.h>
#include <dcc/rest/resources/voice_states.h>
#include <dcc/rest/roles.h>
#include <dcc/rest/threads/create.h>
#include <dcc/rest/threads/lifecycle.h>
#include <dcc/rest/webhooks/execute.h>
#include <dcc/rest/webhooks/management/create.h>
#include <dcc/rest/webhooks/management/delete.h>
#include <dcc/rest/webhooks/management/fetch.h>
#include <dcc/rest/webhooks/management/modify.h>
#include <dcc/rest/webhooks/messages/delete.h>
#include <dcc/rest/webhooks/messages/edit.h>
#include <dcc/rest/webhooks/messages/fetch.h>

static int dcc_app_webhook_token_invalid(const char *webhook_token) {
    return webhook_token == NULL || webhook_token[0] == '\0';
}

dcc_status_t dcc_app_get_channel_webhooks(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_channel_webhooks(dcc_app_client(app), channel_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_webhooks(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_webhooks(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_get_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || webhook_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_webhook(dcc_app_client(app), webhook_id, cb, user_data);
}

dcc_status_t dcc_app_get_webhook_with_token(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || webhook_id == 0U || dcc_app_webhook_token_invalid(webhook_token)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_webhook_with_token(dcc_app_client(app), webhook_id, webhook_token, cb, user_data);
}

dcc_status_t dcc_app_create_webhook(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_webhook(dcc_app_client(app), channel_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_create_webhook_params(
    dcc_app_t *app,
    const dcc_webhook_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_webhook_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_modify_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || webhook_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_webhook(dcc_app_client(app), webhook_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_webhook_params(
    dcc_app_t *app,
    const dcc_webhook_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_webhook_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_modify_webhook_with_token(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || webhook_id == 0U || dcc_app_webhook_token_invalid(webhook_token) || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_webhook_with_token(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        json_body,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_modify_webhook_with_token_params(
    dcc_app_t *app,
    const dcc_webhook_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_webhook_with_token_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_delete_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || webhook_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_webhook(dcc_app_client(app), webhook_id, cb, user_data);
}

dcc_status_t dcc_app_delete_webhook_with_token(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || webhook_id == 0U || dcc_app_webhook_token_invalid(webhook_token)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_webhook_with_token(dcc_app_client(app), webhook_id, webhook_token, cb, user_data);
}

dcc_status_t dcc_app_execute_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *query,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || webhook_id == 0U || dcc_app_webhook_token_invalid(webhook_token) || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_execute_webhook(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        query,
        json_body,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_execute_webhook_options(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || webhook_id == 0U || dcc_app_webhook_token_invalid(webhook_token) || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_execute_webhook_options(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        wait,
        thread_id,
        with_components,
        json_body,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_execute_webhook_message(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &json);
    if (status == DCC_OK) {
        status = dcc_app_execute_webhook_options(
            app,
            webhook_id,
            webhook_token,
            wait,
            thread_id,
            with_components,
            json,
            cb,
            user_data
        );
    }
    dcc_message_builder_json_free(json);
    return status;
}

dcc_status_t dcc_app_execute_webhook_text(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
) {
    if (content == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U,
    };
    return dcc_app_execute_webhook_message(
        app,
        webhook_id,
        webhook_token,
        wait,
        thread_id,
        0U,
        &message,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_execute_webhook_multipart(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *query,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        payload_json == NULL ||
        (files == NULL && file_count > 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_execute_webhook_multipart(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        query,
        payload_json,
        files,
        file_count,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_execute_webhook_multipart_options(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        payload_json == NULL ||
        (files == NULL && file_count > 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_execute_webhook_multipart_options(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        wait,
        thread_id,
        with_components,
        payload_json,
        files,
        file_count,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_get_webhook_message(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        message_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_webhook_message(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        message_id,
        query,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_get_webhook_message_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        message_id == 0U ||
        thread_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_webhook_message_thread(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        message_id,
        thread_id,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_modify_webhook_message(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        message_id == 0U ||
        json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_webhook_message(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        message_id,
        query,
        json_body,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_modify_webhook_message_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        message_id == 0U ||
        thread_id == 0U ||
        json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_webhook_message_thread(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        message_id,
        thread_id,
        json_body,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_modify_webhook_message_builder(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        message_id == 0U ||
        message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_webhook_message_builder(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        message_id,
        query,
        message,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_modify_webhook_message_builder_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        message_id == 0U ||
        thread_id == 0U ||
        message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_webhook_message_builder_thread(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        message_id,
        thread_id,
        message,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_modify_webhook_message_multipart(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        message_id == 0U ||
        payload_json == NULL ||
        (files == NULL && file_count > 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_webhook_message_multipart(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        message_id,
        query,
        payload_json,
        files,
        file_count,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_modify_webhook_message_multipart_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        message_id == 0U ||
        thread_id == 0U ||
        payload_json == NULL ||
        (files == NULL && file_count > 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_webhook_message_multipart_thread(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        message_id,
        thread_id,
        payload_json,
        files,
        file_count,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_delete_webhook_message(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        message_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_webhook_message(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        message_id,
        query,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_delete_webhook_message_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        webhook_id == 0U ||
        dcc_app_webhook_token_invalid(webhook_token) ||
        message_id == 0U ||
        thread_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_webhook_message_thread(
        dcc_app_client(app),
        webhook_id,
        webhook_token,
        message_id,
        thread_id,
        cb,
        user_data
    );
}
