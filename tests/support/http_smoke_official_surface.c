#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <dcc/sugar.h>

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static dcc_status_t call_official_get_gateway(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_gateway(client, cb, user_data);
}

static dcc_status_t call_official_get_current_application(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_current_application(client, cb, user_data);
}

static dcc_status_t call_official_modify_current_application(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_current_application(client, "{\"description\":\"DCC\"}", cb, user_data);
}

static dcc_status_t call_official_modify_current_application_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    const char *scopes[] = {"bot", "applications.commands"};
    dcc_application_install_params_t install_params = {
        .size = sizeof(install_params),
        .scopes = scopes,
        .scope_count = 2,
        .permissions = "2048",
        .has_permissions = 1
    };
    dcc_application_modify_params_t params = {
        .size = sizeof(params),
        .description = "DCC",
        .install_params = &install_params,
        .event_webhooks_status = DCC_APPLICATION_EVENT_WEBHOOK_ENABLED,
        .has_description = 1,
        .has_install_params = 1,
        .has_event_webhooks_status = 1
    };
    return dcc_rest_modify_current_application_params(client, &params, cb, user_data);
}

static dcc_status_t call_official_get_activity_instance(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_activity_instance(client, 123, "inst/id", cb, user_data);
}

static dcc_status_t call_official_send_soundboard_sound(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_send_soundboard_sound(client, 222, "{\"sound_id\":\"777\"}", cb, user_data);
}

static dcc_status_t call_official_send_soundboard_sound_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_soundboard_send_params_t params = {
        .size = sizeof(params),
        .sound_id = 777,
        .source_guild_id = 333
    };
    return dcc_rest_send_soundboard_sound_params(client, 222, &params, cb, user_data);
}

static dcc_status_t call_official_get_default_soundboard_sounds(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_default_soundboard_sounds(client, cb, user_data);
}

static dcc_status_t call_official_get_guild_soundboard_sounds(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_soundboard_sounds(client, 333, cb, user_data);
}

static dcc_status_t call_official_get_guild_soundboard_sound(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_soundboard_sound(client, 333, 777, cb, user_data);
}

static dcc_status_t call_official_create_guild_soundboard_sound(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_guild_soundboard_sound(client, 333, "{\"name\":\"bell\"}", cb, user_data);
}

static dcc_status_t call_official_create_guild_soundboard_sound_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_guild_soundboard_sound_params_t params = {
        .size = sizeof(params),
        .name = "bell",
        .sound = "data:audio/ogg;base64,AAAA",
        .volume = 0.5,
        .emoji_name = "ding",
        .has_volume = 1,
        .has_emoji_name = 1
    };
    return dcc_rest_create_guild_soundboard_sound_params(client, 333, &params, cb, user_data);
}

static dcc_status_t call_official_modify_guild_soundboard_sound(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_soundboard_sound(client, 333, 777, "{\"name\":\"bell2\"}", cb, user_data);
}

static dcc_status_t call_official_modify_guild_soundboard_sound_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_guild_soundboard_sound_params_t params = {
        .size = sizeof(params),
        .name = "bell2",
        .volume = 1.0,
        .emoji_id = 0,
        .has_volume = 1,
        .has_emoji_id = 1
    };
    return dcc_rest_modify_guild_soundboard_sound_params(client, 333, 777, &params, cb, user_data);
}

static dcc_status_t call_official_delete_guild_soundboard_sound(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_guild_soundboard_sound(client, 333, 777, cb, user_data);
}

static dcc_status_t call_official_get_sku_subscriptions(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_sku_subscriptions(client, 444, "limit=2", cb, user_data);
}

static dcc_status_t call_official_get_sku_subscription(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_sku_subscription(client, 444, 555, cb, user_data);
}

static dcc_status_t call_official_bulk_ban_guild_members(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_bulk_ban_guild_members(client, 333, "{\"user_ids\":[\"444\"]}", cb, user_data);
}

static dcc_status_t call_official_bulk_ban_guild_members_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_snowflake_t users[] = {444, 555};
    dcc_bulk_ban_params_t params = {
        .size = sizeof(params),
        .user_ids = users,
        .user_id_count = 2,
        .delete_message_seconds = 60,
        .has_delete_message_seconds = 1
    };
    return dcc_rest_bulk_ban_guild_members_params(client, 333, &params, cb, user_data);
}

static dcc_status_t call_official_get_guild_role(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_role(client, 333, 555, cb, user_data);
}

static dcc_status_t call_official_get_guild_role_member_counts(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_role_member_counts(client, 333, cb, user_data);
}

static dcc_status_t call_official_get_guild_widget_json(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_widget_json(client, 333, cb, user_data);
}

static dcc_status_t call_official_get_guild_widget_png(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_guild_widget_png(client, 333, "banner1", cb, user_data);
}

