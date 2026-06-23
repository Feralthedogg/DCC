#include "internal/app/dcc_app_internal.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define DCC_APP_DEFINITION_MIN_SIZE \
    (offsetof(dcc_app_definition_t, use_default_error_responses) + \
     sizeof(((dcc_app_definition_t *)0)->use_default_error_responses))
#define DCC_APP_DEFINITION_MIDDLEWARE_MIN_SIZE \
    (offsetof(dcc_app_extension_middleware_t, user_data) + \
     sizeof(((dcc_app_extension_middleware_t *)0)->user_data))
#define DCC_APP_DEFINITION_TASK_MIN_SIZE \
    (offsetof(dcc_app_extension_task_t, user_data) + \
     sizeof(((dcc_app_extension_task_t *)0)->user_data))
#define DCC_APP_DEFINITION_COMPONENT_SESSION_MIN_SIZE \
    (offsetof(dcc_app_component_session_route_t, user_data) + \
     sizeof(((dcc_app_component_session_route_t *)0)->user_data))
#define DCC_APP_DEFINITION_LISTENER_MIN_SIZE \
    (offsetof(dcc_app_listener_t, kind) + sizeof(((dcc_app_listener_t *)0)->kind))

static uint8_t dcc_app_definition_has_field(
    const dcc_app_definition_t *definition,
    size_t offset,
    size_t field_size
) {
    return definition != NULL &&
           definition->size >= offset &&
           field_size <= definition->size - offset;
}

static dcc_status_t dcc_app_definition_check_array(const void *items, size_t count) {
    return items != NULL || count == 0U ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_app_validate_env_requirements(
    const dcc_app_env_requirement_t *requirements,
    size_t count
) {
    if (requirements == NULL) {
        return count == 0U ? DCC_OK : DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = DCC_OK;
    for (size_t i = 0; i < count; ++i) {
        const dcc_app_env_requirement_t *requirement = &requirements[i];
        if (requirement->size < offsetof(dcc_app_env_requirement_t, name) +
                sizeof(requirement->name) ||
            requirement->name == NULL ||
            requirement->name[0] == '\0') {
            return DCC_ERR_INVALID_ARG;
        }
        const char *value = getenv(requirement->name);
        if (value == NULL || value[0] == '\0') {
            if (requirement->description != NULL && requirement->description[0] != '\0') {
                fprintf(
                    stderr,
                    "[dcc] missing required env: %s (%s)\n",
                    requirement->name,
                    requirement->description
                );
            } else {
                fprintf(stderr, "[dcc] missing required env: %s\n", requirement->name);
            }
            status = DCC_ERR_NOT_FOUND;
        }
    }
    return status;
}

dcc_status_t dcc_app_validate_definition_env(const dcc_app_definition_t *definition) {
    if (definition == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, required_env_count),
            sizeof(definition->required_env_count)
        )) {
        return DCC_OK;
    }
    return dcc_app_validate_env_requirements(
        definition->required_env,
        definition->required_env_count
    );
}

