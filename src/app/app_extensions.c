#include "internal/app/dcc_app_internal.h"

#include <stddef.h>

#define DCC_APP_EXTENSION_MIN_SIZE \
    (offsetof(dcc_app_extension_t, task_count) + sizeof(((dcc_app_extension_t *)0)->task_count))
#define DCC_APP_EXTENSION_MIDDLEWARE_MIN_SIZE \
    (offsetof(dcc_app_extension_middleware_t, user_data) + sizeof(((dcc_app_extension_middleware_t *)0)->user_data))
#define DCC_APP_EXTENSION_SLASH_MIN_SIZE \
    (offsetof(dcc_app_extension_slash_command_t, out_route) + sizeof(((dcc_app_extension_slash_command_t *)0)->out_route))
#define DCC_APP_EXTENSION_SUBCOMMAND_MIN_SIZE \
    (offsetof(dcc_app_extension_subcommand_t, out_route) + sizeof(((dcc_app_extension_subcommand_t *)0)->out_route))
#define DCC_APP_EXTENSION_AUTOCOMPLETE_MIN_SIZE \
    (offsetof(dcc_app_extension_autocomplete_t, out_route) + sizeof(((dcc_app_extension_autocomplete_t *)0)->out_route))
#define DCC_APP_EXTENSION_CONTEXT_MENU_MIN_SIZE \
    (offsetof(dcc_app_extension_context_menu_t, out_route) + \
     sizeof(((dcc_app_extension_context_menu_t *)0)->out_route))
#define DCC_APP_EXTENSION_COMPONENT_MIN_SIZE \
    (offsetof(dcc_app_extension_component_route_t, out_route) + sizeof(((dcc_app_extension_component_route_t *)0)->out_route))
#define DCC_APP_EXTENSION_EVENT_MIN_SIZE \
    (offsetof(dcc_app_extension_event_t, user_data) + sizeof(((dcc_app_extension_event_t *)0)->user_data))
#define DCC_APP_EXTENSION_MESSAGE_COMMAND_MIN_SIZE \
    (offsetof(dcc_app_extension_message_command_t, user_data) + \
     sizeof(((dcc_app_extension_message_command_t *)0)->user_data))
#define DCC_APP_EXTENSION_TASK_MIN_SIZE \
    (offsetof(dcc_app_extension_task_t, user_data) + sizeof(((dcc_app_extension_task_t *)0)->user_data))

static uint8_t dcc_app_extension_has_field(
    const dcc_app_extension_t *extension,
    size_t offset,
    size_t field_size
) {
    return extension != NULL &&
           extension->size >= offset &&
           field_size <= extension->size - offset;
}

static uint8_t dcc_app_sized_has_field(size_t size, size_t offset, size_t field_size) {
    return size >= offset && field_size <= size - offset;
}

