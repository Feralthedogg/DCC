#include "http_smoke_support.h"

#if !defined(_WIN32)

dcc_status_t call_rest_create_guild_channel(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_guild_channel(client, 333, "{\"name\":\"ops\",\"type\":0}", cb, user_data);
}
dcc_status_t call_rest_create_guild_channel_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_channel_permission_overwrite_t overwrites[] = {
        {
            .id = 555,
            .allow = 1024,
            .deny = 0,
            .type = DCC_CHANNEL_OVERWRITE_ROLE
        }
    };
    const dcc_channel_forum_tag_params_t tags[] = {
        {
            .size = sizeof(dcc_channel_forum_tag_params_t),
            .name = "news",
            .emoji_name = "star",
            .moderated = 0
        },
        {
            .size = sizeof(dcc_channel_forum_tag_params_t),
            .name = "alerts",
            .emoji_id = 999,
            .moderated = 1
        }
    };
    const dcc_channel_params_t params = {
        .size = sizeof(dcc_channel_params_t),
        .guild_id = 333,
        .type = DCC_CHANNEL_FORUM,
        .name = "ops forum",
        .topic = "forum typed",
        .permission_overwrites = overwrites,
        .permission_overwrite_count = sizeof(overwrites) / sizeof(overwrites[0]),
        .position = 2,
        .rate_limit_per_user = 5,
        .default_thread_rate_limit_per_user = 7,
        .parent_id = 777,
        .nsfw = 1,
        .default_auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR,
        .default_sort_order = 1,
        .lock_permissions = 1,
        .available_tags = tags,
        .available_tag_count = sizeof(tags) / sizeof(tags[0]),
        .default_reaction_emoji_name = "star",
        .default_forum_layout = DCC_CHANNEL_FORUM_LAYOUT_LIST_VIEW,
        .require_tag = 1
    };
    return dcc_rest_create_guild_channel_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_get_guild_channels(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_channels(client, 333, cb, user_data);
}
dcc_status_t call_rest_modify_guild_channel_positions(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_channel_positions(client, 333, "[{\"id\":\"222\",\"position\":1}]", cb, user_data);
}
dcc_status_t call_rest_modify_guild_channel_positions_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_channel_position_t positions[] = {
        {
            .channel_id = 222,
            .position = 1,
            .parent_id = 3330,
            .lock_permissions = 1
        }
    };
    const dcc_channel_positions_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .positions = positions,
        .position_count = sizeof(positions) / sizeof(positions[0])
    };
    return dcc_rest_modify_guild_channel_positions_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_modify_channel_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_channel_params_t params = {
        .size = sizeof(dcc_channel_params_t),
        .guild_id = 333,
        .channel_id = 222,
        .type = DCC_CHANNEL_VOICE,
        .name = "voice",
        .position = 3,
        .nsfw = 0,
        .default_auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_DAY,
        .user_limit = 12,
        .bitrate_kbps = 64
    };
    return dcc_rest_modify_channel_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_modify_channel_media_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_channel_forum_tag_params_t tags[] = {
        {
            .size = sizeof(dcc_channel_forum_tag_params_t),
            .name = "clips",
            .emoji_id = 999,
            .moderated = 1
        }
    };
    const dcc_channel_params_t params = {
        .size = sizeof(dcc_channel_params_t),
        .guild_id = 333,
        .channel_id = 222,
        .type = DCC_CHANNEL_MEDIA,
        .name = "media",
        .nsfw = 1,
        .default_auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_WEEK,
        .default_sort_order = 1,
        .available_tags = tags,
        .available_tag_count = sizeof(tags) / sizeof(tags[0]),
        .default_reaction_emoji_id = 888,
        .require_tag = 1,
        .hide_media_download_options = 1
    };
    return dcc_rest_modify_channel_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_modify_channel_permission(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_channel_permission(client, 222, 555, "{\"allow\":\"1024\",\"deny\":\"0\",\"type\":0}", cb, user_data);
}
dcc_status_t call_rest_modify_channel_permission_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_channel_permission_overwrite_t overwrite = {
        .id = 555,
        .allow = 2048,
        .deny = 1024,
        .type = DCC_CHANNEL_OVERWRITE_MEMBER
    };
    return dcc_rest_modify_channel_permission_params(client, 222, &overwrite, cb, user_data);
}
dcc_status_t call_rest_delete_channel_permission(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_channel_permission(client, 222, 555, cb, user_data);
}
dcc_status_t call_rest_follow_news_channel(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_follow_news_channel(client, 222, "{\"webhook_channel_id\":\"333\"}", cb, user_data);
}
dcc_status_t call_rest_trigger_channel_typing(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_trigger_channel_typing(client, 222, cb, user_data);
}
dcc_status_t call_rest_set_channel_voice_status(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_set_channel_voice_status(client, 222, "{\"status\":\"Live\"}", cb, user_data);
}
dcc_status_t call_rest_set_channel_voice_status_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_channel_voice_status_params_t params = {
        .size = sizeof(params),
        .channel_id = 222,
        .status = "Live typed"
    };
    return dcc_rest_set_channel_voice_status_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_get_channel_invites(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_channel_invites(client, 222, cb, user_data);
}
dcc_status_t call_rest_create_channel_invite(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_channel_invite(client, 222, "{\"max_age\":60}", cb, user_data);
}
dcc_status_t call_rest_create_channel_invite_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_invite_params_t params = {
        .size = sizeof(params),
        .channel_id = 222,
        .max_age = 60,
        .max_uses = 2,
        .target_user_id = 444,
        .target_type = DCC_INVITE_TARGET_STREAM,
        .temporary = 1,
        .unique = 1
    };
    return dcc_rest_create_channel_invite_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_edit_message_flags(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_edit_message_flags(client, 222, 777, "{\"flags\":4}", cb, user_data);
}
dcc_status_t call_rest_edit_message_flags_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_message_flags_params_t params = {
        .size = sizeof(params),
        .channel_id = 222,
        .message_id = 777,
        .flags = 4
    };
    return dcc_rest_edit_message_flags_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_crosspost_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_crosspost_message(client, 222, 777, cb, user_data);
}
dcc_status_t call_rest_bulk_delete_messages(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_bulk_delete_messages(client, 222, "{\"messages\":[\"777\",\"778\"]}", cb, user_data);
}
dcc_status_t call_rest_add_message_reaction(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_add_message_reaction(client, 222, 777, "wave:888", cb, user_data);
}
dcc_status_t call_rest_add_message_reaction_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_message_reaction_params_t params = {
        .size = sizeof(params),
        .channel_id = 222,
        .message_id = 777,
        .reaction = "wave:888"
    };
    return dcc_rest_add_message_reaction_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_delete_own_message_reaction(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_own_message_reaction(client, 222, 777, "wave:888", cb, user_data);
}
dcc_status_t call_rest_delete_user_message_reaction(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_user_message_reaction(client, 222, 777, "wave:888", 444, cb, user_data);
}
dcc_status_t call_rest_get_message_reactions(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_message_reactions(client, 222, 777, "wave:888", "limit=2&after=444", cb, user_data);
}
dcc_status_t call_rest_get_message_reactions_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_message_reactions_page(client, 222, 777, "wave:888", 333, 444, 2, cb, user_data);
}
dcc_status_t call_rest_delete_all_message_reactions(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_all_message_reactions(client, 222, 777, cb, user_data);
}
dcc_status_t call_rest_delete_all_message_reactions_for_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_all_message_reactions_for_emoji(client, 222, 777, "wave:888", cb, user_data);
}
dcc_status_t call_rest_pin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_pin_message(client, 222, 777, cb, user_data);
}
dcc_status_t call_rest_unpin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_unpin_message(client, 222, 777, cb, user_data);
}
dcc_status_t call_rest_get_channel_pins(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_channel_pins(client, 222, "limit=50&before=2024-01-01T00:00:00.000000%2B00:00", cb, user_data);
}
dcc_status_t call_rest_get_channel_pins_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_channel_pins_page(client, 222, "2024-01-01T00:00:00.000000+00:00", 0, cb, user_data);
}
dcc_status_t call_rest_legacy_pin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_legacy_pin_message(client, 222, 777, cb, user_data);
}
dcc_status_t call_rest_legacy_unpin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_legacy_unpin_message(client, 222, 777, cb, user_data);
}
dcc_status_t call_rest_get_legacy_channel_pins(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_legacy_channel_pins(client, 222, "limit=50", cb, user_data);
}
dcc_status_t call_rest_get_poll_answer_voters(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_poll_answer_voters(client, 222, 777, 3, "limit=2&after=444", cb, user_data);
}
dcc_status_t call_rest_get_poll_answer_voters_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_poll_answer_voters_page(client, 222, 777, 3, 444, 2, cb, user_data);
}
dcc_status_t call_rest_end_poll(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_end_poll(client, 222, 777, cb, user_data);
}
dcc_status_t call_rest_get_guild_invites(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_invites(client, 333, cb, user_data);
}
dcc_status_t call_rest_create_guild(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_guild(client, "{\"name\":\"ops\"}", cb, user_data);
}
dcc_status_t call_rest_create_guild_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_guild_params_t params = {
        .size = sizeof(params),
        .name = "ops typed",
        .widget_enabled = 1,
        .afk_channel_id = 222,
        .afk_timeout = DCC_GUILD_AFK_300,
        .widget_channel_id = 223,
        .default_message_notifications = DCC_GUILD_NOTIFY_ONLY_MENTIONS,
        .explicit_content_filter = DCC_GUILD_EXPLICIT_CONTENT_ALL_MEMBERS,
        .mfa_level = DCC_GUILD_MFA_ELEVATED,
        .system_channel_id = 224,
        .premium_progress_bar_enabled = 1,
        .rules_channel_id = 225,
        .vanity_url_code = "vanity",
        .description = "desc",
        .safety_alerts_channel_id = 226,
        .banner = "data:image/png;base64,AA==",
        .discovery_splash = "data:image/png;base64,BB==",
        .splash = "data:image/png;base64,CC==",
        .icon = "data:image/png;base64,DD=="
    };
    return dcc_rest_create_guild_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_modify_guild_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_guild_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .name = "ops edit",
        .widget_enabled = 0,
        .default_message_notifications = DCC_GUILD_NOTIFY_ALL,
        .explicit_content_filter = DCC_GUILD_EXPLICIT_CONTENT_MEMBERS_WITHOUT_ROLES,
        .mfa_level = DCC_GUILD_MFA_NONE,
        .system_channel_id = 224,
        .premium_progress_bar_enabled = 0,
        .description = "desc edit"
    };
    return dcc_rest_modify_guild_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_delete_guild(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_guild(client, 333, cb, user_data);
}
dcc_status_t call_rest_modify_current_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_current_guild_member(client, 333, "{\"nick\":\"ops\"}", cb, user_data);
}
dcc_status_t call_rest_modify_current_guild_member_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_current_guild_member_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .nick = "ops typed",
        .banner = NULL,
        .avatar = "data:image/png;base64,AA==",
        .bio = "bio typed"
    };
    return dcc_rest_modify_current_guild_member_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_set_current_guild_member_nickname(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_set_current_guild_member_nickname(client, 333, "{\"nick\":\"ops\"}", cb, user_data);
}
dcc_status_t call_rest_set_current_guild_member_nickname_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_guild_member_nickname_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .nick = NULL
    };
    return dcc_rest_set_current_guild_member_nickname_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_get_guild_audit_log(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_audit_log(client, 333, "limit=2&action_type=20", cb, user_data);
}
dcc_status_t call_rest_get_guild_audit_log_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_audit_log_page(client, 333, 444, 20, 555, 666, 2, cb, user_data);
}
dcc_status_t call_rest_get_guild_preview(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_preview(client, 333, cb, user_data);
}
dcc_status_t call_rest_get_guild_bans(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_bans(client, 333, "limit=2&after=444", cb, user_data);
}
dcc_status_t call_rest_get_guild_bans_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_bans_page(client, 333, 222, 444, 2, cb, user_data);
}
dcc_status_t call_rest_get_guild_ban(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_ban(client, 333, 444, cb, user_data);
}
dcc_status_t call_rest_create_guild_ban(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_guild_ban(client, 333, 444, "{\"delete_message_seconds\":60}", cb, user_data);
}
dcc_status_t call_rest_create_guild_ban_seconds(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_guild_ban_seconds(client, 333, 444, 604801, cb, user_data);
}
dcc_status_t call_rest_create_guild_ban_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_guild_ban_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .user_id = 444,
        .delete_message_seconds = 60
    };
    return dcc_rest_create_guild_ban_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_delete_guild_ban(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_guild_ban(client, 333, 444, cb, user_data);
}
dcc_status_t call_rest_get_guild_integrations(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_integrations(client, 333, cb, user_data);
}
dcc_status_t call_rest_modify_guild_integration(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_integration(client, 333, 555, "{\"expire_behavior\":0}", cb, user_data);
}
dcc_status_t call_rest_delete_guild_integration(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_guild_integration(client, 333, 555, cb, user_data);
}
dcc_status_t call_rest_sync_guild_integration(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_sync_guild_integration(client, 333, 555, cb, user_data);
}
dcc_status_t call_rest_get_guild_widget(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_widget(client, 333, cb, user_data);
}

#endif
