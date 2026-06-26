#include "package_consumer_checks.h"

#include <dcc/dcc.h>
#include <dcc/oauth2.h>
#include <dcc/objects/resources/applications.h>
#include <dcc/objects/resources/lobbies.h>
#include <dcc/rest/official_surface.h>
#include <dcc/sugar.h>
#include <dcc/sugar/official_surface.h>
#include <dcc/webhook_events.h>

#include <stdio.h>
#include <string.h>

static void package_official_rest_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    (void)client;
    (void)response;
    (void)user_data;
}

static void package_official_oauth_cb(const dcc_rest_response_t *response, void *user_data) {
    (void)response;
    (void)user_data;
}

static dcc_status_t package_official_webhook_cb(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    const dcc_webhook_event_t *event,
    void *user_data
) {
    (void)server;
    (void)request;
    (void)event;
    (void)user_data;
    return DCC_OK;
}

#define DCC_PACKAGE_REQUIRE_SYMBOL(symbol_) \
    do { \
        if ((symbol_) == NULL) { \
            fprintf(stderr, "missing official surface symbol: %s\n", #symbol_); \
            return 0; \
        } \
    } while (0)

int dcc_package_consumer_check_official_surface_api(void) {
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_gateway);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_current_application);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_modify_current_application);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_application_modify_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_modify_current_application_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_activity_instance);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_soundboard_send_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_send_soundboard_sound);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_send_soundboard_sound_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_default_soundboard_sounds);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_guild_soundboard_sounds);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_guild_soundboard_sound);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_create_guild_soundboard_sound);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_guild_soundboard_sound_create_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_create_guild_soundboard_sound_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_modify_guild_soundboard_sound);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_guild_soundboard_sound_modify_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_modify_guild_soundboard_sound_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_delete_guild_soundboard_sound);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_sku_subscriptions);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_sku_subscription);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_bulk_ban_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_bulk_ban_guild_members);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_bulk_ban_guild_members_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_guild_role);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_guild_role_member_counts);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_guild_widget_json);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_guild_widget_png);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_guild_incident_actions_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_modify_guild_incident_actions);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_modify_guild_incident_actions_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_invite_target_users);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_put_invite_target_users);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_invite_target_users_job_status);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_entitlement);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_sticker_pack);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_current_user_guild_member);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_delete_current_user_application_role_connection);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_execute_webhook_slack);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_execute_webhook_github);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_legacy_pin_message);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_legacy_unpin_message);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_legacy_channel_pins);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_create_lobby);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_lobby_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_create_lobby_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_create_or_join_lobby);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_lobby_create_or_join_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_create_or_join_lobby_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_lobby);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_modify_lobby);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_modify_lobby_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_delete_lobby);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_add_lobby_member);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_lobby_member_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_add_lobby_member_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_bulk_update_lobby_members);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_lobby_member_bulk_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_bulk_update_lobby_members_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_remove_lobby_member);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_leave_lobby);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_link_lobby_channel);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_lobby_channel_link_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_link_lobby_channel_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_unlink_lobby_channel);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_send_lobby_message);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_lobby_message_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_send_lobby_message_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_get_lobby_messages);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_update_lobby_message_moderation_metadata);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_build_lobby_message_moderation_metadata_body);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_update_lobby_message_moderation_metadata_params);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_create_lobby_channel_invite_for_self);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_create_lobby_channel_invite_for_user);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_rest_official_body_json_free);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_client_request_guild_members);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_client_request_soundboard_sounds);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_client_request_channel_info);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_client_update_presence);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_client_update_presence_raw);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_oauth2_get_current_authorization);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_oauth2_get_current_bot_application);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_oauth2_exchange_code);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_oauth2_refresh_token);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_oauth2_revoke_token);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_webhook_event_type_name);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_webhook_event_type_from_string);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_webhook_event_parse);
    DCC_PACKAGE_REQUIRE_SYMBOL(dcc_webhook_events_add_route);

    dcc_gateway_request_guild_members_options_t members =
        DCC_GATEWAY_MEMBERS(.guild_id = 123, .query = "", .limit = 1);
    dcc_gateway_presence_update_t presence =
        DCC_GATEWAY_PRESENCE(.status = "online", .activities_json = "[]");
    dcc_oauth2_exchange_code_options_t exchange =
        DCC_OAUTH2_EXCHANGE_CODE_OPTIONS(
            .client_id = "client",
            .client_secret = "secret",
            .code = "code",
            .redirect_uri = "https://example.invalid/callback"
        );
    dcc_oauth2_refresh_token_options_t refresh =
        DCC_OAUTH2_REFRESH_TOKEN_OPTIONS(
            .client_id = "client",
            .client_secret = "secret",
            .refresh_token = "refresh"
        );
    dcc_oauth2_revoke_token_options_t revoke =
        DCC_OAUTH2_REVOKE_TOKEN_OPTIONS(
            .client_id = "client",
            .client_secret = "secret",
            .token = "token",
            .token_type_hint = "access_token"
        );
    const char *guild_install_scopes[] = {"bot", "applications.commands"};
    const char *user_install_scopes[] = {"applications.commands"};
    dcc_application_install_params_t guild_install =
        DCC_APPLICATION_INSTALL_PARAMS(guild_install_scopes, 2, "2048");
    dcc_application_install_params_t user_install =
        DCC_APPLICATION_INSTALL_PARAMS(user_install_scopes, 1, "0");
    dcc_application_integration_type_config_t integration_configs[] = {
        DCC_APPLICATION_INTEGRATION_CONFIG(DCC_APPLICATION_INTEGRATION_TYPE_GUILD_INSTALL, &guild_install),
        DCC_APPLICATION_INTEGRATION_CONFIG(DCC_APPLICATION_INTEGRATION_TYPE_USER_INSTALL, &user_install)
    };
    const char *application_tags[] = {"api", "runtime"};
    const char *webhook_types[] = {"APPLICATION_AUTHORIZED", "APPLICATION_DEAUTHORIZED"};
    dcc_application_modify_params_t application_modify =
        DCC_APPLICATION_MODIFY(
            .description = "DCC",
            .has_description = 1,
            .install_params = &guild_install,
            .has_install_params = 1,
            .integration_types_config = integration_configs,
            .integration_type_config_count = 2,
            .has_integration_types_config = 1,
            .flags = 32768,
            .has_flags = 1,
            .icon = NULL,
            .has_icon = 1,
            .tags = application_tags,
            .tag_count = 2,
            .has_tags = 1,
            .event_webhooks_status = DCC_APPLICATION_EVENT_WEBHOOK_ENABLED,
            .has_event_webhooks_status = 1,
            .event_webhooks_types = webhook_types,
            .event_webhooks_type_count = 2,
            .has_event_webhooks_types = 1
        );
    dcc_soundboard_send_params_t soundboard_send =
        DCC_SOUNDBOARD_SEND(.sound_id = 456, .source_guild_id = 789);
    dcc_guild_soundboard_sound_params_t guild_sound_create =
        DCC_GUILD_SOUNDBOARD_SOUND(
            .name = "bell",
            .sound = "data:audio/ogg;base64,AAAA",
            .volume = 0.5,
            .has_volume = 1,
            .emoji_name = "ding",
            .has_emoji_name = 1
        );
    dcc_guild_soundboard_sound_params_t guild_sound_modify =
        DCC_GUILD_SOUNDBOARD_SOUND(
            .name = "bell2",
            .volume = 1.0,
            .has_volume = 1,
            .emoji_id = 0,
            .has_emoji_id = 1
        );
    dcc_snowflake_t bulk_users[] = {111, 222};
    dcc_bulk_ban_params_t bulk_ban =
        DCC_BULK_BAN_BODY(
            .user_ids = bulk_users,
            .user_id_count = 2,
            .delete_message_seconds = 60,
            .has_delete_message_seconds = 1
        );
    dcc_guild_incident_actions_params_t incident_actions =
        DCC_INCIDENT_ACTIONS_BODY(
            .invites_disabled_until = "2026-06-27T00:00:00+00:00",
            .has_invites_disabled_until = 1,
            .dms_disabled_until = NULL,
            .has_dms_disabled_until = 1
        );
    dcc_lobby_create_or_join_params_t lobby_join =
        DCC_LOBBY_CREATE_OR_JOIN(
            .secret = "join-secret",
            .idle_timeout_seconds = 60,
            .has_idle_timeout_seconds = 1,
            .lobby_metadata_json = "{\"topic\":\"redstone\"}",
            .has_lobby_metadata = 1,
            .member_metadata_json = NULL,
            .has_member_metadata = 1
        );
    dcc_lobby_member_spec_t lobby_create_members[] = {
        DCC_LOBBY_MEMBER_SPEC(
            .user_id = 444,
            .metadata_json = "{\"role\":\"tank\"}",
            .has_metadata = 1,
            .flags = 1,
            .has_flags = 1
        )
    };
    dcc_lobby_params_t lobby_create =
        DCC_LOBBY(
            .metadata_json = "{\"topic\":\"redstone\"}",
            .has_metadata = 1,
            .members = lobby_create_members,
            .member_count = 1,
            .has_members = 1,
            .idle_timeout_seconds = 60,
            .has_idle_timeout_seconds = 1
        );
    dcc_lobby_params_t lobby_modify =
        DCC_LOBBY(
            .metadata_json = NULL,
            .has_metadata = 1,
            .members = NULL,
            .member_count = 0,
            .has_members = 1
        );
    dcc_lobby_channel_link_params_t lobby_link =
        DCC_LOBBY_LINK_CHANNEL(.channel_id = 333);
    dcc_lobby_member_params_t lobby_member =
        DCC_LOBBY_MEMBER(
            .metadata_json = "{\"role\":\"tank\"}",
            .has_metadata = 1,
            .flags = 1,
            .has_flags = 1
        );
    dcc_lobby_member_bulk_item_t lobby_bulk_items[] = {
        DCC_LOBBY_BULK_MEMBER(
            .user_id = 444,
            .metadata_json = "{\"role\":\"tank\"}",
            .has_metadata = 1,
            .flags = 1,
            .has_flags = 1
        ),
        DCC_LOBBY_BULK_MEMBER(
            .user_id = 555,
            .remove_member = 1,
            .has_remove_member = 1
        )
    };
    dcc_lobby_member_bulk_params_t lobby_bulk =
        DCC_LOBBY_BULK_MEMBERS(lobby_bulk_items, 2);
    dcc_lobby_message_params_t lobby_message =
        DCC_LOBBY_MESSAGE(
            .content = "hello",
            .metadata_json = "{\"kind\":\"test\"}",
            .has_metadata = 1,
            .flags = 4,
            .has_flags = 1
        );
    dcc_lobby_message_moderation_metadata_item_t moderation_items[] = {
        DCC_LOBBY_MODERATION_METADATA_ITEM("decision", "allow"),
        DCC_LOBBY_MODERATION_METADATA_ITEM("source", "automod")
    };
    dcc_lobby_message_moderation_metadata_params_t moderation =
        DCC_LOBBY_MODERATION_METADATA(moderation_items, 2);
    dcc_webhook_event_route_t route =
        DCC_WEBHOOK_EVENT_ROUTE(package_official_webhook_cb, NULL);
    dcc_webhook_event_route_t unsigned_route =
        DCC_WEBHOOK_EVENT_ROUTE_UNSIGNED(package_official_webhook_cb, NULL);
    dcc_webhook_event_t event = {.size = sizeof(event)};
    dcc_activity_location_kind_t private_location_kind =
        DCC_ACTIVITY_LOCATION_PRIVATE_CHANNEL;
    dcc_activity_location_t private_location = {
        .channel_id = 912954213460484116ULL,
        .kind = DCC_ACTIVITY_LOCATION_PRIVATE_CHANNEL,
        .id = "pc-912954213460484116",
        .kind_text = "pc"
    };
    dcc_snowflake_t activity_users[] = {205519959982473217ULL};
    dcc_activity_instance_t activity = {
        .application_id = 1215413995645968394ULL,
        .launch_id = 1276580072400224306ULL,
        .location = {
            .channel_id = 912954213460484116ULL,
            .guild_id = 912952092627435520ULL,
            .kind = DCC_ACTIVITY_LOCATION_GUILD_CHANNEL,
            .has_guild_id = 1,
            .id = "gc-912952092627435520-912954213460484116",
            .kind_text = "gc"
        },
        .user_ids = activity_users,
        .user_ids_count = 1,
        .instance_id = "i-1276580072400224306-gc-912952092627435520-912954213460484116"
    };
    dcc_lobby_member_t lobby_members[] = {
        {
            .id = 41771983429993000ULL,
            .flags = DCC_LOBBY_MEMBER_CAN_LINK_LOBBY,
            .has_flags = 1,
            .metadata_json = NULL
        }
    };
    dcc_lobby_member_flag_t lobby_member_flag = DCC_LOBBY_MEMBER_CAN_LINK_LOBBY;
    dcc_lobby_t lobby_object = {
        .id = 96008815106887111ULL,
        .application_id = 41771983429993937ULL,
        .members = lobby_members,
        .members_count = 1,
        .metadata_json = "{\"topic\":\"redstone\"}"
    };

    if (members.size != sizeof(members) ||
        presence.size != sizeof(presence) ||
        exchange.size != sizeof(exchange) ||
        refresh.size != sizeof(refresh) ||
        revoke.size != sizeof(revoke) ||
        guild_install.size != sizeof(guild_install) ||
        user_install.size != sizeof(user_install) ||
        integration_configs[0].size != sizeof(integration_configs[0]) ||
        integration_configs[1].size != sizeof(integration_configs[1]) ||
        application_modify.size != sizeof(application_modify) ||
        soundboard_send.size != sizeof(soundboard_send) ||
        guild_sound_create.size != sizeof(guild_sound_create) ||
        guild_sound_modify.size != sizeof(guild_sound_modify) ||
        bulk_ban.size != sizeof(bulk_ban) ||
        incident_actions.size != sizeof(incident_actions) ||
        lobby_join.size != sizeof(lobby_join) ||
        lobby_create.size != sizeof(lobby_create) ||
        lobby_modify.size != sizeof(lobby_modify) ||
        lobby_create_members[0].size != sizeof(lobby_create_members[0]) ||
        lobby_link.size != sizeof(lobby_link) ||
        lobby_member.size != sizeof(lobby_member) ||
        lobby_bulk.size != sizeof(lobby_bulk) ||
        lobby_bulk_items[0].size != sizeof(lobby_bulk_items[0]) ||
        lobby_bulk_items[1].size != sizeof(lobby_bulk_items[1]) ||
        lobby_message.size != sizeof(lobby_message) ||
        moderation.size != sizeof(moderation) ||
        route.size != sizeof(route) ||
        unsigned_route.size != sizeof(unsigned_route)) {
        fprintf(stderr, "official surface sugar initializer size mismatch\n");
        return 0;
    }

    if (activity.location.kind != DCC_ACTIVITY_LOCATION_GUILD_CHANNEL ||
        private_location_kind != DCC_ACTIVITY_LOCATION_PRIVATE_CHANNEL ||
        private_location.kind != DCC_ACTIVITY_LOCATION_PRIVATE_CHANNEL ||
        activity.user_ids_count != 1 ||
        lobby_object.members_count != 1 ||
        lobby_member_flag != DCC_LOBBY_MEMBER_CAN_LINK_LOBBY ||
        (lobby_object.members[0].flags & DCC_LOBBY_MEMBER_CAN_LINK_LOBBY) == 0U) {
        fprintf(stderr, "official surface public object declarations mismatch\n");
        return 0;
    }

    char *json = NULL;
    if (dcc_rest_build_application_modify_body(&application_modify, &json) != DCC_OK ||
        strcmp(json,
               "{\"description\":\"DCC\",\"install_params\":{\"scopes\":[\"bot\",\"applications.commands\"],\"permissions\":\"2048\"},"
               "\"integration_types_config\":{\"0\":{\"oauth2_install_params\":{\"scopes\":[\"bot\",\"applications.commands\"],\"permissions\":\"2048\"}},"
               "\"1\":{\"oauth2_install_params\":{\"scopes\":[\"applications.commands\"],\"permissions\":\"0\"}}},"
               "\"flags\":32768,\"icon\":null,\"tags\":[\"api\",\"runtime\"],\"event_webhooks_status\":2,"
               "\"event_webhooks_types\":[\"APPLICATION_AUTHORIZED\",\"APPLICATION_DEAUTHORIZED\"]}") != 0) {
        fprintf(stderr, "official surface application modify body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_soundboard_send_body(&soundboard_send, &json) != DCC_OK ||
        strcmp(json, "{\"sound_id\":\"456\",\"source_guild_id\":\"789\"}") != 0) {
        fprintf(stderr, "official surface soundboard body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_guild_soundboard_sound_create_body(&guild_sound_create, &json) != DCC_OK ||
        strcmp(json, "{\"name\":\"bell\",\"sound\":\"data:audio/ogg;base64,AAAA\",\"volume\":0.5,\"emoji_name\":\"ding\"}") != 0) {
        fprintf(stderr, "official surface guild soundboard create body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_guild_soundboard_sound_modify_body(&guild_sound_modify, &json) != DCC_OK ||
        strcmp(json, "{\"name\":\"bell2\",\"volume\":1,\"emoji_id\":null}") != 0) {
        fprintf(stderr, "official surface guild soundboard modify body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_bulk_ban_body(&bulk_ban, &json) != DCC_OK ||
        strcmp(json, "{\"user_ids\":[\"111\",\"222\"],\"delete_message_seconds\":60}") != 0) {
        fprintf(stderr, "official surface bulk ban body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_guild_incident_actions_body(&incident_actions, &json) != DCC_OK ||
        strcmp(json, "{\"invites_disabled_until\":\"2026-06-27T00:00:00+00:00\",\"dms_disabled_until\":null}") != 0) {
        fprintf(stderr, "official surface incident actions body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_lobby_create_or_join_body(&lobby_join, &json) != DCC_OK ||
        strcmp(json, "{\"secret\":\"join-secret\",\"idle_timeout_seconds\":60,\"lobby_metadata\":{\"topic\":\"redstone\"},\"member_metadata\":null}") != 0) {
        fprintf(stderr, "official surface lobby create-or-join body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_lobby_body(&lobby_create, &json) != DCC_OK ||
        strcmp(json, "{\"metadata\":{\"topic\":\"redstone\"},\"members\":[{\"id\":\"444\",\"metadata\":{\"role\":\"tank\"},\"flags\":1}],\"idle_timeout_seconds\":60}") != 0) {
        fprintf(stderr, "official surface lobby body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_lobby_body(&lobby_modify, &json) != DCC_OK ||
        strcmp(json, "{\"metadata\":null,\"members\":[]}") != 0) {
        fprintf(stderr, "official surface lobby modify body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_lobby_member_body(&lobby_member, &json) != DCC_OK ||
        strcmp(json, "{\"metadata\":{\"role\":\"tank\"},\"flags\":1}") != 0) {
        fprintf(stderr, "official surface lobby member body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_lobby_member_bulk_body(&lobby_bulk, &json) != DCC_OK ||
        strcmp(json, "[{\"id\":\"444\",\"metadata\":{\"role\":\"tank\"},\"flags\":1},{\"id\":\"555\",\"remove_member\":true}]") != 0) {
        fprintf(stderr, "official surface lobby member bulk body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_lobby_channel_link_body(&lobby_link, &json) != DCC_OK ||
        strcmp(json, "{\"channel_id\":\"333\"}") != 0) {
        fprintf(stderr, "official surface lobby channel link body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_lobby_message_body(&lobby_message, &json) != DCC_OK ||
        strcmp(json, "{\"content\":\"hello\",\"metadata\":{\"kind\":\"test\"},\"flags\":4}") != 0) {
        fprintf(stderr, "official surface lobby message body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;
    if (dcc_rest_build_lobby_message_moderation_metadata_body(&moderation, &json) != DCC_OK ||
        strcmp(json, "{\"decision\":\"allow\",\"source\":\"automod\"}") != 0) {
        fprintf(stderr, "official surface lobby message moderation body mismatch\n");
        dcc_rest_official_body_json_free(json);
        return 0;
    }
    dcc_rest_official_body_json_free(json);
    json = NULL;

    if ((unsigned_route.flags & DCC_INTERACTION_ROUTE_F_UNSIGNED) == 0U) {
        fprintf(stderr, "official surface unsigned webhook route missing unsigned flag\n");
        return 0;
    }
    if (strcmp(dcc_webhook_event_type_name(DCC_WEBHOOK_EVENT_APPLICATION_AUTHORIZED),
               "APPLICATION_AUTHORIZED") != 0) {
        fprintf(stderr, "official surface webhook event type name mismatch\n");
        return 0;
    }
    if (dcc_webhook_event_type_from_string("LOBBY_MESSAGE_CREATE") !=
        DCC_WEBHOOK_EVENT_LOBBY_MESSAGE_CREATE ||
        dcc_webhook_event_type_from_string("PING") != DCC_WEBHOOK_EVENT_PING) {
        fprintf(stderr, "official surface webhook event type parse mismatch\n");
        return 0;
    }
    if (dcc_webhook_event_parse("{\"type\":0}", strlen("{\"type\":0}"), &event) != DCC_OK ||
        event.type != DCC_WEBHOOK_EVENT_PING ||
        event.timestamp != NULL ||
        event.timestamp_len != 0 ||
        event.event_json != NULL ||
        event.event_json_len != 0 ||
        event.data_json != NULL ||
        event.data_json_len != 0) {
        fprintf(stderr, "official surface webhook event parse mismatch\n");
        return 0;
    }
    const char event_payload[] =
        "{\"version\":1,\"application_id\":\"1234560123453231555\",\"type\":1,"
        "\"event\":{\"type\":\"LOBBY_MESSAGE_CREATE\","
        "\"timestamp\":\"2024-10-18T14:42:53.064834\","
        "\"data\":{\"lobby_id\":\"96008815106887111\"}}}";
    const char event_data[] = "{\"lobby_id\":\"96008815106887111\"}";
    event = (dcc_webhook_event_t){.size = sizeof(event)};
    if (dcc_webhook_event_parse(event_payload, sizeof(event_payload) - 1U, &event) != DCC_OK ||
        event.type != DCC_WEBHOOK_EVENT_LOBBY_MESSAGE_CREATE ||
        event.application_id != 1234560123453231555ULL ||
        event.event_json == NULL ||
        event.event_json_len == 0 ||
        event.data_json == NULL ||
        event.data_json_len != sizeof(event_data) - 1U ||
        memcmp(event.data_json, event_data, sizeof(event_data) - 1U) != 0) {
        fprintf(stderr, "official surface webhook event object span mismatch\n");
        return 0;
    }

    const char tricky_payload[] =
        "{\"version\":1,"
        "\"data\":{\"type\":0,\"application_id\":\"999\","
        "\"event\":{\"type\":\"APPLICATION_DEAUTHORIZED\",\"data\":{\"wrong\":true}}},"
        "\"type\":1,\"application_id\":\"1234560123453231555\","
        "\"event\":{\"type\":\"GAME_DIRECT_MESSAGE_CREATE\","
        "\"timestamp\":\"2024-10-18T14:42:53.064834\","
        "\"data\":{\"message_id\":\"456\"}}}";
    const char tricky_data[] = "{\"message_id\":\"456\"}";
    event = (dcc_webhook_event_t){.size = sizeof(event)};
    if (dcc_webhook_event_parse(tricky_payload, sizeof(tricky_payload) - 1U, &event) != DCC_OK ||
        event.type != DCC_WEBHOOK_EVENT_GAME_DIRECT_MESSAGE_CREATE ||
        event.application_id != 1234560123453231555ULL ||
        event.data_json == NULL ||
        event.data_json_len != sizeof(tricky_data) - 1U ||
        memcmp(event.data_json, tricky_data, sizeof(tricky_data) - 1U) != 0) {
        fprintf(stderr, "official surface webhook top-level envelope mismatch\n");
        return 0;
    }
    if (DCC_GATEWAY_REQUEST_MEMBERS(NULL, &members) != DCC_ERR_INVALID_ARG ||
        DCC_GATEWAY_REQUEST_SOUNDBOARD(NULL, NULL, 0) != DCC_ERR_INVALID_ARG ||
        DCC_GATEWAY_REQUEST_CHANNEL_INFO(NULL, 123, NULL, 0) != DCC_ERR_INVALID_ARG ||
        DCC_GATEWAY_UPDATE_PRESENCE(NULL, &presence) != DCC_ERR_INVALID_ARG ||
        DCC_GATEWAY_UPDATE_PRESENCE_RAW(NULL, "{}") != DCC_ERR_INVALID_ARG ||
        DCC_REST_GATEWAY(NULL, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_GATEWAY_BOT(NULL, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_CURRENT_APPLICATION(NULL, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_MODIFY_CURRENT_APPLICATION_PARAMS(
            NULL,
            &application_modify,
            package_official_rest_cb,
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        DCC_REST_ACTIVITY_INSTANCE(NULL, 123, "instance", package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_DEFAULT_SOUNDBOARD_SOUNDS(NULL, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_GUILD_SOUNDBOARD_SOUNDS(NULL, 123, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_GUILD_SOUNDBOARD_SOUND(NULL, 123, 456, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_SEND_SOUNDBOARD_PARAMS(NULL, 123, &soundboard_send, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_CREATE_GUILD_SOUNDBOARD_SOUND_PARAMS(NULL, 123, &guild_sound_create, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_MODIFY_GUILD_SOUNDBOARD_SOUND_PARAMS(NULL, 123, 456, &guild_sound_modify, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_DELETE_GUILD_SOUNDBOARD_SOUND(NULL, 123, 456, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_SKU_SUBSCRIPTIONS(NULL, 123, "limit=1", package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_SKU_SUBSCRIPTION(NULL, 123, 456, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_BULK_BAN_PARAMS(NULL, 123, &bulk_ban, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_GUILD_ROLE(NULL, 123, 456, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_ROLE_MEMBER_COUNTS(NULL, 123, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_WIDGET_JSON(NULL, 123, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_WIDGET_PNG(NULL, 123, "banner1", package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_INCIDENT_ACTIONS_PARAMS(NULL, 123, &incident_actions, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_INVITE_TARGET_USERS(NULL, "invite", package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_PUT_INVITE_TARGET_USERS(
            NULL,
            "invite",
            "targets.csv",
            "id\n123\n",
            7,
            package_official_rest_cb,
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        DCC_REST_INVITE_TARGET_USERS_JOB(NULL, "invite", package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_ENTITLEMENT(NULL, 123, 456, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_STICKER_PACK(NULL, 123, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_CURRENT_USER_GUILD_MEMBER(NULL, 123, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_DELETE_USER_ROLE_CONNECTION(NULL, 123, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_WEBHOOK_SLACK(NULL, 123, "token", "{\"text\":\"hi\"}", package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_WEBHOOK_GITHUB(NULL, 123, "token", "{\"zen\":\"hi\"}", package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_CREATE_LOBBY_PARAMS(NULL, &lobby_create, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_CREATE_OR_JOIN_LOBBY_PARAMS(NULL, &lobby_join, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_LOBBY(NULL, 123, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_MODIFY_LOBBY_PARAMS(NULL, 123, &lobby_modify, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_DELETE_LOBBY(NULL, 123, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_LINK_LOBBY_CHANNEL_PARAMS(NULL, 123, &lobby_link, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_ADD_LOBBY_MEMBER_PARAMS(NULL, 123, 444, &lobby_member, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_BULK_UPDATE_LOBBY_MEMBERS_PARAMS(NULL, 123, &lobby_bulk, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_REMOVE_LOBBY_MEMBER(NULL, 123, 456, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_LEAVE_LOBBY(NULL, 123, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_UNLINK_LOBBY_CHANNEL(NULL, 123, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_SEND_LOBBY_MESSAGE_PARAMS(NULL, 123, &lobby_message, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_LOBBY_MESSAGES(NULL, 123, "limit=1", package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_UPDATE_LOBBY_MESSAGE_MODERATION_PARAMS(NULL, 123, 456, &moderation, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_CREATE_LOBBY_INVITE_SELF(NULL, 123, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_CREATE_LOBBY_INVITE_USER(NULL, 123, 456, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_LEGACY_PIN_MESSAGE(NULL, 123, 456, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_LEGACY_UNPIN_MESSAGE(NULL, 123, 456, package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_REST_LEGACY_CHANNEL_PINS(NULL, 123, "limit=50", package_official_rest_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_OAUTH2_EXCHANGE_CODE(NULL, package_official_oauth_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_OAUTH2_REFRESH_TOKEN(NULL, package_official_oauth_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_OAUTH2_REVOKE_TOKEN(NULL, package_official_oauth_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_OAUTH2_ME(NULL, package_official_oauth_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_OAUTH2_CURRENT_BOT_APPLICATION(NULL, package_official_oauth_cb, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_EVENTS_ADD_ROUTE(NULL, "/events", &route) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "official surface invalid-argument alias check failed\n");
        return 0;
    }

    return 1;
}
