#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_public_rest_gap_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    dcc_message_builder_t message_builder;
    dcc_message_builder_init(&message_builder);
    st = dcc_message_builder_set_content(&message_builder, "builder");
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_message_builder_set_content failed: %s\n", dcc_status_string(st));
        dcc_client_destroy(client);
        return 1;
    }
    const dcc_application_command_permission_t command_permissions[] = {
        {
            .id = 555,
            .type = 1,
            .permission = 1
        }
    };
    const dcc_guild_command_permissions_params_t command_permission_params = {
        .size = sizeof(command_permission_params),
        .application_id = 123,
        .guild_id = 333,
        .command_id = 444,
        .permissions = command_permissions,
        .permission_count = sizeof(command_permissions) / sizeof(command_permissions[0])
    };
    const dcc_guild_command_permissions_update_params_t command_permission_updates[] = {
        {
            .command_id = 444,
            .permissions = command_permissions,
            .permission_count = sizeof(command_permissions) / sizeof(command_permissions[0])
        }
    };
    const dcc_bulk_guild_command_permissions_params_t bulk_command_permission_params = {
        .size = sizeof(bulk_command_permission_params),
        .application_id = 123,
        .guild_id = 333,
        .commands = command_permission_updates,
        .command_count = sizeof(command_permission_updates) / sizeof(command_permission_updates[0])
    };

    http_server_t server;
    pthread_t thread;
    rest_seen_t seen;

