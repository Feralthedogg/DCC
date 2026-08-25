#include <dcc/app.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/command_registry/dcc_command_registry_internal.h"
#include "internal/events/dcc_event_state_internal.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct owned_args {
    const char *value;
} owned_args_t;

typedef struct owned_values_args {
    const char **values;
    size_t value_count;
} owned_values_args_t;

typedef struct contract_state {
    unsigned owned_count;
    unsigned route_count;
    unsigned ready_count;
    unsigned message_create_count;
    unsigned message_update_count;
    unsigned message_delete_count;
    unsigned message_command_count;
    unsigned failed_cleanup_count;
} contract_state_t;

typedef struct exact_app_snapshot {
    dcc_app_listener_entry_t **listeners;
    size_t listener_count;
    size_t listener_cap;
    dcc_app_route_t *routes;
    size_t route_count;
    size_t route_cap;
    dcc_app_schedule_t **schedules;
    size_t schedule_count;
    size_t schedule_cap;
    dcc_listener_id_t next_listener_id;
    dcc_app_route_id_t next_route_id;
    size_t registry_size;
    void *registry_state;
    dcc_command_registry_entry_t *registry_entries;
    size_t registry_entry_count;
    size_t registry_entry_cap;
} exact_app_snapshot_t;

static exact_app_snapshot_t exact_app_snapshot(const dcc_app_t *app) {
    const dcc_command_registry_state_t *registry =
        dcc_command_registry_state_const(&app->registry);
    return (exact_app_snapshot_t){
        .listeners = app->listeners,
        .listener_count = app->listener_count,
        .listener_cap = app->listener_cap,
        .routes = app->routes,
        .route_count = app->route_count,
        .route_cap = app->route_cap,
        .schedules = app->schedules,
        .schedule_count = app->schedule_count,
        .schedule_cap = app->schedule_cap,
        .next_listener_id = app->next_listener_id,
        .next_route_id = app->next_route_id,
        .registry_size = app->registry.size,
        .registry_state = app->registry.state,
        .registry_entries = registry != NULL ? registry->entries : NULL,
        .registry_entry_count = registry != NULL ? registry->entry_count : 0U,
        .registry_entry_cap = registry != NULL ? registry->entry_cap : 0U,
    };
}

static int exact_app_snapshot_equal(
    const exact_app_snapshot_t *left,
    const exact_app_snapshot_t *right
) {
    return memcmp(left, right, sizeof(*left)) == 0;
}

static void failed_listener_cleanup(void *user_data) {
    contract_state_t *state = (contract_state_t *)user_data;
    state->failed_cleanup_count++;
}

