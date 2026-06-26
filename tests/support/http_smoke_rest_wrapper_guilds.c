#include "http_smoke_support.h"

#if !defined(_WIN32)

int run_public_rest_wrapper_expect_guilds_smoke(dcc_client_t *client) {
    if (
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_widget",
                    call_rest_modify_guild_widget,
                    "PATCH",
                    "/guilds/333/widget",
                    "{\"enabled\":true,\"channel_id\":\"222\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_widget_params",
                    call_rest_modify_guild_widget_params,
                    "PATCH",
                    "/guilds/333/widget",
                    "{\"channel_id\":\"222\",\"enabled\":true}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_vanity_url",
                    call_rest_get_guild_vanity_url,
                    "GET",
                    "/guilds/333/vanity-url",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_prune_count",
                    call_rest_get_guild_prune_count,
                    "GET",
                    "/guilds/333/prune?days=7&include_roles=555",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_prune_count_options",
                    call_rest_get_guild_prune_count_options,
                    "GET",
                    "/guilds/333/prune?days=7&include_roles=555%2C556",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "begin_guild_prune",
                    call_rest_begin_guild_prune,
                    "POST",
                    "/guilds/333/prune",
                    "{\"days\":7,\"compute_prune_count\":false}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "begin_guild_prune_options",
                    call_rest_begin_guild_prune_options,
                    "POST",
                    "/guilds/333/prune",
                    "{\"include_roles\":[\"555\",\"556\"],\"compute_prune_count\":false,\"days\":7}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "begin_guild_prune_params",
                    call_rest_begin_guild_prune_params,
                    "POST",
                    "/guilds/333/prune",
                    "{\"include_roles\":[\"555\",\"556\"],\"compute_prune_count\":true,\"days\":7}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_onboarding",
                    call_rest_get_guild_onboarding,
                    "GET",
                    "/guilds/333/onboarding",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_onboarding",
                    call_rest_modify_guild_onboarding,
                    "PUT",
                    "/guilds/333/onboarding",
                    "{\"enabled\":true,\"prompts\":[]}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_onboarding_params",
                    call_rest_modify_guild_onboarding_params,
                    "PUT",
                    "/guilds/333/onboarding",
                    "{\"prompts\":[{\"type\":1,\"title\":\"Pick feeds\",\"options\":[{\"emoji\":{\"name\":\"wave\",\"image\":\"data:image/png;base64,AA==\",\"roles\":[\"777\"]},\"title\":\"News\",\"description\":\"Announcements\",\"channel_ids\":[\"222\",\"223\"],\"role_ids\":[\"555\",\"556\"]}],\"single_select\":true,\"required\":true,\"in_onboarding\":true}],\"default_channel_ids\":[\"222\"],\"enabled\":true,\"mode\":1}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_welcome_screen",
                    call_rest_get_guild_welcome_screen,
                    "GET",
                    "/guilds/333/welcome-screen",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_welcome_screen",
                    call_rest_modify_guild_welcome_screen,
                    "PATCH",
                    "/guilds/333/welcome-screen",
                    "{\"enabled\":true,\"welcome_channels\":[]}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_welcome_screen_params",
                    call_rest_modify_guild_welcome_screen_params,
                    "PATCH",
                    "/guilds/333/welcome-screen",
                    "{\"description\":\"Welcome home\",\"welcome_channels\":[{\"channel_id\":\"222\",\"description\":\"Read first\",\"emoji_id\":\"777\",\"emoji_name\":\"wave\"}],\"enabled\":true}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_auto_moderation_rules",
                    call_rest_get_auto_moderation_rules,
                    "GET",
                    "/guilds/333/auto-moderation/rules",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_auto_moderation_rule",
                    call_rest_get_auto_moderation_rule,
                    "GET",
                    "/guilds/333/auto-moderation/rules/999",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_auto_moderation_rule",
                    call_rest_create_auto_moderation_rule,
                    "POST",
                    "/guilds/333/auto-moderation/rules",
                    "{\"name\":\"links\",\"event_type\":1,\"trigger_type\":3,\"actions\":[]}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_auto_moderation_rule_params",
                    call_rest_create_auto_moderation_rule_params,
                    "POST",
                    "/guilds/333/auto-moderation/rules",
                    "{\"name\":\"links\",\"enabled\":true,\"event_type\":1,\"trigger_type\":1,\"trigger_metadata\":{\"keyword_filter\":[\"bad\",\"evil*\"],\"regex_patterns\":[\"spam+\"],\"presets\":[1,3],\"allow_list\":[\"good\"],\"mention_total_limit\":4,\"mention_raid_protection_enabled\":true},\"actions\":[{\"type\":1,\"metadata\":{\"custom_message\":\"Stop\"}},{\"type\":2,\"metadata\":{\"channel_id\":\"444\"}},{\"type\":3,\"metadata\":{\"duration_seconds\":60}}],\"exempt_roles\":[\"555\"],\"exempt_channels\":[\"666\"]}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_auto_moderation_rule",
                    call_rest_modify_auto_moderation_rule,
                    "PATCH",
                    "/guilds/333/auto-moderation/rules/999",
                    "{\"enabled\":false}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_auto_moderation_rule_params",
                    call_rest_modify_auto_moderation_rule_params,
                    "PATCH",
                    "/guilds/333/auto-moderation/rules/999",
                    "{\"id\":\"999\",\"name\":\"links edited\",\"enabled\":false,\"event_type\":1,\"trigger_type\":3,\"trigger_metadata\":{\"keyword_filter\":[],\"regex_patterns\":[],\"presets\":[],\"allow_list\":[],\"mention_total_limit\":0,\"mention_raid_protection_enabled\":false}}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_auto_moderation_rule",
                    call_rest_delete_auto_moderation_rule,
                    "DELETE",
                    "/guilds/333/auto-moderation/rules/999",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_scheduled_events",
                    call_rest_get_guild_scheduled_events,
                    "GET",
                    "/guilds/333/scheduled-events?with_user_count=true",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_scheduled_events_with_user_count",
                    call_rest_get_guild_scheduled_events_with_user_count,
                    "GET",
                    "/guilds/333/scheduled-events?with_user_count=true",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_scheduled_event",
                    call_rest_get_guild_scheduled_event,
                    "GET",
                    "/guilds/333/scheduled-events/999?with_user_count=true",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_scheduled_event_with_user_count",
                    call_rest_get_guild_scheduled_event_with_user_count,
                    "GET",
                    "/guilds/333/scheduled-events/999?with_user_count=true",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_scheduled_event",
                    call_rest_create_guild_scheduled_event,
                    "POST",
                    "/guilds/333/scheduled-events",
                    "{\"name\":\"meet\",\"entity_type\":3}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_scheduled_event_params",
                    call_rest_create_guild_scheduled_event_params,
                    "POST",
                    "/guilds/333/scheduled-events",
                    "{\"name\":\"meet typed\",\"description\":\"typed desc\",\"privacy_level\":2,\"status\":1,\"entity_type\":3,\"guild_id\":\"333\",\"scheduled_start_time\":\"2026-06-16T00:00:00.000000+00:00\",\"scheduled_end_time\":\"2026-06-16T01:00:00.000000+00:00\",\"entity_metadata\":{\"location\":\"Seoul\"}}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_scheduled_event",
                    call_rest_modify_guild_scheduled_event,
                    "PATCH",
                    "/guilds/333/scheduled-events/999",
                    "{\"status\":2}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_scheduled_event_params",
                    call_rest_modify_guild_scheduled_event_params,
                    "PATCH",
                    "/guilds/333/scheduled-events/999",
                    "{\"id\":\"999\",\"name\":\"meet edit\",\"description\":\"edit desc\",\"image\":\"data:image/png;base64,Zm9v\",\"privacy_level\":2,\"status\":2,\"entity_type\":1,\"entity_id\":\"555\",\"channel_id\":\"222\",\"guild_id\":\"333\",\"creator_id\":\"444\",\"scheduled_start_time\":\"2026-06-17T00:00:00.000000+00:00\",\"entity_metadata\":{\"location\":null}}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_guild_scheduled_event",
                    call_rest_delete_guild_scheduled_event,
                    "DELETE",
                    "/guilds/333/scheduled-events/999",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_scheduled_event_users",
                    call_rest_get_guild_scheduled_event_users,
                    "GET",
                    "/guilds/333/scheduled-events/999/users?with_member=true&limit=2&after=444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_scheduled_event_users_page",
                    call_rest_get_guild_scheduled_event_users_page,
                    "GET",
                    "/guilds/333/scheduled-events/999/users?with_member=true&limit=2&after=444&before=222",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_stage_instance",
                    call_rest_create_stage_instance,
                    "POST",
                    "/stage-instances",
                    "{\"channel_id\":\"222\",\"topic\":\"stage\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_stage_instance_params",
                    call_rest_create_stage_instance_params,
                    "POST",
                    "/stage-instances",
                    "{\"channel_id\":\"222\",\"topic\":\"stage typed\",\"privacy_level\":2}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_stage_instance",
                    call_rest_get_stage_instance,
                    "GET",
                    "/stage-instances/222",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_stage_instance",
                    call_rest_modify_stage_instance,
                    "PATCH",
                    "/stage-instances/222",
                    "{\"topic\":\"updated\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_stage_instance_params",
                    call_rest_modify_stage_instance_params,
                    "PATCH",
                    "/stage-instances/222",
                    "{\"channel_id\":\"222\",\"topic\":\"updated typed\",\"privacy_level\":2}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_stage_instance",
                    call_rest_delete_stage_instance,
                    "DELETE",
                    "/stage-instances/222",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_template",
                    call_rest_get_template,
                    "GET",
                    "/guilds/templates/tpl%2Fabc",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_from_template",
                    call_rest_create_guild_from_template,
                    "POST",
                    "/guilds/templates/tpl%2Fabc",
                    "{\"name\":\"New Guild\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_from_template_params",
                    call_rest_create_guild_from_template_params,
                    "POST",
                    "/guilds/templates/tpl%2Fabc",
                    "{\"name\":\"New Guild Typed\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_guild_templates",
                    call_rest_get_guild_templates,
                    "GET",
                    "/guilds/333/templates",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_template",
                    call_rest_create_guild_template,
                    "POST",
                    "/guilds/333/templates",
                    "{\"name\":\"tpl\",\"description\":\"desc\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_guild_template_params",
                    call_rest_create_guild_template_params,
                    "POST",
                    "/guilds/333/templates",
                    "{\"name\":\"tpl typed\",\"description\":\"desc typed\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_template",
                    call_rest_modify_guild_template,
                    "PATCH",
                    "/guilds/333/templates/tpl%2Fabc",
                    "{\"name\":\"tpl2\",\"description\":\"desc2\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_guild_template_params",
                    call_rest_modify_guild_template_params,
                    "PATCH",
                    "/guilds/333/templates/tpl%2Fabc",
                    "{\"name\":\"tpl2 typed\",\"description\":\"desc2 typed\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "sync_guild_template",
                    call_rest_sync_guild_template,
                    "PUT",
                    "/guilds/333/templates/tpl%2Fabc",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "delete_guild_template",
                    call_rest_delete_guild_template,
                    "DELETE",
                    "/guilds/333/templates/tpl%2Fabc",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_current_application",
                    call_rest_get_current_application,
                    "GET",
                    "/applications/@me",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_current_user",
                    call_rest_get_current_user,
                    "GET",
                    "/users/@me",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_current_user",
                    call_rest_modify_current_user,
                    "PATCH",
                    "/users/@me",
                    "{\"username\":\"bot\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "modify_current_user_params",
                    call_rest_modify_current_user_params,
                    "PATCH",
                    "/users/@me",
                    "{\"nickname\":\"bot typed\",\"avatar\":\"data:image/png;base64,AA==\",\"banner\":\"data:image/png;base64,BB==\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_user",
                    call_rest_get_user,
                    "GET",
                    "/users/444",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_current_user_connections",
                    call_rest_get_current_user_connections,
                    "GET",
                    "/users/@me/connections",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_current_user_guilds",
                    call_rest_get_current_user_guilds,
                    "GET",
                    "/users/@me/guilds?limit=2&after=333",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "get_current_user_dms",
                    call_rest_get_current_user_dms,
                    "GET",
                    "/users/@me/channels",
                    NULL
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_dm_channel",
                    call_rest_create_dm_channel,
                    "POST",
                    "/users/@me/channels",
                    "{\"recipient_id\":\"444\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_dm_channel_params",
                    call_rest_create_dm_channel_params,
                    "POST",
                    "/users/@me/channels",
                    "{\"recipient_id\":\"444\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_direct_message_cached",
                    call_rest_create_direct_message,
                    "POST",
                    "/channels/222/messages",
                    "{\"content\":\"hi\"}"
                ) != 0 ||
        run_public_rest_wrapper_expect(
                    client,
                    "create_direct_message_builder_cached",
                    call_rest_create_direct_message_builder,
                    "POST",
                    "/channels/222/messages",
                    "{\"content\":\"hi builder\"}"
                ) != 0) {
        return 1;
    }
    return 0;
}
#endif
