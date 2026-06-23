#include "internal/app/dcc_app_internal.h"

#include "internal/dcc_core_internal.h"

#include <stdlib.h>
#include <string.h>

static int dcc_app_options_has_field(const dcc_app_options_t *options, size_t offset, size_t field_size) {
    return options != NULL && options->size >= offset && field_size <= options->size - offset;
}

static dcc_status_t dcc_app_register_dispatchers(dcc_app_t *app) {
    dcc_status_t status = dcc_client_on(app->client, DCC_EVENT_SLASH_COMMAND, dcc_app_dispatch_event, app, NULL);
    if (status == DCC_OK) {
        status = dcc_client_on(app->client, DCC_EVENT_BUTTON_CLICK, dcc_app_dispatch_event, app, NULL);
    }
    if (status == DCC_OK) {
        status = dcc_client_on(app->client, DCC_EVENT_SELECT_CLICK, dcc_app_dispatch_event, app, NULL);
    }
    if (status == DCC_OK) {
        status = dcc_client_on(app->client, DCC_EVENT_FORM_SUBMIT, dcc_app_dispatch_event, app, NULL);
    }
    if (status == DCC_OK) {
        status = dcc_client_on(app->client, DCC_EVENT_AUTOCOMPLETE, dcc_app_dispatch_event, app, NULL);
    }
    if (status == DCC_OK) {
        status = dcc_client_on(app->client, DCC_EVENT_USER_CONTEXT_MENU, dcc_app_dispatch_event, app, NULL);
    }
    if (status == DCC_OK) {
        status = dcc_client_on(app->client, DCC_EVENT_MESSAGE_CONTEXT_MENU, dcc_app_dispatch_event, app, NULL);
    }
    return status;
}

dcc_store_t *dcc_app_store(dcc_app_t *app) {
    return app != NULL && app->store_open ? &app->store : NULL;
}

void dcc_app_store_close(dcc_app_t *app) {
    if (app == NULL || !app->store_open) {
        return;
    }
    dcc_store_deinit(&app->store);
    memset(&app->store, 0, sizeof(app->store));
    app->store_open = 0U;
}