static dcc_status_t call_official_modify_guild_incident_actions(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_guild_incident_actions(client, 333, "{\"invites_disabled_until\":null}", cb, user_data);
}

static dcc_status_t call_official_modify_guild_incident_actions_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_guild_incident_actions_params_t params = {
        .size = sizeof(params),
        .invites_disabled_until = NULL,
        .dms_disabled_until = "2026-06-27T00:00:00+00:00",
        .has_invites_disabled_until = 1,
        .has_dms_disabled_until = 1
    };
    return dcc_rest_modify_guild_incident_actions_params(client, 333, &params, cb, user_data);
}

static dcc_status_t call_official_get_invite_target_users(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_invite_target_users(client, "inv/abc", cb, user_data);
}

static dcc_status_t call_official_get_invite_target_users_job_status(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_get_invite_target_users_job_status(client, "inv/abc", cb, user_data);
}

static dcc_status_t call_official_get_entitlement(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_entitlement(client, 123, 999, cb, user_data);
}

static dcc_status_t call_official_get_sticker_pack(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_sticker_pack(client, 888, cb, user_data);
}

static dcc_status_t call_official_get_current_user_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_current_user_guild_member(client, 333, cb, user_data);
}

static dcc_status_t call_official_delete_current_user_application_role_connection(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_delete_current_user_application_role_connection(client, 123, cb, user_data);
}

static dcc_status_t call_official_execute_webhook_slack(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_execute_webhook_slack(client, 666, "tok/en", "{\"text\":\"hi\"}", cb, user_data);
}

static dcc_status_t call_official_execute_webhook_github(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_execute_webhook_github(client, 666, "tok/en", "{\"zen\":\"hi\"}", cb, user_data);
}

static dcc_status_t call_official_create_lobby(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_lobby(client, "{\"metadata\":{}}", cb, user_data);
}

static dcc_status_t call_official_create_lobby_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_lobby_member_spec_t members[] = {
        DCC_LOBBY_MEMBER_SPEC(
            .user_id = 444,
            .metadata_json = "{\"role\":\"tank\"}",
            .has_metadata = 1,
            .flags = 1,
            .has_flags = 1
        )
    };
    dcc_lobby_params_t params = {
        .size = sizeof(params),
        .metadata_json = "{\"topic\":\"redstone\"}",
        .members = members,
        .member_count = 1,
        .idle_timeout_seconds = 60,
        .has_metadata = 1,
        .has_members = 1,
        .has_idle_timeout_seconds = 1
    };
    return dcc_rest_create_lobby_params(client, &params, cb, user_data);
}

static dcc_status_t call_official_create_or_join_lobby(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_create_or_join_lobby(client, "{\"secret\":\"join\"}", cb, user_data);
}

static dcc_status_t call_official_create_or_join_lobby_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_lobby_create_or_join_params_t params = {
        .size = sizeof(params),
        .secret = "join-secret",
        .idle_timeout_seconds = 60,
        .lobby_metadata_json = "{\"topic\":\"redstone\"}",
        .member_metadata_json = NULL,
        .has_idle_timeout_seconds = 1,
        .has_lobby_metadata = 1,
        .has_member_metadata = 1
    };
    return dcc_rest_create_or_join_lobby_params(client, &params, cb, user_data);
}

static dcc_status_t call_official_get_lobby(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_lobby(client, 700, cb, user_data);
}

static dcc_status_t call_official_modify_lobby(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_modify_lobby(client, 700, "{\"metadata\":{\"k\":\"v\"}}", cb, user_data);
}

static dcc_status_t call_official_modify_lobby_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_lobby_params_t params = {
        .size = sizeof(params),
        .metadata_json = NULL,
        .members = NULL,
        .member_count = 0,
        .has_metadata = 1,
        .has_members = 1
    };
    return dcc_rest_modify_lobby_params(client, 700, &params, cb, user_data);
}

static dcc_status_t call_official_delete_lobby(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_delete_lobby(client, 700, cb, user_data);
}

static dcc_status_t call_official_add_lobby_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_add_lobby_member(client, 700, 444, "{\"metadata\":{}}", cb, user_data);
}

static dcc_status_t call_official_add_lobby_member_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_lobby_member_params_t params = {
        .size = sizeof(params),
        .metadata_json = "{\"role\":\"tank\"}",
        .flags = 1,
        .has_metadata = 1,
        .has_flags = 1
    };
    return dcc_rest_add_lobby_member_params(client, 700, 444, &params, cb, user_data);
}

static dcc_status_t call_official_bulk_update_lobby_members(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_bulk_update_lobby_members(client, 700, "[{\"id\":\"444\"}]", cb, user_data);
}

static dcc_status_t call_official_bulk_update_lobby_members_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_lobby_member_bulk_item_t items[] = {
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
    dcc_lobby_member_bulk_params_t params = {
        .size = sizeof(params),
        .items = items,
        .item_count = 2
    };
    return dcc_rest_bulk_update_lobby_members_params(client, 700, &params, cb, user_data);
}

