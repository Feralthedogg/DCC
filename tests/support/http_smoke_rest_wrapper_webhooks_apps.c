#include "http_smoke_support.h"

#if !defined(_WIN32)

int run_public_rest_wrapper_expect_webhooks_apps_smoke(dcc_client_t *client) {
    if (
        run_public_rest_wrapper_expect(
                    client,
                    "leave_thread",
                    call_rest_leave_thread,
                    "DELETE",
                    "/channels/999/thread-members/@me",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "add_thread_member",
                    call_rest_add_thread_member,
                    "PUT",
                    "/channels/999/thread-members/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "add_thread_member_params",
                    call_rest_add_thread_member_params,
                    "PUT",
                    "/channels/999/thread-members/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "remove_thread_member",
                    call_rest_remove_thread_member,
                    "DELETE",
                    "/channels/999/thread-members/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_thread_member",
                    call_rest_get_thread_member,
                    "GET",
                    "/channels/999/thread-members/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_thread_members",
                    call_rest_get_thread_members,
                    "GET",
                    "/channels/999/thread-members?with_member=true&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_active_threads",
                    call_rest_get_active_threads,
                    "GET",
                    "/guilds/333/threads/active",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_public_archived_threads",
                    call_rest_get_public_archived_threads,
                    "GET",
                    "/channels/222/threads/archived/public?before=2024-01-01T00:00:00.000000%2B00:00&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_public_archived_threads_page",
                    call_rest_get_public_archived_threads_page,
                    "GET",
                    "/channels/222/threads/archived/public?before=1704067200&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_private_archived_threads",
                    call_rest_get_private_archived_threads,
                    "GET",
                    "/channels/222/threads/archived/private?before=2024-01-01T00:00:00.000000%2B00:00&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_private_archived_threads_page",
                    call_rest_get_private_archived_threads_page,
                    "GET",
                    "/channels/222/threads/archived/private?before=1704067200&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_joined_private_archived_threads",
                    call_rest_get_joined_private_archived_threads,
                    "GET",
                    "/channels/222/users/@me/threads/archived/private?before=777&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_joined_private_archived_threads_page",
                    call_rest_get_joined_private_archived_threads_page,
                    "GET",
                    "/channels/222/users/@me/threads/archived/private?before=777&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_emojis",
                    call_rest_get_guild_emojis,
                    "GET",
                    "/guilds/333/emojis",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_emoji",
                    call_rest_get_guild_emoji,
                    "GET",
                    "/guilds/333/emojis/777",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_emoji",
                    call_rest_create_guild_emoji,
                    "POST",
                    "/guilds/333/emojis",
                    "{\"name\":\"wave\",\"image\":\"data:image/png;base64,AA==\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_emoji_params",
                    call_rest_create_guild_emoji_params,
                    "POST",
                    "/guilds/333/emojis",
                    "{\"name\":\"wave-typed\",\"image\":\"data:image/png;base64,AA==\",\"roles\":[\"555\",\"556\"]}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_emoji",
                    call_rest_modify_guild_emoji,
                    "PATCH",
                    "/guilds/333/emojis/777",
                    "{\"name\":\"wave2\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_emoji_params",
                    call_rest_modify_guild_emoji_params,
                    "PATCH",
                    "/guilds/333/emojis/777",
                    "{\"name\":\"wave-edited\",\"roles\":[]}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_guild_emoji",
                    call_rest_delete_guild_emoji,
                    "DELETE",
                    "/guilds/333/emojis/777",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_stickers",
                    call_rest_get_guild_stickers,
                    "GET",
                    "/guilds/333/stickers",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_sticker",
                    call_rest_get_guild_sticker,
                    "GET",
                    "/guilds/333/stickers/888",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_sticker",
                    call_rest_modify_guild_sticker,
                    "PATCH",
                    "/guilds/333/stickers/888",
                    "{\"name\":\"slap\",\"tags\":\"slap\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_guild_sticker",
                    call_rest_delete_guild_sticker,
                    "DELETE",
                    "/guilds/333/stickers/888",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_sticker",
                    call_rest_get_sticker,
                    "GET",
                    "/stickers/888",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_sticker_packs",
                    call_rest_get_sticker_packs,
                    "GET",
                    "/sticker-packs",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_invite",
                    call_rest_get_invite,
                    "GET",
                    "/invites/a%2Fb?with_counts=true&with_expiration=true",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_invite_full",
                    call_rest_get_invite_full,
                    "GET",
                    "/invites/a%2Fb?with_counts=true&with_expiration=true",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_invite",
                    call_rest_delete_invite,
                    "DELETE",
                    "/invites/a%2Fb",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_webhook",
                    call_rest_create_webhook,
                    "POST",
                    "/channels/222/webhooks",
                    "{\"name\":\"relay\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_webhook_params",
                    call_rest_create_webhook_params,
                    "POST",
                    "/channels/222/webhooks",
                    "{\"name\":\"relay typed\",\"channel_id\":\"222\",\"avatar\":\"data:image/png;base64,AA==\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_channel_webhooks",
                    call_rest_get_channel_webhooks,
                    "GET",
                    "/channels/222/webhooks",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_webhooks",
                    call_rest_get_guild_webhooks,
                    "GET",
                    "/guilds/333/webhooks",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_webhook",
                    call_rest_get_webhook,
                    "GET",
                    "/webhooks/666",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_webhook_with_token",
                    call_rest_get_webhook_with_token,
                    "GET",
                    "/webhooks/666/tok%2Fen",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_webhook",
                    call_rest_modify_webhook,
                    "PATCH",
                    "/webhooks/666",
                    "{\"name\":\"relay2\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_webhook_params",
                    call_rest_modify_webhook_params,
                    "PATCH",
                    "/webhooks/666",
                    "{\"name\":\"relay2 typed\",\"channel_id\":\"223\",\"avatar\":\"data:image/png;base64,BB==\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_webhook_with_token",
                    call_rest_modify_webhook_with_token,
                    "PATCH",
                    "/webhooks/666/tok%2Fen",
                    "{\"name\":\"relay3\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_webhook_with_token_params",
                    call_rest_modify_webhook_with_token_params,
                    "PATCH",
                    "/webhooks/666/tok%2Fen",
                    "{\"name\":\"relay3 typed\",\"avatar\":\"data:image/png;base64,CC==\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_webhook",
                    call_rest_delete_webhook,
                    "DELETE",
                    "/webhooks/666",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_webhook_with_token",
                    call_rest_delete_webhook_with_token,
                    "DELETE",
                    "/webhooks/666/tok%2Fen",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "execute_webhook",
                    call_rest_execute_webhook,
                    "POST",
                    "/webhooks/666/tok%2Fen?wait=true&with_components=true",
                    "{\"content\":\"hook\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "execute_webhook_options",
                    call_rest_execute_webhook_options,
                    "POST",
                    "/webhooks/666/tok%2Fen?wait=true&thread_id=888&with_components=true",
                    "{\"content\":\"hook\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_webhook_message",
                    call_rest_get_webhook_message,
                    "GET",
                    "/webhooks/666/tok%2Fen/messages/777?thread_id=888",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_webhook_message_thread",
                    call_rest_get_webhook_message_thread,
                    "GET",
                    "/webhooks/666/tok%2Fen/messages/777?thread_id=888",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_webhook_message",
                    call_rest_modify_webhook_message,
                    "PATCH",
                    "/webhooks/666/tok%2Fen/messages/777?thread_id=888",
                    "{\"content\":\"edited\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_webhook_message_builder",
                    call_rest_modify_webhook_message_builder,
                    "PATCH",
                    "/webhooks/666/tok%2Fen/messages/777?thread_id=888",
                    "{\"content\":\"edited builder\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_webhook_message_thread",
                    call_rest_modify_webhook_message_thread,
                    "PATCH",
                    "/webhooks/666/tok%2Fen/messages/777?thread_id=888",
                    "{\"content\":\"edited\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_webhook_message",
                    call_rest_delete_webhook_message,
                    "DELETE",
                    "/webhooks/666/tok%2Fen/messages/777?thread_id=888",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_webhook_message_thread",
                    call_rest_delete_webhook_message_thread,
                    "DELETE",
                    "/webhooks/666/tok%2Fen/messages/777?thread_id=888",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_application_emojis",
                    call_rest_get_application_emojis,
                    "GET",
                    "/applications/123/emojis",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_application_emoji",
                    call_rest_get_application_emoji,
                    "GET",
                    "/applications/123/emojis/777",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_application_emoji",
                    call_rest_create_application_emoji,
                    "POST",
                    "/applications/123/emojis",
                    "{\"name\":\"appwave\",\"image\":\"data:image/png;base64,AA==\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_application_emoji_params",
                    call_rest_create_application_emoji_params,
                    "POST",
                    "/applications/123/emojis",
                    "{\"name\":\"appwave-typed\",\"image\":\"data:image/png;base64,BB==\",\"roles\":[]}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_application_emoji",
                    call_rest_modify_application_emoji,
                    "PATCH",
                    "/applications/123/emojis/777",
                    "{\"name\":\"appwave2\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_application_emoji_params",
                    call_rest_modify_application_emoji_params,
                    "PATCH",
                    "/applications/123/emojis/777",
                    "{\"name\":\"appwave-edited\",\"roles\":[]}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_application_emoji",
                    call_rest_delete_application_emoji,
                    "DELETE",
                    "/applications/123/emojis/777",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_all_global_commands",
                    call_rest_delete_all_global_commands,
                    "PUT",
                    "/applications/123/commands",
                    "[]"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_all_guild_commands",
                    call_rest_delete_all_guild_commands,
                    "PUT",
                    "/applications/123/guilds/333/commands",
                    "[]"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "bulk_edit_guild_command_permissions",
                    call_rest_bulk_edit_guild_command_permissions,
                    "PUT",
                    "/applications/123/guilds/333/commands/permissions",
                    "[{\"id\":\"444\",\"permissions\":[]}]"
                ) != 0) {
        return 1;
    }
    return 0;
}
#endif
