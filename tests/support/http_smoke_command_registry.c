#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int build_command(
    dcc_application_command_builder_t *command,
    const char *name,
    const char *description,
    dcc_application_command_type_t type
) {
    dcc_application_command_builder_init(command);
    if (dcc_application_command_builder_set_name(command, name) != DCC_OK ||
        dcc_application_command_builder_set_type(command, type) != DCC_OK) {
        return 0;
    }
    if (type == DCC_APPLICATION_COMMAND_CHAT_INPUT &&
        dcc_application_command_builder_set_description(command, description) != DCC_OK) {
        return 0;
    }
    return 1;
}

static int expect_request(
    http_server_t *server,
    pthread_t thread,
    dcc_status_t status,
    const rest_seen_t *seen,
    const char *method,
    const char *path,
    const char *body
) {
    (void)pthread_join(thread, NULL);
    close(server->fd);
    if (status != DCC_OK ||
        !seen->called ||
        strcmp(server->method, method) != 0 ||
        strcmp(server->path, path) != 0 ||
        (body != NULL && strcmp(server->body, body) != 0)) {
        fprintf(
            stderr,
            "unexpected command registry request: st=%s method=%s path=%s body=%s\n",
            dcc_status_string(status),
            server->method,
            server->path,
            server->body
        );
        return 0;
    }
    return 1;
}

static int command_registry_plan_smoke(
    dcc_client_t *client,
    const dcc_application_command_builder_t *ping,
    const dcc_application_command_builder_t *echo,
    const dcc_application_command_builder_t *inspect,
    const char *ping_json
) {
    dcc_command_registry_t registry;
    dcc_command_registry_init(&registry);
    dcc_command_registry_options_t opts;
    dcc_command_registry_options_init(&opts);
    if (opts.guild_id != 0 ||
        dcc_command_registry_options_set_guild(&opts, 0) != DCC_ERR_INVALID_ARG ||
        dcc_command_registry_options_set_delete_stale(&opts, 1) != DCC_OK ||
        dcc_command_registry_options_set_dry_run(&opts, 1) != DCC_OK ||
        dcc_command_registry_add_builder(&registry, ping) != DCC_OK ||
        dcc_command_registry_add_builder(&registry, echo) != DCC_OK ||
        dcc_command_registry_add_builder(&registry, inspect) != DCC_OK ||
        dcc_command_registry_add_builder(&registry, ping) != DCC_ERR_STATE) {
        fprintf(stderr, "failed to initialize command registry plan smoke\n");
        dcc_command_registry_deinit(&registry);
        return 1;
    }

    const dcc_command_registry_remote_entry_t remote[] = {
        {
            .size = sizeof(remote[0]),
            .id = 111,
            .name = "ping",
            .type = DCC_APPLICATION_COMMAND_CHAT_INPUT,
            .canonical_json = ping_json,
        },
        {
            .size = sizeof(remote[0]),
            .id = 222,
            .name = "echo",
            .type = DCC_APPLICATION_COMMAND_CHAT_INPUT,
            .canonical_json = "{\"name\":\"echo\",\"description\":\"old\",\"type\":1}",
        },
        {
            .size = sizeof(remote[0]),
            .id = 333,
            .name = "old",
            .type = DCC_APPLICATION_COMMAND_CHAT_INPUT,
            .canonical_json = "{\"name\":\"old\",\"description\":\"old\",\"type\":1}",
        },
    };
    dcc_command_registry_plan_t plan;
    dcc_status_t status = dcc_command_registry_build_plan(
        &registry,
        &opts,
        remote,
        sizeof(remote) / sizeof(remote[0]),
        &plan
    );
    if (status != DCC_OK ||
        plan.create_count != 1 ||
        plan.update_count != 1 ||
        plan.delete_stale_count != 1 ||
        plan.noop_count != 1) {
        fprintf(stderr, "unexpected command registry plan counts: %s\n", dcc_status_string(status));
        dcc_command_registry_deinit(&registry);
        return 1;
    }

    char text[1024];
    size_t text_len = 0;
    status = dcc_command_registry_plan_format(&plan, text, sizeof(text), &text_len);
    if (status != DCC_OK ||
        text_len == 0 ||
        strstr(text, "warning=global command changes may take time to propagate") == NULL ||
        strstr(text, "create name=inspect") == NULL ||
        strstr(text, "update name=echo") == NULL ||
        strstr(text, "delete_stale name=old") == NULL) {
        fprintf(stderr, "unexpected command registry plan text: %s\n", text);
        dcc_command_registry_plan_deinit(&plan);
        dcc_command_registry_deinit(&registry);
        return 1;
    }

    rest_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    status = dcc_command_registry_apply(client, 123, &opts, &plan, rest_cb, &seen);
    if (status != DCC_OK || seen.called) {
        fprintf(stderr, "dry-run command registry apply should not issue REST calls\n");
        dcc_command_registry_plan_deinit(&plan);
        dcc_command_registry_deinit(&registry);
        return 1;
    }

    dcc_command_registry_plan_deinit(&plan);
    dcc_command_registry_deinit(&registry);
    return 0;
}