static dcc_status_t call_official_remove_lobby_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_remove_lobby_member(client, 700, 444, cb, user_data);
}

static dcc_status_t call_official_leave_lobby(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_leave_lobby(client, 700, cb, user_data);
}

static dcc_status_t call_official_link_lobby_channel(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_link_lobby_channel(client, 700, "{\"channel_id\":\"222\"}", cb, user_data);
}

static dcc_status_t call_official_link_lobby_channel_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_lobby_channel_link_params_t params = {
        .size = sizeof(params),
        .channel_id = 222
    };
    return dcc_rest_link_lobby_channel_params(client, 700, &params, cb, user_data);
}

static dcc_status_t call_official_unlink_lobby_channel(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_unlink_lobby_channel(client, 700, cb, user_data);
}

static dcc_status_t call_official_send_lobby_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_send_lobby_message(client, 700, "{\"content\":\"hello\"}", cb, user_data);
}

static dcc_status_t call_official_send_lobby_message_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    dcc_lobby_message_params_t params = {
        .size = sizeof(params),
        .content = "hello",
        .metadata_json = "{\"kind\":\"test\"}",
        .flags = 4,
        .has_metadata = 1,
        .has_flags = 1
    };
    return dcc_rest_send_lobby_message_params(client, 700, &params, cb, user_data);
}

static dcc_status_t call_official_get_lobby_messages(dcc_client_t *client, dcc_rest_cb cb, void *user_data) {
    return dcc_rest_get_lobby_messages(client, 700, "limit=2", cb, user_data);
}

static dcc_status_t call_official_update_lobby_message_moderation_metadata(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_update_lobby_message_moderation_metadata(
        client,
        700,
        900,
        "{\"decision\":\"allow\"}",
        cb,
        user_data
    );
}

static dcc_status_t call_official_update_lobby_message_moderation_metadata_params(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_lobby_message_moderation_metadata_item_t items[] = {
        DCC_LOBBY_MODERATION_METADATA_ITEM("decision", "allow"),
        DCC_LOBBY_MODERATION_METADATA_ITEM("source", "automod")
    };
    dcc_lobby_message_moderation_metadata_params_t params = {
        .size = sizeof(params),
        .items = items,
        .item_count = 2
    };
    return dcc_rest_update_lobby_message_moderation_metadata_params(client, 700, 900, &params, cb, user_data);
}

static dcc_status_t call_official_create_lobby_channel_invite_for_self(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_create_lobby_channel_invite_for_self(client, 700, cb, user_data);
}

static dcc_status_t call_official_create_lobby_channel_invite_for_user(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_create_lobby_channel_invite_for_user(client, 700, 444, cb, user_data);
}

static int run_official_rest_wrapper_expect(
    dcc_client_t *client,
    const char *label,
    rest_wrapper_call_fn call,
    const char *expected_method,
    const char *expected_path,
    const char *expected_body
) {
    http_server_t server;
    pthread_t thread;
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start %s server: %s\n", label, strerror(errno));
        return 1;
    }

    rest_seen_t seen;
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    dcc_status_t st = call(client, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);

    int body_ok = expected_body != NULL ? strcmp(server.body, expected_body) == 0 : server.body_len == 0;
    if (st != DCC_OK ||
        !seen.called ||
        seen.status != 200 ||
        strcmp(server.method, expected_method) != 0 ||
        strcmp(server.path, expected_path) != 0 ||
        !body_ok) {
        fprintf(stderr,
                "unexpected %s wrapper request: st=%s called=%d status=%u method=%s path=%s body=%s\n",
                label,
                dcc_status_string(st),
                seen.called,
                seen.status,
                server.method,
                server.path,
                server.body);
        return 1;
    }
    return 0;
}

static int run_official_surface_multipart_smoke(dcc_client_t *client) {
    http_server_t server;
    pthread_t thread;
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start official multipart server: %s\n", strerror(errno));
        return 1;
    }

    rest_seen_t seen;
    const char csv[] = "id\n444\n";
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    dcc_status_t st = dcc_rest_put_invite_target_users(
        client,
        "inv/abc",
        "target.csv",
        csv,
        sizeof(csv) - 1U,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);

    if (st != DCC_OK ||
        !seen.called ||
        !expect_multipart_request(
            &server,
            "PUT",
            "/invites/inv%2Fabc/target-users",
            NULL,
            NULL,
            "name=\"target_users_file\"; filename=\"target.csv\"\r\nContent-Type: text/csv\r\n\r\nid\n444\n")) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        return 1;
    }
    return 0;
}