dcc_status_t dcc_app_store_open_file(dcc_app_t *app, const char *path) {
    if (app == NULL || path == NULL || path[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_store_t store;
    memset(&store, 0, sizeof(store));
    dcc_status_t status = dcc_store_file_open(path, &store);
    if (status != DCC_OK) {
        return status;
    }

    dcc_app_store_close(app);
    app->store = store;
    app->store_open = 1U;
    return DCC_OK;
}

dcc_status_t dcc_app_create(const dcc_app_options_t *options, dcc_app_t **out) {
    if (options == NULL || out == NULL || options->size < offsetof(dcc_app_options_t, client) + sizeof(options->client)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_app_t *app = (dcc_app_t *)calloc(1U, sizeof(*app));
    if (app == NULL) {
        return DCC_ERR_NOMEM;
    }
    atomic_init(&app->stopping, false);
    dcc_command_registry_init(&app->registry);
    if (dcc_app_options_has_field(
            options,
            offsetof(dcc_app_options_t, command_registry),
            sizeof(options->command_registry)
        )) {
        app->command_registry_options = options->command_registry;
    } else {
        dcc_command_registry_options_init(&app->command_registry_options);
    }
    if (dcc_app_options_has_field(
            options,
            offsetof(dcc_app_options_t, auto_defer_after_ms),
            sizeof(options->auto_defer_after_ms)
        )) {
        app->auto_defer_after_ms = options->auto_defer_after_ms;
    }
    if (dcc_app_options_has_field(
            options,
            offsetof(dcc_app_options_t, auto_defer_ephemeral),
            sizeof(options->auto_defer_ephemeral)
        )) {
        app->auto_defer_ephemeral = options->auto_defer_ephemeral != 0U;
    }
    dcc_app_command_sync_options_init(&app->command_sync_options);
    if (dcc_app_options_has_field(
            options,
            offsetof(dcc_app_options_t, command_sync),
            sizeof(options->command_sync)
        )) {
        app->command_sync_options = options->command_sync;
    }
    if (app->command_sync_options.command_registry.size == 0U) {
        app->command_sync_options.command_registry = app->command_registry_options;
    }
    if (dcc_app_options_has_field(
            options,
            offsetof(dcc_app_options_t, command_sync_on_ready),
            sizeof(options->command_sync_on_ready)
        )) {
        app->command_sync_on_ready = options->command_sync_on_ready != 0U;
    }
    if (dcc_app_options_has_field(
            options,
            offsetof(dcc_app_options_t, store_file_path),
            sizeof(options->store_file_path)
        ) &&
        options->store_file_path != NULL &&
        options->store_file_path[0] != '\0') {
        dcc_status_t status = dcc_app_store_open_file(app, options->store_file_path);
        if (status != DCC_OK) {
            dcc_command_registry_deinit(&app->registry);
            free(app);
            return status;
        }
    }

    dcc_status_t status = dcc_client_create(&options->client, &app->client);
    if (status != DCC_OK) {
        dcc_app_store_close(app);
        dcc_command_registry_deinit(&app->registry);
        free(app);
        return status;
    }

    status = dcc_app_register_dispatchers(app);
    if (status != DCC_OK) {
        dcc_client_destroy(app->client);
        dcc_app_store_close(app);
        dcc_command_registry_deinit(&app->registry);
        free(app);
        return status;
    }
    if (app->command_sync_on_ready) {
        status = dcc_app_register_command_sync_listener(app);
        if (status != DCC_OK) {
            dcc_client_destroy(app->client);
            dcc_app_store_close(app);
            dcc_command_registry_deinit(&app->registry);
            free(app);
            return status;
        }
    }

    *out = app;
    return DCC_OK;
}

void dcc_app_destroy(dcc_app_t *app) {
    if (app == NULL) {
        return;
    }
    (void)dcc_app_stop(app);
    dcc_app_stop_schedules(app);
    for (size_t i = 0; i < app->component_session_listener_count; ++i) {
        if (app->component_session_listeners[i].listener.state != NULL) {
            (void)dcc_client_off_component_session(
                app->client,
                &app->component_session_listeners[i].listener
            );
        }
        free(app->component_session_listeners[i].state);
    }
    free(app->component_session_listeners);
    for (size_t i = 0; i < app->event_listener_count; ++i) {
        (void)dcc_client_off(
            app->client,
            app->event_listeners[i].type,
            app->event_listeners[i].id
        );
        free(app->event_listeners[i].state);
    }
    if (app->command_sync_listener_registered) {
        (void)dcc_client_off(app->client, DCC_EVENT_READY, app->command_sync_listener_id);
    }
    free(app->event_listeners);
    dcc_client_destroy(app->client);
    dcc_command_registry_deinit(&app->registry);
    for (size_t i = 0; i < app->route_count; ++i) {
        free(app->routes[i].key);
        if (app->routes[i].user_data_cleanup != NULL) {
            app->routes[i].user_data_cleanup(app->routes[i].user_data);
        }
        for (size_t j = 0; j < app->routes[i].middleware_count; ++j) {
            if (app->routes[i].middlewares[j].cleanup != NULL) {
                app->routes[i].middlewares[j].cleanup(app->routes[i].middlewares[j].user_data);
            }
        }
        free(app->routes[i].middlewares);
    }
    free(app->routes);
    for (size_t i = 0; i < app->middleware_count; ++i) {
        if (app->middlewares[i].cleanup != NULL) {
            app->middlewares[i].cleanup(app->middlewares[i].user_data);
        }
    }
    free(app->middlewares);
    for (size_t i = 0; i < app->schedule_count; ++i) {
        free(app->schedules[i]);
    }
    free(app->schedules);
    for (size_t i = 0; i < app->module_count; ++i) {
        if (app->modules[i].cleanup != NULL) {
            app->modules[i].cleanup(app->modules[i].user_data);
        }
    }
    free(app->modules);
    dcc_app_clear_state(app);
    dcc_app_store_close(app);
    free(app);
}

dcc_client_t *dcc_app_client(dcc_app_t *app) {
    return app != NULL ? app->client : NULL;
}

dcc_command_registry_t *dcc_app_command_registry(dcc_app_t *app) {
    return app != NULL ? &app->registry : NULL;
}

dcc_status_t dcc_app_start(dcc_app_t *app) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    atomic_store_explicit(&app->stopping, false, memory_order_release);
    dcc_status_t status = dcc_client_start(app->client);
    if (status != DCC_OK) {
        return status;
    }
    app->started = 1U;
    status = dcc_app_start_schedules(app);
    if (status != DCC_OK) {
        (void)dcc_client_stop(app->client);
        app->started = 0U;
    }
    return status;
}

dcc_status_t dcc_app_stop(dcc_app_t *app) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    atomic_store_explicit(&app->stopping, true, memory_order_release);
    dcc_app_stop_schedules(app);
    app->started = 0U;
    return dcc_client_stop(app->client);
}

dcc_status_t dcc_app_wait(dcc_app_t *app) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_client_wait(app->client);
    dcc_app_stop_schedules(app);
    app->started = 0U;
    return status;
}

dcc_status_t dcc_app_run(dcc_app_t *app) {
    dcc_status_t status = dcc_app_start(app);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_app_wait(app);
}

dcc_status_t dcc_app_auto_defer(dcc_app_t *app, uint64_t after_ms) {
    if (app == NULL || after_ms == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    app->auto_defer_after_ms = after_ms;
    app->auto_defer_ephemeral = 0U;
    return DCC_OK;
}

dcc_status_t dcc_app_auto_defer_ephemeral(dcc_app_t *app, uint64_t after_ms) {
    if (app == NULL || after_ms == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    app->auto_defer_after_ms = after_ms;
    app->auto_defer_ephemeral = 1U;
    return DCC_OK;
}

dcc_status_t dcc_app_disable_auto_defer(dcc_app_t *app) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    app->auto_defer_after_ms = 0U;
    app->auto_defer_ephemeral = 0U;
    return DCC_OK;
}