static dcc_status_t plain_handler(dcc_ctx_t *ctx, void *user_data) {
    contract_state_t *state = (contract_state_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->route_count++;
    return DCC_OK;
}

static dcc_status_t typed_owned_handler(dcc_ctx_t *ctx, void *args, void *user_data) {
    owned_args_t *owned = (owned_args_t *)args;
    contract_state_t *state = (contract_state_t *)user_data;
    if (ctx == NULL || owned == NULL || state == NULL || owned->value == NULL ||
        strcmp(owned->value, "owned-fallback") != 0) {
        return DCC_ERR_INVALID_ARG;
    }
    state->owned_count++;
    return DCC_OK;
}

static dcc_status_t typed_owned_values_handler(
    dcc_ctx_t *ctx,
    void *args,
    void *user_data
) {
    owned_values_args_t *owned = (owned_values_args_t *)args;
    contract_state_t *state = (contract_state_t *)user_data;
    if (ctx == NULL || owned == NULL || state == NULL || owned->value_count != 2U ||
        owned->values == NULL || strcmp(owned->values[0], "owned-a") != 0 ||
        strcmp(owned->values[1], "owned-b") != 0) {
        return DCC_ERR_INVALID_ARG;
    }
    state->owned_count++;
    return DCC_OK;
}

static dcc_status_t ready_handler(
    dcc_app_t *app,
    const dcc_ready_event_t *ready,
    void *user_data
) {
    contract_state_t *state = (contract_state_t *)user_data;
    if (app == NULL || ready == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->ready_count++;
    return DCC_OK;
}

static dcc_status_t message_handler(
    dcc_app_t *app,
    const dcc_message_t *message,
    const dcc_event_t *event,
    void *user_data
) {
    contract_state_t *state = (contract_state_t *)user_data;
    if (app == NULL || message == NULL || event == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_event_type(event) == DCC_EVENT_MESSAGE_CREATE) {
        state->message_create_count++;
    } else if (dcc_event_type(event) == DCC_EVENT_MESSAGE_UPDATE) {
        state->message_update_count++;
    } else if (dcc_event_type(event) == DCC_EVENT_MESSAGE_DELETE) {
        state->message_delete_count++;
    }
    return DCC_OK;
}

static dcc_status_t message_command_handler(
    dcc_app_t *app,
    const dcc_message_t *message,
    const char *args,
    const dcc_event_t *event,
    void *user_data
) {
    contract_state_t *state = (contract_state_t *)user_data;
    if (app == NULL || message == NULL || args == NULL || event == NULL || state == NULL ||
        strcmp(args, "alpha beta") != 0) {
        return DCC_ERR_INVALID_ARG;
    }
    state->message_command_count++;
    return DCC_OK;
}

static dcc_status_t task_handler(dcc_app_t *app, void *user_data) {
    return app != NULL && user_data != NULL ? DCC_OK : DCC_ERR_INVALID_ARG;
}

static dcc_status_t policy_middleware(dcc_ctx_t *ctx, void *user_data) {
    return ctx != NULL && user_data != NULL ? DCC_OK : DCC_ERR_INVALID_ARG;
}

static dcc_status_t create_app(dcc_app_t **out) {
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;
    return dcc_app_create(&options, out);
}

static dcc_listener_t slash_listener(const char *name, void *user_data) {
    dcc_listener_t listener;
    dcc_listener_init(&listener, DCC_LISTENER_SLASH);
    listener.handler.plain = plain_handler;
    listener.user_data = user_data;
    listener.target.route.name = name;
    listener.target.route.description = "contract";
    return listener;
}

static int expect_invalid_without_mutation(
    dcc_app_t *app,
    const dcc_listener_t *listener,
    const char *label
) {
    size_t command_count = dcc_command_registry_count(dcc_app_command_registry(app));
    size_t route_count = app->route_count;
    size_t listener_count = app->listener_count;
    size_t schedule_count = app->schedule_count;
    dcc_listener_id_t next_listener_id = app->next_listener_id;
    dcc_app_route_id_t next_route_id = app->next_route_id;
    dcc_listener_id_t out_id = UINT64_C(99);
    dcc_status_t status = dcc_app_listen(app, listener, &out_id);
    if (status != DCC_ERR_INVALID_ARG || out_id != 0U ||
        dcc_command_registry_count(dcc_app_command_registry(app)) != command_count ||
        app->route_count != route_count || app->listener_count != listener_count ||
        app->schedule_count != schedule_count || app->next_listener_id != next_listener_id ||
        app->next_route_id != next_route_id) {
        fprintf(stderr, "%s was not rejected without mutation (status=%d)\n", label, status);
        return 1;
    }
    return 0;
}

static int dispatch_slash(dcc_app_t *app, const char *name) {
    static dcc_snowflake_t next_id = 1U;
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_SLASH_COMMAND;
    event.data.interaction.name = name;
    event.data.interaction.token = "contract-token";
    event.data.interaction.id = next_id++;
    event.data.interaction.application_id = 2U;
    return dcc_event_bus_dispatch(&app->client->events, app->client, &event) == DCC_OK ? 0 : 1;
}

static int test_validation_matrix(dcc_app_t *app, contract_state_t *state) {
    dcc_listener_t listener = slash_listener("invalid", state);
    dcc_listener_t invalid = listener;
    dcc_listener_route_policy_t invalid_policy = {
        .size = sizeof(invalid_policy),
        .version = DCC_LISTENER_ROUTE_POLICY_VERSION,
        .guild_only = 2U,
    };
    invalid.policy = &invalid_policy;
    if (expect_invalid_without_mutation(app, &invalid, "non-boolean guild_only") != 0) {
        return 1;
    }
    invalid = listener;
    invalid_policy.guild_only = 0U;
    invalid_policy.dm_only = 1U;
    invalid_policy.nsfw_only = 1U;
    invalid.policy = &invalid_policy;
    if (expect_invalid_without_mutation(app, &invalid, "dm_only + nsfw_only") != 0) {
        return 1;
    }
    invalid = listener;
    invalid.target.route.command_name = "inactive";
    if (expect_invalid_without_mutation(app, &invalid, "inactive slash command_name") != 0) {
        return 1;
    }

    dcc_application_command_builder_t component_command;
    dcc_application_command_builder_init(&component_command);
    (void)dcc_application_command_builder_set_name(&component_command, "component-command");
    invalid = slash_listener("button-id", state);
    invalid.kind = DCC_LISTENER_BUTTON;
    invalid.target.route.command = &component_command;
    if (expect_invalid_without_mutation(app, &invalid, "component command builder") != 0) {
        return 1;
    }

    dcc_listener_t ready;
    dcc_listener_init(&ready, DCC_LISTENER_READY);
    ready.handler.ready = ready_handler;
    ready.user_data = state;
    ready.target.event.type = DCC_EVENT_LOG;
    if (expect_invalid_without_mutation(app, &ready, "inactive ready event type") != 0) {
        return 1;
    }

    dcc_listener_t task;
    dcc_listener_init(&task, DCC_LISTENER_TASK);
    task.handler.task = task_handler;
    task.user_data = state;
    task.target.schedule.kind = DCC_LISTENER_SCHEDULE_INTERVAL;
    task.target.schedule.interval_ms = 1000U;
    task.target.schedule.hour = 1U;
    if (expect_invalid_without_mutation(app, &task, "inactive interval hour") != 0) {
        return 1;
    }

    dcc_listener_binding_t binding = {
        .size = sizeof(binding),
        .version = DCC_LISTENER_BINDING_VERSION,
        .name = "value",
        .type = DCC_CTX_OPTION_BIND_STRING,
        .field_offset = offsetof(owned_args_t, value),
        .fallback_string = "fallback",
        .fallback_values = (const char **)&binding.name,
        .fallback_values_count = 1U,
    };
    invalid = listener;
    invalid.handler.typed = typed_owned_handler;
    invalid.args_size = sizeof(owned_args_t);
    dcc_listener_bindings_t bindings = {
        .size = sizeof(bindings),
        .version = DCC_LISTENER_BINDINGS_VERSION,
        .kind = DCC_LISTENER_BIND_OPTIONS,
        .items.options = &binding,
        .count = 1U,
    };
    invalid.bindings = &bindings;
    if (expect_invalid_without_mutation(app, &invalid, "inactive option fallback_values") != 0) {
        return 1;
    }
    return 0;
}

static int dispatch_interaction(
    dcc_app_t *app,
    dcc_event_type_t type,
    const char *name,
    const char *custom_id,
    const dcc_interaction_option_t *options,
    size_t option_count
) {
    static dcc_snowflake_t next_id = 100U;
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = type;
    event.data.interaction.name = name;
    event.data.interaction.custom_id = custom_id;
    event.data.interaction.options = options;
    event.data.interaction.options_count = option_count;
    event.data.interaction.token = "contract-token";
    event.data.interaction.id = next_id++;
    event.data.interaction.application_id = 2U;
    return dcc_event_bus_dispatch(&app->client->events, app->client, &event) == DCC_OK ? 0 : 1;
}

static int dispatch_object_event(dcc_app_t *app, dcc_event_type_t type, const char *content) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = type;
    event.data.message.content = content;
    return dcc_event_bus_dispatch(&app->client->events, app->client, &event) == DCC_OK ? 0 : 1;
}

static int test_listener_kind_coverage(dcc_app_t *app, contract_state_t *state) {
    dcc_listener_id_t ids[9] = {0};
    size_t id_count = 0U;
    dcc_listener_t listener;

    dcc_listener_init(&listener, DCC_LISTENER_READY);
    listener.handler.ready = ready_handler;
    listener.user_data = state;
    if (dcc_app_listen(app, &listener, &ids[id_count++]) != DCC_OK ||
        dispatch_object_event(app, DCC_EVENT_READY, NULL) != 0) {
        return 1;
    }

    const dcc_listener_kind_t message_kinds[3] = {
        DCC_LISTENER_MESSAGE_CREATE,
        DCC_LISTENER_MESSAGE_UPDATE,
        DCC_LISTENER_MESSAGE_DELETE,
    };
    const dcc_event_type_t message_events[3] = {
        DCC_EVENT_MESSAGE_CREATE,
        DCC_EVENT_MESSAGE_UPDATE,
        DCC_EVENT_MESSAGE_DELETE,
    };
    for (size_t i = 0U; i < 3U; ++i) {
        dcc_listener_init(&listener, message_kinds[i]);
        listener.handler.message = message_handler;
        listener.user_data = state;
        if (dcc_app_listen(app, &listener, &ids[id_count++]) != DCC_OK ||
            dispatch_object_event(app, message_events[i], "message") != 0) {
            return 1;
        }
    }

    dcc_listener_init(&listener, DCC_LISTENER_MESSAGE_COMMAND);
    listener.handler.message_command = message_command_handler;
    listener.user_data = state;
    listener.target.message_command.prefix = "!";
    listener.target.message_command.name = "ping";
    if (dcc_app_listen(app, &listener, &ids[id_count++]) != DCC_OK ||
        dispatch_object_event(app, DCC_EVENT_MESSAGE_CREATE, "!ping alpha beta") != 0) {
        return 1;
    }

    dcc_listener_init(&listener, DCC_LISTENER_BUTTON_PREFIX);
    listener.handler.plain = plain_handler;
    listener.user_data = state;
    listener.target.route.name = "prefix:";
    if (dcc_app_listen(app, &listener, &ids[id_count++]) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, NULL, "prefix:value", NULL, 0U) != 0) {
        return 1;
    }

    dcc_interaction_option_t subcommand = {
        .name = "leaf",
        .type = DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND,
    };
    dcc_listener_init(&listener, DCC_LISTENER_SUBCOMMAND);
    listener.handler.plain = plain_handler;
    listener.user_data = state;
    listener.target.route.command_name = "root";
    listener.target.route.subcommand_path = "leaf";
    listener.target.route.description = "root command";
    if (dcc_app_listen(app, &listener, &ids[id_count++]) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_SLASH_COMMAND, "root", NULL, &subcommand, 1U) != 0) {
        return 1;
    }

    dcc_listener_init(&listener, DCC_LISTENER_AUTOCOMPLETE);
    listener.handler.plain = plain_handler;
    listener.user_data = state;
    listener.target.route.command_name = "root-auto";
    listener.target.route.subcommand_path = "leaf";
    listener.target.route.description = "root autocomplete command";
    if (dcc_app_listen(app, &listener, &ids[id_count++]) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_AUTOCOMPLETE, "root-auto", NULL, &subcommand, 1U) != 0) {
        return 1;
    }

    if (state->ready_count != 1U || state->message_create_count != 2U ||
        state->message_update_count != 1U || state->message_delete_count != 1U ||
        state->message_command_count != 1U || state->route_count != 3U) {
        fprintf(stderr, "listener kind coverage did not dispatch every canonical adapter\n");
        return 1;
    }
    for (size_t i = 0U; i < id_count; ++i) {
        if (dcc_app_unlisten(app, ids[i]) != DCC_OK) {
            return 1;
        }
    }
    return 0;
}

