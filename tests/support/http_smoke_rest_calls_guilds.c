#include "http_smoke_support.h"
#include "task9_test_legacy_shims.h"

#if !defined(_WIN32)

dcc_status_t call_rest_modify_guild_widget(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_guild_widget_update_t body = DCC_REST_GUILD_WIDGET_UPDATE_INIT;
    body.present = DCC_REST_GUILD_WIDGET_UPDATE_PRESENT_ENABLED |
        DCC_REST_GUILD_WIDGET_UPDATE_PRESENT_CHANNEL_ID;
    body.enabled=1;body.channel_id=222;
    dcc_rest_call_options_t options=rest_call_options_from_legacy(cb,user_data);
    return dcc_rest_modify_guild_widget(client,333,&body,&options,NULL);
}
dcc_status_t call_rest_modify_guild_widget_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return call_rest_modify_guild_widget(client,cb,user_data);
}
dcc_status_t call_rest_get_guild_vanity_url(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options=rest_call_options_from_legacy(cb,user_data);return dcc_rest_get_guild_vanity_url(client,333,&options,NULL);
}
dcc_status_t call_rest_get_guild_prune_count(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_snowflake_t roles[]={555};dcc_rest_guild_prune_query_t query=DCC_REST_GUILD_PRUNE_QUERY_INIT;query.present=DCC_REST_GUILD_PRUNE_QUERY_PRESENT_DAYS|DCC_REST_GUILD_PRUNE_QUERY_PRESENT_INCLUDE_ROLES;query.days=7;query.include_roles=roles;query.include_role_count=1;dcc_rest_call_options_t options=rest_call_options_from_legacy(cb,user_data);return dcc_rest_get_guild_prune_count(client,333,&query,&options,NULL);
}
dcc_status_t call_rest_get_guild_prune_count_options(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_snowflake_t roles[]={555,556};dcc_rest_guild_prune_query_t query=DCC_REST_GUILD_PRUNE_QUERY_INIT;query.present=DCC_REST_GUILD_PRUNE_QUERY_PRESENT_DAYS|DCC_REST_GUILD_PRUNE_QUERY_PRESENT_INCLUDE_ROLES;query.days=7;query.include_roles=roles;query.include_role_count=2;dcc_rest_call_options_t options=rest_call_options_from_legacy(cb,user_data);return dcc_rest_get_guild_prune_count(client,333,&query,&options,NULL);
}
dcc_status_t call_rest_begin_guild_prune(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_guild_prune_t body=DCC_REST_GUILD_PRUNE_INIT;body.present=DCC_REST_GUILD_PRUNE_PRESENT_DAYS|DCC_REST_GUILD_PRUNE_PRESENT_COMPUTE_PRUNE_COUNT;body.days=7;dcc_rest_call_options_t options=rest_call_options_from_legacy(cb,user_data);return dcc_rest_begin_guild_prune(client,333,&body,&options,NULL);
}
dcc_status_t call_rest_begin_guild_prune_options(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_snowflake_t roles[]={555,556};dcc_rest_guild_prune_t body=DCC_REST_GUILD_PRUNE_INIT;body.present=DCC_REST_GUILD_PRUNE_PRESENT_DAYS|DCC_REST_GUILD_PRUNE_PRESENT_INCLUDE_ROLES|DCC_REST_GUILD_PRUNE_PRESENT_COMPUTE_PRUNE_COUNT;body.days=7;body.include_roles=roles;body.include_role_count=2;dcc_rest_call_options_t options=rest_call_options_from_legacy(cb,user_data);return dcc_rest_begin_guild_prune(client,333,&body,&options,NULL);
}
dcc_status_t call_rest_begin_guild_prune_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_snowflake_t roles[]={555,556};dcc_rest_guild_prune_t body=DCC_REST_GUILD_PRUNE_INIT;body.present=DCC_REST_GUILD_PRUNE_PRESENT_DAYS|DCC_REST_GUILD_PRUNE_PRESENT_INCLUDE_ROLES|DCC_REST_GUILD_PRUNE_PRESENT_COMPUTE_PRUNE_COUNT;body.days=7;body.include_roles=roles;body.include_role_count=2;body.compute_prune_count=1;dcc_rest_call_options_t options=rest_call_options_from_legacy(cb,user_data);return dcc_rest_begin_guild_prune(client,333,&body,&options,NULL);
}
dcc_status_t call_rest_get_guild_onboarding(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_onboarding(client, 333, cb, user_data);
}
dcc_status_t call_rest_modify_guild_onboarding(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_onboarding(client, 333, "{\"enabled\":true,\"prompts\":[]}", cb, user_data);
}
dcc_status_t call_rest_modify_guild_onboarding_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_snowflake_t emoji_roles[] = {777};
    const dcc_onboarding_emoji_params_t emoji = {
        .size = sizeof(emoji),
        .name = "wave",
        .image = "data:image/png;base64,AA==",
        .roles = emoji_roles,
        .role_count = sizeof(emoji_roles) / sizeof(emoji_roles[0])
    };
    const dcc_snowflake_t option_channels[] = {222, 223};
    const dcc_snowflake_t option_roles[] = {555, 556};
    const dcc_onboarding_prompt_option_params_t options[] = {
        {
            .size = sizeof(options[0]),
            .emoji = &emoji,
            .title = "News",
            .description = "Announcements",
            .channel_ids = option_channels,
            .channel_id_count = sizeof(option_channels) / sizeof(option_channels[0]),
            .role_ids = option_roles,
            .role_id_count = sizeof(option_roles) / sizeof(option_roles[0])
        }
    };
    const dcc_onboarding_prompt_params_t prompts[] = {
        {
            .size = sizeof(prompts[0]),
            .type = DCC_ONBOARDING_PROMPT_DROPDOWN,
            .title = "Pick feeds",
            .options = options,
            .option_count = sizeof(options) / sizeof(options[0]),
            .flags = DCC_ONBOARDING_PROMPT_SINGLE_SELECT |
                     DCC_ONBOARDING_PROMPT_REQUIRED |
                     DCC_ONBOARDING_PROMPT_IN_ONBOARDING
        }
    };
    const dcc_snowflake_t default_channels[] = {222};
    const dcc_onboarding_params_t params = {
        .size = sizeof(params),
        .guild_id = 333,
        .prompts = prompts,
        .prompt_count = sizeof(prompts) / sizeof(prompts[0]),
        .default_channel_ids = default_channels,
        .default_channel_id_count = sizeof(default_channels) / sizeof(default_channels[0]),
        .enabled = 1,
        .mode = DCC_ONBOARDING_MODE_ADVANCED
    };
    return dcc_rest_modify_guild_onboarding_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_get_guild_welcome_screen(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_call_options_t options=rest_call_options_from_legacy(cb,user_data);return dcc_rest_get_guild_welcome_screen(client,333,&options,NULL);
}
dcc_status_t call_rest_modify_guild_welcome_screen(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_guild_welcome_screen_update_t body=DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_INIT;body.present=DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_ENABLED|DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_WELCOME_CHANNELS;body.enabled=1;dcc_rest_call_options_t options=rest_call_options_from_legacy(cb,user_data);return dcc_rest_modify_guild_welcome_screen(client,333,&body,&options,NULL);
}
dcc_status_t call_rest_modify_guild_welcome_screen_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_welcome_channel_t channel=DCC_REST_WELCOME_CHANNEL_INIT(222,"Read first");channel.present=DCC_REST_WELCOME_CHANNEL_PRESENT_EMOJI_NAME;channel.emoji_name="wave";dcc_rest_guild_welcome_screen_update_t body=DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_INIT;body.present=DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_ENABLED|DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_WELCOME_CHANNELS|DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_DESCRIPTION;body.enabled=1;body.welcome_channels=&channel;body.welcome_channel_count=1;body.description="Welcome home";dcc_rest_call_options_t options=rest_call_options_from_legacy(cb,user_data);return dcc_rest_modify_guild_welcome_screen(client,333,&body,&options,NULL);
}
dcc_status_t call_rest_get_auto_moderation_rules(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_auto_moderation_rules(client, 333, cb, user_data);
}
dcc_status_t call_rest_get_auto_moderation_rule(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_auto_moderation_rule(client, 333, 999, cb, user_data);
}
dcc_status_t call_rest_create_auto_moderation_rule(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_auto_moderation_rule(client, 333, "{\"name\":\"links\",\"event_type\":1,\"trigger_type\":3,\"actions\":[]}", cb, user_data);
}
dcc_status_t call_rest_create_auto_moderation_rule_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const char *keywords[] = { "bad", "evil*" };
    const char *regex_patterns[] = { "spam+" };
    const dcc_auto_moderation_preset_type_t presets[] = {
        DCC_AUTO_MODERATION_PRESET_PROFANITY,
        DCC_AUTO_MODERATION_PRESET_SLURS
    };
    const char *allow_list[] = { "good" };
    const dcc_auto_moderation_action_params_t actions[] = {
        {
            .size = sizeof(dcc_auto_moderation_action_params_t),
            .type = DCC_AUTO_MODERATION_ACTION_BLOCK_MESSAGE,
            .custom_message = "Stop"
        },
        {
            .size = sizeof(dcc_auto_moderation_action_params_t),
            .type = DCC_AUTO_MODERATION_ACTION_SEND_ALERT,
            .channel_id = 444
        },
        {
            .size = sizeof(dcc_auto_moderation_action_params_t),
            .type = DCC_AUTO_MODERATION_ACTION_TIMEOUT,
            .duration_seconds = 60
        }
    };
    const dcc_snowflake_t exempt_roles[] = { 555 };
    const dcc_snowflake_t exempt_channels[] = { 666 };
    const dcc_auto_moderation_rule_params_t params = {
        .size = sizeof(dcc_auto_moderation_rule_params_t),
        .name = "links",
        .event_type = DCC_AUTO_MODERATION_EVENT_MESSAGE_SEND,
        .trigger_type = DCC_AUTO_MODERATION_TRIGGER_KEYWORD,
        .trigger_metadata = {
            .size = sizeof(dcc_auto_moderation_metadata_params_t),
            .keywords = keywords,
            .keyword_count = sizeof(keywords) / sizeof(keywords[0]),
            .regex_patterns = regex_patterns,
            .regex_pattern_count = sizeof(regex_patterns) / sizeof(regex_patterns[0]),
            .presets = presets,
            .preset_count = sizeof(presets) / sizeof(presets[0]),
            .allow_list = allow_list,
            .allow_list_count = sizeof(allow_list) / sizeof(allow_list[0]),
            .mention_total_limit = 4,
            .mention_raid_protection_enabled = 1
        },
        .actions = actions,
        .action_count = sizeof(actions) / sizeof(actions[0]),
        .exempt_roles = exempt_roles,
        .exempt_role_count = sizeof(exempt_roles) / sizeof(exempt_roles[0]),
        .exempt_channels = exempt_channels,
        .exempt_channel_count = sizeof(exempt_channels) / sizeof(exempt_channels[0]),
        .enabled = 1
    };
    return dcc_rest_create_auto_moderation_rule_params(client, 333, &params, cb, user_data);
}
dcc_status_t call_rest_modify_auto_moderation_rule(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_auto_moderation_rule(client, 333, 999, "{\"enabled\":false}", cb, user_data);
}
dcc_status_t call_rest_modify_auto_moderation_rule_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_auto_moderation_rule_params_t params = {
        .size = sizeof(dcc_auto_moderation_rule_params_t),
        .rule_id = 999,
        .name = "links edited",
        .event_type = DCC_AUTO_MODERATION_EVENT_MESSAGE_SEND,
        .trigger_type = DCC_AUTO_MODERATION_TRIGGER_SPAM,
        .trigger_metadata = {
            .size = sizeof(dcc_auto_moderation_metadata_params_t)
        },
        .enabled = 0
    };
    return dcc_rest_modify_auto_moderation_rule_params(client, 333, &params, cb, user_data);
}
dcc_status_t call_rest_delete_auto_moderation_rule(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_auto_moderation_rule(client, 333, 999, cb, user_data);
}
dcc_status_t call_rest_get_guild_scheduled_events(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_scheduled_events(client, 333, "with_user_count=true", cb, user_data);
}
dcc_status_t call_rest_get_guild_scheduled_events_with_user_count(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_get_guild_scheduled_events_with_user_count(client, 333, cb, user_data);
}
dcc_status_t call_rest_get_guild_scheduled_event(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_scheduled_event(client, 333, 999, "with_user_count=true", cb, user_data);
}
dcc_status_t call_rest_get_guild_scheduled_event_with_user_count(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_get_guild_scheduled_event_with_user_count(client, 333, 999, cb, user_data);
}
dcc_status_t call_rest_create_guild_scheduled_event(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_guild_scheduled_event(client, 333, "{\"name\":\"meet\",\"entity_type\":3}", cb, user_data);
}
dcc_status_t call_rest_create_guild_scheduled_event_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_scheduled_event_params_t params = {
        .size = sizeof(dcc_scheduled_event_params_t),
        .version = DCC_SCHEDULED_EVENT_PARAMS_VERSION,
        .present = DCC_SCHEDULED_EVENT_PARAMS_PRESENT_NAME |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_DESCRIPTION |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_START_TIME |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_END_TIME |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_LOCATION |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_PRIVACY_LEVEL |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_STATUS |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_ENTITY_TYPE,
        .nulls = 0U,
        .guild_id = 333,
        .name = "meet typed",
        .description = "typed desc",
        .scheduled_start_time = "2026-06-16T00:00:00.000000+00:00",
        .scheduled_end_time = "2026-06-16T01:00:00.000000+00:00",
        .location = "Seoul",
        .privacy_level = DCC_SCHEDULED_EVENT_PRIVACY_GUILD_ONLY,
        .status = DCC_SCHEDULED_EVENT_STATUS_SCHEDULED,
        .entity_type = DCC_SCHEDULED_EVENT_ENTITY_EXTERNAL
    };
    return dcc_rest_create_guild_scheduled_event_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_modify_guild_scheduled_event(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_scheduled_event(client, 333, 999, "{\"status\":2}", cb, user_data);
}
dcc_status_t call_rest_modify_guild_scheduled_event_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const dcc_scheduled_event_params_t params = {
        .size = sizeof(dcc_scheduled_event_params_t),
        .version = DCC_SCHEDULED_EVENT_PARAMS_VERSION,
        .present = DCC_SCHEDULED_EVENT_PARAMS_PRESENT_CHANNEL_ID |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_NAME |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_DESCRIPTION |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_IMAGE |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_SCHEDULED_START_TIME |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_PRIVACY_LEVEL |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_STATUS |
            DCC_SCHEDULED_EVENT_PARAMS_PRESENT_ENTITY_TYPE,
        .nulls = 0U,
        .guild_id = 333,
        .event_id = 999,
        .creator_id = 444,
        .entity_id = 555,
        .channel_id = 222,
        .name = "meet edit",
        .description = "edit desc",
        .image = "data:image/png;base64,Zm9v",
        .scheduled_start_time = "2026-06-17T00:00:00.000000+00:00",
        .privacy_level = DCC_SCHEDULED_EVENT_PRIVACY_GUILD_ONLY,
        .status = DCC_SCHEDULED_EVENT_STATUS_ACTIVE,
        .entity_type = DCC_SCHEDULED_EVENT_ENTITY_STAGE_INSTANCE
    };
    return dcc_rest_modify_guild_scheduled_event_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_delete_guild_scheduled_event(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_guild_scheduled_event(client, 333, 999, cb, user_data);
}
dcc_status_t call_rest_get_guild_scheduled_event_users(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_scheduled_event_users(client, 333, 999, "with_member=true&limit=2&after=444", cb, user_data);
}
dcc_status_t call_rest_get_guild_scheduled_event_users_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_scheduled_event_users_page(client, 333, 999, 2, 0, 444, cb, user_data);
}
dcc_status_t call_rest_create_stage_instance(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_stage_instance(client, "{\"channel_id\":\"222\",\"topic\":\"stage\"}", cb, user_data);
}
dcc_status_t call_rest_create_stage_instance_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_stage_instance_params_t params = {
        .size = sizeof(params),
        .version = DCC_STAGE_INSTANCE_PARAMS_VERSION,
        .present = DCC_STAGE_INSTANCE_PARAMS_PRESENT_CHANNEL_ID |
            DCC_STAGE_INSTANCE_PARAMS_PRESENT_TOPIC |
            DCC_STAGE_INSTANCE_PARAMS_PRESENT_PRIVACY_LEVEL |
            DCC_STAGE_INSTANCE_PARAMS_PRESENT_SEND_START_NOTIFICATION |
            DCC_STAGE_INSTANCE_PARAMS_PRESENT_GUILD_SCHEDULED_EVENT_ID,
        .channel_id = 222,
        .topic = "stage typed",
        .privacy_level = DCC_STAGE_PRIVACY_GUILD_ONLY,
        .send_start_notification = 0U,
        .guild_scheduled_event_id = 999U
    };
    return dcc_rest_create_stage_instance_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_get_stage_instance(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_stage_instance(client, 222, cb, user_data);
}
dcc_status_t call_rest_modify_stage_instance(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_stage_instance(client, 222, "{\"topic\":\"updated\"}", cb, user_data);
}
dcc_status_t call_rest_modify_stage_instance_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_stage_instance_params_t params = {
        .size = sizeof(params),
        .version = DCC_STAGE_INSTANCE_PARAMS_VERSION,
        .present = DCC_STAGE_INSTANCE_PARAMS_PRESENT_PRIVACY_LEVEL,
        .channel_id = 222,
        .privacy_level = DCC_STAGE_PRIVACY_GUILD_ONLY
    };
    return dcc_rest_modify_stage_instance_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_delete_stage_instance(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_stage_instance(client, 222, cb, user_data);
}
dcc_status_t call_rest_get_template(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_template(client, "tpl/abc", cb, user_data);
}
dcc_status_t call_rest_get_guild_templates(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_templates(client, 333, cb, user_data);
}
dcc_status_t call_rest_create_guild_template(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_guild_template(client, 333, "{\"name\":\"tpl\",\"description\":\"desc\"}", cb, user_data);
}
dcc_status_t call_rest_create_guild_template_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_template_params_t params = {
        .size = sizeof(dcc_template_params_t),
        .version = DCC_TEMPLATE_PARAMS_VERSION,
        .present = DCC_TEMPLATE_PARAMS_PRESENT_NAME |
            DCC_TEMPLATE_PARAMS_PRESENT_DESCRIPTION,
        .nulls = 0U,
        .guild_id = 333,
        .name = "tpl typed",
        .description = "desc typed"
    };
    return dcc_rest_create_guild_template_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_modify_guild_template(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_template(client, 333, "tpl/abc", "{\"name\":\"tpl2\",\"description\":\"desc2\"}", cb, user_data);
}
dcc_status_t call_rest_modify_guild_template_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_template_params_t params = {
        .size = sizeof(dcc_template_params_t),
        .version = DCC_TEMPLATE_PARAMS_VERSION,
        .present = DCC_TEMPLATE_PARAMS_PRESENT_NAME |
            DCC_TEMPLATE_PARAMS_PRESENT_DESCRIPTION,
        .nulls = 0U,
        .guild_id = 333,
        .code = "tpl/abc",
        .name = "tpl2 typed",
        .description = "desc2 typed"
    };
    return dcc_rest_modify_guild_template_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_sync_guild_template(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_sync_guild_template(client, 333, "tpl/abc", cb, user_data);
}
dcc_status_t call_rest_delete_guild_template(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_guild_template(client, 333, "tpl/abc", cb, user_data);
}
dcc_status_t call_rest_get_current_application(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_current_application(client, cb, user_data);
}
dcc_status_t call_rest_get_current_user(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_current_user(client, cb, user_data);
}
dcc_status_t call_rest_modify_current_user(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_current_user(client, "{\"username\":\"bot\"}", cb, user_data);
}
dcc_status_t call_rest_modify_current_user_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    const dcc_current_user_params_t params = {
        .size = sizeof(params),
        .nickname = "bot typed",
        .avatar = "data:image/png;base64,AA==",
        .banner = "data:image/png;base64,BB=="
    };
    return dcc_rest_modify_current_user_params(client, &params, cb, user_data);
}
dcc_status_t call_rest_get_user(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_user(client, 444, cb, user_data);
}
dcc_status_t call_rest_get_current_user_connections(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_current_user_connections(client, cb, user_data);
}
dcc_status_t call_rest_get_current_user_guilds(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_rest_current_user_guilds_query_t query=DCC_REST_CURRENT_USER_GUILDS_QUERY_INIT;query.present=DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_LIMIT|DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_AFTER;query.limit=2;query.after=333;dcc_rest_call_options_t options=rest_call_options_from_legacy(cb,user_data);return dcc_rest_get_current_user_guilds(client,&query,&options,NULL);
}
dcc_status_t call_rest_get_current_user_dms(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    (void)client;
    (void)cb;
    (void)user_data;
    return DCC_ERR_INVALID_ARG;
}
dcc_status_t call_rest_create_dm_channel(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_dm_channel_params_t params = DCC_DM_CHANNEL_PARAMS_INIT;
    dcc_rest_call_options_t options = rest_call_options_from_legacy(cb, user_data);
    params.recipient_id = 444;
    return dcc_rest_create_dm_channel(client, &params, &options, NULL);
}
dcc_status_t call_rest_create_dm_channel_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return call_rest_create_dm_channel(client, cb, user_data);
}
dcc_status_t call_rest_create_direct_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_direct_message(client, 444, "{\"content\":\"hi\"}", cb, user_data);
}
dcc_status_t call_rest_create_direct_message_builder(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    dcc_status_t status = dcc_message_builder_set_content(&message, "hi builder");
    if (status != DCC_OK) {
        return status;
    }
    return dcc_rest_create_direct_message_builder(client, 444, &message, cb, user_data);
}

#endif