static int command_registry_snapshot_smoke(
    const dcc_application_command_builder_t *ping,
    const dcc_application_command_builder_t *echo,
    const dcc_application_command_builder_t *inspect
) {
    static const char remote_json[] =
        "["
        "{\"id\":\"111\",\"name\":\"ping\",\"description\":\"pong\",\"type\":1},"
        "{\"id\":\"222\",\"name\":\"echo\",\"description\":\"old\",\"type\":1},"
        "{\"id\":\"333\",\"name\":\"old\",\"description\":\"old\",\"type\":1}"
        "]";

    dcc_command_registry_remote_snapshot_t snapshot;
    dcc_status_t status = dcc_command_registry_remote_snapshot_parse_json(
        remote_json,
        sizeof(remote_json) - 1U,
        &snapshot
    );
    if (status != DCC_OK ||
        snapshot.entry_count != 3 ||
        snapshot.entries[0].id != 111 ||
        strcmp(snapshot.entries[0].name, "ping") != 0) {
        fprintf(stderr, "failed to parse command registry remote snapshot: %s\n", dcc_status_string(status));
        return 1;
    }

    dcc_command_registry_t registry;
    dcc_command_registry_init(&registry);
    dcc_command_registry_options_t opts;
    dcc_command_registry_options_init(&opts);
    (void)dcc_command_registry_options_set_delete_stale(&opts, 1);
    if (dcc_command_registry_add_builder(&registry, ping) != DCC_OK ||
        dcc_command_registry_add_builder(&registry, echo) != DCC_OK ||
        dcc_command_registry_add_builder(&registry, inspect) != DCC_OK) {
        fprintf(stderr, "failed to initialize snapshot registry\n");
        dcc_command_registry_remote_snapshot_deinit(&snapshot);
        dcc_command_registry_deinit(&registry);
        return 1;
    }

    dcc_command_registry_plan_t plan;
    status = dcc_command_registry_build_plan(
        &registry,
        &opts,
        snapshot.entries,
        snapshot.entry_count,
        &plan
    );
    int failed = status != DCC_OK ||
        plan.create_count != 1 ||
        plan.update_count != 1 ||
        plan.delete_stale_count != 1 ||
        plan.noop_count != 1;
    if (failed) {
        fprintf(stderr, "unexpected snapshot plan counts: %s\n", dcc_status_string(status));
    }
    dcc_command_registry_plan_deinit(&plan);
    dcc_command_registry_remote_snapshot_deinit(&snapshot);
    dcc_command_registry_deinit(&registry);
    return failed;
}

static int command_registry_apply_smoke(
    dcc_client_t *client,
    const dcc_application_command_builder_t *command,
    const dcc_command_registry_options_t *opts,
    const dcc_command_registry_remote_entry_t *remote,
    size_t remote_count,
    const char *method,
    const char *path,
    const char *body
) {
    dcc_command_registry_t registry;
    dcc_command_registry_init(&registry);
    if (command != NULL &&
        dcc_command_registry_add_builder(&registry, command) != DCC_OK) {
        fprintf(stderr, "failed to add command for apply smoke\n");
        dcc_command_registry_deinit(&registry);
        return 1;
    }

    dcc_command_registry_plan_t plan;
    dcc_status_t status = dcc_command_registry_build_plan(&registry, opts, remote, remote_count, &plan);
    if (status != DCC_OK) {
        fprintf(stderr, "failed to build apply smoke plan: %s\n", dcc_status_string(status));
        dcc_command_registry_deinit(&registry);
        return 1;
    }

    http_server_t server;
    pthread_t thread;
    rest_seen_t seen;
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start command registry server: %s\n", strerror(errno));
        dcc_command_registry_plan_deinit(&plan);
        dcc_command_registry_deinit(&registry);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    status = dcc_command_registry_apply(client, 123, opts, &plan, rest_cb, &seen);
    int ok = expect_request(&server, thread, status, &seen, method, path, body);
    dcc_command_registry_plan_deinit(&plan);
    dcc_command_registry_deinit(&registry);
    return ok ? 0 : 1;
}