static int test_transactional_retry(dcc_app_t *app, contract_state_t *state) {
    dcc_application_command_builder_t invalid_command;
    dcc_application_command_builder_init(&invalid_command);
    invalid_command.name = "schema-rollback";
    invalid_command.type = DCC_APPLICATION_COMMAND_CHAT_INPUT;
    invalid_command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE;
    invalid_command.description = "schema rollback";
    invalid_command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION;
    dcc_listener_t invalid_listener = slash_listener("ignored", state);
    invalid_listener.target.route.command = &invalid_command;
    invalid_listener.target.route.name = NULL;
    invalid_listener.target.route.description = NULL;

    size_t command_count = dcc_command_registry_count(dcc_app_command_registry(app));
    size_t route_count = app->route_count;
    size_t listener_count = app->listener_count;
    size_t schedule_count = app->schedule_count;
    dcc_listener_id_t next_listener_id = app->next_listener_id;
    dcc_app_route_id_t next_route_id = app->next_route_id;
    dcc_listener_id_t invalid_id = UINT64_C(99);
    dcc_status_t status = dcc_app_listen(app, &invalid_listener, &invalid_id);
    if (status != DCC_ERR_INVALID_ARG || invalid_id != 0U ||
        dcc_command_registry_count(dcc_app_command_registry(app)) != command_count ||
        app->route_count != route_count || app->listener_count != listener_count ||
        app->schedule_count != schedule_count || app->next_listener_id != next_listener_id ||
        app->next_route_id != next_route_id) {
        fprintf(stderr, "failed command-schema registration leaked App state\n");
        return 1;
    }
    invalid_command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME;
    dcc_listener_id_t schema_retry_id = 0U;
    if (dcc_app_listen(app, &invalid_listener, &schema_retry_id) != DCC_OK ||
        schema_retry_id == 0U ||
        dcc_command_registry_count(dcc_app_command_registry(app)) != command_count + 1U ||
        app->route_count != route_count + 1U || app->listener_count != listener_count + 1U ||
        dcc_app_unlisten(app, schema_retry_id) != DCC_OK) {
        fprintf(stderr, "command-schema registration retry did not commit exactly once\n");
        return 1;
    }

    dcc_application_command_builder_t command;
    dcc_application_command_builder_init(&command);
    if (dcc_application_command_builder_set_name(&command, "rollback") != DCC_OK ||
        dcc_application_command_builder_set_type(&command, DCC_APPLICATION_COMMAND_CHAT_INPUT) != DCC_OK ||
        dcc_application_command_builder_set_description(&command, "rollback") != DCC_OK) {
        return 1;
    }
    dcc_listener_t listener = slash_listener("rollback", state);
    listener.target.route.command = &command;
    listener.target.route.name = NULL;
    listener.target.route.description = NULL;

    command_count = dcc_command_registry_count(dcc_app_command_registry(app));
    route_count = app->route_count;
    listener_count = app->listener_count;
    schedule_count = app->schedule_count;
    next_listener_id = app->next_listener_id;
    dcc_app_route_id_t saved_route_id = app->next_route_id;
    app->next_route_id = UINT64_MAX;
    dcc_listener_id_t failed_id = UINT64_C(99);
    status = dcc_app_listen(app, &listener, &failed_id);
    app->next_route_id = saved_route_id;
    if (status != DCC_ERR_NOMEM || failed_id != 0U ||
        dcc_command_registry_count(dcc_app_command_registry(app)) != command_count ||
        app->route_count != route_count || app->listener_count != listener_count ||
        app->schedule_count != schedule_count || app->next_listener_id != next_listener_id) {
        fprintf(
            stderr,
            "failed route registration leaked state: status=%d id=%llu commands=%zu/%zu routes=%zu/%zu listeners=%zu/%zu schedules=%zu/%zu next=%llu/%llu\n",
            status,
            (unsigned long long)failed_id,
            dcc_command_registry_count(dcc_app_command_registry(app)),
            command_count,
            app->route_count,
            route_count,
            app->listener_count,
            listener_count,
            app->schedule_count,
            schedule_count,
            (unsigned long long)app->next_listener_id,
            (unsigned long long)next_listener_id
        );
        return 1;
    }

    dcc_listener_id_t retry_id = 0U;
    if (dcc_app_listen(app, &listener, &retry_id) != DCC_OK || retry_id == 0U ||
        dcc_command_registry_count(dcc_app_command_registry(app)) != command_count + 1U ||
        app->route_count != route_count + 1U || app->listener_count != listener_count + 1U ||
        dcc_app_unlisten(app, retry_id) != DCC_OK) {
        fprintf(stderr, "registration retry did not commit exactly once\n");
        return 1;
    }
    return 0;
}

