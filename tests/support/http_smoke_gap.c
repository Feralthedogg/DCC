#include "http_smoke_support.h"
#include "task9_test_legacy_shims.h"

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
    if (st == DCC_OK) {
        st = rest_activate_client(client);
    }
    if (st != DCC_OK) {
        fprintf(stderr, "REST gap client setup failed: %s\n", dcc_status_string(st));
        dcc_client_destroy(client);
        return 1;
    }
    dcc_message_builder_t message_builder;
    dcc_message_builder_t edited_message;
    dcc_message_builder_t follow_message;
    dcc_message_builder_t follow_edit_message;
    dcc_message_builder_init(&message_builder);
    dcc_message_builder_init(&edited_message);
    dcc_message_builder_init(&follow_message);
    dcc_message_builder_init(&follow_edit_message);
    if (dcc_message_builder_set_content(&message_builder, "builder") != DCC_OK ||
        dcc_message_builder_set_content(&edited_message, "edited") != DCC_OK ||
        dcc_message_builder_set_content(&follow_message, "follow") != DCC_OK ||
        dcc_message_builder_set_content(&follow_edit_message, "follow-edit") != DCC_OK) {
        fprintf(stderr, "failed to initialize endpoint message payloads\n");
        dcc_client_destroy(client);
        return 1;
    }
    dcc_rest_message_payload_t builder_payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_t edited_payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_t follow_payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_t follow_edit_payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    builder_payload.message = &message_builder;
    edited_payload.message = &edited_message;
    follow_payload.message = &follow_message;
    follow_edit_payload.message = &follow_edit_message;
    dcc_rest_message_list_query_t messages_before = DCC_REST_MESSAGE_LIST_QUERY_INIT;
    messages_before.present = DCC_REST_MESSAGE_LIST_QUERY_PRESENT_BEFORE |
        DCC_REST_MESSAGE_LIST_QUERY_PRESENT_LIMIT;
    messages_before.before = 777;
    messages_before.limit = 2;
    dcc_rest_message_list_query_t messages_around = DCC_REST_MESSAGE_LIST_QUERY_INIT;
    messages_around.present = DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AROUND |
        DCC_REST_MESSAGE_LIST_QUERY_PRESENT_LIMIT;
    messages_around.around = 888;
    messages_around.limit = 100;
    dcc_rest_message_list_query_t messages_after = DCC_REST_MESSAGE_LIST_QUERY_INIT;
    messages_after.present = DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AFTER |
        DCC_REST_MESSAGE_LIST_QUERY_PRESENT_LIMIT;
    messages_after.after = 666;
    messages_after.limit = 25;

    const dcc_application_command_permission_t command_permissions[] = {
        {
            .id = 555,
            .type = 1,
            .permission = 1
        }
    };
    const dcc_guild_command_permissions_params_t command_permission_params = {
        .size = sizeof(command_permission_params),
        .version = DCC_GUILD_COMMAND_PERMISSIONS_PARAMS_VERSION,
        .present = DCC_GUILD_COMMAND_PERMISSIONS_PARAMS_PRESENT_PERMISSIONS,
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
    dcc_rest_call_options_t call_options = rest_call_options(&seen);
    dcc_channel_params_t channel_modify = DCC_CHANNEL_PARAMS_INIT;
    channel_modify.payload.guild.present = DCC_CHANNEL_GUILD_PRESENT_NAME;
    channel_modify.payload.guild.name = "ops";
    dcc_rest_guild_update_t guild_modify = DCC_REST_GUILD_UPDATE_INIT;
    guild_modify.present = DCC_REST_GUILD_UPDATE_PRESENT_NAME;
    guild_modify.name = "ops";

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
        st = rest_await_submission(client, st); \
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
        dcc_rest_modify_channel(client, 222, &channel_modify, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "delete_channel",
        "DELETE",
        "/channels/222",
        NULL,
        dcc_rest_delete_channel(client, 222, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "get_channel_messages",
        "GET",
        "/channels/222/messages?before=777&limit=2",
        NULL,
        dcc_rest_get_channel_messages(client, 222, &messages_before, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "get_channel_messages_page",
        "GET",
        "/channels/222/messages?around=888&limit=100",
        NULL,
        dcc_rest_get_channel_messages(client, 222, &messages_around, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "get_channel_messages_after",
        "GET",
        "/channels/222/messages?after=666&limit=25",
        NULL,
        dcc_rest_get_channel_messages(client, 222, &messages_after, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "edit_message",
        "PATCH",
        "/channels/222/messages/777",
        "{\"content\":\"edited\"}",
        dcc_rest_edit_message(client, 222, 777, &edited_payload, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "delete_message",
        "DELETE",
        "/channels/222/messages/777",
        NULL,
        dcc_rest_delete_message(client, 222, 777, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "get_guild",
        "GET",
        "/guilds/333",
        NULL,
        dcc_rest_get_guild(client, 333, NULL, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "modify_guild",
        "PATCH",
        "/guilds/333",
        "{\"name\":\"ops\"}",
        dcc_rest_modify_guild(client, 333, &guild_modify, &call_options, NULL)
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
        "interaction_original_response_get",
        "GET",
        "/webhooks/123/tok/messages/@original",
        NULL,
        dcc_rest_interaction_original_response_get(client, 123, "tok", &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "interaction_original_response_edit",
        "PATCH",
        "/webhooks/123/tok/messages/@original",
        "{\"content\":\"edited\"}",
        dcc_rest_interaction_original_response_edit(client, 123, "tok", &edited_payload, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "interaction_original_response_edit_builder",
        "PATCH",
        "/webhooks/123/tok/messages/@original",
        "{\"content\":\"builder\"}",
        dcc_rest_interaction_original_response_edit(client, 123, "tok", &builder_payload, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "interaction_original_response_delete",
        "DELETE",
        "/webhooks/123/tok/messages/@original",
        NULL,
        dcc_rest_interaction_original_response_delete(client, 123, "tok", &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "interaction_followup_create",
        "POST",
        "/webhooks/123/tok",
        "{\"content\":\"follow\"}",
        dcc_rest_interaction_followup_create(client, 123, "tok", &follow_payload, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "interaction_followup_create_builder",
        "POST",
        "/webhooks/123/tok",
        "{\"content\":\"builder\"}",
        dcc_rest_interaction_followup_create(client, 123, "tok", &builder_payload, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "interaction_followup_get",
        "GET",
        "/webhooks/123/tok/messages/777",
        NULL,
        dcc_rest_interaction_followup_get(client, 123, "tok", 777, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "interaction_followup_edit",
        "PATCH",
        "/webhooks/123/tok/messages/777",
        "{\"content\":\"follow-edit\"}",
        dcc_rest_interaction_followup_edit(client, 123, "tok", 777, &follow_edit_payload, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "interaction_followup_edit_builder",
        "PATCH",
        "/webhooks/123/tok/messages/777",
        "{\"content\":\"builder\"}",
        dcc_rest_interaction_followup_edit(client, 123, "tok", 777, &builder_payload, &call_options, NULL)
    );
    EXPECT_REST_GAP(
        "interaction_followup_delete",
        "DELETE",
        "/webhooks/123/tok/messages/777",
        NULL,
        dcc_rest_interaction_followup_delete(client, 123, "tok", 777, &call_options, NULL)
    );

#undef EXPECT_REST_GAP

    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);
    return 0;
}
#endif