static dcc_status_t dcc_app_extension_check_array(const void *items, size_t count) {
    return items != NULL || count == 0U ? DCC_OK : DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_app_extension_register_middleware(
    dcc_app_t *app,
    const dcc_app_extension_middleware_t *middleware
) {
    if (middleware == NULL || middleware->size < DCC_APP_EXTENSION_MIDDLEWARE_MIN_SIZE ||
        middleware->middleware == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_use(app, middleware->middleware, middleware->user_data);
}

static dcc_status_t dcc_app_extension_register_slash(
    dcc_app_t *app,
    const dcc_app_extension_slash_command_t *command
) {
    if (command == NULL || command->size < DCC_APP_EXTENSION_SLASH_MIN_SIZE ||
        command->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_app_route_id_t *out_route = command->out_route != NULL ? command->out_route : &route_id;
    dcc_status_t status;
    if (command->builder != NULL) {
        status = dcc_app_slash_builder_route(
            app,
            command->builder,
            command->handler,
            command->user_data,
            out_route
        );
    } else {
        status = dcc_app_slash_route(
            app,
            command->name,
            command->description,
            command->handler,
            command->user_data,
            out_route
        );
    }
    if (status != DCC_OK) {
        return status;
    }
    route_id = command->out_route != NULL ? *command->out_route : route_id;
    return dcc_app_sized_has_field(
        command->size,
        offsetof(dcc_app_extension_slash_command_t, policy),
        sizeof(command->policy)
    ) ? dcc_app_route_apply_policy_internal(app, route_id, &command->policy) : DCC_OK;
}

static dcc_status_t dcc_app_extension_register_subcommand(
    dcc_app_t *app,
    const dcc_app_extension_subcommand_t *subcommand
) {
    if (subcommand == NULL || subcommand->size < DCC_APP_EXTENSION_SUBCOMMAND_MIN_SIZE ||
        subcommand->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_app_route_id_t *out_route = subcommand->out_route != NULL ? subcommand->out_route : &route_id;
    dcc_status_t status = dcc_app_subcommand_route(
        app,
        subcommand->command_name,
        subcommand->subcommand_path,
        subcommand->handler,
        subcommand->user_data,
        out_route
    );
    if (status != DCC_OK) {
        return status;
    }
    route_id = subcommand->out_route != NULL ? *subcommand->out_route : route_id;
    return dcc_app_sized_has_field(
        subcommand->size,
        offsetof(dcc_app_extension_subcommand_t, policy),
        sizeof(subcommand->policy)
    ) ? dcc_app_route_apply_policy_internal(app, route_id, &subcommand->policy) : DCC_OK;
}

static dcc_status_t dcc_app_extension_register_autocomplete(
    dcc_app_t *app,
    const dcc_app_extension_autocomplete_t *autocomplete
) {
    if (autocomplete == NULL || autocomplete->size < DCC_APP_EXTENSION_AUTOCOMPLETE_MIN_SIZE ||
        autocomplete->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_app_route_id_t *out_route = autocomplete->out_route != NULL ? autocomplete->out_route : &route_id;
    dcc_status_t status = dcc_app_autocomplete_route(
        app,
        autocomplete->name,
        autocomplete->handler,
        autocomplete->user_data,
        out_route
    );
    if (status != DCC_OK) {
        return status;
    }
    route_id = autocomplete->out_route != NULL ? *autocomplete->out_route : route_id;
    return dcc_app_sized_has_field(
        autocomplete->size,
        offsetof(dcc_app_extension_autocomplete_t, policy),
        sizeof(autocomplete->policy)
    ) ? dcc_app_route_apply_policy_internal(app, route_id, &autocomplete->policy) : DCC_OK;
}

static dcc_status_t dcc_app_extension_register_context_menu(
    dcc_app_t *app,
    const dcc_app_extension_context_menu_t *menu
) {
    if (menu == NULL || menu->size < DCC_APP_EXTENSION_CONTEXT_MENU_MIN_SIZE ||
        menu->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_app_route_id_t *out_route = menu->out_route != NULL ? menu->out_route : &route_id;
    dcc_status_t status = DCC_ERR_INVALID_ARG;

    switch (menu->type) {
        case DCC_APP_EXTENSION_USER_CONTEXT_MENU:
            status = dcc_app_user_context_menu_route(
                app,
                menu->name,
                menu->handler,
                menu->user_data,
                out_route
            );
            break;
        case DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU:
            status = dcc_app_message_context_menu_route(
                app,
                menu->name,
                menu->handler,
                menu->user_data,
                out_route
            );
            break;
    }

    if (status != DCC_OK) {
        return status;
    }
    route_id = menu->out_route != NULL ? *menu->out_route : route_id;
    return dcc_app_sized_has_field(
        menu->size,
        offsetof(dcc_app_extension_context_menu_t, policy),
        sizeof(menu->policy)
    ) ? dcc_app_route_apply_policy_internal(app, route_id, &menu->policy) : DCC_OK;
}

static dcc_status_t dcc_app_extension_register_component(
    dcc_app_t *app,
    const dcc_app_extension_component_route_t *route
) {
    if (route == NULL || route->size < DCC_APP_EXTENSION_COMPONENT_MIN_SIZE ||
        route->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_app_route_id_t *out_route = route->out_route != NULL ? route->out_route : &route_id;
    dcc_status_t status = DCC_ERR_INVALID_ARG;

    switch (route->type) {
        case DCC_APP_EXTENSION_BUTTON:
            status = dcc_app_button_route(
                app,
                route->custom_id,
                route->handler,
                route->user_data,
                out_route
            );
            break;
        case DCC_APP_EXTENSION_BUTTON_PREFIX:
            status = dcc_app_button_prefix_route(
                app,
                route->custom_id,
                route->handler,
                route->user_data,
                out_route
            );
            break;
        case DCC_APP_EXTENSION_SELECT:
            status = dcc_app_select_route(
                app,
                route->custom_id,
                route->handler,
                route->user_data,
                out_route
            );
            break;
        case DCC_APP_EXTENSION_SELECT_PREFIX:
            status = dcc_app_select_prefix_route(
                app,
                route->custom_id,
                route->handler,
                route->user_data,
                out_route
            );
            break;
        case DCC_APP_EXTENSION_MODAL:
            status = dcc_app_modal_route(
                app,
                route->custom_id,
                route->handler,
                route->user_data,
                out_route
            );
            break;
        case DCC_APP_EXTENSION_MODAL_PREFIX:
            status = dcc_app_modal_prefix_route(
                app,
                route->custom_id,
                route->handler,
                route->user_data,
                out_route
            );
            break;
    }

    if (status != DCC_OK) {
        return status;
    }
    route_id = route->out_route != NULL ? *route->out_route : route_id;
    return dcc_app_sized_has_field(
        route->size,
        offsetof(dcc_app_extension_component_route_t, policy),
        sizeof(route->policy)
    ) ? dcc_app_route_apply_policy_internal(app, route_id, &route->policy) : DCC_OK;
}

static dcc_status_t dcc_app_extension_register_event(
    dcc_app_t *app,
    const dcc_app_extension_event_t *event
) {
    if (event == NULL || event->size < DCC_APP_EXTENSION_EVENT_MIN_SIZE) {
        return DCC_ERR_INVALID_ARG;
    }

    switch (event->type) {
        case DCC_APP_EXTENSION_EVENT:
            return dcc_app_on(app, event->event_type, event->event_handler, event->user_data);
        case DCC_APP_EXTENSION_READY:
            return dcc_app_sized_has_field(event->size, offsetof(dcc_app_extension_event_t, once), sizeof(event->once)) &&
                    event->once
                ? dcc_app_on_ready_once(app, event->ready_handler, event->user_data)
                : dcc_app_on_ready(app, event->ready_handler, event->user_data);
        case DCC_APP_EXTENSION_MESSAGE_CREATE:
            return dcc_app_on_message_create(app, event->message_handler, event->user_data);
        case DCC_APP_EXTENSION_MESSAGE_UPDATE:
            return dcc_app_on_message_update(app, event->message_handler, event->user_data);
        case DCC_APP_EXTENSION_MESSAGE_DELETE:
            return dcc_app_on_message_delete(app, event->message_handler, event->user_data);
    }

    return DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_app_extension_register_message_command(
    dcc_app_t *app,
    const dcc_app_extension_message_command_t *command
) {
    if (command == NULL || command->size < DCC_APP_EXTENSION_MESSAGE_COMMAND_MIN_SIZE ||
        command->handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    return dcc_app_on_message_command(
        app,
        command->prefix,
        command->name,
        command->handler,
        command->user_data
    );
}

static dcc_status_t dcc_app_extension_register_task(
    dcc_app_t *app,
    const dcc_app_extension_task_t *task
) {
    if (task == NULL || task->size < DCC_APP_EXTENSION_TASK_MIN_SIZE ||
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

dcc_status_t dcc_app_extension_register(dcc_app_t *app, const dcc_app_extension_t *extension) {
    if (app == NULL || extension == NULL || extension->size < DCC_APP_EXTENSION_MIN_SIZE) {
        return DCC_ERR_INVALID_ARG;
    }
    (void)extension->name;

    const dcc_app_typed_slash_command_t *typed_slash_commands = NULL;
    size_t typed_slash_command_count = 0U;
    const dcc_app_typed_subcommand_t *typed_subcommands = NULL;
    size_t typed_subcommand_count = 0U;
    const dcc_app_typed_autocomplete_t *typed_autocompletes = NULL;
    size_t typed_autocomplete_count = 0U;
    const dcc_app_typed_modal_t *typed_modals = NULL;
    size_t typed_modal_count = 0U;
    const dcc_app_typed_component_t *typed_buttons = NULL;
    size_t typed_button_count = 0U;
    const dcc_app_typed_component_t *typed_selects = NULL;
    size_t typed_select_count = 0U;
    const dcc_app_extension_message_command_t *message_commands = NULL;
    size_t message_command_count = 0U;
    const dcc_app_extension_context_menu_t *context_menus = NULL;
    size_t context_menu_count = 0U;

    if (dcc_app_extension_has_field(
            extension,
            offsetof(dcc_app_extension_t, typed_slash_command_count),
            sizeof(extension->typed_slash_command_count)
        )) {
        typed_slash_commands = extension->typed_slash_commands;
        typed_slash_command_count = extension->typed_slash_command_count;
    }
    if (dcc_app_extension_has_field(
            extension,
            offsetof(dcc_app_extension_t, typed_modal_count),
            sizeof(extension->typed_modal_count)
        )) {
        typed_modals = extension->typed_modals;
        typed_modal_count = extension->typed_modal_count;
    }
    if (dcc_app_extension_has_field(
            extension,
            offsetof(dcc_app_extension_t, typed_button_count),
            sizeof(extension->typed_button_count)
        )) {
        typed_buttons = extension->typed_buttons;
        typed_button_count = extension->typed_button_count;
    }
    if (dcc_app_extension_has_field(
            extension,
            offsetof(dcc_app_extension_t, typed_select_count),
            sizeof(extension->typed_select_count)
        )) {
        typed_selects = extension->typed_selects;
        typed_select_count = extension->typed_select_count;
    }
    if (dcc_app_extension_has_field(
            extension,
            offsetof(dcc_app_extension_t, message_command_count),
            sizeof(extension->message_command_count)
        )) {
        message_commands = extension->message_commands;
        message_command_count = extension->message_command_count;
    }
    if (dcc_app_extension_has_field(
            extension,
            offsetof(dcc_app_extension_t, typed_subcommand_count),
            sizeof(extension->typed_subcommand_count)
        )) {
        typed_subcommands = extension->typed_subcommands;
        typed_subcommand_count = extension->typed_subcommand_count;
    }
    if (dcc_app_extension_has_field(
            extension,
            offsetof(dcc_app_extension_t, typed_autocomplete_count),
            sizeof(extension->typed_autocomplete_count)
        )) {
        typed_autocompletes = extension->typed_autocompletes;
        typed_autocomplete_count = extension->typed_autocomplete_count;
    }
    if (dcc_app_extension_has_field(
            extension,
            offsetof(dcc_app_extension_t, context_menu_count),
            sizeof(extension->context_menu_count)
        )) {
        context_menus = extension->context_menus;
        context_menu_count = extension->context_menu_count;
    }

    dcc_status_t status = dcc_app_extension_check_array(extension->middlewares, extension->middleware_count);
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(extension->slash_commands, extension->slash_command_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(extension->subcommands, extension->subcommand_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(extension->autocompletes, extension->autocomplete_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(extension->component_routes, extension->component_route_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(extension->views, extension->view_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(extension->events, extension->event_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(extension->tasks, extension->task_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(message_commands, message_command_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(typed_slash_commands, typed_slash_command_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(typed_subcommands, typed_subcommand_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(typed_autocompletes, typed_autocomplete_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(context_menus, context_menu_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(typed_modals, typed_modal_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(typed_buttons, typed_button_count);
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_check_array(typed_selects, typed_select_count);
    }
    if (status != DCC_OK) {
        return status;
    }

    for (size_t i = 0; i < extension->middleware_count; ++i) {
        status = dcc_app_extension_register_middleware(app, &extension->middlewares[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < extension->slash_command_count; ++i) {
        status = dcc_app_extension_register_slash(app, &extension->slash_commands[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < extension->subcommand_count; ++i) {
        status = dcc_app_extension_register_subcommand(app, &extension->subcommands[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < extension->autocomplete_count; ++i) {
        status = dcc_app_extension_register_autocomplete(app, &extension->autocompletes[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < context_menu_count; ++i) {
        status = dcc_app_extension_register_context_menu(app, &context_menus[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < extension->component_route_count; ++i) {
        status = dcc_app_extension_register_component(app, &extension->component_routes[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < typed_slash_command_count; ++i) {
        status = dcc_app_slash_typed(app, &typed_slash_commands[i]);
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
    for (size_t i = 0; i < typed_modal_count; ++i) {
        status = dcc_app_modal_typed(app, &typed_modals[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < typed_button_count; ++i) {
        status = dcc_app_button_typed(app, &typed_buttons[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < typed_select_count; ++i) {
        status = dcc_app_select_typed(app, &typed_selects[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < extension->view_count; ++i) {
        status = dcc_app_view_register(app, &extension->views[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < extension->event_count; ++i) {
        status = dcc_app_extension_register_event(app, &extension->events[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < message_command_count; ++i) {
        status = dcc_app_extension_register_message_command(app, &message_commands[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    for (size_t i = 0; i < extension->task_count; ++i) {
        status = dcc_app_extension_register_task(app, &extension->tasks[i]);
        if (status != DCC_OK) {
            return status;
        }
    }

    return DCC_OK;
}