static int expect_failed_listen_exact(
    dcc_app_t *app,
    const dcc_listener_t *listener,
    dcc_status_t expected_status,
    const char *label
) {
    exact_app_snapshot_t before = exact_app_snapshot(app);
    dcc_listener_id_t id = UINT64_C(99);
    dcc_status_t status = dcc_app_listen(app, listener, &id);
    exact_app_snapshot_t after = exact_app_snapshot(app);
    if (status != expected_status || id != 0U ||
        !exact_app_snapshot_equal(&before, &after)) {
        fprintf(
            stderr,
            "%s changed transaction state: status=%d/%d id=%llu "
            "listeners=%p/%p %zu/%zu %zu/%zu routes=%p/%p %zu/%zu %zu/%zu "
            "registry=%p/%p entries=%p/%p %zu/%zu %zu/%zu\n",
            label,
            status,
            expected_status,
            (unsigned long long)id,
            (void *)before.listeners,
            (void *)after.listeners,
            before.listener_count,
            after.listener_count,
            before.listener_cap,
            after.listener_cap,
            (void *)before.routes,
            (void *)after.routes,
            before.route_count,
            after.route_count,
            before.route_cap,
            after.route_cap,
            before.registry_state,
            after.registry_state,
            (void *)before.registry_entries,
            (void *)after.registry_entries,
            before.registry_entry_count,
            after.registry_entry_count,
            before.registry_entry_cap,
            after.registry_entry_cap
        );
        return 1;
    }
    return 0;
}

static dcc_listener_t implicit_command_listener(
    dcc_listener_kind_t kind,
    uint8_t nested,
    const char *root_name,
    const char *description,
    contract_state_t *state
) {
    dcc_listener_t listener;
    dcc_listener_init(&listener, kind);
    listener.handler.plain = plain_handler;
    listener.user_data = state;
    listener.target.route.description = description;
    if (kind == DCC_LISTENER_SUBCOMMAND || nested) {
        listener.target.route.command_name = root_name;
        listener.target.route.subcommand_path = "leaf";
    } else {
        listener.target.route.name = root_name;
    }
    return listener;
}

static int registry_chat_descriptions_are_nonempty(const dcc_app_t *app) {
    const dcc_command_registry_state_t *registry =
        dcc_command_registry_state_const(&app->registry);
    if (registry == NULL) {
        return 1;
    }
    for (size_t i = 0U; i < registry->entry_count; ++i) {
        const dcc_application_command_builder_t *command = &registry->entries[i].command;
        if (dcc_command_registry_builder_type(command) == DCC_APPLICATION_COMMAND_CHAT_INPUT &&
            (command->description == NULL || command->description[0] == '\0')) {
            return 0;
        }
    }
    return 1;
}

static int test_implicit_command_descriptions(contract_state_t *state) {
    dcc_app_t *app = NULL;
    if (create_app(&app) != DCC_OK) {
        return 1;
    }

    dcc_listener_id_t seed_route_id = 0U;
    dcc_listener_t seed_route = slash_listener("implicit-seed", state);
    dcc_listener_id_t seed_schedule_id = 0U;
    dcc_listener_t seed_schedule;
    dcc_listener_init(&seed_schedule, DCC_LISTENER_TASK);
    seed_schedule.handler.task = task_handler;
    seed_schedule.user_data = state;
    seed_schedule.target.schedule.kind = DCC_LISTENER_SCHEDULE_INTERVAL;
    seed_schedule.target.schedule.interval_ms = UINT64_C(60000);
    if (dcc_app_listen(app, &seed_route, &seed_route_id) != DCC_OK ||
        dcc_app_listen(app, &seed_schedule, &seed_schedule_id) != DCC_OK ||
        app->listener_count != 2U || app->route_count != 1U || app->schedule_count != 1U ||
        dcc_command_registry_count(&app->registry) != 1U) {
        fprintf(stderr, "implicit-description non-empty seed state failed\n");
        return 1;
    }

    static const struct {
        dcc_listener_kind_t kind;
        uint8_t nested;
        const char *name;
        const char *label;
    } cases[] = {
        {DCC_LISTENER_SLASH, 0U, "implicit-slash", "implicit slash"},
        {DCC_LISTENER_SUBCOMMAND, 1U, "implicit-subcommand", "implicit subcommand"},
        {DCC_LISTENER_AUTOCOMPLETE, 0U, "implicit-autocomplete", "implicit autocomplete"},
        {DCC_LISTENER_AUTOCOMPLETE, 1U, "implicit-auto-subcommand", "implicit subcommand autocomplete"},
    };
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        dcc_listener_t listener = implicit_command_listener(
            cases[i].kind,
            cases[i].nested,
            cases[i].name,
            NULL,
            state
        );
        if (expect_failed_listen_exact(app, &listener, DCC_ERR_INVALID_ARG, cases[i].label) != 0) {
            return 1;
        }
        listener.target.route.description = "";
        if (expect_failed_listen_exact(app, &listener, DCC_ERR_INVALID_ARG, cases[i].label) != 0) {
            return 1;
        }

        size_t command_count = dcc_command_registry_count(&app->registry);
        listener.target.route.description = "non-empty implicit schema";
        dcc_listener_id_t id = 0U;
        dcc_status_t status = dcc_app_listen(app, &listener, &id);
        const dcc_command_registry_state_t *registry =
            dcc_command_registry_state_const(&app->registry);
        const dcc_application_command_builder_t *command =
            registry != NULL && registry->entry_count != 0U
            ? &registry->entries[registry->entry_count - 1U].command
            : NULL;
        if (status != DCC_OK || id == 0U ||
            dcc_command_registry_count(&app->registry) != command_count + 1U ||
            command == NULL || command->name == NULL ||
            strcmp(command->name, cases[i].name) != 0 ||
            dcc_command_registry_builder_type(command) != DCC_APPLICATION_COMMAND_CHAT_INPUT ||
            command->description == NULL ||
            strcmp(command->description, "non-empty implicit schema") != 0 ||
            !registry_chat_descriptions_are_nonempty(app) ||
            dcc_app_unlisten(app, id) != DCC_OK) {
            fprintf(stderr, "%s clean retry did not commit one valid CHAT_INPUT schema\n", cases[i].label);
            return 1;
        }
    }

    if (dcc_app_unlisten(app, seed_schedule_id) != DCC_OK ||
        dcc_app_unlisten(app, seed_route_id) != DCC_OK) {
        return 1;
    }
    return dcc_app_destroy(app) == DCC_OK ? 0 : 1;
}

