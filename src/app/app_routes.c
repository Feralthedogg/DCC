#include "internal/app/dcc_app_internal.h"

#include "internal/dcc_core_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_app_routes_reserve(dcc_app_t *app, size_t need) {
    if (need <= app->route_cap) {
        return DCC_OK;
    }
    size_t next_cap = app->route_cap != 0U ? app->route_cap : 8U;
    while (next_cap < need) {
        if (next_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap *= 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(*app->routes)) {
        return DCC_ERR_NOMEM;
    }
    dcc_app_route_t *next = (dcc_app_route_t *)realloc(app->routes, next_cap * sizeof(*app->routes));
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    app->routes = next;
    app->route_cap = next_cap;
    return DCC_OK;
}

static dcc_status_t dcc_app_middlewares_reserve(dcc_app_t *app, size_t need) {
    if (need <= app->middleware_cap) {
        return DCC_OK;
    }
    size_t next_cap = app->middleware_cap != 0U ? app->middleware_cap : 4U;
    while (next_cap < need) {
        if (next_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap *= 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(*app->middlewares)) {
        return DCC_ERR_NOMEM;
    }
    dcc_app_middleware_t *next =
        (dcc_app_middleware_t *)realloc(app->middlewares, next_cap * sizeof(*app->middlewares));
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    app->middlewares = next;
    app->middleware_cap = next_cap;
    return DCC_OK;
}

static dcc_status_t dcc_app_route_middlewares_reserve(dcc_app_route_t *route, size_t need) {
    if (need <= route->middleware_cap) {
        return DCC_OK;
    }
    size_t next_cap = route->middleware_cap != 0U ? route->middleware_cap : 2U;
    while (next_cap < need) {
        if (next_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap *= 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(*route->middlewares)) {
        return DCC_ERR_NOMEM;
    }
    dcc_app_middleware_t *next =
        (dcc_app_middleware_t *)realloc(route->middlewares, next_cap * sizeof(*route->middlewares));
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    route->middlewares = next;
    route->middleware_cap = next_cap;
    return DCC_OK;
}

dcc_status_t dcc_app_use_internal(
    dcc_app_t *app,
    dcc_app_middleware_fn middleware,
    void *user_data,
    void (*cleanup)(void *user_data)
) {
    if (app == NULL || middleware == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_listener_lock(app);
    if (app->middleware_count == SIZE_MAX) {
        dcc_app_listener_unlock(app);
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status = dcc_app_middlewares_reserve(app, app->middleware_count + 1U);
    if (status != DCC_OK) {
        dcc_app_listener_unlock(app);
        return status;
    }
    app->middlewares[app->middleware_count].fn = middleware;
    app->middlewares[app->middleware_count].user_data = user_data;
    app->middlewares[app->middleware_count].cleanup = cleanup;
    app->middleware_count++;
    dcc_app_listener_unlock(app);
    return DCC_OK;
}

dcc_status_t dcc_app_use(
    dcc_app_t *app,
    dcc_app_middleware_fn middleware,
    void *user_data
) {
    return dcc_app_use_internal(app, middleware, user_data, NULL);
}

dcc_status_t dcc_app_add_route(
    dcc_app_t *app,
    dcc_event_type_t type,
    const char *key,
    uint8_t prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_add_route_with_cleanup(
        app,
        type,
        key,
        prefix,
        handler,
        user_data,
        NULL,
        out_route
    );
}

dcc_status_t dcc_app_add_route_with_cleanup(
    dcc_app_t *app,
    dcc_event_type_t type,
    const char *key,
    uint8_t prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    void (*user_data_cleanup)(void *user_data),
    dcc_app_route_id_t *out_route
) {
    if (app == NULL || key == NULL || key[0] == '\0' || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (app->route_count == SIZE_MAX || app->next_route_id == UINT64_MAX) {
        return DCC_ERR_NOMEM;
    }
    if (out_route != NULL) {
        *out_route = DCC_APP_ROUTE_INVALID;
    }
    dcc_app_listener_lock(app);
    dcc_status_t status = dcc_app_routes_reserve(app, app->route_count + 1U);
    if (status != DCC_OK) {
        dcc_app_listener_unlock(app);
        return status;
    }
    char *copy = dcc_strdup(key);
    if (copy == NULL) {
        dcc_app_listener_unlock(app);
        return DCC_ERR_NOMEM;
    }

    dcc_app_route_t *route = &app->routes[app->route_count++];
    memset(route, 0, sizeof(*route));
    route->id = ++app->next_route_id;
    route->type = type;
    route->key = copy;
    route->prefix = prefix != 0U;
    route->legacy_handler = handler;
    route->user_data = user_data;
    route->context_user_data = user_data;
    route->user_data_cleanup = user_data_cleanup;
    if (out_route != NULL) {
        *out_route = route->id;
    }
    dcc_app_listener_unlock(app);
    return DCC_OK;
}

dcc_status_t dcc_app_add_canonical_route_with_cleanup(
    dcc_app_t *app,
    dcc_event_type_t type,
    const char *key,
    uint8_t prefix,
    dcc_app_handler_fn handler,
    void *user_data,
    void *context_user_data,
    void *listener_state,
    void (*user_data_cleanup)(void *user_data),
    dcc_app_route_id_t *out_route
) {
    if (app == NULL || key == NULL || key[0] == '\0' || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (app->route_count == SIZE_MAX || app->next_route_id == UINT64_MAX) {
        return DCC_ERR_NOMEM;
    }
    if (out_route != NULL) {
        *out_route = DCC_APP_ROUTE_INVALID;
    }
    dcc_app_listener_lock(app);
    dcc_status_t status = dcc_app_routes_reserve(app, app->route_count + 1U);
    if (status != DCC_OK) {
        dcc_app_listener_unlock(app);
        return status;
    }
    char *copy = dcc_strdup(key);
    if (copy == NULL) {
        dcc_app_listener_unlock(app);
        return DCC_ERR_NOMEM;
    }

    dcc_app_route_t *route = &app->routes[app->route_count++];
    memset(route, 0, sizeof(*route));
    route->id = ++app->next_route_id;
    route->type = type;
    route->key = copy;
    route->prefix = prefix != 0U;
    route->handler = handler;
    route->user_data = user_data;
    route->context_user_data = context_user_data;
    route->listener_state = listener_state;
    route->user_data_cleanup = user_data_cleanup;
    if (out_route != NULL) {
        *out_route = route->id;
    }
    dcc_app_listener_unlock(app);
    return DCC_OK;
}

dcc_status_t dcc_app_remove_route_internal(dcc_app_t *app, dcc_app_route_id_t route_id) {
    if (app == NULL || route_id == DCC_APP_ROUTE_INVALID) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_listener_lock(app);
    size_t index = app->route_count;
    for (size_t i = 0U; i < app->route_count; ++i) {
        if (app->routes[i].id == route_id) {
            index = i;
            break;
        }
    }
    if (index == app->route_count) {
        dcc_app_listener_unlock(app);
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_route_t removed = app->routes[index];
    if (index + 1U < app->route_count) {
        memmove(
            &app->routes[index],
            &app->routes[index + 1U],
            (app->route_count - index - 1U) * sizeof(*app->routes)
        );
    }
    app->route_count--;
    memset(&app->routes[app->route_count], 0, sizeof(*app->routes));
    dcc_app_listener_unlock(app);

    free(removed.key);
    dcc_app_callback_frame_t cleanup_frame;
    dcc_app_callback_frame_enter(&cleanup_frame, app, removed.listener_state);
    if (removed.user_data_cleanup != NULL) {
        removed.user_data_cleanup(removed.user_data);
    }
    for (size_t i = 0U; i < removed.middleware_count; ++i) {
        if (removed.middlewares[i].cleanup != NULL) {
            removed.middlewares[i].cleanup(removed.middlewares[i].user_data);
        }
    }
    free(removed.middlewares);
    dcc_app_callback_frame_leave(&cleanup_frame);
    return DCC_OK;
}

static int dcc_app_route_matches(const dcc_app_route_t *route, dcc_event_type_t type, const char *value) {
    if (route == NULL || route->type != type || value == NULL || route->key == NULL) {
        return 0;
    }
    if (!route->prefix) {
        return strcmp(route->key, value) == 0;
    }
    return strncmp(value, route->key, strlen(route->key)) == 0;
}

static dcc_app_route_t *dcc_app_find_route(dcc_app_t *app, dcc_event_type_t type, const char *value) {
    if (app == NULL || value == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < app->route_count; ++i) {
        if (!app->routes[i].prefix && dcc_app_route_matches(&app->routes[i], type, value)) {
            return &app->routes[i];
        }
    }
    for (size_t i = 0; i < app->route_count; ++i) {
        if (app->routes[i].prefix && dcc_app_route_matches(&app->routes[i], type, value)) {
            return &app->routes[i];
        }
    }
    return NULL;
}

static dcc_app_route_t *dcc_app_find_route_by_id(dcc_app_t *app, dcc_app_route_id_t route_id) {
    if (app == NULL || route_id == DCC_APP_ROUTE_INVALID) {
        return NULL;
    }
    for (size_t i = 0; i < app->route_count; ++i) {
        if (app->routes[i].id == route_id) {
            return &app->routes[i];
        }
    }
    return NULL;
}

static int dcc_app_event_uses_interaction_name(dcc_event_type_t type) {
    return type == DCC_EVENT_SLASH_COMMAND ||
           type == DCC_EVENT_AUTOCOMPLETE ||
           type == DCC_EVENT_USER_CONTEXT_MENU ||
           type == DCC_EVENT_MESSAGE_CONTEXT_MENU;
}

static const char *dcc_app_interaction_operation(
    const dcc_event_t *event,
    const dcc_interaction_t *interaction
) {
    if (interaction != NULL) {
        if (interaction->name != NULL && interaction->name[0] != '\0') {
            return interaction->name;
        }
        if (interaction->custom_id != NULL && interaction->custom_id[0] != '\0') {
            return interaction->custom_id;
        }
    }
    return event != NULL ? dcc_event_type_name(dcc_event_type(event)) : "interaction handler";
}

dcc_status_t dcc_app_route_use_internal(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_app_middleware_fn middleware,
    void *user_data,
    void (*cleanup)(void *user_data)
) {
    if (middleware == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_listener_lock(app);
    dcc_app_route_t *route = dcc_app_find_route_by_id(app, route_id);
    if (route == NULL) {
        dcc_app_listener_unlock(app);
        return DCC_ERR_INVALID_ARG;
    }
    if (route->middleware_count == SIZE_MAX) {
        dcc_app_listener_unlock(app);
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status = dcc_app_route_middlewares_reserve(route, route->middleware_count + 1U);
    if (status != DCC_OK) {
        dcc_app_listener_unlock(app);
        return status;
    }
    route->middlewares[route->middleware_count].fn = middleware;
    route->middlewares[route->middleware_count].user_data = user_data;
    route->middlewares[route->middleware_count].cleanup = cleanup;
    route->middleware_count++;
    dcc_app_listener_unlock(app);
    return DCC_OK;
}

dcc_status_t dcc_app_route_use(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_app_middleware_fn middleware,
    void *user_data
) {
    return dcc_app_route_use_internal(app, route_id, middleware, user_data, NULL);
}

static dcc_status_t dcc_app_run_middlewares(
    dcc_ctx_t *ctx,
    const dcc_app_middleware_t *middlewares,
    size_t middleware_count
) {
    if (ctx == NULL || middlewares == NULL) {
        return middleware_count == 0U ? DCC_OK : DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < middleware_count; ++i) {
        dcc_status_t status = middlewares[i].fn(ctx, middlewares[i].user_data);
        if (status != DCC_OK || dcc_event_is_cancelled(ctx->event)) {
            return status != DCC_OK ? status : DCC_ERR_STATE;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_app_snapshot_middlewares(
    dcc_app_t *app,
    dcc_app_middleware_t **out_middlewares,
    size_t *out_middleware_count
) {
    if (app == NULL || out_middlewares == NULL || out_middleware_count == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_middlewares = NULL;
    *out_middleware_count = 0U;
    dcc_app_listener_lock(app);
    size_t middleware_count = app->middleware_count;
    if (middleware_count > SIZE_MAX / sizeof(*app->middlewares)) {
        dcc_app_listener_unlock(app);
        return DCC_ERR_NOMEM;
    }
    dcc_app_middleware_t *middlewares = NULL;
    if (middleware_count != 0U) {
        middlewares = (dcc_app_middleware_t *)malloc(
            middleware_count * sizeof(*middlewares)
        );
        if (middlewares == NULL) {
            dcc_app_listener_unlock(app);
            return DCC_ERR_NOMEM;
        }
        memcpy(middlewares, app->middlewares, middleware_count * sizeof(*middlewares));
    }
    dcc_app_listener_unlock(app);
    *out_middlewares = middlewares;
    *out_middleware_count = middleware_count;
    return DCC_OK;
}

dcc_status_t dcc_app_dispatch_handler(
    dcc_app_t *app,
    dcc_client_t *client,
    const dcc_event_t *event,
    const dcc_interaction_t *interaction,
    const dcc_component_session_result_t *component_session,
    const dcc_app_middleware_t *route_middlewares,
    size_t route_middleware_count,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    if (app == NULL || client == NULL || event == NULL || interaction == NULL || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_ctx_t ctx = {
        .app = app,
        .client = client,
        .event = event,
        .interaction = interaction,
        .component_session = component_session,
        .user_data = user_data,
    };
    dcc_app_callback_frame_t callback_frame;
    dcc_app_callback_frame_enter(&callback_frame, app, NULL);
    dcc_app_middleware_t *app_middlewares = NULL;
    size_t app_middleware_count = 0U;
    dcc_status_t status = dcc_app_snapshot_middlewares(
        app,
        &app_middlewares,
        &app_middleware_count
    );
    if (status != DCC_OK) {
        dcc_app_callback_frame_leave(&callback_frame);
        return status;
    }
    dcc_flow_init(&ctx.flow, client, interaction);
    if (dcc_event_type(event) != DCC_EVENT_AUTOCOMPLETE) {
        (void)dcc_app_auto_defer_start(&ctx);
    }
    status = dcc_app_run_middlewares(&ctx, app_middlewares, app_middleware_count);
    if (status == DCC_OK) {
        status = dcc_app_run_middlewares(&ctx, route_middlewares, route_middleware_count);
    }
    if (status != DCC_OK) {
        (void)dcc_app_report_handler_error(
            app,
            &ctx,
            status,
            dcc_app_interaction_operation(event, interaction),
            dcc_status_string(status)
        );
        dcc_app_auto_defer_finish(&ctx);
        free(app_middlewares);
        dcc_app_callback_frame_leave(&callback_frame);
        return status;
    }
    handler(&ctx, user_data);
    dcc_app_auto_defer_finish(&ctx);
    free(app_middlewares);
    dcc_app_callback_frame_leave(&callback_frame);
    return DCC_OK;
}

dcc_status_t dcc_app_dispatch_canonical_handler(
    dcc_app_t *app,
    dcc_client_t *client,
    const dcc_event_t *event,
    const dcc_interaction_t *interaction,
    const dcc_component_session_result_t *component_session,
    const dcc_app_middleware_t *route_middlewares,
    size_t route_middleware_count,
    dcc_app_handler_fn handler,
    void *user_data,
    void *context_user_data
) {
    if (app == NULL || client == NULL || event == NULL || interaction == NULL || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_ctx_t ctx = {
        .app = app,
        .client = client,
        .event = event,
        .interaction = interaction,
        .component_session = component_session,
        .user_data = context_user_data,
    };
    dcc_app_callback_frame_t callback_frame;
    dcc_app_callback_frame_enter(&callback_frame, app, NULL);
    dcc_app_middleware_t *app_middlewares = NULL;
    size_t app_middleware_count = 0U;
    dcc_status_t status = dcc_app_snapshot_middlewares(
        app,
        &app_middlewares,
        &app_middleware_count
    );
    if (status != DCC_OK) {
        dcc_app_callback_frame_leave(&callback_frame);
        return status;
    }
    dcc_flow_init(&ctx.flow, client, interaction);
    if (dcc_event_type(event) != DCC_EVENT_AUTOCOMPLETE) {
        (void)dcc_app_auto_defer_start(&ctx);
    }
    status = dcc_app_run_middlewares(&ctx, app_middlewares, app_middleware_count);
    if (status == DCC_OK) {
        status = dcc_app_run_middlewares(&ctx, route_middlewares, route_middleware_count);
    }
    if (status == DCC_OK) {
        status = handler(&ctx, user_data);
    }
    if (status != DCC_OK) {
        (void)dcc_app_report_handler_error(
            app,
            &ctx,
            status,
            dcc_app_interaction_operation(event, interaction),
            dcc_status_string(status)
        );
    }
    dcc_app_auto_defer_finish(&ctx);
    free(app_middlewares);
    dcc_app_callback_frame_leave(&callback_frame);
    return status;
}

void dcc_app_dispatch_event(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    dcc_app_t *app = (dcc_app_t *)user_data;
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (app == NULL || interaction == NULL) {
        return;
    }

    dcc_event_type_t type = dcc_event_type(event);
    const char *key = dcc_app_event_uses_interaction_name(type) ? interaction->name : interaction->custom_id;
    dcc_app_route_t *route = NULL;
    dcc_app_route_t route_snapshot;
    dcc_app_middleware_t *route_middlewares = NULL;
    uint8_t listener_acquired = 0U;
    char subcommand_path[128];
    char subcommand_key[256];
    dcc_app_listener_lock(app);
    if ((type == DCC_EVENT_SLASH_COMMAND || type == DCC_EVENT_AUTOCOMPLETE) &&
        dcc_ctx_subcommand_path(
            &(dcc_ctx_t){ .interaction = interaction },
            subcommand_path,
            sizeof(subcommand_path)
        ) == DCC_OK &&
        dcc_app_subcommand_key(
            interaction->name,
            subcommand_path,
            subcommand_key,
            sizeof(subcommand_key)
        ) == DCC_OK) {
        route = dcc_app_find_route(app, type, subcommand_key);
    }
    if (route == NULL) {
        route = dcc_app_find_route(app, type, key);
    }
    if (route == NULL || (route->handler == NULL && route->legacy_handler == NULL)) {
        dcc_app_listener_unlock(app);
        return;
    }
    if (route->listener_state != NULL) {
        listener_acquired = dcc_app_listener_acquire(route->listener_state);
        if (!listener_acquired) {
            dcc_app_listener_unlock(app);
            return;
        }
    }
    if (route->middleware_count > SIZE_MAX / sizeof(*route->middlewares)) {
        void *listener_state = route->listener_state;
        dcc_app_listener_unlock(app);
        if (listener_acquired) {
            dcc_app_listener_release(listener_state);
        }
        return;
    }
    if (route->middleware_count != 0U) {
        route_middlewares = (dcc_app_middleware_t *)malloc(
            route->middleware_count * sizeof(*route_middlewares)
        );
        if (route_middlewares == NULL) {
            void *listener_state = route->listener_state;
            dcc_app_listener_unlock(app);
            if (listener_acquired) {
                dcc_app_listener_release(listener_state);
            }
            return;
        }
        memcpy(
            route_middlewares,
            route->middlewares,
            route->middleware_count * sizeof(*route_middlewares)
        );
    }
    route_snapshot = *route;
    route_snapshot.middlewares = route_middlewares;
    route_snapshot.middleware_cap = route_snapshot.middleware_count;
    dcc_app_listener_unlock(app);
    if (route_snapshot.handler != NULL) {
        (void)dcc_app_dispatch_canonical_handler(
            app, client, event, interaction, NULL, route_snapshot.middlewares,
            route_snapshot.middleware_count, route_snapshot.handler, route_snapshot.user_data,
            route_snapshot.context_user_data
        );
    } else {
        (void)dcc_app_dispatch_handler(
            app, client, event, interaction, NULL, route_snapshot.middlewares,
            route_snapshot.middleware_count, route_snapshot.legacy_handler,
            route_snapshot.user_data
        );
    }
    free(route_middlewares);
    if (listener_acquired) {
        dcc_app_listener_release(route_snapshot.listener_state);
    }
}

dcc_status_t dcc_app_slash_builder(
    dcc_app_t *app,
    const dcc_application_command_builder_t *command,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    return dcc_app_slash_builder_route(app, command, handler, user_data, NULL);
}

dcc_status_t dcc_app_slash_builder_route(
    dcc_app_t *app,
    const dcc_application_command_builder_t *command,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    dcc_builder_abi_view_t view;
    if (app == NULL || command == NULL || handler == NULL ||
        dcc_application_command_builder_abi_validate(command, &view) != DCC_OK ||
        !dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME) ||
        command->name == NULL || command->name[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_command_registry_add_builder(&app->registry, command);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_app_add_route(app, DCC_EVENT_SLASH_COMMAND, command->name, 0U, handler, user_data, out_route);
}

dcc_status_t dcc_app_slash(
    dcc_app_t *app,
    const char *name,
    const char *description,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    return dcc_app_slash_route(app, name, description, handler, user_data, NULL);
}

dcc_status_t dcc_app_slash_route(
    dcc_app_t *app,
    const char *name,
    const char *description,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    if (app == NULL || name == NULL || name[0] == '\0' || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_application_command_builder_t command;
    dcc_application_command_builder_init(&command);
    dcc_status_t status = dcc_application_command_builder_set_name(&command, name);
    if (status == DCC_OK) {
        status = dcc_application_command_builder_set_description(&command, description != NULL ? description : "");
    }
    if (status == DCC_OK) {
        status = dcc_application_command_builder_set_type(&command, DCC_APPLICATION_COMMAND_CHAT_INPUT);
    }
    return status == DCC_OK ? dcc_app_slash_builder_route(app, &command, handler, user_data, out_route) : status;
}

static dcc_status_t dcc_app_context_menu_route(
    dcc_app_t *app,
    const char *name,
    dcc_application_command_type_t command_type,
    dcc_event_type_t event_type,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    if (app == NULL || name == NULL || name[0] == '\0' || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_application_command_builder_t command;
    dcc_application_command_builder_init(&command);
    dcc_status_t status = dcc_application_command_builder_set_name(&command, name);
    if (status == DCC_OK) {
        status = dcc_application_command_builder_set_type(&command, command_type);
    }
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_command_registry_add_builder(&app->registry, &command);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_app_add_route(app, event_type, command.name, 0U, handler, user_data, out_route);
}

dcc_status_t dcc_app_user_context_menu(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    return dcc_app_user_context_menu_route(app, name, handler, user_data, NULL);
}

dcc_status_t dcc_app_user_context_menu_route(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_context_menu_route(
        app,
        name,
        DCC_APPLICATION_COMMAND_USER,
        DCC_EVENT_USER_CONTEXT_MENU,
        handler,
        user_data,
        out_route
    );
}

dcc_status_t dcc_app_message_context_menu(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    return dcc_app_message_context_menu_route(app, name, handler, user_data, NULL);
}

dcc_status_t dcc_app_message_context_menu_route(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_context_menu_route(
        app,
        name,
        DCC_APPLICATION_COMMAND_MESSAGE,
        DCC_EVENT_MESSAGE_CONTEXT_MENU,
        handler,
        user_data,
        out_route
    );
}

dcc_status_t dcc_app_autocomplete(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    return dcc_app_autocomplete_route(app, name, handler, user_data, NULL);
}

dcc_status_t dcc_app_autocomplete_route(
    dcc_app_t *app,
    const char *name,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_add_route(app, DCC_EVENT_AUTOCOMPLETE, name, 0U, handler, user_data, out_route);
}

dcc_status_t dcc_app_button(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    return dcc_app_button_route(app, custom_id, handler, user_data, NULL);
}

dcc_status_t dcc_app_button_route(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_add_route(app, DCC_EVENT_BUTTON_CLICK, custom_id, 0U, handler, user_data, out_route);
}

dcc_status_t dcc_app_button_prefix(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    return dcc_app_button_prefix_route(app, custom_id_prefix, handler, user_data, NULL);
}

dcc_status_t dcc_app_button_prefix_route(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_add_route(app, DCC_EVENT_BUTTON_CLICK, custom_id_prefix, 1U, handler, user_data, out_route);
}

dcc_status_t dcc_app_select(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    return dcc_app_select_route(app, custom_id, handler, user_data, NULL);
}

dcc_status_t dcc_app_select_route(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_add_route(app, DCC_EVENT_SELECT_CLICK, custom_id, 0U, handler, user_data, out_route);
}

dcc_status_t dcc_app_select_prefix(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    return dcc_app_select_prefix_route(app, custom_id_prefix, handler, user_data, NULL);
}

dcc_status_t dcc_app_select_prefix_route(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_add_route(app, DCC_EVENT_SELECT_CLICK, custom_id_prefix, 1U, handler, user_data, out_route);
}

dcc_status_t dcc_app_modal(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    return dcc_app_modal_route(app, custom_id, handler, user_data, NULL);
}

dcc_status_t dcc_app_modal_route(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_add_route(app, DCC_EVENT_FORM_SUBMIT, custom_id, 0U, handler, user_data, out_route);
}

dcc_status_t dcc_app_modal_prefix(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data
) {
    return dcc_app_modal_prefix_route(app, custom_id_prefix, handler, user_data, NULL);
}

dcc_status_t dcc_app_modal_prefix_route(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_legacy_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    return dcc_app_add_route(app, DCC_EVENT_FORM_SUBMIT, custom_id_prefix, 1U, handler, user_data, out_route);
}