static int expect_official_body_json(
    const char *label,
    dcc_status_t status,
    char *json,
    const char *expected
) {
    int ok = status == DCC_OK && json != NULL && strcmp(json, expected) == 0;
    if (!ok) {
        fprintf(stderr,
                "unexpected %s official body: st=%s body=%s expected=%s\n",
                label,
                dcc_status_string(status),
                json != NULL ? json : "(null)",
                expected);
    }
    dcc_rest_official_body_json_free(json);
    return ok ? 0 : 1;
}

static int run_public_rest_official_body_builder_edges(void) {
    char *json = NULL;
    const char *scopes[] = {"bot", "applications.commands"};
    dcc_application_install_params_t install = {
        .size = sizeof(install),
        .scopes = scopes,
        .scope_count = 2,
        .permissions = "2048",
        .has_permissions = 1
    };
    dcc_application_integration_type_config_t integration_configs[] = {
        {
            .size = sizeof(integration_configs[0]),
            .integration_type = DCC_APPLICATION_INTEGRATION_TYPE_GUILD_INSTALL,
            .oauth2_install_params = &install,
            .has_oauth2_install_params = 1
        }
    };
    const char *tags[] = {"alpha", "quote\"tag"};
    const char *event_types[] = {"APPLICATION_AUTHORIZED", "LOBBY_MESSAGE_CREATE"};
    dcc_application_modify_params_t application = {
        .size = sizeof(application),
        .custom_install_url = "https://example.test/install?x=1",
        .description = "DCC \"runtime\" \\ core",
        .role_connections_verification_url = "https://example.test/verify",
        .install_params = &install,
        .integration_types_config = integration_configs,
        .integration_type_config_count = 1,
        .flags = 32768,
        .icon = NULL,
        .cover_image = "cover-data",
        .interactions_endpoint_url = "https://example.test/interactions",
        .tags = tags,
        .tag_count = 2,
        .event_webhooks_url = "https://example.test/events",
        .event_webhooks_status = DCC_APPLICATION_EVENT_WEBHOOK_ENABLED,
        .event_webhooks_types = event_types,
        .event_webhooks_type_count = 2,
        .has_custom_install_url = 1,
        .has_description = 1,
        .has_role_connections_verification_url = 1,
        .has_install_params = 1,
        .has_integration_types_config = 1,
        .has_flags = 1,
        .has_icon = 1,
        .has_cover_image = 1,
        .has_interactions_endpoint_url = 1,
        .has_tags = 1,
        .has_event_webhooks_url = 1,
        .has_event_webhooks_status = 1,
        .has_event_webhooks_types = 1
    };
    if (expect_official_body_json(
            "application_modify_escape_edges",
            dcc_rest_build_application_modify_body(&application, &json),
            json,
            "{\"custom_install_url\":\"https://example.test/install?x=1\",\"description\":\"DCC \\\"runtime\\\" \\\\ core\",\"role_connections_verification_url\":\"https://example.test/verify\",\"install_params\":{\"scopes\":[\"bot\",\"applications.commands\"],\"permissions\":\"2048\"},\"integration_types_config\":{\"0\":{\"oauth2_install_params\":{\"scopes\":[\"bot\",\"applications.commands\"],\"permissions\":\"2048\"}}},\"flags\":32768,\"icon\":null,\"cover_image\":\"cover-data\",\"interactions_endpoint_url\":\"https://example.test/interactions\",\"tags\":[\"alpha\",\"quote\\\"tag\"],\"event_webhooks_url\":\"https://example.test/events\",\"event_webhooks_status\":2,\"event_webhooks_types\":[\"APPLICATION_AUTHORIZED\",\"LOBBY_MESSAGE_CREATE\"]}") != 0) {
        return 1;
    }

    dcc_guild_incident_actions_params_t incident = {
        .size = sizeof(incident),
        .invites_disabled_until = "2026-06-27T00:00:00+00:00",
        .dms_disabled_until = NULL,
        .has_invites_disabled_until = 1,
        .has_dms_disabled_until = 1
    };
    json = NULL;
    if (expect_official_body_json(
            "incident_nullable_edges",
            dcc_rest_build_guild_incident_actions_body(&incident, &json),
            json,
            "{\"invites_disabled_until\":\"2026-06-27T00:00:00+00:00\",\"dms_disabled_until\":null}") != 0) {
        return 1;
    }

    dcc_lobby_message_params_t lobby_message = {
        .size = sizeof(lobby_message),
        .content = "hello \"world\" \\ slash",
        .metadata_json = "{\"raw\":true}",
        .flags = 4,
        .has_metadata = 1,
        .has_flags = 1
    };
    json = NULL;
    if (expect_official_body_json(
            "lobby_message_raw_metadata_edges",
            dcc_rest_build_lobby_message_body(&lobby_message, &json),
            json,
            "{\"content\":\"hello \\\"world\\\" \\\\ slash\",\"metadata\":{\"raw\":true},\"flags\":4}") != 0) {
        return 1;
    }

    dcc_lobby_message_moderation_metadata_item_t moderation_items[] = {
        DCC_LOBBY_MODERATION_METADATA_ITEM("decision\"key", "allow\\value"),
        DCC_LOBBY_MODERATION_METADATA_ITEM("source", "auto\"mod")
    };
    dcc_lobby_message_moderation_metadata_params_t moderation = {
        .size = sizeof(moderation),
        .items = moderation_items,
        .item_count = 2
    };
    json = NULL;
    if (expect_official_body_json(
            "lobby_moderation_metadata_escape_edges",
            dcc_rest_build_lobby_message_moderation_metadata_body(&moderation, &json),
            json,
            "{\"decision\\\"key\":\"allow\\\\value\",\"source\":\"auto\\\"mod\"}") != 0) {
        return 1;
    }

    return 0;
}