int run_public_command_registry_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t client_opts = {
        .size = sizeof(client_opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t status = dcc_client_create(&client_opts, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(status));
        return 1;
    }

    dcc_application_command_builder_t ping;
    dcc_application_command_builder_t echo;
    dcc_application_command_builder_t inspect;
    if (!build_command(&ping, "ping", "pong", DCC_APPLICATION_COMMAND_CHAT_INPUT) ||
        !build_command(&echo, "echo", "repeat", DCC_APPLICATION_COMMAND_CHAT_INPUT) ||
        !build_command(&inspect, "inspect", NULL, DCC_APPLICATION_COMMAND_USER)) {
        fprintf(stderr, "failed to build command registry smoke commands\n");
        dcc_client_destroy(client);
        return 1;
    }

    char *ping_json = NULL;
    char *echo_json = NULL;
    if (dcc_application_command_builder_build_json(&ping, &ping_json) != DCC_OK ||
        dcc_application_command_builder_build_json(&echo, &echo_json) != DCC_OK) {
        fprintf(stderr, "failed to build command JSON for registry smoke\n");
        dcc_application_command_builder_json_free(ping_json);
        dcc_application_command_builder_json_free(echo_json);
        dcc_client_destroy(client);
        return 1;
    }

    int failed = command_registry_plan_smoke(client, &ping, &echo, &inspect, ping_json);
    if (!failed) {
        failed = command_registry_snapshot_smoke(&ping, &echo, &inspect);
    }

    dcc_command_registry_options_t global_opts;
    dcc_command_registry_options_init(&global_opts);
    if (!failed) {
        failed = command_registry_apply_smoke(
            client,
            &ping,
            &global_opts,
            NULL,
            0,
            "POST",
            "/applications/123/commands",
            ping_json
        );
    }

    dcc_command_registry_remote_entry_t update_remote = {
        .size = sizeof(update_remote),
        .id = 222,
        .name = "echo",
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT,
        .canonical_json = "{\"name\":\"echo\",\"description\":\"old\",\"type\":1}",
    };
    if (!failed) {
        failed = command_registry_apply_smoke(
            client,
            &echo,
            &global_opts,
            &update_remote,
            1,
            "PATCH",
            "/applications/123/commands/222",
            echo_json
        );
    }

    dcc_command_registry_options_t delete_opts;
    dcc_command_registry_options_init(&delete_opts);
    (void)dcc_command_registry_options_set_delete_stale(&delete_opts, 1);
    dcc_command_registry_remote_entry_t stale_remote = {
        .size = sizeof(stale_remote),
        .id = 333,
        .name = "old",
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT,
        .canonical_json = "{\"name\":\"old\",\"description\":\"old\",\"type\":1}",
    };
    if (!failed) {
        failed = command_registry_apply_smoke(
            client,
            NULL,
            &delete_opts,
            &stale_remote,
            1,
            "DELETE",
            "/applications/123/commands/333",
            NULL
        );
    }

    dcc_command_registry_options_t guild_opts;
    dcc_command_registry_options_init(&guild_opts);
    (void)dcc_command_registry_options_set_guild(&guild_opts, 333);
    if (!failed) {
        failed = command_registry_apply_smoke(
            client,
            &ping,
            &guild_opts,
            NULL,
            0,
            "POST",
            "/applications/123/guilds/333/commands",
            ping_json
        );
    }

    dcc_application_command_builder_json_free(ping_json);
    dcc_application_command_builder_json_free(echo_json);
    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);
    return failed;
}

#endif