static dcc_status_t dcc_app_definition_apply_middleware(
    dcc_app_t *app,
    const dcc_app_extension_middleware_t *middleware
) {
    if (middleware == NULL ||
        middleware->size < DCC_APP_DEFINITION_MIDDLEWARE_MIN_SIZE ||
        middleware->middleware == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_use(app, middleware->middleware, middleware->user_data);
}

static dcc_status_t dcc_app_definition_apply_task(
    dcc_app_t *app,
    const dcc_app_extension_task_t *task
) {
    if (task == NULL || task->size < DCC_APP_DEFINITION_TASK_MIN_SIZE ||
        task->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    switch (task->type) {
        case DCC_APP_EXTENSION_TASK_EVERY_MS:
            return dcc_app_every_ms(app, task->interval, task->handler, task->user_data);
        case DCC_APP_EXTENSION_TASK_EVERY_SECONDS:
            return dcc_app_every_seconds(app, task->interval, task->handler, task->user_data);
        case DCC_APP_EXTENSION_TASK_EVERY_MINUTES:
            return dcc_app_every_minutes(app, task->interval, task->handler, task->user_data);
        case DCC_APP_EXTENSION_TASK_EVERY_HOURS:
            return dcc_app_every_hours(app, task->interval, task->handler, task->user_data);
        case DCC_APP_EXTENSION_TASK_EVERY_KST:
            return dcc_app_every_kst(app, task->hhmm, task->handler, task->user_data);
        case DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST:
            return dcc_app_every_day_at_kst(app, task->hour, task->minute, task->handler, task->user_data);
    }

    return DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_app_definition_apply_listener(
    dcc_app_t *app,
    const dcc_app_listener_t *listener
) {
    if (app == NULL || listener == NULL || listener->size < DCC_APP_DEFINITION_LISTENER_MIN_SIZE) {
        return DCC_ERR_INVALID_ARG;
    }

    switch (listener->kind) {
        case DCC_APP_LISTENER_SLASH: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .slash_commands = &listener->slash,
                .slash_command_count = 1U
            };
            return dcc_app_extension_register(app, &extension);
        }
        case DCC_APP_LISTENER_SUBCOMMAND: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .subcommands = &listener->subcommand,
                .subcommand_count = 1U
            };
            return dcc_app_extension_register(app, &extension);
        }
        case DCC_APP_LISTENER_AUTOCOMPLETE: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .autocompletes = &listener->autocomplete,
                .autocomplete_count = 1U
            };
            return dcc_app_extension_register(app, &extension);
        }
        case DCC_APP_LISTENER_CONTEXT_MENU: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .context_menus = &listener->context_menu,
                .context_menu_count = 1U
            };
            return dcc_app_extension_register(app, &extension);
        }
        case DCC_APP_LISTENER_COMPONENT: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .component_routes = &listener->component,
                .component_route_count = 1U
            };
            return dcc_app_extension_register(app, &extension);
        }
        case DCC_APP_LISTENER_EVENT: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .events = &listener->event,
                .event_count = 1U
            };
            return dcc_app_extension_register(app, &extension);
        }
        case DCC_APP_LISTENER_MESSAGE_COMMAND:
            return dcc_app_on_message_command(
                app,
                listener->message_command.prefix,
                listener->message_command.name,
                listener->message_command.handler,
                listener->message_command.user_data
            );
        case DCC_APP_LISTENER_TASK:
            return dcc_app_definition_apply_task(app, &listener->task);
        case DCC_APP_LISTENER_TYPED_SLASH:
            return dcc_app_slash_typed(app, &listener->typed_slash);
        case DCC_APP_LISTENER_TYPED_SUBCOMMAND:
            return dcc_app_subcommand_typed(app, &listener->typed_subcommand);
        case DCC_APP_LISTENER_TYPED_AUTOCOMPLETE:
            return dcc_app_autocomplete_typed(app, &listener->typed_autocomplete);
        case DCC_APP_LISTENER_TYPED_MODAL:
            return dcc_app_modal_typed(app, &listener->typed_modal);
        case DCC_APP_LISTENER_TYPED_BUTTON:
            return dcc_app_button_typed(app, &listener->typed_component);
        case DCC_APP_LISTENER_TYPED_SELECT:
            return dcc_app_select_typed(app, &listener->typed_component);
        case DCC_APP_LISTENER_MODAL_FLOW:
            return dcc_app_modal_flow_register(app, &listener->modal_flow);
        case DCC_APP_LISTENER_COMMAND_SCHEMA:
            return dcc_command_registry_add_builder(&app->registry, &listener->command_schema);
        default:
            return DCC_ERR_INVALID_ARG;
    }
}