static int run_public_rest_official_surface_routes(dcc_client_t *client) {
    if (
        run_official_rest_wrapper_expect(client, "official_get_gateway", call_official_get_gateway,
            "GET", "/gateway", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_current_application",
            call_official_get_current_application, "GET", "/applications/@me", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_modify_current_application",
            call_official_modify_current_application, "PATCH", "/applications/@me",
            "{\"description\":\"DCC\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_modify_current_application_params",
            call_official_modify_current_application_params, "PATCH", "/applications/@me",
            "{\"description\":\"DCC\",\"install_params\":{\"scopes\":[\"bot\",\"applications.commands\"],\"permissions\":\"2048\"},\"event_webhooks_status\":2}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_activity_instance",
            call_official_get_activity_instance, "GET",
            "/applications/123/activity-instances/inst%2Fid", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_send_soundboard_sound",
            call_official_send_soundboard_sound, "POST", "/channels/222/send-soundboard-sound",
            "{\"sound_id\":\"777\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_send_soundboard_sound_params",
            call_official_send_soundboard_sound_params, "POST", "/channels/222/send-soundboard-sound",
            "{\"sound_id\":\"777\",\"source_guild_id\":\"333\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_default_soundboard_sounds",
            call_official_get_default_soundboard_sounds, "GET", "/soundboard-default-sounds", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_guild_soundboard_sounds",
            call_official_get_guild_soundboard_sounds, "GET", "/guilds/333/soundboard-sounds", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_guild_soundboard_sound",
            call_official_get_guild_soundboard_sound, "GET", "/guilds/333/soundboard-sounds/777", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_create_guild_soundboard_sound",
            call_official_create_guild_soundboard_sound, "POST", "/guilds/333/soundboard-sounds",
            "{\"name\":\"bell\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_create_guild_soundboard_sound_params",
            call_official_create_guild_soundboard_sound_params, "POST", "/guilds/333/soundboard-sounds",
            "{\"name\":\"bell\",\"sound\":\"data:audio/ogg;base64,AAAA\",\"volume\":0.5,\"emoji_name\":\"ding\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_modify_guild_soundboard_sound",
            call_official_modify_guild_soundboard_sound, "PATCH", "/guilds/333/soundboard-sounds/777",
            "{\"name\":\"bell2\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_modify_guild_soundboard_sound_params",
            call_official_modify_guild_soundboard_sound_params, "PATCH", "/guilds/333/soundboard-sounds/777",
            "{\"name\":\"bell2\",\"volume\":1,\"emoji_id\":null}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_delete_guild_soundboard_sound",
            call_official_delete_guild_soundboard_sound, "DELETE", "/guilds/333/soundboard-sounds/777", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_sku_subscriptions",
            call_official_get_sku_subscriptions, "GET", "/skus/444/subscriptions?limit=2", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_sku_subscription",
            call_official_get_sku_subscription, "GET", "/skus/444/subscriptions/555", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_bulk_ban_guild_members",
            call_official_bulk_ban_guild_members, "POST", "/guilds/333/bulk-ban",
            "{\"user_ids\":[\"444\"]}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_bulk_ban_guild_members_params",
            call_official_bulk_ban_guild_members_params, "POST", "/guilds/333/bulk-ban",
            "{\"user_ids\":[\"444\",\"555\"],\"delete_message_seconds\":60}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_guild_role",
            call_official_get_guild_role, "GET", "/guilds/333/roles/555", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_guild_role_member_counts",
            call_official_get_guild_role_member_counts, "GET", "/guilds/333/roles/member-counts", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_guild_widget_json",
            call_official_get_guild_widget_json, "GET", "/guilds/333/widget.json", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_guild_widget_png",
            call_official_get_guild_widget_png, "GET", "/guilds/333/widget.png?style=banner1", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_modify_guild_incident_actions",
            call_official_modify_guild_incident_actions, "PUT", "/guilds/333/incident-actions",
            "{\"invites_disabled_until\":null}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_modify_guild_incident_actions_params",
            call_official_modify_guild_incident_actions_params, "PUT", "/guilds/333/incident-actions",
            "{\"invites_disabled_until\":null,\"dms_disabled_until\":\"2026-06-27T00:00:00+00:00\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_invite_target_users",
            call_official_get_invite_target_users, "GET", "/invites/inv%2Fabc/target-users", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_invite_target_users_job_status",
            call_official_get_invite_target_users_job_status, "GET",
            "/invites/inv%2Fabc/target-users/job-status", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_entitlement",
            call_official_get_entitlement, "GET", "/applications/123/entitlements/999", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_sticker_pack",
            call_official_get_sticker_pack, "GET", "/sticker-packs/888", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_current_user_guild_member",
            call_official_get_current_user_guild_member, "GET", "/users/@me/guilds/333/member", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_delete_current_user_application_role_connection",
            call_official_delete_current_user_application_role_connection, "DELETE",
            "/users/@me/applications/123/role-connection", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_execute_webhook_slack",
            call_official_execute_webhook_slack, "POST", "/webhooks/666/tok%2Fen/slack",
            "{\"text\":\"hi\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_execute_webhook_github",
            call_official_execute_webhook_github, "POST", "/webhooks/666/tok%2Fen/github",
            "{\"zen\":\"hi\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_create_lobby",
            call_official_create_lobby, "POST", "/lobbies", "{\"metadata\":{}}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_create_lobby_params",
            call_official_create_lobby_params, "POST", "/lobbies",
            "{\"metadata\":{\"topic\":\"redstone\"},\"members\":[{\"id\":\"444\",\"metadata\":{\"role\":\"tank\"},\"flags\":1}],\"idle_timeout_seconds\":60}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_create_or_join_lobby",
            call_official_create_or_join_lobby, "PUT", "/lobbies", "{\"secret\":\"join\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_create_or_join_lobby_params",
            call_official_create_or_join_lobby_params, "PUT", "/lobbies",
            "{\"secret\":\"join-secret\",\"idle_timeout_seconds\":60,\"lobby_metadata\":{\"topic\":\"redstone\"},\"member_metadata\":null}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_lobby",
            call_official_get_lobby, "GET", "/lobbies/700", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_modify_lobby",
            call_official_modify_lobby, "PATCH", "/lobbies/700", "{\"metadata\":{\"k\":\"v\"}}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_modify_lobby_params",
            call_official_modify_lobby_params, "PATCH", "/lobbies/700",
            "{\"metadata\":null,\"members\":[]}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_delete_lobby",
            call_official_delete_lobby, "DELETE", "/lobbies/700", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_add_lobby_member",
            call_official_add_lobby_member, "PUT", "/lobbies/700/members/444", "{\"metadata\":{}}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_add_lobby_member_params",
            call_official_add_lobby_member_params, "PUT", "/lobbies/700/members/444",
            "{\"metadata\":{\"role\":\"tank\"},\"flags\":1}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_bulk_update_lobby_members",
            call_official_bulk_update_lobby_members, "POST", "/lobbies/700/members/bulk",
            "[{\"id\":\"444\"}]") != 0 ||
        run_official_rest_wrapper_expect(client, "official_bulk_update_lobby_members_params",
            call_official_bulk_update_lobby_members_params, "POST", "/lobbies/700/members/bulk",
            "[{\"id\":\"444\",\"metadata\":{\"role\":\"tank\"},\"flags\":1},{\"id\":\"555\",\"remove_member\":true}]") != 0 ||
        run_official_rest_wrapper_expect(client, "official_remove_lobby_member",
            call_official_remove_lobby_member, "DELETE", "/lobbies/700/members/444", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_leave_lobby",
            call_official_leave_lobby, "DELETE", "/lobbies/700/members/@me", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_link_lobby_channel",
            call_official_link_lobby_channel, "PATCH", "/lobbies/700/channel-linking",
            "{\"channel_id\":\"222\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_link_lobby_channel_params",
            call_official_link_lobby_channel_params, "PATCH", "/lobbies/700/channel-linking",
            "{\"channel_id\":\"222\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_unlink_lobby_channel",
            call_official_unlink_lobby_channel, "PATCH", "/lobbies/700/channel-linking", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_send_lobby_message",
            call_official_send_lobby_message, "POST", "/lobbies/700/messages", "{\"content\":\"hello\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_send_lobby_message_params",
            call_official_send_lobby_message_params, "POST", "/lobbies/700/messages",
            "{\"content\":\"hello\",\"metadata\":{\"kind\":\"test\"},\"flags\":4}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_get_lobby_messages",
            call_official_get_lobby_messages, "GET", "/lobbies/700/messages?limit=2", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_update_lobby_message_moderation_metadata",
            call_official_update_lobby_message_moderation_metadata, "PUT",
            "/lobbies/700/messages/900/moderation-metadata", "{\"decision\":\"allow\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_update_lobby_message_moderation_metadata_params",
            call_official_update_lobby_message_moderation_metadata_params, "PUT",
            "/lobbies/700/messages/900/moderation-metadata", "{\"decision\":\"allow\",\"source\":\"automod\"}") != 0 ||
        run_official_rest_wrapper_expect(client, "official_create_lobby_channel_invite_for_self",
            call_official_create_lobby_channel_invite_for_self, "POST",
            "/lobbies/700/members/@me/invites", NULL) != 0 ||
        run_official_rest_wrapper_expect(client, "official_create_lobby_channel_invite_for_user",
            call_official_create_lobby_channel_invite_for_user, "POST",
            "/lobbies/700/members/444/invites", NULL) != 0
    ) {
        return 1;
    }
    return run_official_surface_multipart_smoke(client);
}

