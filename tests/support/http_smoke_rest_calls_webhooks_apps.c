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
    dcc_rest_webhook_builder_t builder = DCC_REST_WEBHOOK_BUILDER_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    builder.present = DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME;
    builder.name = "relay";
    return dcc_rest_create_webhook(client, 222, &builder, &options, NULL);
}
dcc_status_t call_rest_create_webhook_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_webhook_builder_t builder = DCC_REST_WEBHOOK_BUILDER_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    builder.present = DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME |
        DCC_REST_WEBHOOK_BUILDER_PRESENT_AVATAR;
    builder.name = "relay typed";
    builder.avatar = "data:image/png;base64,AA==";
    return dcc_rest_create_webhook(client, 222, &builder, &options, NULL);
}
dcc_status_t call_rest_get_channel_webhooks(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_get_channel_webhooks(client, 222, &options, NULL);
}
dcc_status_t call_rest_get_guild_webhooks(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_get_guild_webhooks(client, 333, &options, NULL);
}
dcc_status_t call_rest_get_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_get_webhook(client, 666, NULL, &options, NULL);
}
dcc_status_t call_rest_get_webhook_with_token(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_get_webhook(client, 666, "tok/en", &options, NULL);
}
dcc_status_t call_rest_modify_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_webhook_builder_t builder = DCC_REST_WEBHOOK_BUILDER_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    builder.present = DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME;
    builder.name = "relay2";
    return dcc_rest_modify_webhook(client, 666, NULL, &builder, &options, NULL);
}
dcc_status_t call_rest_modify_webhook_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_webhook_builder_t builder = DCC_REST_WEBHOOK_BUILDER_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    builder.present = DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME |
        DCC_REST_WEBHOOK_BUILDER_PRESENT_AVATAR |
        DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID;
    builder.name = "relay2 typed";
    builder.avatar = "data:image/png;base64,BB==";
    builder.channel_id = 223;
    return dcc_rest_modify_webhook(client, 666, NULL, &builder, &options, NULL);
}
dcc_status_t call_rest_modify_webhook_with_token(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_webhook_builder_t builder = DCC_REST_WEBHOOK_BUILDER_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    builder.present = DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME;
    builder.name = "relay3";
    return dcc_rest_modify_webhook(client, 666, "tok/en", &builder, &options, NULL);
}
dcc_status_t call_rest_modify_webhook_with_token_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_webhook_builder_t builder = DCC_REST_WEBHOOK_BUILDER_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    builder.present = DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME |
        DCC_REST_WEBHOOK_BUILDER_PRESENT_AVATAR;
    builder.name = "relay3 typed";
    builder.avatar = "data:image/png;base64,CC==";
    return dcc_rest_modify_webhook(client, 666, "tok/en", &builder, &options, NULL);
}
dcc_status_t call_rest_delete_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_delete_webhook(client, 666, NULL, &options, NULL);
}
dcc_status_t call_rest_delete_webhook_with_token(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_delete_webhook(client, 666, "tok/en", &options, NULL);
}
dcc_status_t call_rest_execute_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    dcc_status_t status = dcc_message_builder_set_content(&message, "hook");
    dcc_rest_webhook_execute_t execute = DCC_REST_WEBHOOK_EXECUTE_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    execute.message = &message;
    execute.present = DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS;
    execute.wait = 1U;
    execute.with_components = 1U;
    return status == DCC_OK
        ? dcc_rest_execute_webhook(client, 666, "tok/en", &execute, &options, NULL)
        : status;
}
dcc_status_t call_rest_execute_webhook_options(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    dcc_status_t status = dcc_message_builder_set_content(&message, "hook");
    dcc_rest_webhook_execute_t execute = DCC_REST_WEBHOOK_EXECUTE_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    execute.message = &message;
    execute.present = DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID |
        DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS;
    execute.wait = 1U;
    execute.thread_id = 888;
    execute.with_components = 1U;
    return status == DCC_OK
        ? dcc_rest_execute_webhook(client, 666, "tok/en", &execute, &options, NULL)
        : status;
}
dcc_status_t call_rest_get_webhook_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_webhook_message_query_t query = DCC_REST_WEBHOOK_MESSAGE_QUERY_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    query.present = DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID;
    query.thread_id = 888;
    return dcc_rest_get_webhook_message(client, 666, "tok/en", 777, &query, &options, NULL);
}
dcc_status_t call_rest_get_webhook_message_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return call_rest_get_webhook_message(client, cb, user_data);
}
static dcc_status_t call_modify_webhook_message_content(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data,
    const char *content
) {
    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    dcc_status_t status = dcc_message_builder_set_content(&message, content);
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_webhook_message_edit_t edit = DCC_REST_WEBHOOK_MESSAGE_EDIT_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    payload.message = &message;
    edit.payload = &payload;
    edit.present = DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_THREAD_ID;
    edit.thread_id = 888;
    return status == DCC_OK
        ? dcc_rest_modify_webhook_message(client, 666, "tok/en", 777, &edit, &options, NULL)
        : status;
}
dcc_status_t call_rest_modify_webhook_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return call_modify_webhook_message_content(client, cb, user_data, "edited");
}
dcc_status_t call_rest_modify_webhook_message_builder(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return call_modify_webhook_message_content(client, cb, user_data, "edited builder");
}
dcc_status_t call_rest_modify_webhook_message_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return call_modify_webhook_message_content(client, cb, user_data, "edited");
}
dcc_status_t call_rest_delete_webhook_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_webhook_message_query_t query = DCC_REST_WEBHOOK_MESSAGE_QUERY_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    query.present = DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID;
    query.thread_id = 888;
    return dcc_rest_delete_webhook_message(client, 666, "tok/en", 777, &query, &options, NULL);
}
dcc_status_t call_rest_delete_webhook_message_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return call_rest_delete_webhook_message(client, cb, user_data);
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