#define EXPECT_REST_GAP(label, expected_method, expected_path, expected_body, call_expr) \
    do { \
        if (start_server(&server, &thread) != 0) { \
            fprintf(stderr, "failed to start %s server: %s\n", (label), strerror(errno)); \
            (void)unsetenv("DCC_DISCORD_API_BASE"); \
            dcc_client_destroy(client); \
            return 1; \
        } \
        set_api_base_for_server(&server); \
        memset(&seen, 0, sizeof(seen)); \
        st = (call_expr); \
        (void)pthread_join(thread, NULL); \
        close(server.fd); \
        int body_ok__ = (expected_body) != NULL ? strcmp(server.body, (expected_body)) == 0 : server.body_len == 0; \
        if (st != DCC_OK || \
            !seen.called || \
            seen.status != 200 || \
            strcmp(server.method, (expected_method)) != 0 || \
            strcmp(server.path, (expected_path)) != 0 || \
            !body_ok__) { \
            fprintf(stderr, \
                    "unexpected %s gap request: st=%s called=%d status=%u method=%s path=%s body=%s\n", \
                    (label), \
                    dcc_status_string(st), \
                    seen.called, \
                    seen.status, \
                    server.method, \
                    server.path, \
                    server.body); \
            (void)unsetenv("DCC_DISCORD_API_BASE"); \
            dcc_client_destroy(client); \
            return 1; \
        } \
    } while (0)

    EXPECT_REST_GAP(
        "modify_channel",
        "PATCH",
        "/channels/222",
        "{\"name\":\"ops\"}",
        dcc_rest_modify_channel(client, 222, "{\"name\":\"ops\"}", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "delete_channel",
        "DELETE",
        "/channels/222",
        NULL,
        dcc_rest_delete_channel(client, 222, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "get_channel_messages",
        "GET",
        "/channels/222/messages?limit=2&before=777",
        NULL,
        dcc_rest_get_channel_messages(client, 222, "limit=2&before=777", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "get_channel_messages_page",
        "GET",
        "/channels/222/messages?after=666&around=888&before=777&limit=100",
        NULL,
        dcc_rest_get_channel_messages_page(client, 222, 888, 777, 666, 500, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "edit_message",
        "PATCH",
        "/channels/222/messages/777",
        "{\"content\":\"edited\"}",
        dcc_rest_edit_message(client, 222, 777, "{\"content\":\"edited\"}", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "delete_message",
        "DELETE",
        "/channels/222/messages/777",
        NULL,
        dcc_rest_delete_message(client, 222, 777, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "get_guild",
        "GET",
        "/guilds/333",
        NULL,
        dcc_rest_get_guild(client, 333, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "modify_guild",
        "PATCH",
        "/guilds/333",
        "{\"name\":\"ops\"}",
        dcc_rest_modify_guild(client, 333, "{\"name\":\"ops\"}", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "bulk_overwrite_global_commands",
        "PUT",
        "/applications/123/commands",
        "[{\"name\":\"ping\",\"type\":1}]",
        dcc_rest_bulk_overwrite_global_commands(client, 123, "[{\"name\":\"ping\",\"type\":1}]", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "bulk_overwrite_application_commands_default_global",
        "PUT",
        "/applications/123/commands",
        "[{\"name\":\"ping\",\"type\":1}]",
        dcc_rest_bulk_overwrite_application_commands(
            client,
            123,
            NULL,
            "[{\"name\":\"ping\",\"type\":1}]",
            rest_cb,
            &seen
        )
    );
    EXPECT_REST_GAP(
        "get_global_commands",
        "GET",
        "/applications/123/commands?with_localizations=true",
        NULL,
        dcc_rest_get_global_commands(client, 123, "with_localizations=true", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "get_global_commands_with_localizations",
        "GET",
        "/applications/123/commands?with_localizations=true",
        NULL,
        dcc_rest_get_global_commands_with_localizations(client, 123, 1, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "get_global_command",
        "GET",
        "/applications/123/commands/444",
        NULL,
        dcc_rest_get_global_command(client, 123, 444, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "create_global_command",
        "POST",
        "/applications/123/commands",
        "{\"name\":\"ping\",\"type\":1}",
        dcc_rest_create_global_command(client, 123, "{\"name\":\"ping\",\"type\":1}", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "create_application_command_default_global",
        "POST",
        "/applications/123/commands",
        "{\"name\":\"ping\",\"type\":1}",
        dcc_rest_create_application_command(client, 123, NULL, "{\"name\":\"ping\",\"type\":1}", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "edit_global_command",
        "PATCH",
        "/applications/123/commands/444",
        "{\"description\":\"pong\"}",
        dcc_rest_edit_global_command(client, 123, 444, "{\"description\":\"pong\"}", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "delete_global_command",
        "DELETE",
        "/applications/123/commands/444",
        NULL,
        dcc_rest_delete_global_command(client, 123, 444, rest_cb, &seen)
    );
    dcc_application_command_registration_options_t global_command_registration;
    dcc_application_command_registration_options_init(&global_command_registration);
    EXPECT_REST_GAP(
        "delete_all_application_commands_default_global",
        "PUT",
        "/applications/123/commands",
        "[]",
        dcc_rest_delete_all_application_commands(client, 123, &global_command_registration, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "bulk_overwrite_guild_commands",
        "PUT",
        "/applications/123/guilds/333/commands",
        "[{\"name\":\"guildping\",\"type\":1}]",
        dcc_rest_bulk_overwrite_guild_commands(
            client,
            123,
            333,
            "[{\"name\":\"guildping\",\"type\":1}]",
            rest_cb,
            &seen
        )
    );
    EXPECT_REST_GAP(
        "get_guild_commands",
        "GET",
        "/applications/123/guilds/333/commands?with_localizations=true",
        NULL,
        dcc_rest_get_guild_commands(client, 123, 333, "with_localizations=true", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "get_guild_commands_with_localizations",
        "GET",
        "/applications/123/guilds/333/commands?with_localizations=true",
        NULL,
        dcc_rest_get_guild_commands_with_localizations(client, 123, 333, 1, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "get_guild_command",
        "GET",
        "/applications/123/guilds/333/commands/444",
        NULL,
        dcc_rest_get_guild_command(client, 123, 333, 444, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "create_guild_command",
        "POST",
        "/applications/123/guilds/333/commands",
        "{\"name\":\"guildping\",\"type\":1}",
        dcc_rest_create_guild_command(client, 123, 333, "{\"name\":\"guildping\",\"type\":1}", rest_cb, &seen)
    );
    dcc_application_command_registration_options_t command_registration;
    if (dcc_application_command_registration_options_set_guild(&command_registration, 333) != DCC_OK) {
        fprintf(stderr, "failed to set specific guild command registration options\n");
        dcc_client_destroy(client);
        return 1;
    }
    EXPECT_REST_GAP(
        "bulk_overwrite_application_commands_specific_guild",
        "PUT",
        "/applications/123/guilds/333/commands",
        "[{\"name\":\"guildping\",\"type\":1}]",
        dcc_rest_bulk_overwrite_application_commands(
            client,
            123,
            &command_registration,
            "[{\"name\":\"guildping\",\"type\":1}]",
            rest_cb,
            &seen
        )
    );
    EXPECT_REST_GAP(
        "create_application_command_specific_guild",
        "POST",
        "/applications/123/guilds/333/commands",
        "{\"name\":\"guildping\",\"type\":1}",
        dcc_rest_create_application_command(
            client,
            123,
            &command_registration,
            "{\"name\":\"guildping\",\"type\":1}",
            rest_cb,
            &seen
        )
    );
    EXPECT_REST_GAP(
        "delete_all_application_commands_specific_guild",
        "PUT",
        "/applications/123/guilds/333/commands",
        "[]",
        dcc_rest_delete_all_application_commands(client, 123, &command_registration, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "edit_guild_command",
        "PATCH",
        "/applications/123/guilds/333/commands/444",
        "{\"description\":\"guildpong\"}",
        dcc_rest_edit_guild_command(client, 123, 333, 444, "{\"description\":\"guildpong\"}", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "delete_guild_command",
        "DELETE",
        "/applications/123/guilds/333/commands/444",
        NULL,
        dcc_rest_delete_guild_command(client, 123, 333, 444, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "get_all_guild_command_permissions",
        "GET",
        "/applications/123/guilds/333/commands/permissions",
        NULL,
        dcc_rest_get_all_guild_command_permissions(client, 123, 333, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "get_guild_command_permissions",
        "GET",
        "/applications/123/guilds/333/commands/444/permissions",
        NULL,
        dcc_rest_get_guild_command_permissions(client, 123, 333, 444, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "edit_guild_command_permissions_params",
        "PUT",
        "/applications/123/guilds/333/commands/444/permissions",
        "{\"permissions\":[{\"id\":\"555\",\"type\":1,\"permission\":true}]}",
        dcc_rest_edit_guild_command_permissions_params(client, &command_permission_params, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "bulk_edit_guild_command_permissions_params",
        "PUT",
        "/applications/123/guilds/333/commands/permissions",
        "[{\"id\":\"444\",\"permissions\":[{\"id\":\"555\",\"type\":1,\"permission\":true}]}]",
        dcc_rest_bulk_edit_guild_command_permissions_params(client, &bulk_command_permission_params, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "interaction_original_response_get",
        "GET",
        "/webhooks/123/tok/messages/@original",
        NULL,
        dcc_rest_interaction_original_response_get(client, 123, "tok", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "interaction_original_response_edit",
        "PATCH",
        "/webhooks/123/tok/messages/@original",
        "{\"content\":\"edited\"}",
        dcc_rest_interaction_original_response_edit(client, 123, "tok", "{\"content\":\"edited\"}", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "interaction_original_response_edit_builder",
        "PATCH",
        "/webhooks/123/tok/messages/@original",
        "{\"content\":\"builder\"}",
        dcc_rest_interaction_original_response_edit_builder(client, 123, "tok", &message_builder, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "interaction_original_response_delete",
        "DELETE",
        "/webhooks/123/tok/messages/@original",
        NULL,
        dcc_rest_interaction_original_response_delete(client, 123, "tok", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "interaction_followup_create",
        "POST",
        "/webhooks/123/tok",
        "{\"content\":\"follow\"}",
        dcc_rest_interaction_followup_create(client, 123, "tok", "{\"content\":\"follow\"}", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "interaction_followup_create_builder",
        "POST",
        "/webhooks/123/tok",
        "{\"content\":\"builder\"}",
        dcc_rest_interaction_followup_create_builder(client, 123, "tok", &message_builder, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "interaction_followup_get",
        "GET",
        "/webhooks/123/tok/messages/777",
        NULL,
        dcc_rest_interaction_followup_get(client, 123, "tok", 777, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "interaction_followup_edit",
        "PATCH",
        "/webhooks/123/tok/messages/777",
        "{\"content\":\"follow-edit\"}",
        dcc_rest_interaction_followup_edit(client, 123, "tok", 777, "{\"content\":\"follow-edit\"}", rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "interaction_followup_edit_builder",
        "PATCH",
        "/webhooks/123/tok/messages/777",
        "{\"content\":\"builder\"}",
        dcc_rest_interaction_followup_edit_builder(client, 123, "tok", 777, &message_builder, rest_cb, &seen)
    );
    EXPECT_REST_GAP(
        "interaction_followup_delete",
        "DELETE",
        "/webhooks/123/tok/messages/777",
        NULL,
        dcc_rest_interaction_followup_delete(client, 123, "tok", 777, rest_cb, &seen)
    );

#undef EXPECT_REST_GAP

    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);
    return 0;
}
#endif