typedef struct oauth_seen {
    int called;
    uint16_t status;
    dcc_status_t error;
} oauth_seen_t;

static void oauth_cb(const dcc_rest_response_t *response, void *user_data) {
    oauth_seen_t *seen = (oauth_seen_t *)user_data;
    seen->called = 1;
    seen->status = response != NULL ? response->status : 0;
    seen->error = response != NULL ? response->error : DCC_ERR_INVALID_ARG;
}

static int run_oauth2_expect(
    const char *label,
    dcc_status_t (*call)(dcc_oauth2_cb, void *),
    const char *expected_method,
    const char *expected_path,
    const char *expected_body,
    const char *expected_authorization
) {
    http_server_t server;
    pthread_t thread;
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start %s OAuth2 server: %s\n", label, strerror(errno));
        return 1;
    }

    oauth_seen_t seen;
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    dcc_status_t st = call(oauth_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);

    int body_ok = expected_body != NULL ? strcmp(server.body, expected_body) == 0 : server.body_len == 0;
    int auth_ok = expected_authorization == NULL ||
        strstr(server.headers, expected_authorization) != NULL;
    int content_type_ok = expected_body == NULL ||
        strstr(server.headers, "Content-Type: application/x-www-form-urlencoded") != NULL;

    if (st != DCC_OK ||
        !seen.called ||
        seen.status != 200 ||
        strcmp(server.method, expected_method) != 0 ||
        strcmp(server.path, expected_path) != 0 ||
        !body_ok ||
        !auth_ok ||
        !content_type_ok) {
        fprintf(stderr,
                "unexpected %s OAuth2 request: st=%s called=%d status=%u method=%s path=%s body=%s headers=%s\n",
                label,
                dcc_status_string(st),
                seen.called,
                seen.status,
                server.method,
                server.path,
                server.body,
                server.headers);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        return 1;
    }
    return 0;
}