static int test_exact_failure_transactions(contract_state_t *state) {
    dcc_app_t *app = NULL;
    if (create_app(&app) != DCC_OK) {
        return 1;
    }
    dcc_listener_id_t seed_route_id = 0U;
    dcc_listener_t seed_route = slash_listener("transaction-seed", state);
    dcc_listener_id_t seed_schedule_id = 0U;
    dcc_listener_t seed_schedule;
    dcc_listener_init(&seed_schedule, DCC_LISTENER_TASK);
    seed_schedule.handler.task = task_handler;
    seed_schedule.user_data = state;
    seed_schedule.target.schedule.kind = DCC_LISTENER_SCHEDULE_INTERVAL;
    seed_schedule.target.schedule.interval_ms = UINT64_C(60000);
    if (dcc_app_listen(app, &seed_route, &seed_route_id) != DCC_OK ||
        dcc_app_listen(app, &seed_schedule, &seed_schedule_id) != DCC_OK ||
        app->listener_count == 0U || app->route_count == 0U || app->schedule_count == 0U ||
        dcc_command_registry_count(&app->registry) == 0U) {
        fprintf(stderr, "transaction failure tests did not start from non-empty App state\n");
        return 1;
    }
    dcc_listener_middleware_t middleware = {
        .size = sizeof(middleware),
        .version = DCC_LISTENER_MIDDLEWARE_VERSION,
        .callback = policy_middleware,
        .user_data = state,
    };
    dcc_listener_t policy_listener = slash_listener("policy-oom", state);
    policy_listener.cleanup = failed_listener_cleanup;
    dcc_listener_route_policy_t policy = {
        .size = sizeof(policy),
        .version = DCC_LISTENER_ROUTE_POLICY_VERSION,
        .middlewares = &middleware,
        .middleware_count = 1U,
    };
    policy_listener.policy = &policy;
    unsigned cleanup_count = state->failed_cleanup_count;
    app->listener_test_fail_policy_allocation = 1U;
    if (expect_failed_listen_exact(
            app,
            &policy_listener,
            DCC_ERR_NOMEM,
            "route policy allocation failure"
        ) != 0 || state->failed_cleanup_count != cleanup_count) {
        return 1;
    }
    dcc_listener_id_t retry_id = 0U;
    if (dcc_app_listen(app, &policy_listener, &retry_id) != DCC_OK || retry_id == 0U ||
        dcc_app_unlisten(app, retry_id) != DCC_OK ||
        state->failed_cleanup_count != cleanup_count + 1U) {
        fprintf(stderr, "route policy failpoint did not reset for a clean retry\n");
        return 1;
    }

    dcc_listener_t schedule_listener;
    dcc_listener_init(&schedule_listener, DCC_LISTENER_TASK);
    schedule_listener.handler.task = task_handler;
    schedule_listener.user_data = state;
    schedule_listener.cleanup = failed_listener_cleanup;
    schedule_listener.target.schedule.kind = DCC_LISTENER_SCHEDULE_INTERVAL;
    schedule_listener.target.schedule.interval_ms = 1U;
    cleanup_count = state->failed_cleanup_count;
    app->listener_test_fail_schedule_allocation = 1U;
    if (expect_failed_listen_exact(
            app,
            &schedule_listener,
            DCC_ERR_NOMEM,
            "schedule allocation failure"
        ) != 0 || state->failed_cleanup_count != cleanup_count) {
        return 1;
    }
    retry_id = 0U;
    if (dcc_app_listen(app, &schedule_listener, &retry_id) != DCC_OK || retry_id == 0U ||
        dcc_app_unlisten(app, retry_id) != DCC_OK ||
        state->failed_cleanup_count != cleanup_count + 1U) {
        fprintf(stderr, "schedule allocation failpoint did not reset for a clean retry\n");
        return 1;
    }

    dcc_app_route_id_t saved_route_id = app->next_route_id;
    app->next_route_id = UINT64_MAX;
    dcc_listener_t route_id_listener = slash_listener("route-id-oom", state);
    route_id_listener.cleanup = failed_listener_cleanup;
    cleanup_count = state->failed_cleanup_count;
    if (expect_failed_listen_exact(
            app,
            &route_id_listener,
            DCC_ERR_NOMEM,
            "route ID exhaustion"
        ) != 0 || state->failed_cleanup_count != cleanup_count) {
        return 1;
    }
    app->next_route_id = saved_route_id;
    retry_id = 0U;
    if (dcc_app_listen(app, &route_id_listener, &retry_id) != DCC_OK || retry_id == 0U ||
        dcc_app_unlisten(app, retry_id) != DCC_OK ||
        state->failed_cleanup_count != cleanup_count + 1U) {
        fprintf(stderr, "route ID failure did not permit a clean retry\n");
        return 1;
    }

    size_t registry_seed_index = 0U;
    dcc_command_registry_state_t *registry_state =
        dcc_command_registry_state_get(&app->registry, 0);
    while (registry_state != NULL && registry_state->entry_count < registry_state->entry_cap) {
        char name[32];
        (void)snprintf(name, sizeof(name), "direct-seed-%zu", registry_seed_index++);
        dcc_application_command_builder_t seed;
        dcc_application_command_builder_init(&seed);
        if (dcc_application_command_builder_set_name(&seed, name) != DCC_OK ||
            dcc_application_command_builder_set_description(&seed, "seed") != DCC_OK ||
            dcc_application_command_builder_set_type(
                &seed,
                DCC_APPLICATION_COMMAND_CHAT_INPUT
            ) != DCC_OK ||
            dcc_command_registry_add_builder(&app->registry, &seed) != DCC_OK) {
            return 1;
        }
        registry_state = dcc_command_registry_state_get(&app->registry, 0);
    }

    dcc_application_command_builder_t direct_growth_command;
    dcc_application_command_builder_init(&direct_growth_command);
    direct_growth_command.name = "direct-registry-growth";
    direct_growth_command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME;
    direct_growth_command.description = "direct registry growth";
    direct_growth_command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION;
    direct_growth_command.type = DCC_APPLICATION_COMMAND_CHAT_INPUT;
    direct_growth_command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE;
    exact_app_snapshot_t direct_before = exact_app_snapshot(app);
    dcc_command_registry_test_fail_next_growth();
    dcc_status_t direct_status = dcc_command_registry_add_builder(
        &app->registry,
        &direct_growth_command
    );
    exact_app_snapshot_t direct_after = exact_app_snapshot(app);
    if (direct_status != DCC_ERR_NOMEM ||
        !exact_app_snapshot_equal(&direct_before, &direct_after)) {
        fprintf(stderr, "direct public registry growth failure changed state\n");
        return 1;
    }
    if (dcc_command_registry_add_builder(&app->registry, &direct_growth_command) != DCC_OK) {
        fprintf(stderr, "direct public registry failpoint did not reset for clean retry\n");
        return 1;
    }

    registry_state = dcc_command_registry_state_get(&app->registry, 0);
    while (registry_state != NULL && registry_state->entry_count < registry_state->entry_cap) {
        char name[32];
        (void)snprintf(name, sizeof(name), "listener-seed-%zu", registry_seed_index++);
        dcc_application_command_builder_t seed;
        dcc_application_command_builder_init(&seed);
        if (dcc_application_command_builder_set_name(&seed, name) != DCC_OK ||
            dcc_application_command_builder_set_description(&seed, "seed") != DCC_OK ||
            dcc_application_command_builder_set_type(
                &seed,
                DCC_APPLICATION_COMMAND_CHAT_INPUT
            ) != DCC_OK ||
            dcc_command_registry_add_builder(&app->registry, &seed) != DCC_OK) {
            return 1;
        }
        registry_state = dcc_command_registry_state_get(&app->registry, 0);
    }
    dcc_application_command_builder_t growth_command;
    dcc_application_command_builder_init(&growth_command);
    growth_command.name = "listener-registry-growth";
    growth_command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME;
    growth_command.description = "registry growth oom";
    growth_command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION;
    growth_command.type = DCC_APPLICATION_COMMAND_CHAT_INPUT;
    growth_command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE;
    dcc_listener_t growth_listener = slash_listener("ignored", state);
    growth_listener.target.route.name = NULL;
    growth_listener.target.route.description = NULL;
    growth_listener.target.route.command = &growth_command;
    growth_listener.cleanup = failed_listener_cleanup;
    cleanup_count = state->failed_cleanup_count;
    dcc_command_registry_test_fail_next_growth();
    if (expect_failed_listen_exact(
            app,
            &growth_listener,
            DCC_ERR_NOMEM,
            "command registry growth allocation failure"
        ) != 0 || state->failed_cleanup_count != cleanup_count) {
        return 1;
    }
    retry_id = 0U;
    if (dcc_app_listen(app, &growth_listener, &retry_id) != DCC_OK || retry_id == 0U ||
        dcc_app_unlisten(app, retry_id) != DCC_OK ||
        state->failed_cleanup_count != cleanup_count + 1U) {
        fprintf(stderr, "listener registry growth failpoint did not reset for a clean retry\n");
        return 1;
    }

    dcc_application_command_option_builder_t placeholder_option;
    dcc_application_command_option_builder_init(&placeholder_option);
    dcc_application_command_builder_t oversized;
    dcc_application_command_builder_init(&oversized);
    oversized.name = "schema-copy-overflow";
    oversized.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME;
    oversized.description = "registry growth oom";
    oversized.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION;
    oversized.type = DCC_APPLICATION_COMMAND_CHAT_INPUT;
    oversized.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE |
        DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS;
    oversized.options = &placeholder_option;
    oversized.options_count = SIZE_MAX / sizeof(placeholder_option) + 1U;
    dcc_listener_t registry_listener = slash_listener("ignored", state);
    registry_listener.target.route.name = NULL;
    registry_listener.target.route.description = NULL;
    registry_listener.target.route.command = &oversized;
    registry_listener.cleanup = failed_listener_cleanup;
    cleanup_count = state->failed_cleanup_count;
    if (expect_failed_listen_exact(
            app,
            &registry_listener,
            DCC_ERR_INVALID_ARG,
            "command schema deep-copy failure"
        ) != 0 || state->failed_cleanup_count != cleanup_count) {
        return 1;
    }


    const char *partial_fallbacks[2] = {"partial-a", "partial-b"};
    dcc_listener_binding_t partial_binding = {
        .size = sizeof(partial_binding),
        .version = DCC_LISTENER_BINDING_VERSION,
        .name = "values",
        .type = DCC_CTX_FORM_BIND_VALUES,
        .field_offset = offsetof(owned_values_args_t, values),
        .count_offset = offsetof(owned_values_args_t, value_count),
        .has_count_offset = 1U,
        .fallback_values = partial_fallbacks,
        .fallback_values_count = 2U,
    };
    dcc_listener_t partial_listener;
    dcc_listener_init(&partial_listener, DCC_LISTENER_MODAL);
    partial_listener.handler.typed = typed_owned_values_handler;
    partial_listener.user_data = state;
    partial_listener.cleanup = failed_listener_cleanup;
    partial_listener.args_size = sizeof(owned_values_args_t);
    dcc_listener_bindings_t partial_bindings = {
        .size = sizeof(partial_bindings),
        .version = DCC_LISTENER_BINDINGS_VERSION,
        .kind = DCC_LISTENER_BIND_FORM,
        .items.forms = &partial_binding,
        .count = 1U,
    };
    partial_listener.bindings = &partial_bindings;
    partial_listener.target.route.name = "partial-metadata-oom";
    cleanup_count = state->failed_cleanup_count;
    app->listener_test_fail_metadata_copy_after = 1U;
    if (expect_failed_listen_exact(
            app,
            &partial_listener,
            DCC_ERR_NOMEM,
            "partial metadata ownership failure"
        ) != 0 || state->failed_cleanup_count != cleanup_count) {
        return 1;
    }
    retry_id = 0U;
    if (dcc_app_listen(app, &partial_listener, &retry_id) != DCC_OK || retry_id == 0U ||
        dcc_app_unlisten(app, retry_id) != DCC_OK ||
        state->failed_cleanup_count != cleanup_count + 1U) {
        fprintf(stderr, "metadata-copy failpoint did not reset for a clean retry\n");
        return 1;
    }

    const char *one_fallback = "fallback";
    dcc_listener_binding_t huge_binding = {
        .size = sizeof(huge_binding),
        .version = DCC_LISTENER_BINDING_VERSION,
        .name = "values",
        .type = DCC_CTX_FORM_BIND_VALUES,
        .field_offset = offsetof(owned_values_args_t, values),
        .count_offset = offsetof(owned_values_args_t, value_count),
        .has_count_offset = 1U,
        .fallback_values = &one_fallback,
        .fallback_values_count = SIZE_MAX / sizeof(const char *) + 1U,
    };
    dcc_listener_t metadata_listener;
    dcc_listener_init(&metadata_listener, DCC_LISTENER_MODAL);
    metadata_listener.handler.typed = typed_owned_values_handler;
    metadata_listener.user_data = state;
    metadata_listener.cleanup = failed_listener_cleanup;
    metadata_listener.args_size = sizeof(owned_values_args_t);
    dcc_listener_bindings_t metadata_bindings = {
        .size = sizeof(metadata_bindings),
        .version = DCC_LISTENER_BINDINGS_VERSION,
        .kind = DCC_LISTENER_BIND_FORM,
        .items.forms = &huge_binding,
        .count = 1U,
    };
    metadata_listener.bindings = &metadata_bindings;
    metadata_listener.target.route.name = "metadata-oom";
    cleanup_count = state->failed_cleanup_count;
    if (expect_failed_listen_exact(
            app,
            &metadata_listener,
            DCC_ERR_NOMEM,
            "partial metadata copy failure"
        ) != 0 || state->failed_cleanup_count != cleanup_count) {
        return 1;
    }
    huge_binding.fallback_values_count = 1U;
    retry_id = 0U;
    if (dcc_app_listen(app, &metadata_listener, &retry_id) != DCC_OK || retry_id == 0U ||
        dcc_app_unlisten(app, retry_id) != DCC_OK ||
        state->failed_cleanup_count != cleanup_count + 1U) {
        fprintf(stderr, "metadata overflow failure did not permit a clean retry\n");
        return 1;
    }
    if (dcc_app_unlisten(app, seed_schedule_id) != DCC_OK ||
        dcc_app_unlisten(app, seed_route_id) != DCC_OK) {
        return 1;
    }
    return dcc_app_destroy(app) == DCC_OK ? 0 : 1;
}

