#include <dcc/app.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/events/dcc_event_state_internal.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct listener_smoke_state {
    unsigned handler_count;
    unsigned typed_count;
    unsigned event_count;
    unsigned task_count;
    unsigned error_count;
    unsigned cleanup_count;
    unsigned event_cleanup_count;
    unsigned task_cleanup_count;
    unsigned destroy_cleanup_count;
    dcc_status_t error_status;
} listener_smoke_state_t;

typedef struct typed_smoke_args {
    const char *value;
} typed_smoke_args_t;

static dcc_status_t failing_handler(dcc_ctx_t *ctx, void *user_data) {
    listener_smoke_state_t *state = (listener_smoke_state_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->handler_count++;
    return DCC_ERR_STATE;
}

static dcc_status_t passive_handler(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    listener_smoke_state_t *state = (listener_smoke_state_t *)user_data;
    if (state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->handler_count++;
    return DCC_OK;
}

static dcc_status_t failing_typed_handler(
    dcc_ctx_t *ctx,
    void *args,
    void *user_data
) {
    typed_smoke_args_t *typed = (typed_smoke_args_t *)args;
    listener_smoke_state_t *state = (listener_smoke_state_t *)user_data;
    if (ctx == NULL || typed == NULL || state == NULL || typed->value == NULL ||
        strcmp(typed->value, "fallback") != 0) {
        return DCC_ERR_INVALID_ARG;
    }
    state->typed_count++;
    return DCC_ERR_STATE;
}

static dcc_status_t failing_event_handler(
    dcc_app_t *app,
    const dcc_event_t *event,
    void *user_data
) {
    listener_smoke_state_t *state = (listener_smoke_state_t *)user_data;
    if (app == NULL || event == NULL || state == NULL ||
        dcc_event_type(event) != DCC_EVENT_LOG) {
        return DCC_ERR_INVALID_ARG;
    }
    state->event_count++;
    return DCC_ERR_STATE;
}

static dcc_status_t failing_task_handler(dcc_app_t *app, void *user_data) {
    listener_smoke_state_t *state = (listener_smoke_state_t *)user_data;
    if (app == NULL || state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    state->task_count++;
    return DCC_ERR_STATE;
}

static void on_error(
    dcc_app_t *app,
    const dcc_error_t *error,
    void *user_data
) {
    listener_smoke_state_t *state = (listener_smoke_state_t *)user_data;
    if (app == NULL || state == NULL || error == NULL ||
        error->size != sizeof(*error) || error->version != DCC_ERROR_VERSION ||
        error->origin != DCC_ERROR_HANDLER || error->message == NULL ||
        error->message[0] == '\0') {
        return;
    }
    state->error_count++;
    state->error_status = error->status;
}

static void cleanup_listener(void *user_data) {
    listener_smoke_state_t *state = (listener_smoke_state_t *)user_data;
    if (state != NULL) {
        state->cleanup_count++;
    }
}

static void cleanup_event_listener(void *user_data) {
    listener_smoke_state_t *state = (listener_smoke_state_t *)user_data;
    if (state != NULL) {
        state->event_cleanup_count++;
    }
}

static void cleanup_task_listener(void *user_data) {
    listener_smoke_state_t *state = (listener_smoke_state_t *)user_data;
    if (state != NULL) {
        state->task_cleanup_count++;
    }
}

static void cleanup_destroyed_listener(void *user_data) {
    listener_smoke_state_t *state = (listener_smoke_state_t *)user_data;
    if (state != NULL) {
        state->destroy_cleanup_count++;
    }
}

static dcc_status_t dispatch_slash(dcc_app_t *app, const char *name) {
    static dcc_snowflake_t next_id = 1U;
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_SLASH_COMMAND;
    event.data.interaction.name = name;
    event.data.interaction.token = "listener-smoke-token";
    event.data.interaction.id = next_id++;
    event.data.interaction.application_id = 2U;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static dcc_status_t dispatch_log(dcc_app_t *app) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_LOG;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static dcc_listener_t slash_listener(
    const char *name,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_cleanup_fn cleanup
) {
    dcc_listener_t listener;
    dcc_listener_init(&listener, DCC_LISTENER_SLASH);
    listener.handler.plain = handler;
    listener.user_data = user_data;
    listener.cleanup = cleanup;
    listener.target.route.name = name;
    listener.target.route.description = "listener smoke";
    return listener;
}

int main(void) {
    dcc_listener_t initialized;
    dcc_listener_init(&initialized, DCC_LISTENER_EVENT);
    if (initialized.target.event.size != sizeof(initialized.target.event) ||
        initialized.target.event.version != DCC_LISTENER_TARGET_VERSION) {
        fprintf(stderr, "event target metadata was not initialized for its selected kind\n");
        return 1;
    }
    dcc_listener_init(&initialized, DCC_LISTENER_MESSAGE_COMMAND);
    if (initialized.target.message_command.size != sizeof(initialized.target.message_command) ||
        initialized.target.message_command.version != DCC_LISTENER_TARGET_VERSION) {
        fprintf(stderr, "message-command target metadata was not initialized for its selected kind\n");
        return 1;
    }
    dcc_listener_init(&initialized, DCC_LISTENER_TASK);
    if (initialized.target.schedule.size != sizeof(initialized.target.schedule) ||
        initialized.target.schedule.version != DCC_LISTENER_TARGET_VERSION) {
        fprintf(stderr, "schedule target metadata was not initialized for its selected kind\n");
        return 1;
    }

    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;

    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create(&options, &app);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_app_create failed: %s\n", dcc_status_string(status));
        return 1;
    }

    listener_smoke_state_t state;
    memset(&state, 0, sizeof(state));
    if (dcc_app_on_error(app, on_error, &state) != DCC_OK) {
        fprintf(stderr, "error adapter setup failed\n");
        dcc_app_destroy(app);
        return 1;
    }

    dcc_listener_id_t ignored = 99U;
    dcc_listener_t listener = slash_listener("listener-smoke", failing_handler, &state, cleanup_listener);
    dcc_listener_t invalid = listener;
    invalid.size = offsetof(dcc_listener_t, target);
    if (dcc_app_listen(app, &invalid, &ignored) != DCC_ERR_INVALID_ARG || ignored != 0U) {
        fprintf(stderr, "short listener size was accepted\n");
        dcc_app_destroy(app);
        return 1;
    }
    invalid = listener;
    invalid.version = DCC_LISTENER_VERSION + 1U;
    if (dcc_app_listen(app, &invalid, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "unsupported listener version was accepted\n");
        dcc_app_destroy(app);
        return 1;
    }
    dcc_listener_binding_t typed_binding = {
        .size = sizeof(typed_binding),
        .version = DCC_LISTENER_BINDING_VERSION,
        .name = "value",
        .type = DCC_CTX_OPTION_BIND_STRING,
        .field_offset = offsetof(typed_smoke_args_t, value),
        .fallback_string = "fallback",
    };
    dcc_listener_t typed_listener =
        slash_listener("typed-listener-smoke", failing_handler, &state, NULL);
    typed_listener.handler.typed = failing_typed_handler;
    typed_listener.args_size = sizeof(typed_smoke_args_t);
    typed_listener.bindings.kind = DCC_LISTENER_BIND_OPTIONS;
    typed_listener.bindings.items.options = &typed_binding;
    typed_listener.bindings.count = 1U;
    invalid = typed_listener;
    invalid.bindings.kind = DCC_LISTENER_BIND_NONE;
    if (dcc_app_listen(app, &invalid, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "typed handler with incompatible binding metadata was accepted\n");
        dcc_app_destroy(app);
        return 1;
    }
    dcc_listener_binding_t out_of_bounds_binding = typed_binding;
    out_of_bounds_binding.field_offset = sizeof(typed_smoke_args_t) - 1U;
    invalid = typed_listener;
    invalid.bindings.items.options = &out_of_bounds_binding;
    if (dcc_app_listen(app, &invalid, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "typed binding extending past args_size was accepted\n");
        dcc_app_destroy(app);
        return 1;
    }
    invalid = listener;
    invalid.kind = (dcc_listener_kind_t)(DCC_LISTENER_TASK + 1);
    if (dcc_app_listen(app, &invalid, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "unsupported listener kind was accepted\n");
        dcc_app_destroy(app);
        return 1;
    }
    invalid = listener;
    invalid.handler.plain = NULL;
    if (dcc_app_listen(app, &invalid, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "listener without its kind-compatible handler was accepted\n");
        dcc_app_destroy(app);
        return 1;
    }
    invalid = listener;
    invalid.policy.size = sizeof(invalid.policy);
    invalid.policy.version = DCC_LISTENER_ROUTE_POLICY_VERSION + 1U;
    if (dcc_app_listen(app, &invalid, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "unsupported nested policy version was accepted\n");
        dcc_app_destroy(app);
        return 1;
    }
    invalid = listener;
    invalid.bindings.size = sizeof(invalid.bindings);
    invalid.bindings.version = DCC_LISTENER_BINDINGS_VERSION;
    invalid.bindings.kind = DCC_LISTENER_BIND_OPTIONS;
    invalid.bindings.count = 1U;
    invalid.bindings.items.options = NULL;
    if (dcc_app_listen(app, &invalid, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "invalid typed bindings were accepted\n");
        dcc_app_destroy(app);
        return 1;
    }

    dcc_listener_id_t id = 0U;
    status = dcc_app_listen(app, &listener, &id);
    if (status != DCC_OK || id == 0U) {
        fprintf(stderr, "canonical listener registration failed: %s\n", dcc_status_string(status));
        dcc_app_destroy(app);
        return 1;
    }
    if (dispatch_slash(app, "listener-smoke") != DCC_OK ||
        state.handler_count != 1U || state.error_count != 1U ||
        state.error_status != DCC_ERR_STATE) {
        fprintf(stderr, "handler status did not reach the error adapter exactly once\n");
        dcc_app_destroy(app);
        return 1;
    }

    if (dcc_app_unlisten(app, id) != DCC_OK || state.cleanup_count != 1U ||
        dcc_app_unlisten(app, id) != DCC_ERR_INVALID_ARG ||
        dispatch_slash(app, "listener-smoke") != DCC_OK ||
        state.handler_count != 1U || state.error_count != 1U) {
        fprintf(stderr, "unlisten did not prevent dispatch or clean up exactly once\n");
        dcc_app_destroy(app);
        return 1;
    }

    dcc_listener_id_t typed_id = 0U;
    if (dcc_app_listen(app, &typed_listener, &typed_id) != DCC_OK || typed_id == 0U ||
        dispatch_slash(app, "typed-listener-smoke") != DCC_OK ||
        state.typed_count != 1U || state.error_count != 2U ||
        dcc_app_unlisten(app, typed_id) != DCC_OK) {
        fprintf(stderr, "typed listener did not propagate its returned status\n");
        dcc_app_destroy(app);
        return 1;
    }

    dcc_listener_t event_listener;
    dcc_listener_init(&event_listener, DCC_LISTENER_EVENT);
    event_listener.handler.event = failing_event_handler;
    event_listener.user_data = &state;
    event_listener.cleanup = cleanup_event_listener;
    event_listener.target.event.type = DCC_EVENT_LOG;
    invalid = event_listener;
    invalid.policy.guild_only = 1U;
    if (dcc_app_listen(app, &invalid, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "route-only policy was accepted for an event listener\n");
        dcc_app_destroy(app);
        return 1;
    }
    invalid = event_listener;
    invalid.target.event.version = DCC_LISTENER_TARGET_VERSION + 1U;
    if (dcc_app_listen(app, &invalid, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "unsupported event target version was accepted\n");
        dcc_app_destroy(app);
        return 1;
    }

    dcc_listener_id_t event_id = 0U;
    if (dcc_app_listen(app, &event_listener, &event_id) != DCC_OK || event_id == 0U ||
        dispatch_log(app) != DCC_OK || state.event_count != 1U || state.error_count != 3U ||
        dcc_app_unlisten(app, event_id) != DCC_OK || state.event_cleanup_count != 1U ||
        dispatch_log(app) != DCC_OK || state.event_count != 1U || state.error_count != 3U) {
        fprintf(stderr, "event unlisten did not stop dispatch and clean up exactly once\n");
        dcc_app_destroy(app);
        return 1;
    }

    dcc_listener_t task_listener;
    dcc_listener_init(&task_listener, DCC_LISTENER_TASK);
    task_listener.handler.task = failing_task_handler;
    task_listener.user_data = &state;
    task_listener.cleanup = cleanup_task_listener;
    task_listener.target.schedule.kind = DCC_LISTENER_SCHEDULE_INTERVAL;
    task_listener.target.schedule.interval_ms = 1U;
    dcc_listener_id_t task_id = 0U;
    if (dcc_app_listen(app, &task_listener, &task_id) != DCC_OK || task_id == 0U ||
        app->listener_count == 0U || app->schedule_count == 0U) {
        fprintf(stderr, "canonical task listener registration failed\n");
        dcc_app_destroy(app);
        return 1;
    }
    dcc_app_listener_entry_t *task_entry = app->listeners[app->listener_count - 1U];
    dcc_app_schedule_t *task_schedule = app->schedules[app->schedule_count - 1U];
    if (task_schedule->canonical_fn != dcc_app_listener_run_task ||
        task_schedule->listener_state != task_entry ||
        dcc_app_listener_run_task(task_entry, app) != DCC_ERR_STATE ||
        state.task_count != 1U || state.error_count != 4U ||
        dcc_app_unlisten(app, task_id) != DCC_OK || state.task_cleanup_count != 1U ||
        dcc_app_listener_run_task(task_entry, app) != DCC_OK ||
        state.task_count != 1U || state.error_count != 4U) {
        fprintf(stderr, "task adapter did not stop dispatch and clean up exactly once\n");
        dcc_app_destroy(app);
        return 1;
    }

    dcc_listener_t destroy_listener =
        slash_listener("destroy-smoke", passive_handler, &state, cleanup_destroyed_listener);
    if (dcc_app_listen(app, &destroy_listener, NULL) != DCC_OK) {
        fprintf(stderr, "destruction cleanup listener registration failed\n");
        dcc_app_destroy(app);
        return 1;
    }

    dcc_app_destroy(app);
    if (state.cleanup_count != 1U || state.event_cleanup_count != 1U ||
        state.task_cleanup_count != 1U || state.destroy_cleanup_count != 1U) {
        fprintf(
            stderr,
            "App destruction cleanup counts were route=%u event=%u task=%u destroy=%u\n",
            state.cleanup_count,
            state.event_cleanup_count,
            state.task_cleanup_count,
            state.destroy_cleanup_count
        );
        return 1;
    }
    return 0;
}