static dcc_status_t call_oauth2_me(dcc_oauth2_cb cb, void *user_data) {
    return dcc_oauth2_get_current_authorization("access/token", cb, user_data);
}

static dcc_status_t call_oauth2_current_bot_application(dcc_oauth2_cb cb, void *user_data) {
    return dcc_oauth2_get_current_bot_application("bot/token", cb, user_data);
}

static dcc_status_t call_oauth2_exchange(dcc_oauth2_cb cb, void *user_data) {
    dcc_oauth2_exchange_code_options_t options = DCC_OAUTH2_EXCHANGE_CODE_OPTIONS(
        .client_id = "123",
        .client_secret = "sec/ret",
        .code = "code 1",
        .redirect_uri = "https://example.test/cb"
    );
    return dcc_oauth2_exchange_code(&options, cb, user_data);
}

static dcc_status_t call_oauth2_refresh(dcc_oauth2_cb cb, void *user_data) {
    dcc_oauth2_refresh_token_options_t options = DCC_OAUTH2_REFRESH_TOKEN_OPTIONS(
        .client_id = "123",
        .client_secret = "sec/ret",
        .refresh_token = "refresh token"
    );
    return dcc_oauth2_refresh_token(&options, cb, user_data);
}

static dcc_status_t call_oauth2_revoke(dcc_oauth2_cb cb, void *user_data) {
    dcc_oauth2_revoke_token_options_t options = DCC_OAUTH2_REVOKE_TOKEN_OPTIONS(
        .client_id = "123",
        .client_secret = "sec/ret",
        .token = "refresh token",
        .token_type_hint = "refresh_token"
    );
    return dcc_oauth2_revoke_token(&options, cb, user_data);
}