static dcc_listener_t command_schema_listener(
    dcc_listener_kind_t kind,
    const dcc_application_command_builder_t *command,
    contract_state_t *state
) {
    dcc_listener_t listener;
    dcc_listener_init(&listener, kind);
    listener.handler.plain = plain_handler;
    listener.user_data = state;
    listener.target.route.command = command;
    if (kind == DCC_LISTENER_SUBCOMMAND) {
        listener.target.route.command_name = command->name;
        listener.target.route.subcommand_path = "leaf";
    }
    return listener;
}

static int expect_command_schema_case(
    dcc_app_t *app,
    dcc_listener_kind_t kind,
    const dcc_application_command_builder_t *command,
    contract_state_t *state,
    uint8_t accepted,
    const char *label
) {
    dcc_listener_t listener = command_schema_listener(kind, command, state);
    if (!accepted) {
        return expect_failed_listen_exact(app, &listener, DCC_ERR_INVALID_ARG, label);
    }
    size_t command_count = dcc_command_registry_count(&app->registry);
    dcc_listener_id_t id = 0U;
    dcc_status_t status = dcc_app_listen(app, &listener, &id);
    if (status != DCC_OK || id == 0U ||
        dcc_command_registry_count(&app->registry) != command_count + 1U ||
        dcc_app_unlisten(app, id) != DCC_OK) {
        fprintf(stderr, "%s command schema was not accepted exactly once: status=%d\n", label, status);
        return 1;
    }
    return 0;
}

