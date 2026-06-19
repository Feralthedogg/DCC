#include "http_smoke_support.h"

#if !defined(_WIN32)

dcc_status_t call_rest_leave_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_leave_thread(client, 999, cb, user_data);
}
dcc_status_t call_rest_add_thread_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_add_thread_member(client, 999, 444, cb, user_data);
}
dcc_status_t call_rest_add_thread_member_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_thread_member_params_t params = {
        .size = sizeof(params),
        .thread_id = 999,
        .user_id = 444
    };
    return dcc_rest_add_thread_member_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_remove_thread_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_remove_thread_member(client, 999, 444, cb, user_data);
}
dcc_status_t call_rest_get_thread_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_thread_member(client, 999, 444, cb, user_data);
}
dcc_status_t call_rest_get_thread_members(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_thread_members(client, 999, "with_member=true&limit=2", cb, user_data);
}
dcc_status_t call_rest_get_active_threads(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_active_threads(client, 333, cb, user_data);
}
dcc_status_t call_rest_get_public_archived_threads(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_public_archived_threads(client, 222, "before=2024-01-01T00:00:00.000000%2B00:00&limit=2", cb, user_data);
}
dcc_status_t call_rest_get_public_archived_threads_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_public_archived_threads_page(client, 222, 1704067200, 2, cb, user_data);
}
dcc_status_t call_rest_get_private_archived_threads(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_private_archived_threads(client, 222, "before=2024-01-01T00:00:00.000000%2B00:00&limit=2", cb, user_data);
}
dcc_status_t call_rest_get_private_archived_threads_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_private_archived_threads_page(client, 222, 1704067200, 2, cb, user_data);
}
dcc_status_t call_rest_get_joined_private_archived_threads(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_joined_private_archived_threads(client, 222, "before=777&limit=2", cb, user_data);
}
dcc_status_t call_rest_get_joined_private_archived_threads_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_joined_private_archived_threads_page(client, 222, 777, 2, cb, user_data);
}
dcc_status_t call_rest_get_guild_emojis(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_emojis(client, 333, cb, user_data);
}
dcc_status_t call_rest_get_guild_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_emoji(client, 333, 777, cb, user_data);
}
dcc_status_t call_rest_create_guild_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_guild_emoji(client, 333, "{\"name\":\"wave\",\"image\":\"data:image/png;base64,AA==\"}", cb, user_data);
}
dcc_status_t call_rest_create_guild_emoji_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_snowflake_t roles[] = {555, 556};
    const dcc_emoji_params_t params = {
        .size = sizeof(params),
        .name = "wave-typed",
        .image = "data:image/png;base64,AA==",
        .roles = roles,
        .role_count = sizeof(roles) / sizeof(roles[0])
    };
    return dcc_rest_create_guild_emoji_params(client, 333, &params, cb, user_data);
}
dcc_status_t call_rest_modify_guild_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_emoji(client, 333, 777, "{\"name\":\"wave2\"}", cb, user_data);
}
dcc_status_t call_rest_modify_guild_emoji_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_emoji_params_t params = {
        .size = sizeof(params),
        .emoji_id = 777,
        .name = "wave-edited"
    };
    return dcc_rest_modify_guild_emoji_params(client, 333, &params, cb, user_data);
}
dcc_status_t call_rest_delete_guild_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_guild_emoji(client, 333, 777, cb, user_data);
}
dcc_status_t call_rest_get_guild_stickers(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_stickers(client, 333, cb, user_data);
}
dcc_status_t call_rest_get_guild_sticker(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_sticker(client, 333, 888, cb, user_data);
}
dcc_status_t call_rest_modify_guild_sticker(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_sticker(client, 333, 888, "{\"name\":\"slap\",\"tags\":\"slap\"}", cb, user_data);
}
dcc_status_t call_rest_delete_guild_sticker(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_guild_sticker(client, 333, 888, cb, user_data);
}
dcc_status_t call_rest_get_sticker(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_sticker(client, 888, cb, user_data);
}
dcc_status_t call_rest_get_sticker_packs(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_sticker_packs(client, cb, user_data);
}
dcc_status_t call_rest_get_invite(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_invite(client, "a/b", "with_counts=true&with_expiration=true", cb, user_data);
}
dcc_status_t call_rest_get_invite_full(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_invite_full(client, "a/b", cb, user_data);
}
dcc_status_t call_rest_delete_invite(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_invite(client, "a/b", cb, user_data);
}
dcc_status_t call_rest_create_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_webhook(client, 222, "{\"name\":\"relay\"}", cb, user_data);
}
dcc_status_t call_rest_create_webhook_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_webhook_params_t params = {
        .size = sizeof(params),
        .channel_id = 222,
        .name = "relay typed",
        .avatar = "data:image/png;base64,AA=="
    };
    return dcc_rest_create_webhook_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_get_channel_webhooks(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_channel_webhooks(client, 222, cb, user_data);
}
dcc_status_t call_rest_get_guild_webhooks(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_webhooks(client, 333, cb, user_data);
}
dcc_status_t call_rest_get_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_webhook(client, 666, cb, user_data);
}
dcc_status_t call_rest_get_webhook_with_token(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_webhook_with_token(client, 666, "tok/en", cb, user_data);
}
dcc_status_t call_rest_modify_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_webhook(client, 666, "{\"name\":\"relay2\"}", cb, user_data);
}
dcc_status_t call_rest_modify_webhook_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_webhook_params_t params = {
        .size = sizeof(params),
        .webhook_id = 666,
        .channel_id = 223,
        .name = "relay2 typed",
        .avatar = "data:image/png;base64,BB=="
    };
    return dcc_rest_modify_webhook_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_modify_webhook_with_token(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_webhook_with_token(client, 666, "tok/en", "{\"name\":\"relay3\"}", cb, user_data);
}
dcc_status_t call_rest_modify_webhook_with_token_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_webhook_params_t params = {
        .size = sizeof(params),
        .webhook_id = 666,
        .name = "relay3 typed",
        .avatar = "data:image/png;base64,CC==",
        .token = "tok/en"
    };
    return dcc_rest_modify_webhook_with_token_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_delete_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_webhook(client, 666, cb, user_data);
}
dcc_status_t call_rest_delete_webhook_with_token(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_webhook_with_token(client, 666, "tok/en", cb, user_data);
}
dcc_status_t call_rest_execute_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_execute_webhook(
        client,
        666,
        "tok/en",
        "wait=true&with_components=true",
        "{\"content\":\"hook\"}",
        cb,
        user_data
    );
}
dcc_status_t call_rest_execute_webhook_options(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_execute_webhook_options(
        client,
        666,
        "tok/en",
        1,
        888,
        1,
        "{\"content\":\"hook\"}",
        cb,
        user_data
    );
}
dcc_status_t call_rest_get_webhook_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_webhook_message(client, 666, "tok/en", 777, "thread_id=888", cb, user_data);
}
dcc_status_t call_rest_get_webhook_message_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_webhook_message_thread(client, 666, "tok/en", 777, 888, cb, user_data);
}
dcc_status_t call_rest_modify_webhook_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_webhook_message(
        client,
        666,
        "tok/en",
        777,
        "thread_id=888",
        "{\"content\":\"edited\"}",
        cb,
        user_data
    );
}
dcc_status_t call_rest_modify_webhook_message_builder(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    dcc_status_t status = dcc_message_builder_set_content(&message, "edited builder");
    if (status != DCC_OK) {
        return status;
    }
    return dcc_rest_modify_webhook_message_builder(
        client,
        666,
        "tok/en",
        777,
        "thread_id=888",
        &message,
        cb,
        user_data
    );
}
dcc_status_t call_rest_modify_webhook_message_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_webhook_message_thread(
        client,
        666,
        "tok/en",
        777,
        888,
        "{\"content\":\"edited\"}",
        cb,
        user_data
    );
}
dcc_status_t call_rest_delete_webhook_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_webhook_message(client, 666, "tok/en", 777, "thread_id=888", cb, user_data);
}
dcc_status_t call_rest_delete_webhook_message_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_webhook_message_thread(client, 666, "tok/en", 777, 888, cb, user_data);
}
dcc_status_t call_rest_get_application_emojis(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_application_emojis(client, 123, cb, user_data);
}
dcc_status_t call_rest_get_application_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_application_emoji(client, 123, 777, cb, user_data);
}
dcc_status_t call_rest_create_application_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_application_emoji(client, 123, "{\"name\":\"appwave\",\"image\":\"data:image/png;base64,AA==\"}", cb, user_data);
}
dcc_status_t call_rest_create_application_emoji_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_emoji_params_t params = {
        .size = sizeof(params),
        .name = "appwave-typed",
        .image = "data:image/png;base64,BB=="
    };
    return dcc_rest_create_application_emoji_params(client, 123, &params, cb, user_data);
}
dcc_status_t call_rest_modify_application_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_application_emoji(client, 123, 777, "{\"name\":\"appwave2\"}", cb, user_data);
}
dcc_status_t call_rest_modify_application_emoji_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_emoji_params_t params = {
        .size = sizeof(params),
        .emoji_id = 777,
        .name = "appwave-edited"
    };
    return dcc_rest_modify_application_emoji_params(client, 123, &params, cb, user_data);
}
dcc_status_t call_rest_delete_application_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_application_emoji(client, 123, 777, cb, user_data);
}
dcc_status_t call_rest_delete_all_global_commands(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_all_global_commands(client, 123, cb, user_data);
}
dcc_status_t call_rest_delete_all_guild_commands(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_all_guild_commands(client, 123, 333, cb, user_data);
}
dcc_status_t call_rest_bulk_edit_guild_command_permissions(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_bulk_edit_guild_command_permissions(
        client,
        123,
        333,
        "[{\"id\":\"444\",\"permissions\":[]}]",
        cb,
        user_data
    );
}

#endif