static int run_public_oauth2_surface_smoke(void) {
    if (
        run_oauth2_expect("oauth2_me", call_oauth2_me, "GET", "/oauth2/@me", NULL,
            "Authorization: Bearer access/token") != 0 ||
        run_oauth2_expect("oauth2_current_bot_application", call_oauth2_current_bot_application,
            "GET", "/oauth2/applications/@me", NULL, "Authorization: Bot bot/token") != 0 ||
        run_oauth2_expect("oauth2_exchange", call_oauth2_exchange, "POST", "/oauth2/token",
            "grant_type=authorization_code&client_id=123&client_secret=sec%2Fret&code=code%201&redirect_uri=https%3A%2F%2Fexample.test%2Fcb",
            NULL) != 0 ||
        run_oauth2_expect("oauth2_refresh", call_oauth2_refresh, "POST", "/oauth2/token",
            "grant_type=refresh_token&client_id=123&client_secret=sec%2Fret&refresh_token=refresh%20token",
            NULL) != 0 ||
        run_oauth2_expect("oauth2_revoke", call_oauth2_revoke, "POST", "/oauth2/token/revoke",
            "client_id=123&client_secret=sec%2Fret&token=refresh%20token&token_type_hint=refresh_token",
            NULL) != 0
    ) {
        return 1;
    }
    return 0;
}

static int run_public_webhook_event_surface_smoke(void) {
    const char ping[] = "{\"type\":0}";
    dcc_webhook_event_t event = {.size = sizeof(event)};
    if (dcc_webhook_event_parse(ping, sizeof(ping) - 1U, &event) != DCC_OK ||
        event.type != DCC_WEBHOOK_EVENT_PING ||
        strcmp(dcc_webhook_event_type_name(event.type), "PING") != 0 ||
        dcc_webhook_event_type_from_string("PING") != DCC_WEBHOOK_EVENT_PING) {
        fprintf(stderr, "webhook event PING parse failed\n");
        return 1;
    }

    const char timestamp[] = "2024-10-18T14:42:53.064834";
    const char payload[] =
        "{\"type\":1,\"event\":{\"type\":\"APPLICATION_AUTHORIZED\","
        "\"note\":\"timestamp\","
        "\"timestamp\":\"2024-10-18T14:42:53.064834\","
        "\"data\":{\"guild_id\":\"456\",\"timestamp\":\"nested\"}},"
        "\"application_id\":\"123\"}";
    const char data_json[] = "{\"guild_id\":\"456\",\"timestamp\":\"nested\"}";
    event = (dcc_webhook_event_t){.size = sizeof(event)};
    if (dcc_webhook_event_parse(payload, sizeof(payload) - 1U, &event) != DCC_OK ||
        event.type != DCC_WEBHOOK_EVENT_APPLICATION_AUTHORIZED ||
        event.application_id != 123 ||
        strcmp(event.type_name, "APPLICATION_AUTHORIZED") != 0 ||
        event.timestamp == NULL ||
        event.timestamp_len != sizeof(timestamp) - 1U ||
        memcmp(event.timestamp, timestamp, sizeof(timestamp) - 1U) != 0 ||
        event.event_json == NULL ||
        event.event_json_len == 0 ||
        event.data_json == NULL ||
        event.data_json_len != sizeof(data_json) - 1U ||
        memcmp(event.data_json, data_json, sizeof(data_json) - 1U) != 0 ||
        dcc_webhook_event_type_from_string("LOBBY_MESSAGE_CREATE") != DCC_WEBHOOK_EVENT_LOBBY_MESSAGE_CREATE ||
        dcc_webhook_events_add_route(NULL, "/events", &(dcc_webhook_event_route_t){.size = sizeof(dcc_webhook_event_route_t)}) !=
            DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "webhook event parse/type smoke failed\n");
        return 1;
    }

    const char tricky_data_json[] = "{\"message_id\":\"456\"}";
    const char tricky_payload[] =
        "{\"version\":1,"
        "\"data\":{\"type\":0,\"application_id\":\"999\","
        "\"event\":{\"type\":\"APPLICATION_DEAUTHORIZED\",\"data\":{\"wrong\":true}}},"
        "\"type\":1,\"application_id\":\"123\","
        "\"event\":{\"type\":\"GAME_DIRECT_MESSAGE_CREATE\","
        "\"timestamp\":\"2024-10-18T14:42:53.064834\","
        "\"data\":{\"message_id\":\"456\"}}}";
    event = (dcc_webhook_event_t){.size = sizeof(event)};
    if (dcc_webhook_event_parse(tricky_payload, sizeof(tricky_payload) - 1U, &event) != DCC_OK ||
        event.type != DCC_WEBHOOK_EVENT_GAME_DIRECT_MESSAGE_CREATE ||
        event.application_id != 123 ||
        event.data_json == NULL ||
        event.data_json_len != sizeof(tricky_data_json) - 1U ||
        memcmp(event.data_json, tricky_data_json, sizeof(tricky_data_json) - 1U) != 0) {
        fprintf(stderr, "webhook event top-level envelope parse failed\n");
        return 1;
    }
    return 0;
}

int run_public_rest_official_surface_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "official surface client create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    int ok = run_public_rest_official_surface_routes(client) == 0 &&
             run_public_rest_official_body_builder_edges() == 0 &&
             run_public_oauth2_surface_smoke() == 0 &&
             run_public_webhook_event_surface_smoke() == 0;
    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);
    return ok ? 0 : 1;
}

#endif