static dcc_application_command_builder_t command_builder(
    const char *name,
    dcc_application_command_type_t type,
    uint8_t has_type,
    const char *description
) {
    dcc_application_command_builder_t command;
    dcc_application_command_builder_init(&command);
    command.name = name;
    command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME;
    command.type = (uint32_t)type;
    if (has_type != 0U) {
        command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE;
    }
    command.description = description;
    if (description != NULL) {
        command.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION;
    }
    return command;
}

static int test_command_type_matrix(contract_state_t *state) {
    dcc_app_t *app = NULL;
    if (create_app(&app) != DCC_OK) {
        return 1;
    }
    dcc_application_command_builder_t slash = command_builder(
        "matrix-slash",
        DCC_APPLICATION_COMMAND_CHAT_INPUT,
        1U,
        "slash"
    );
    dcc_application_command_builder_t default_slash = command_builder(
        "matrix-default-slash",
        DCC_APPLICATION_COMMAND_CHAT_INPUT,
        0U,
        "default slash"
    );
    dcc_application_command_builder_t subcommand = command_builder(
        "matrix-subcommand",
        DCC_APPLICATION_COMMAND_CHAT_INPUT,
        1U,
        "subcommand"
    );
    dcc_application_command_builder_t autocomplete = command_builder(
        "matrix-autocomplete",
        DCC_APPLICATION_COMMAND_CHAT_INPUT,
        1U,
        "autocomplete"
    );
    dcc_application_command_builder_t user = command_builder(
        "matrix-user",
        DCC_APPLICATION_COMMAND_USER,
        1U,
        NULL
    );
    dcc_application_command_builder_t message = command_builder(
        "matrix-message",
        DCC_APPLICATION_COMMAND_MESSAGE,
        1U,
        NULL
    );
    if (expect_command_schema_case(app, DCC_LISTENER_SLASH, &slash, state, 1U, "slash/chat") ||
        expect_command_schema_case(app, DCC_LISTENER_SLASH, &default_slash, state, 1U, "slash/default-chat") ||
        expect_command_schema_case(app, DCC_LISTENER_SUBCOMMAND, &subcommand, state, 1U, "subcommand/chat") ||
        expect_command_schema_case(app, DCC_LISTENER_AUTOCOMPLETE, &autocomplete, state, 1U, "autocomplete/chat") ||
        expect_command_schema_case(app, DCC_LISTENER_USER_CONTEXT_MENU, &user, state, 1U, "user-menu/user") ||
        expect_command_schema_case(app, DCC_LISTENER_MESSAGE_CONTEXT_MENU, &message, state, 1U, "message-menu/message")) {
        return 1;
    }

    dcc_application_command_builder_t slash_user = command_builder(
        "reject-slash-user",
        DCC_APPLICATION_COMMAND_USER,
        1U,
        NULL
    );
    dcc_application_command_builder_t sub_message = command_builder(
        "reject-sub-message",
        DCC_APPLICATION_COMMAND_MESSAGE,
        1U,
        NULL
    );
    dcc_application_command_builder_t autocomplete_user = command_builder(
        "reject-autocomplete-user",
        DCC_APPLICATION_COMMAND_USER,
        1U,
        NULL
    );
    dcc_application_command_builder_t user_default = command_builder(
        "reject-user-default",
        DCC_APPLICATION_COMMAND_CHAT_INPUT,
        0U,
        "default chat"
    );
    dcc_application_command_builder_t message_chat = command_builder(
        "reject-message-chat",
        DCC_APPLICATION_COMMAND_CHAT_INPUT,
        1U,
        "chat"
    );
    dcc_application_command_builder_t menu_description = command_builder(
        "reject-menu-description",
        DCC_APPLICATION_COMMAND_USER,
        1U,
        "not allowed"
    );
    dcc_application_command_builder_t menu_options = command_builder(
        "reject-menu-options",
        DCC_APPLICATION_COMMAND_MESSAGE,
        1U,
        NULL
    );
    menu_options.options_json = "[]";
    menu_options.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON;
    dcc_application_command_builder_t missing_description = command_builder(
        "reject-missing-description",
        DCC_APPLICATION_COMMAND_CHAT_INPUT,
        1U,
        NULL
    );
    dcc_application_command_builder_t empty_description = command_builder(
        "reject-empty-description",
        DCC_APPLICATION_COMMAND_CHAT_INPUT,
        1U,
        ""
    );
    dcc_application_command_builder_t non_boolean_type = slash;
    non_boolean_type.name = "reject-unknown-bit";
    non_boolean_type.present |= UINT64_C(1) << 63U;
    dcc_application_command_builder_t non_boolean_name = slash;
    non_boolean_name.name = "reject-uncovered-bit";
    non_boolean_name.size = offsetof(dcc_application_command_builder_t, type);
    non_boolean_name.present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE;
    dcc_application_command_builder_t component_schema = slash;
    component_schema.name = "reject-component-schema";
    if (expect_command_schema_case(app, DCC_LISTENER_SLASH, &slash_user, state, 0U, "slash/user") ||
        expect_command_schema_case(app, DCC_LISTENER_SUBCOMMAND, &sub_message, state, 0U, "subcommand/message") ||
        expect_command_schema_case(app, DCC_LISTENER_AUTOCOMPLETE, &autocomplete_user, state, 0U, "autocomplete/user") ||
        expect_command_schema_case(app, DCC_LISTENER_USER_CONTEXT_MENU, &user_default, state, 0U, "user-menu/default-chat") ||
        expect_command_schema_case(app, DCC_LISTENER_MESSAGE_CONTEXT_MENU, &message_chat, state, 0U, "message-menu/chat") ||
        expect_command_schema_case(app, DCC_LISTENER_USER_CONTEXT_MENU, &menu_description, state, 0U, "user-menu/description") ||
        expect_command_schema_case(app, DCC_LISTENER_MESSAGE_CONTEXT_MENU, &menu_options, state, 0U, "message-menu/options") ||
        expect_command_schema_case(app, DCC_LISTENER_SLASH, &missing_description, state, 0U, "chat/missing-description") ||
        expect_command_schema_case(app, DCC_LISTENER_SLASH, &empty_description, state, 0U, "chat/empty-description") ||
        expect_command_schema_case(app, DCC_LISTENER_SLASH, &non_boolean_type, state, 0U, "command/non-boolean-has-type") ||
        expect_command_schema_case(app, DCC_LISTENER_SLASH, &non_boolean_name, state, 0U, "command/non-boolean-has-name") ||
        expect_command_schema_case(app, DCC_LISTENER_BUTTON, &component_schema, state, 0U, "component/schema")) {
        return 1;
    }
    return dcc_app_destroy(app) == DCC_OK ? 0 : 1;
}

