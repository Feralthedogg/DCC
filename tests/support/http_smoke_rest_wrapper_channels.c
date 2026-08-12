#include "http_smoke_support.h"

#if !defined(_WIN32)

int run_public_rest_wrapper_expect_channels_smoke(dcc_client_t *client) {
    if (
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_channel",
                    call_rest_create_guild_channel,
                    "POST",
                    "/guilds/333/channels",
                    "{\"name\":\"ops\",\"type\":0}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_channel_params",
                    call_rest_create_guild_channel_params,
                    "POST",
                    "/guilds/333/channels",
                    "{\"name\":\"ops forum\",\"type\":15,\"topic\":\"forum typed\",\"rate_limit_per_user\":5,\"position\":2,\"permission_overwrites\":[{\"id\":\"555\",\"allow\":\"1024\",\"deny\":\"0\",\"type\":0}],\"parent_id\":\"777\",\"nsfw\":true,\"default_auto_archive_duration\":60,\"default_reaction_emoji\":{\"emoji_name\":\"star\"},\"available_tags\":[{\"name\":\"news\",\"moderated\":false,\"emoji_name\":\"star\"},{\"name\":\"alerts\",\"moderated\":true,\"emoji_id\":\"999\"}],\"default_sort_order\":1,\"default_forum_layout\":1,\"default_thread_rate_limit_per_user\":7,\"flags\":16}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_channels",
                    call_rest_get_guild_channels,
                    "GET",
                    "/guilds/333/channels",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_channel_positions",
                    call_rest_modify_guild_channel_positions,
                    "PATCH",
                    "/guilds/333/channels",
                    "[{\"id\":\"222\",\"position\":1}]"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_channel_positions_params",
                    call_rest_modify_guild_channel_positions_params,
                    "PATCH",
                    "/guilds/333/channels",
                    "[{\"id\":\"222\",\"position\":1,\"lock_permissions\":true,\"parent_id\":\"3330\"}]"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_channel_params",
                    call_rest_modify_channel_params,
                    "PATCH",
                    "/channels/222",
                    "{\"name\":\"voice\",\"type\":2,\"bitrate\":64000,\"user_limit\":12,\"position\":3,\"parent_id\":null,\"nsfw\":false}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_channel_media_params",
                    call_rest_modify_channel_media_params,
                    "PATCH",
                    "/channels/222",
                    "{\"name\":\"media\",\"type\":16,\"nsfw\":true,\"default_auto_archive_duration\":10080,\"default_reaction_emoji\":{\"emoji_id\":\"888\"},\"available_tags\":[{\"name\":\"clips\",\"moderated\":true,\"emoji_id\":\"999\"}],\"default_sort_order\":1,\"flags\":32784}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_channel_permission",
                    call_rest_modify_channel_permission,
                    "PUT",
                    "/channels/222/permissions/555",
                    "{\"allow\":\"1024\",\"deny\":\"0\",\"type\":0}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_channel_permission_params",
                    call_rest_modify_channel_permission_params,
                    "PUT",
                    "/channels/222/permissions/555",
                    "{\"allow\":\"2048\",\"deny\":\"1024\",\"type\":1}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_channel_permission",
                    call_rest_delete_channel_permission,
                    "DELETE",
                    "/channels/222/permissions/555",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "follow_news_channel",
                    call_rest_follow_news_channel,
                    "POST",
                    "/channels/222/followers",
                    "{\"webhook_channel_id\":\"333\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "trigger_channel_typing",
                    call_rest_trigger_channel_typing,
                    "POST",
                    "/channels/222/typing",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "set_channel_voice_status",
                    call_rest_set_channel_voice_status,
                    "PUT",
                    "/channels/222/voice-status",
                    "{\"status\":\"Live\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "set_channel_voice_status_params",
                    call_rest_set_channel_voice_status_params,
                    "PUT",
                    "/channels/222/voice-status",
                    "{\"status\":\"Live typed\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_channel_invites",
                    call_rest_get_channel_invites,
                    "GET",
                    "/channels/222/invites",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_channel_invite",
                    call_rest_create_channel_invite,
                    "POST",
                    "/channels/222/invites",
                    "{\"max_age\":60}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_channel_invite_params",
                    call_rest_create_channel_invite_params,
                    "POST",
                    "/channels/222/invites",
                    "{\"max_age\":60,\"max_uses\":2,\"temporary\":true,\"unique\":true,\"target_type\":1,\"target_user_id\":\"444\",\"role_ids\":[\"555\"]}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "edit_message_flags",
                    call_rest_edit_message_flags,
                    "PATCH",
                    "/channels/222/messages/777",
                    "{\"flags\":4}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "edit_message_flags_params",
                    call_rest_edit_message_flags_params,
                    "PATCH",
                    "/channels/222/messages/777",
                    "{\"flags\":4}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "crosspost_message",
                    call_rest_crosspost_message,
                    "POST",
                    "/channels/222/messages/777/crosspost",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "bulk_delete_messages",
                    call_rest_bulk_delete_messages,
                    "POST",
                    "/channels/222/messages/bulk-delete",
                    "{\"messages\":[\"777\",\"778\"]}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "add_message_reaction",
                    call_rest_add_message_reaction,
                    "PUT",
                    "/channels/222/messages/777/reactions/wave%3A888/@me",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "add_message_reaction_params",
                    call_rest_add_message_reaction_params,
                    "PUT",
                    "/channels/222/messages/777/reactions/wave%3A888/@me",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_own_message_reaction",
                    call_rest_delete_own_message_reaction,
                    "DELETE",
                    "/channels/222/messages/777/reactions/wave%3A888/@me",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_user_message_reaction",
                    call_rest_delete_user_message_reaction,
                    "DELETE",
                    "/channels/222/messages/777/reactions/wave%3A888/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_message_reactions",
                    call_rest_get_message_reactions,
                    "GET",
                    "/channels/222/messages/777/reactions/wave%3A888?after=444&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_message_reactions_page",
                    call_rest_get_message_reactions_page,
                    "GET",
                    "/channels/222/messages/777/reactions/wave%3A888?type=1&after=444&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_all_message_reactions",
                    call_rest_delete_all_message_reactions,
                    "DELETE",
                    "/channels/222/messages/777/reactions",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_all_message_reactions_for_emoji",
                    call_rest_delete_all_message_reactions_for_emoji,
                    "DELETE",
                    "/channels/222/messages/777/reactions/wave%3A888",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "pin_message",
                    call_rest_pin_message,
                    "PUT",
                    "/channels/222/messages/pins/777",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "unpin_message",
                    call_rest_unpin_message,
                    "DELETE",
                    "/channels/222/messages/pins/777",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_channel_pins",
                    call_rest_get_channel_pins,
                    "GET",
                    "/channels/222/messages/pins?before=2024-01-01T00%3A00%3A00.000000%2B00%3A00&limit=50",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_channel_pins_page",
                    call_rest_get_channel_pins_page,
                    "GET",
                    "/channels/222/messages/pins?before=2024-01-01T00%3A00%3A00.000000%2B00%3A00",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "legacy_pin_message",
                    call_rest_legacy_pin_message,
                    "PUT",
                    "/channels/222/pins/777",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "legacy_unpin_message",
                    call_rest_legacy_unpin_message,
                    "DELETE",
                    "/channels/222/pins/777",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_legacy_channel_pins",
                    call_rest_get_legacy_channel_pins,
                    "GET",
                    "/channels/222/pins",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_poll_answer_voters",
                    call_rest_get_poll_answer_voters,
                    "GET",
                    "/channels/222/polls/777/answers/3?after=444&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_poll_answer_voters_page",
                    call_rest_get_poll_answer_voters_page,
                    "GET",
                    "/channels/222/polls/777/answers/3?after=444&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "end_poll",
                    call_rest_end_poll,
                    "POST",
                    "/channels/222/polls/777/expire",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_invites",
                    call_rest_get_guild_invites,
                    "GET",
                    "/guilds/333/invites",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_params",
                    call_rest_modify_guild_params,
                    "PATCH",
                    "/guilds/333",
                    "{\"name\":\"ops edit\",\"default_message_notifications\":0,\"explicit_content_filter\":1,\"system_channel_id\":\"224\",\"description\":\"desc edit\",\"premium_progress_bar_enabled\":false}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_current_guild_member",
                    call_rest_modify_current_guild_member,
                    "PATCH",
                    "/guilds/333/members/@me",
                    "{\"nick\":\"ops\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_current_guild_member_params",
                    call_rest_modify_current_guild_member_params,
                    "PATCH",
                    "/guilds/333/members/@me",
                    "{\"nick\":\"ops typed\",\"avatar\":\"data:image/png;base64,AA==\",\"bio\":\"bio typed\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "set_current_guild_member_nickname",
                    call_rest_set_current_guild_member_nickname,
                    "PATCH",
                    "/guilds/333/members/@me/nick",
                    "{\"nick\":\"ops\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "set_current_guild_member_nickname_params",
                    call_rest_set_current_guild_member_nickname_params,
                    "PATCH",
                    "/guilds/333/members/@me/nick",
                    "{\"nick\":null}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_audit_log",
                    call_rest_get_guild_audit_log,
                    "GET",
                    "/guilds/333/audit-logs?action_type=20&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_audit_log_page",
                    call_rest_get_guild_audit_log_page,
                    "GET",
                    "/guilds/333/audit-logs?user_id=444&action_type=20&before=555&after=666&limit=2",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_preview",
                    call_rest_get_guild_preview,
                    "GET",
                    "/guilds/333/preview",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_bans",
                    call_rest_get_guild_bans,
                    "GET",
                    "/guilds/333/bans?limit=2&after=444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_bans_page",
                    call_rest_get_guild_bans_page,
                    "GET",
                    "/guilds/333/bans?limit=2&before=222&after=444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_ban",
                    call_rest_get_guild_ban,
                    "GET",
                    "/guilds/333/bans/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_ban",
                    call_rest_create_guild_ban,
                    "PUT",
                    "/guilds/333/bans/444",
                    "{\"delete_message_seconds\":60}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_ban_seconds",
                    call_rest_create_guild_ban_seconds,
                    "PUT",
                    "/guilds/333/bans/444",
                    "{\"delete_message_seconds\":604800}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_ban_params",
                    call_rest_create_guild_ban_params,
                    "PUT",
                    "/guilds/333/bans/444",
                    "{\"delete_message_seconds\":60}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_guild_ban",
                    call_rest_delete_guild_ban,
                    "DELETE",
                    "/guilds/333/bans/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_integrations",
                    call_rest_get_guild_integrations,
                    "GET",
                    "/guilds/333/integrations",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_guild_integration",
                    call_rest_delete_guild_integration,
                    "DELETE",
                    "/guilds/333/integrations/555",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_widget",
                    call_rest_get_guild_widget,
                    "GET",
                    "/guilds/333/widget",
                    NULL
                ) != 0) {
        return 1;
    }
    return 0;
}
#endif
