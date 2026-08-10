#include <dcc/app.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/client/dcc_client_state_internal.h"
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
} contract_state_t;

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
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_SLASH_COMMAND;
    event.data.interaction.name = name;
    event.data.interaction.token = "contract-token";
    event.data.interaction.id = 1U;
    event.data.interaction.application_id = 2U;
    return dcc_event_bus_dispatch(&app->client->events, app->client, &event) == DCC_OK ? 0 : 1;
}

static int test_validation_matrix(dcc_app_t *app, contract_state_t *state) {
    dcc_listener_t listener = slash_listener("invalid", state);
    dcc_listener_t invalid = listener;
    invalid.policy.guild_only = 2U;
    if (expect_invalid_without_mutation(app, &invalid, "non-boolean guild_only") != 0) {
        return 1;
    }
    invalid = listener;
    invalid.policy.dm_only = 1U;
    invalid.policy.nsfw_only = 1U;
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
    invalid.bindings.kind = DCC_LISTENER_BIND_OPTIONS;
    invalid.bindings.items.options = &binding;
    invalid.bindings.count = 1U;
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
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = type;
    event.data.interaction.name = name;
    event.data.interaction.custom_id = custom_id;
    event.data.interaction.options = options;
    event.data.interaction.options_count = option_count;
    event.data.interaction.token = "contract-token";
    event.data.interaction.id = 1U;
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
    if (dcc_app_listen(app, &listener, &ids[id_count++]) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_SLASH_COMMAND, "root", NULL, &subcommand, 1U) != 0) {
        return 1;
    }

    dcc_listener_init(&listener, DCC_LISTENER_AUTOCOMPLETE);
    listener.handler.plain = plain_handler;
    listener.user_data = state;
    listener.target.route.command_name = "root";
    listener.target.route.subcommand_path = "leaf";
    if (dcc_app_listen(app, &listener, &ids[id_count++]) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_AUTOCOMPLETE, "root", NULL, &subcommand, 1U) != 0) {
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
    invalid_command.has_type = 1U;
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
    invalid_command.has_name = 1U;
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
    listener.bindings.kind = DCC_LISTENER_BIND_OPTIONS;
    listener.bindings.items.options = &binding;
    listener.bindings.count = 1U;
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
    listener.validators.items = &validator;
    listener.validators.count = 1U;
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
    values_listener.bindings.kind = DCC_LISTENER_BIND_COMPONENT;
    values_listener.bindings.items.components = &values_binding;
    values_listener.bindings.count = 1U;
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
                 test_transactional_retry(app, &state) != 0 ||
                 test_metadata_ownership(app, &state) != 0 ||
                 test_listener_kind_coverage(app, &state) != 0;
    dcc_app_destroy(app);
    return failed;
}