static dcc_status_t dcc_app_definition_apply_component_session(
    dcc_app_t *app,
    const dcc_app_component_session_route_t *route
) {
    if (app == NULL || route == NULL ||
        route->size < DCC_APP_DEFINITION_COMPONENT_SESSION_MIN_SIZE ||
        route->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if ((route->session == NULL) == (route->store == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    return route->store != NULL
        ? dcc_app_component_session_store(
            app,
            route->store,
            route->options,
            route->handler,
            route->user_data
        )
        : dcc_app_component_session(
            app,
            route->session,
            route->options,
            route->handler,
            route->user_data
        );
}

dcc_status_t dcc_app_apply(dcc_app_t *app, const dcc_app_definition_t *definition) {
    if (app == NULL || definition == NULL || definition->size < DCC_APP_DEFINITION_MIN_SIZE) {
        return DCC_ERR_INVALID_ARG;
    }
    (void)definition->name;

    dcc_status_t status = dcc_app_validate_definition_env(definition);
    if (status != DCC_OK) {
        return status;
    }

    const dcc_app_extension_middleware_t *middlewares = NULL;
    size_t middleware_count = 0U;
    const dcc_snowflake_t *owner_user_ids = NULL;
    size_t owner_user_id_count = 0U;
    const uint32_t *channel_types = NULL;
    size_t channel_type_count = 0U;
    const dcc_snowflake_t *required_role_ids = NULL;
    size_t required_role_id_count = 0U;
    const dcc_snowflake_t *any_role_ids = NULL;
    size_t any_role_id_count = 0U;
    const dcc_app_extension_message_command_t *message_commands = NULL;
    size_t message_command_count = 0U;
    const dcc_app_typed_subcommand_t *typed_subcommands = NULL;
    size_t typed_subcommand_count = 0U;
    const dcc_app_typed_autocomplete_t *typed_autocompletes = NULL;
    size_t typed_autocomplete_count = 0U;
    const dcc_app_check_t *checks = NULL;
    size_t check_count = 0U;
    dcc_app_cooldown_options_t cooldown = {0};
    const dcc_app_extension_task_t *tasks = NULL;
    size_t task_count = 0U;
    const dcc_app_view_t *views = NULL;
    size_t view_count = 0U;
    const dcc_app_component_session_route_t *component_sessions = NULL;
    size_t component_session_count = 0U;
    const dcc_app_extension_event_t *events = NULL;
    size_t event_count = 0U;
    const dcc_app_listener_t *listeners = NULL;
    size_t listener_count = 0U;
    dcc_app_error_fn error_handler = NULL;
    void *error_user_data = NULL;
    dcc_permission_t required_permissions = 0U;
    uint8_t guild_only = 0U;
    uint8_t dm_only = 0U;
    uint8_t nsfw_only = 0U;

    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, middleware_count),
            sizeof(definition->middleware_count)
        )) {
        middlewares = definition->middlewares;
        middleware_count = definition->middleware_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, owner_user_id_count),
            sizeof(definition->owner_user_id_count)
        )) {
        owner_user_ids = definition->owner_user_ids;
        owner_user_id_count = definition->owner_user_id_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, required_permissions),
            sizeof(definition->required_permissions)
        )) {
        required_permissions = definition->required_permissions;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, channel_type_count),
            sizeof(definition->channel_type_count)
        )) {
        channel_types = definition->channel_types;
        channel_type_count = definition->channel_type_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, required_role_id_count),
            sizeof(definition->required_role_id_count)
        )) {
        required_role_ids = definition->required_role_ids;
        required_role_id_count = definition->required_role_id_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, any_role_id_count),
            sizeof(definition->any_role_id_count)
        )) {
        any_role_ids = definition->any_role_ids;
        any_role_id_count = definition->any_role_id_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, message_command_count),
            sizeof(definition->message_command_count)
        )) {
        message_commands = definition->message_commands;
        message_command_count = definition->message_command_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, typed_subcommand_count),
            sizeof(definition->typed_subcommand_count)
        )) {
        typed_subcommands = definition->typed_subcommands;
        typed_subcommand_count = definition->typed_subcommand_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, typed_autocomplete_count),
            sizeof(definition->typed_autocomplete_count)
        )) {
        typed_autocompletes = definition->typed_autocompletes;
        typed_autocomplete_count = definition->typed_autocomplete_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, check_count),
            sizeof(definition->check_count)
        )) {
        checks = definition->checks;
        check_count = definition->check_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, cooldown),
            sizeof(definition->cooldown)
        )) {
        cooldown = definition->cooldown;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, task_count),
            sizeof(definition->task_count)
        )) {
        tasks = definition->tasks;
        task_count = definition->task_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, view_count),
            sizeof(definition->view_count)
        )) {
        views = definition->views;
        view_count = definition->view_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, component_session_count),
            sizeof(definition->component_session_count)
        )) {
        component_sessions = definition->component_sessions;
        component_session_count = definition->component_session_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, event_count),
            sizeof(definition->event_count)
        )) {
        events = definition->events;
        event_count = definition->event_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, error_handler),
            sizeof(definition->error_handler)
        )) {
        error_handler = definition->error_handler;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, error_user_data),
            sizeof(definition->error_user_data)
        )) {
        error_user_data = definition->error_user_data;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, listener_count),
            sizeof(definition->listener_count)
        )) {
        listeners = definition->listeners;
        listener_count = definition->listener_count;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, guild_only),
            sizeof(definition->guild_only)
        )) {
        guild_only = definition->guild_only != 0U;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, dm_only),
            sizeof(definition->dm_only)
        )) {
        dm_only = definition->dm_only != 0U;
    }
    if (dcc_app_definition_has_field(
            definition,
            offsetof(dcc_app_definition_t, nsfw_only),
            sizeof(definition->nsfw_only)
        )) {
        nsfw_only = definition->nsfw_only != 0U;
    }
    if ((guild_only && dm_only) || (dm_only && nsfw_only)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (error_handler == NULL && error_user_data != NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    status = dcc_app_definition_check_array(definition->modules, definition->module_count);
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(definition->extensions, definition->extension_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(
            definition->typed_slash_commands,
            definition->typed_slash_command_count
        );
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(typed_subcommands, typed_subcommand_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(typed_autocompletes, typed_autocomplete_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(definition->typed_modals, definition->typed_modal_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(definition->typed_buttons, definition->typed_button_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(definition->typed_selects, definition->typed_select_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(middlewares, middleware_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(owner_user_ids, owner_user_id_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(channel_types, channel_type_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(required_role_ids, required_role_id_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(any_role_ids, any_role_id_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(checks, check_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(message_commands, message_command_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(tasks, task_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(views, view_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(component_sessions, component_session_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(events, event_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_definition_check_array(listeners, listener_count);
    }
    if (status != DCC_OK) {
        return status;
    }

    if (definition->use_default_error_responses) {
        status = dcc_app_use_default_error_responses(app);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (error_handler != NULL) {
        status = dcc_app_on_error(app, error_handler, error_user_data);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (definition->auto_defer_after_ms != 0U) {
        status = definition->auto_defer_ephemeral
            ? dcc_app_auto_defer_ephemeral(app, definition->auto_defer_after_ms)
            : dcc_app_auto_defer(app, definition->auto_defer_after_ms);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < middleware_count; ++i) {
        status = dcc_app_definition_apply_middleware(app, &middlewares[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (guild_only) {
        status = dcc_app_use_guild_only(app);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (dm_only) {
        status = dcc_app_use_dm_only(app);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (nsfw_only) {
        status = dcc_app_use_nsfw_only(app);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (channel_type_count != 0U) {
        status = dcc_app_use_channel_types(app, channel_types, channel_type_count);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (owner_user_id_count != 0U) {
        status = dcc_app_use_user_allowlist(app, owner_user_ids, owner_user_id_count);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (required_permissions != 0U) {
        status = dcc_app_use_required_permissions(app, required_permissions);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (required_role_id_count != 0U) {
        status = dcc_app_use_required_roles(app, required_role_ids, required_role_id_count);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (any_role_id_count != 0U) {
        status = dcc_app_use_any_role(app, any_role_ids, any_role_id_count);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < check_count; ++i) {
        status = dcc_app_use_check(app, &checks[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (cooldown.limit != 0U && cooldown.window_ms != 0U) {
        status = dcc_app_use_cooldown(app, &cooldown);
        if (status != DCC_OK) {
            return status;
        }
    }

    for (size_t i = 0; i < definition->module_count; ++i) {
        status = dcc_app_module_register(app, &definition->modules[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < definition->extension_count; ++i) {
        status = dcc_app_extension_register(app, &definition->extensions[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (event_count != 0U) {
        dcc_app_extension_t event_extension = {
            .size = sizeof(event_extension),
            .name = definition->name,
            .events = events,
            .event_count = event_count
        };
        status = dcc_app_extension_register(app, &event_extension);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < message_command_count; ++i) {
        const dcc_app_extension_message_command_t *command = &message_commands[i];
        if (command->size < offsetof(dcc_app_extension_message_command_t, user_data) +
                sizeof(command->user_data)) {
            return DCC_ERR_INVALID_ARG;
        }
        status = dcc_app_on_message_command(
            app,
            command->prefix,
            command->name,
            command->handler,
            command->user_data
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < task_count; ++i) {
        status = dcc_app_definition_apply_task(app, &tasks[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < view_count; ++i) {
        status = dcc_app_view_register(app, &views[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < component_session_count; ++i) {
        status = dcc_app_definition_apply_component_session(app, &component_sessions[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < listener_count; ++i) {
        status = dcc_app_definition_apply_listener(app, &listeners[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < definition->typed_slash_command_count; ++i) {
        status = dcc_app_slash_typed(app, &definition->typed_slash_commands[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < typed_subcommand_count; ++i) {
        status = dcc_app_subcommand_typed(app, &typed_subcommands[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < typed_autocomplete_count; ++i) {
        status = dcc_app_autocomplete_typed(app, &typed_autocompletes[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < definition->typed_modal_count; ++i) {
        status = dcc_app_modal_typed(app, &definition->typed_modals[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < definition->typed_button_count; ++i) {
        status = dcc_app_button_typed(app, &definition->typed_buttons[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < definition->typed_select_count; ++i) {
        status = dcc_app_select_typed(app, &definition->typed_selects[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (definition->command_sync != NULL) {
        status = dcc_app_sync_commands_on_ready(app, definition->command_sync);
        if (status != DCC_OK) {
            return status;
        }
    }

    return DCC_OK;
}

dcc_status_t dcc_app_create_defined(
    const dcc_app_options_t *options,
    const dcc_app_definition_t *definition,
    dcc_app_t **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_validate_definition_env(definition);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_app_create(options, &app);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_app_apply(app, definition);
    if (status != DCC_OK) {
        dcc_app_destroy(app);
        return status;
    }

    *out = app;
    return DCC_OK;
}

dcc_status_t dcc_app_create_from_env_defined(
    const char *token_env,
    const dcc_app_definition_t *definition,
    dcc_app_t **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_status_t status = dcc_app_validate_definition_env(definition);
    if (status != DCC_OK) {
        return status;
    }

    dcc_app_options_t options;
    dcc_app_options_init(&options);
    status = dcc_app_options_from_env(&options, token_env);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_app_create_defined(&options, definition, out);
}

dcc_status_t dcc_app_run_defined(
    const dcc_app_options_t *options,
    const dcc_app_definition_t *definition
) {
    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create_defined(options, definition, &app);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_app_run(app);
    dcc_app_destroy(app);
    return status;
}

dcc_status_t dcc_app_run_from_env_defined(
    const char *token_env,
    const dcc_app_definition_t *definition
) {
    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create_from_env_defined(token_env, definition, &app);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_app_run(app);
    dcc_app_destroy(app);
    return status;
}

dcc_status_t dcc_app_run_dotenv_defined(
    const char *token_env,
    const dcc_app_definition_t *definition
) {
    dcc_status_t status = dcc_app_load_dotenv();
    if (status != DCC_OK && status != DCC_ERR_NOT_FOUND) {
        return status;
    }
    return dcc_app_run_from_env_defined(token_env, definition);
}
