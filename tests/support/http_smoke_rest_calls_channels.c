#include "http_smoke_support.h"

#if !defined(_WIN32)

dcc_status_t call_rest_create_guild_channel(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.payload.guild.present = DCC_CHANNEL_GUILD_PRESENT_NAME |
        DCC_CHANNEL_GUILD_PRESENT_TYPE;
    params.payload.guild.name = "ops";
    params.payload.guild.type = DCC_CHANNEL_TEXT;
    return dcc_rest_create_guild_channel(client, 333, &params, &options, NULL);
}
dcc_status_t call_rest_create_guild_channel_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_channel_permission_overwrite_t overwrites[] = {
        {
            .size = sizeof(dcc_channel_permission_overwrite_t),
            .version = DCC_CHANNEL_PERMISSION_OVERWRITE_VERSION,
            .present = DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_ALLOW |
                DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_DENY,
            .id = 555,
            .allow = 1024,
            .deny = 0,
            .type = DCC_CHANNEL_OVERWRITE_ROLE
        }
    };
    const dcc_channel_forum_tag_params_t tags[] = {
        {
            .size = sizeof(dcc_channel_forum_tag_params_t),
            .version = DCC_CHANNEL_FORUM_TAG_PARAMS_VERSION,
            .present = DCC_CHANNEL_FORUM_TAG_PRESENT_NAME |
                DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_NAME |
                DCC_CHANNEL_FORUM_TAG_PRESENT_MODERATED,
            .name = "news",
            .emoji_name = "star",
            .moderated = 0
        },
        {
            .size = sizeof(dcc_channel_forum_tag_params_t),
            .version = DCC_CHANNEL_FORUM_TAG_PARAMS_VERSION,
            .present = DCC_CHANNEL_FORUM_TAG_PRESENT_NAME |
                DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_ID |
                DCC_CHANNEL_FORUM_TAG_PRESENT_MODERATED,
            .name = "alerts",
            .emoji_id = 999,
            .moderated = 1
        }
    };
    dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.payload.guild.present = DCC_CHANNEL_GUILD_PRESENT_NAME |
        DCC_CHANNEL_GUILD_PRESENT_TYPE | DCC_CHANNEL_GUILD_PRESENT_TOPIC |
        DCC_CHANNEL_GUILD_PRESENT_PERMISSION_OVERWRITES |
        DCC_CHANNEL_GUILD_PRESENT_POSITION |
        DCC_CHANNEL_GUILD_PRESENT_RATE_LIMIT_PER_USER |
        DCC_CHANNEL_GUILD_PRESENT_DEFAULT_THREAD_RATE_LIMIT_PER_USER |
        DCC_CHANNEL_GUILD_PRESENT_FLAGS | DCC_CHANNEL_GUILD_PRESENT_PARENT_ID |
        DCC_CHANNEL_GUILD_PRESENT_NSFW |
        DCC_CHANNEL_GUILD_PRESENT_DEFAULT_AUTO_ARCHIVE_DURATION |
        DCC_CHANNEL_GUILD_PRESENT_AVAILABLE_TAGS |
        DCC_CHANNEL_GUILD_PRESENT_DEFAULT_REACTION_EMOJI |
        DCC_CHANNEL_GUILD_PRESENT_DEFAULT_SORT_ORDER |
        DCC_CHANNEL_GUILD_PRESENT_DEFAULT_FORUM_LAYOUT;
    params.payload.guild.name = "ops forum";
    params.payload.guild.type = DCC_CHANNEL_FORUM;
    params.payload.guild.topic = "forum typed";
    params.payload.guild.permission_overwrites = overwrites;
    params.payload.guild.permission_overwrite_count = sizeof(overwrites) / sizeof(overwrites[0]);
    params.payload.guild.position = 2;
    params.payload.guild.rate_limit_per_user = 5;
    params.payload.guild.default_thread_rate_limit_per_user = 7;
    params.payload.guild.flags = 16U;
    params.payload.guild.parent_id = 777;
    params.payload.guild.nsfw = 1;
    params.payload.guild.default_auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR;
    params.payload.guild.available_tags = tags;
    params.payload.guild.available_tag_count = sizeof(tags) / sizeof(tags[0]);
    params.payload.guild.default_reaction_emoji.emoji_name = "star";
    params.payload.guild.default_sort_order = 1;
    params.payload.guild.default_forum_layout = DCC_CHANNEL_FORUM_LAYOUT_LIST_VIEW;
    return dcc_rest_create_guild_channel(client, 333, &params, &options, NULL);
}
dcc_status_t call_rest_get_guild_channels(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_get_guild_channels(client, 333, &options, NULL);
}
dcc_status_t call_rest_modify_guild_channel_positions(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_channel_position_t position = DCC_CHANNEL_POSITION_INIT;
    dcc_channel_positions_params_t params = DCC_CHANNEL_POSITIONS_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    position.present = DCC_CHANNEL_POSITION_PRESENT_POSITION;
    position.channel_id = 222;
    position.position = 1;
    params.positions = &position;
    params.position_count = 1U;
    return dcc_rest_modify_guild_channel_positions(client, 333, &params, &options, NULL);
}
dcc_status_t call_rest_modify_guild_channel_positions_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_channel_position_t position = DCC_CHANNEL_POSITION_INIT;
    dcc_channel_positions_params_t params = DCC_CHANNEL_POSITIONS_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    position.present = DCC_CHANNEL_POSITION_PRESENT_POSITION |
        DCC_CHANNEL_POSITION_PRESENT_PARENT_ID |
        DCC_CHANNEL_POSITION_PRESENT_LOCK_PERMISSIONS;
    position.channel_id = 222;
    position.position = 1;
    position.parent_id = 3330;
    position.lock_permissions = 1;
    params.positions = &position;
    params.position_count = 1U;
    return dcc_rest_modify_guild_channel_positions(client, 333, &params, &options, NULL);
}
dcc_status_t call_rest_modify_channel_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.payload.guild.present = DCC_CHANNEL_GUILD_PRESENT_TYPE |
        DCC_CHANNEL_GUILD_PRESENT_NAME | DCC_CHANNEL_GUILD_PRESENT_POSITION |
        DCC_CHANNEL_GUILD_PRESENT_NSFW |
        DCC_CHANNEL_GUILD_PRESENT_USER_LIMIT | DCC_CHANNEL_GUILD_PRESENT_BITRATE |
        DCC_CHANNEL_GUILD_PRESENT_PARENT_ID;
    params.payload.guild.type = DCC_CHANNEL_VOICE;
    params.payload.guild.name = "voice";
    params.payload.guild.position = 3;
    params.payload.guild.nsfw = 0;
    params.payload.guild.user_limit = 12;
    params.payload.guild.bitrate = 64000;
    params.payload.guild.nulls = DCC_CHANNEL_GUILD_NULL_PARENT_ID;
    return dcc_rest_modify_channel(client, 222, &params, &options, NULL);
}
dcc_status_t call_rest_modify_channel_media_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_channel_forum_tag_params_t tags[] = {
        {
            .size = sizeof(dcc_channel_forum_tag_params_t),
            .version = DCC_CHANNEL_FORUM_TAG_PARAMS_VERSION,
            .present = DCC_CHANNEL_FORUM_TAG_PRESENT_NAME |
                DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_ID |
                DCC_CHANNEL_FORUM_TAG_PRESENT_MODERATED,
            .name = "clips",
            .emoji_id = 999,
            .moderated = 1
        }
    };
    dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.payload.guild.present = DCC_CHANNEL_GUILD_PRESENT_TYPE |
        DCC_CHANNEL_GUILD_PRESENT_NAME | DCC_CHANNEL_GUILD_PRESENT_NSFW |
        DCC_CHANNEL_GUILD_PRESENT_DEFAULT_AUTO_ARCHIVE_DURATION |
        DCC_CHANNEL_GUILD_PRESENT_DEFAULT_SORT_ORDER |
        DCC_CHANNEL_GUILD_PRESENT_AVAILABLE_TAGS |
        DCC_CHANNEL_GUILD_PRESENT_DEFAULT_REACTION_EMOJI |
        DCC_CHANNEL_GUILD_PRESENT_FLAGS;
    params.payload.guild.type = DCC_CHANNEL_MEDIA;
    params.payload.guild.name = "media";
    params.payload.guild.nsfw = 1;
    params.payload.guild.default_auto_archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_WEEK;
    params.payload.guild.default_sort_order = 1;
    params.payload.guild.available_tags = tags;
    params.payload.guild.available_tag_count = sizeof(tags) / sizeof(tags[0]);
    params.payload.guild.default_reaction_emoji.emoji_id = 888;
    params.payload.guild.flags = 32784U;
    return dcc_rest_modify_channel(client, 222, &params, &options, NULL);
}
dcc_status_t call_rest_modify_channel_permission(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_channel_permission_overwrite_t overwrite = DCC_CHANNEL_PERMISSION_OVERWRITE_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    overwrite.present = DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_ALLOW |
        DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_DENY;
    overwrite.allow = 1024;
    overwrite.deny = 0;
    overwrite.type = DCC_CHANNEL_OVERWRITE_ROLE;
    return dcc_rest_modify_channel_permission(client, 222, 555, &overwrite, &options, NULL);
}
dcc_status_t call_rest_modify_channel_permission_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_channel_permission_overwrite_t overwrite = DCC_CHANNEL_PERMISSION_OVERWRITE_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    overwrite.present = DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_ALLOW |
        DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_DENY;
    overwrite.id = 555;
    overwrite.allow = 2048;
    overwrite.deny = 1024;
    overwrite.type = DCC_CHANNEL_OVERWRITE_MEMBER;
    return dcc_rest_modify_channel_permission(client, 222, 555, &overwrite, &options, NULL);
}
dcc_status_t call_rest_delete_channel_permission(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_delete_channel_permission(client, 222, 555, &options, NULL);
}
dcc_status_t call_rest_follow_news_channel(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_follow_news_channel_t follow = DCC_REST_FOLLOW_NEWS_CHANNEL_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    follow.webhook_channel_id = 333;
    return dcc_rest_follow_news_channel(client, 222, &follow, &options, NULL);
}
dcc_status_t call_rest_trigger_channel_typing(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_trigger_channel_typing(client, 222, &options, NULL);
}
dcc_status_t call_rest_set_channel_voice_status(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_channel_voice_status_params_t params = DCC_CHANNEL_VOICE_STATUS_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.present = DCC_CHANNEL_VOICE_STATUS_PRESENT_STATUS;
    params.status = "Live";
    return dcc_rest_set_channel_voice_status(client, 222, &params, &options, NULL);
}
dcc_status_t call_rest_set_channel_voice_status_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_channel_voice_status_params_t params = DCC_CHANNEL_VOICE_STATUS_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.present = DCC_CHANNEL_VOICE_STATUS_PRESENT_STATUS;
    params.status = "Live typed";
    return dcc_rest_set_channel_voice_status(client, 222, &params, &options, NULL);
}
dcc_status_t call_rest_get_channel_invites(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_get_channel_invites(client, 222, &options, NULL);
}
dcc_status_t call_rest_create_channel_invite(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_invite_params_t params = DCC_INVITE_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.present = DCC_INVITE_PARAMS_PRESENT_MAX_AGE;
    params.max_age = 60;
    return dcc_rest_create_channel_invite(client, 222, &params, &options, NULL);
}
dcc_status_t call_rest_create_channel_invite_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_invite_params_t params = DCC_INVITE_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    const dcc_snowflake_t role_ids[] = {555};
    params.present = DCC_INVITE_PARAMS_PRESENT_MAX_AGE |
        DCC_INVITE_PARAMS_PRESENT_MAX_USES |
        DCC_INVITE_PARAMS_PRESENT_TARGET_USER_ID |
        DCC_INVITE_PARAMS_PRESENT_TARGET_TYPE |
        DCC_INVITE_PARAMS_PRESENT_TEMPORARY |
        DCC_INVITE_PARAMS_PRESENT_UNIQUE |
        DCC_INVITE_PARAMS_PRESENT_ROLE_IDS;
    params.max_age = 60;
    params.max_uses = 2;
    params.target_user_id = 444;
    params.target_type = DCC_INVITE_TARGET_STREAM;
    params.temporary = 1;
    params.unique = 1;
    params.role_ids = role_ids;
    params.role_id_count = 1U;
    return dcc_rest_create_channel_invite(client, 222, &params, &options, NULL);
}
dcc_status_t call_rest_edit_message_flags(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    dcc_status_t status = dcc_message_builder_set_flags(&message, 4U);
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    payload.message = &message;
    return status == DCC_OK
        ? dcc_rest_edit_message(client, 222, 777, &payload, &options, NULL)
        : status;
}
dcc_status_t call_rest_edit_message_flags_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return call_rest_edit_message_flags(client, cb, user_data);
}
dcc_status_t call_rest_crosspost_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_crosspost_message(client, 222, 777, &options, NULL);
}
dcc_status_t call_rest_bulk_delete_messages(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_snowflake_t message_ids[] = {777, 778};
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_bulk_delete_messages(client, 222, message_ids, 2U, &options, NULL);
}
dcc_status_t call_rest_add_message_reaction(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_add_message_reaction(client, 222, 777, "wave:888", &options, NULL);
}
dcc_status_t call_rest_add_message_reaction_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return call_rest_add_message_reaction(client, cb, user_data);
}
dcc_status_t call_rest_delete_own_message_reaction(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_delete_own_message_reaction(client, 222, 777, "wave:888", &options, NULL);
}
dcc_status_t call_rest_delete_user_message_reaction(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_delete_user_message_reaction(client, 222, 777, "wave:888", 444, &options, NULL);
}
dcc_status_t call_rest_get_message_reactions(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_reaction_query_t query = DCC_REST_REACTION_QUERY_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    query.present = DCC_REST_REACTION_QUERY_PRESENT_AFTER |
        DCC_REST_REACTION_QUERY_PRESENT_LIMIT;
    query.after = 444;
    query.limit = 2;
    return dcc_rest_get_message_reactions(client, 222, 777, "wave:888", &query, &options, NULL);
}
dcc_status_t call_rest_get_message_reactions_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_reaction_query_t query = DCC_REST_REACTION_QUERY_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    query.present = DCC_REST_REACTION_QUERY_PRESENT_TYPE |
        DCC_REST_REACTION_QUERY_PRESENT_AFTER |
        DCC_REST_REACTION_QUERY_PRESENT_LIMIT;
    query.type = DCC_REST_REACTION_BURST;
    query.after = 444;
    query.limit = 2;
    return dcc_rest_get_message_reactions(client, 222, 777, "wave:888", &query, &options, NULL);
}
dcc_status_t call_rest_delete_all_message_reactions(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_delete_all_message_reactions(client, 222, 777, &options, NULL);
}
dcc_status_t call_rest_delete_all_message_reactions_for_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_delete_all_message_reactions_for_emoji(client, 222, 777, "wave:888", &options, NULL);
}
dcc_status_t call_rest_pin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_pin_message(client, 222, 777, &options, NULL);
}
dcc_status_t call_rest_unpin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_unpin_message(client, 222, 777, &options, NULL);
}
dcc_status_t call_rest_get_channel_pins(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_pin_page_t query = DCC_REST_PIN_PAGE_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    query.present = DCC_REST_PIN_PAGE_PRESENT_BEFORE |
        DCC_REST_PIN_PAGE_PRESENT_LIMIT;
    query.before = "2024-01-01T00:00:00.000000+00:00";
    query.limit = 50;
    return dcc_rest_get_channel_pins(client, 222, &query, &options, NULL);
}
dcc_status_t call_rest_get_channel_pins_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_pin_page_t query = DCC_REST_PIN_PAGE_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    query.present = DCC_REST_PIN_PAGE_PRESENT_BEFORE;
    query.before = "2024-01-01T00:00:00.000000+00:00";
    return dcc_rest_get_channel_pins(client, 222, &query, &options, NULL);
}
dcc_status_t call_rest_legacy_pin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_legacy_pin_message(client, 222, 777, &options, NULL);
}
dcc_status_t call_rest_legacy_unpin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_legacy_unpin_message(client, 222, 777, &options, NULL);
}
dcc_status_t call_rest_get_legacy_channel_pins(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_get_legacy_channel_pins(client, 222, &options, NULL);
}
dcc_status_t call_rest_get_poll_answer_voters(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_id_page_t page = DCC_REST_ID_PAGE_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    page.present = DCC_REST_ID_PAGE_PRESENT_AFTER | DCC_REST_ID_PAGE_PRESENT_LIMIT;
    page.after = 444;
    page.limit = 2;
    return dcc_rest_get_poll_answer_voters(client, 222, 777, 3, &page, &options, NULL);
}
dcc_status_t call_rest_get_poll_answer_voters_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return call_rest_get_poll_answer_voters(client, cb, user_data);
}
dcc_status_t call_rest_end_poll(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_end_poll(client, 222, 777, &options, NULL);
}
dcc_status_t call_rest_get_guild_invites(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    return dcc_rest_get_guild_invites(client, 333, &options, NULL);
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
dcc_status_t call_rest_delete_guild_integration(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_guild_integration(client, 333, 555, cb, user_data);
}
dcc_status_t call_rest_get_guild_widget(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_widget(client, 333, cb, user_data);
}

#endif
