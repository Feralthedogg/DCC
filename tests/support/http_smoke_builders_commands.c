#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_public_rest_application_command_builder_smoke(void) {
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

    dcc_application_command_builder_t command;
    dcc_application_command_builder_init(&command);
    if (dcc_application_command_builder_set_name(&command, "ping") != DCC_OK ||
        dcc_application_command_builder_set_name_localizations_json(&command, "{\"ko\":\"ping-ko\"}") != DCC_OK ||
        dcc_application_command_builder_set_description(&command, "say \"pong\"\nnow") != DCC_OK ||
        dcc_application_command_builder_set_description_localizations_json(&command, "{\"ko\":\"pong-ko\"}") != DCC_OK ||
        dcc_application_command_builder_set_type(&command, DCC_APPLICATION_COMMAND_CHAT_INPUT) != DCC_OK ||
        dcc_application_command_builder_set_options_json(
            &command,
            "[{\"type\":3,\"name\":\"text\",\"description\":\"input\",\"required\":true}]"
        ) != DCC_OK ||
        dcc_application_command_builder_set_default_member_permissions(&command, 2048) != DCC_OK ||
        dcc_application_command_builder_set_dm_permission(&command, 0) != DCC_OK ||
        dcc_application_command_builder_set_nsfw(&command, 1) != DCC_OK ||
        dcc_application_command_builder_set_integration_types_json(&command, "[0]") != DCC_OK ||
        dcc_application_command_builder_set_contexts_json(&command, "[0,1]") != DCC_OK) {
        fprintf(stderr, "failed to build application command builder state\n");
        dcc_client_destroy(client);
        return 1;
    }

    const char expected_body[] =
        "{\"name\":\"ping\","
        "\"name_localizations\":{\"ko\":\"ping-ko\"},"
        "\"description\":\"say \\\"pong\\\"\\nnow\","
        "\"description_localizations\":{\"ko\":\"pong-ko\"},"
        "\"type\":1,"
        "\"options\":[{\"type\":3,\"name\":\"text\",\"description\":\"input\",\"required\":true}],"
        "\"default_member_permissions\":\"2048\","
        "\"dm_permission\":false,"
        "\"nsfw\":true,"
        "\"integration_types\":[0],"
        "\"contexts\":[0,1]}";

    http_server_t server;
    pthread_t thread;
    rest_seen_t seen;

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start application command builder server: %s\n", strerror(errno));
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_global_command_builder(client, 123, &command, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/applications/123/commands") != 0 ||
        strcmp(server.content_type, "application/json") != 0 ||
        strcmp(server.body, expected_body) != 0) {
        fprintf(stderr,
                "unexpected global command builder request: st=%s method=%s path=%s content_type=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.content_type,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start default-global command builder server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_application_command_builder(client, 123, NULL, &command, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/applications/123/commands") != 0 ||
        strcmp(server.body, expected_body) != 0) {
        fprintf(stderr,
                "unexpected default application command builder request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start explicit-global command builder server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    dcc_application_command_registration_options_t global_registration;
    dcc_application_command_registration_options_init(&global_registration);
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_application_command_builder(
        client,
        123,
        &global_registration,
        &command,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/applications/123/commands") != 0 ||
        strcmp(server.body, expected_body) != 0) {
        fprintf(stderr,
                "unexpected explicit-global application command builder request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start global command edit builder server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_edit_global_command_builder(client, 123, 444, &command, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "PATCH") != 0 ||
        strcmp(server.path, "/applications/123/commands/444") != 0 ||
        strcmp(server.body, expected_body) != 0) {
        fprintf(stderr,
                "unexpected global command edit builder request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start guild command create builder server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_guild_command_builder(client, 123, 333, &command, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/applications/123/guilds/333/commands") != 0 ||
        strcmp(server.body, expected_body) != 0) {
        fprintf(stderr,
                "unexpected guild command create builder request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start scoped guild command builder server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    dcc_application_command_registration_options_t guild_registration;
    if (dcc_application_command_registration_options_set_guild(&guild_registration, 333) != DCC_OK) {
        fprintf(stderr, "failed to set guild command registration options\n");
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_create_application_command_builder(
        client,
        123,
        &guild_registration,
        &command,
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "POST") != 0 ||
        strcmp(server.path, "/applications/123/guilds/333/commands") != 0 ||
        strcmp(server.body, expected_body) != 0) {
        fprintf(stderr,
                "unexpected scoped guild command builder request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start guild command edit builder server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_edit_guild_command_builder(client, 123, 333, 444, &command, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "PATCH") != 0 ||
        strcmp(server.path, "/applications/123/guilds/333/commands/444") != 0 ||
        strcmp(server.body, expected_body) != 0) {
        fprintf(stderr,
                "unexpected guild command edit builder request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    dcc_application_command_builder_t user_command;
    dcc_application_command_builder_init(&user_command);
    if (dcc_application_command_builder_set_name(&user_command, "inspect") != DCC_OK ||
        dcc_application_command_builder_set_type(&user_command, DCC_APPLICATION_COMMAND_USER) != DCC_OK ||
        dcc_application_command_builder_set_default_member_permissions_null(&user_command) != DCC_OK) {
        fprintf(stderr, "failed to build user command state\n");
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    dcc_application_command_builder_t commands[] = { command, user_command };
    char expected_array[2048];
    snprintf(expected_array, sizeof(expected_array), "[%s,{\"name\":\"inspect\",\"type\":2,\"default_member_permissions\":null}]", expected_body);

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start global command bulk builder server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_bulk_overwrite_global_commands_builder(
        client,
        123,
        commands,
        sizeof(commands) / sizeof(commands[0]),
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "PUT") != 0 ||
        strcmp(server.path, "/applications/123/commands") != 0 ||
        strcmp(server.body, expected_array) != 0) {
        fprintf(stderr,
                "unexpected global command bulk builder request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start default-global command bulk builder server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_bulk_overwrite_application_commands_builder(
        client,
        123,
        NULL,
        commands,
        sizeof(commands) / sizeof(commands[0]),
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "PUT") != 0 ||
        strcmp(server.path, "/applications/123/commands") != 0 ||
        strcmp(server.body, expected_array) != 0) {
        fprintf(stderr,
                "unexpected default-global command bulk builder request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start guild command bulk builder server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_bulk_overwrite_guild_commands_builder(
        client,
        123,
        333,
        commands,
        sizeof(commands) / sizeof(commands[0]),
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "PUT") != 0 ||
        strcmp(server.path, "/applications/123/guilds/333/commands") != 0 ||
        strcmp(server.body, expected_array) != 0) {
        fprintf(stderr,
                "unexpected guild command bulk builder request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start scoped command bulk builder server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_bulk_overwrite_application_commands_builder(
        client,
        123,
        &guild_registration,
        commands,
        sizeof(commands) / sizeof(commands[0]),
        rest_cb,
        &seen
    );
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        strcmp(server.method, "PUT") != 0 ||
        strcmp(server.path, "/applications/123/guilds/333/commands") != 0 ||
        strcmp(server.body, expected_array) != 0) {
        fprintf(stderr,
                "unexpected scoped command bulk builder request: st=%s method=%s path=%s body=%s\n",
                dcc_status_string(st),
                server.method,
                server.path,
                server.body);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);
    return 0;
}

#endif