static int test_metadata_ownership(dcc_app_t *app, contract_state_t *state) {
    char name[32] = "value";
    char fallback[32] = "owned-fallback";
    dcc_listener_binding_t binding = {
        .size = sizeof(binding),
        .version = DCC_LISTENER_BINDING_VERSION,
        .name = name,
        .type = DCC_CTX_OPTION_BIND_STRING,
        .field_offset = offsetof(owned_args_t, value),
        .fallback_string = fallback,
    };
    dcc_listener_t listener = slash_listener("owned-metadata", state);
    listener.handler.typed = typed_owned_handler;
    listener.args_size = sizeof(owned_args_t);
    dcc_listener_bindings_t bindings = {
        .size = sizeof(bindings),
        .version = DCC_LISTENER_BINDINGS_VERSION,
        .kind = DCC_LISTENER_BIND_OPTIONS,
        .items.options = &binding,
        .count = 1U,
    };
    listener.bindings = &bindings;
    char *validator_field = (char *)malloc(16U);
    if (validator_field == NULL) {
        return 1;
    }
    (void)strcpy(validator_field, "owned-field");
    dcc_listener_validator_t validator = {
        .size = sizeof(validator),
        .version = DCC_LISTENER_VALIDATOR_VERSION,
        .field = validator_field,
        .kind = DCC_LISTENER_VALIDATE_STRING_REQUIRED,
        .field_offset = offsetof(owned_args_t, value),
    };
    dcc_listener_validators_t validators = {
        .size = sizeof(validators),
        .version = DCC_LISTENER_VALIDATORS_VERSION,
        .items = &validator,
        .count = 1U,
    };
    listener.validators = &validators;
    dcc_listener_id_t id = 0U;
    if (dcc_app_listen(app, &listener, &id) != DCC_OK) {
        free(validator_field);
        return 1;
    }
    free(validator_field);
    (void)strcpy(name, "overwritten");
    (void)strcpy(fallback, "borrowed-broken");
    if (dispatch_slash(app, "owned-metadata") != 0 || state->owned_count != 1U ||
        dcc_app_unlisten(app, id) != DCC_OK) {
        fprintf(stderr, "listener retained borrowed binding metadata\n");
        return 1;
    }

    char value_a[16] = "owned-a";
    char value_b[16] = "owned-b";
    const char *fallback_values[2] = {value_a, value_b};
    dcc_listener_binding_t values_binding = {
        .size = sizeof(values_binding),
        .version = DCC_LISTENER_BINDING_VERSION,
        .type = DCC_CTX_COMPONENT_BIND_VALUES,
        .field_offset = offsetof(owned_values_args_t, values),
        .count_offset = offsetof(owned_values_args_t, value_count),
        .has_count_offset = 1U,
        .fallback_values = fallback_values,
        .fallback_values_count = 2U,
    };
    dcc_listener_t values_listener;
    dcc_listener_init(&values_listener, DCC_LISTENER_SELECT);
    values_listener.handler.typed = typed_owned_values_handler;
    values_listener.user_data = state;
    values_listener.args_size = sizeof(owned_values_args_t);
    dcc_listener_bindings_t values_bindings = {
        .size = sizeof(values_bindings),
        .version = DCC_LISTENER_BINDINGS_VERSION,
        .kind = DCC_LISTENER_BIND_COMPONENT,
        .items.components = &values_binding,
        .count = 1U,
    };
    values_listener.bindings = &values_bindings;
    values_listener.target.route.name = "owned-values";
    dcc_listener_id_t values_id = 0U;
    if (dcc_app_listen(app, &values_listener, &values_id) != DCC_OK) {
        return 1;
    }
    (void)strcpy(value_a, "changed-a");
    (void)strcpy(value_b, "changed-b");
    fallback_values[0] = "borrowed-a";
    fallback_values[1] = "borrowed-b";
    if (dispatch_interaction(
            app,
            DCC_EVENT_SELECT_CLICK,
            NULL,
            "owned-values",
            NULL,
            0U
        ) != 0 || state->owned_count != 2U ||
        dcc_app_unlisten(app, values_id) != DCC_OK) {
        fprintf(stderr, "listener retained borrowed fallback value metadata\n");
        return 1;
    }
    return 0;
}

int main(void) {
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;
    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK) {
        return 1;
    }
    contract_state_t state = {0};
    int failed = test_validation_matrix(app, &state) != 0 ||
                 test_implicit_command_descriptions(&state) != 0 ||
                 test_exact_failure_transactions(&state) != 0 ||
                 test_command_type_matrix(&state) != 0 ||
                 test_transactional_retry(app, &state) != 0 ||
                 test_metadata_ownership(app, &state) != 0 ||
                 test_listener_kind_coverage(app, &state) != 0;
    dcc_app_destroy(app);
    return failed;
}
