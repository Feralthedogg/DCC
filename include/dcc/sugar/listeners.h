#ifndef DCC_SUGAR_LISTENERS_H
#define DCC_SUGAR_LISTENERS_H

#include <stddef.h>

#include <dcc/app.h>
#include <dcc/sugar/app_extension.h>
#include <dcc/sugar/app_module.h>
#include <dcc/sugar/commands.h>
#include <dcc/sugar/decorators.h>
#include <dcc/sugar/events.h>

typedef dcc_app_listener_kind_t dcc_sugar_listener_kind_t;
typedef dcc_app_listener_t dcc_sugar_listener_t;
typedef dcc_app_listener_list_t dcc_sugar_listener_list_t;

#define DCC_SUGAR_LISTENER_NONE DCC_APP_LISTENER_NONE
#define DCC_SUGAR_LISTENER_SLASH DCC_APP_LISTENER_SLASH
#define DCC_SUGAR_LISTENER_SUBCOMMAND DCC_APP_LISTENER_SUBCOMMAND
#define DCC_SUGAR_LISTENER_AUTOCOMPLETE DCC_APP_LISTENER_AUTOCOMPLETE
#define DCC_SUGAR_LISTENER_CONTEXT_MENU DCC_APP_LISTENER_CONTEXT_MENU
#define DCC_SUGAR_LISTENER_COMPONENT DCC_APP_LISTENER_COMPONENT
#define DCC_SUGAR_LISTENER_EVENT DCC_APP_LISTENER_EVENT
#define DCC_SUGAR_LISTENER_MESSAGE_COMMAND DCC_APP_LISTENER_MESSAGE_COMMAND
#define DCC_SUGAR_LISTENER_TASK DCC_APP_LISTENER_TASK
#define DCC_SUGAR_LISTENER_TYPED_SLASH DCC_APP_LISTENER_TYPED_SLASH
#define DCC_SUGAR_LISTENER_TYPED_SUBCOMMAND DCC_APP_LISTENER_TYPED_SUBCOMMAND
#define DCC_SUGAR_LISTENER_TYPED_AUTOCOMPLETE DCC_APP_LISTENER_TYPED_AUTOCOMPLETE
#define DCC_SUGAR_LISTENER_TYPED_MODAL DCC_APP_LISTENER_TYPED_MODAL
#define DCC_SUGAR_LISTENER_TYPED_BUTTON DCC_APP_LISTENER_TYPED_BUTTON
#define DCC_SUGAR_LISTENER_TYPED_SELECT DCC_APP_LISTENER_TYPED_SELECT
#define DCC_SUGAR_LISTENER_MODAL_FLOW DCC_APP_LISTENER_MODAL_FLOW
#define DCC_SUGAR_LISTENER_COMMAND_SCHEMA DCC_APP_LISTENER_COMMAND_SCHEMA

static inline dcc_status_t dcc_sugar_listener_register_extension(
    dcc_app_t *app,
    const dcc_app_extension_t *extension
) {
    return dcc_app_extension_register(app, extension);
}

static inline dcc_status_t dcc_sugar_listener_register_task(
    dcc_app_t *app,
    const dcc_app_extension_task_t *task
) {
    if (app == NULL || task == NULL || task->handler == NULL) {
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
        default:
            return DCC_ERR_INVALID_ARG;
    }
}

static inline dcc_status_t dcc_sugar_listener_register(
    dcc_app_t *app,
    const dcc_sugar_listener_t *listener
) {
    if (app == NULL || listener == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    switch (listener->kind) {
        case DCC_SUGAR_LISTENER_SLASH: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .slash_commands = &listener->slash,
                .slash_command_count = 1U
            };
            return dcc_sugar_listener_register_extension(app, &extension);
        }
        case DCC_SUGAR_LISTENER_SUBCOMMAND: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .subcommands = &listener->subcommand,
                .subcommand_count = 1U
            };
            return dcc_sugar_listener_register_extension(app, &extension);
        }
        case DCC_SUGAR_LISTENER_AUTOCOMPLETE: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .autocompletes = &listener->autocomplete,
                .autocomplete_count = 1U
            };
            return dcc_sugar_listener_register_extension(app, &extension);
        }
        case DCC_SUGAR_LISTENER_CONTEXT_MENU: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .context_menus = &listener->context_menu,
                .context_menu_count = 1U
            };
            return dcc_sugar_listener_register_extension(app, &extension);
        }
        case DCC_SUGAR_LISTENER_COMPONENT: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .component_routes = &listener->component,
                .component_route_count = 1U
            };
            return dcc_sugar_listener_register_extension(app, &extension);
        }
        case DCC_SUGAR_LISTENER_EVENT: {
            dcc_app_extension_t extension = {
                .size = sizeof(extension),
                .events = &listener->event,
                .event_count = 1U
            };
            return dcc_sugar_listener_register_extension(app, &extension);
        }
        case DCC_SUGAR_LISTENER_MESSAGE_COMMAND:
            return dcc_app_on_message_command(
                app,
                listener->message_command.prefix,
                listener->message_command.name,
                listener->message_command.handler,
                listener->message_command.user_data
            );
        case DCC_SUGAR_LISTENER_TASK:
            return dcc_sugar_listener_register_task(app, &listener->task);
        case DCC_SUGAR_LISTENER_TYPED_SLASH:
            return dcc_app_slash_typed(app, &listener->typed_slash);
        case DCC_SUGAR_LISTENER_TYPED_SUBCOMMAND:
            return dcc_app_subcommand_typed(app, &listener->typed_subcommand);
        case DCC_SUGAR_LISTENER_TYPED_AUTOCOMPLETE:
            return dcc_app_autocomplete_typed(app, &listener->typed_autocomplete);
        case DCC_SUGAR_LISTENER_TYPED_MODAL:
            return dcc_app_modal_typed(app, &listener->typed_modal);
        case DCC_SUGAR_LISTENER_TYPED_BUTTON:
            return dcc_app_button_typed(app, &listener->typed_component);
        case DCC_SUGAR_LISTENER_TYPED_SELECT:
            return dcc_app_select_typed(app, &listener->typed_component);
        case DCC_SUGAR_LISTENER_MODAL_FLOW:
            return dcc_app_modal_flow_register(app, &listener->modal_flow);
        case DCC_SUGAR_LISTENER_COMMAND_SCHEMA:
            return dcc_command_registry_add_builder(
                dcc_app_command_registry(app),
                &listener->command_schema
            );
        default:
            return DCC_ERR_INVALID_ARG;
    }
}

static inline dcc_status_t dcc_sugar_listeners_register(
    dcc_app_t *app,
    const dcc_sugar_listener_list_t *listeners
) {
    if (app == NULL || listeners == NULL || (listeners->count != 0U && listeners->items == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < listeners->count; ++i) {
        dcc_status_t status = dcc_sugar_listener_register(app, &listeners->items[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

#define DCC_LISTENERS_SETUP_NAME(name_) dcc_listeners_setup_ ## name_
#define DCC_LISTENERS_SETUP(name_) DCC_LISTENERS_SETUP_NAME(name_)

#define DCC_LISTENERS(...) \
    ((dcc_sugar_listener_list_t){ \
        .items = (dcc_sugar_listener_t[]){ __VA_ARGS__ }, \
        .count = sizeof((dcc_sugar_listener_t[]){ __VA_ARGS__ }) / sizeof(dcc_sugar_listener_t) \
    })

#define DCC_LISTENERS_REGISTER(app_, ...) \
    dcc_sugar_listeners_register((app_), &DCC_LISTENERS(__VA_ARGS__))

#define DCC_USE_LISTENERS(app_, ...) \
    DCC_LISTENERS_REGISTER((app_), __VA_ARGS__)

#define DCC_DECLARE_LISTENERS(name_) \
    dcc_status_t DCC_LISTENERS_SETUP_NAME(name_)(dcc_app_t *app, void *user_data)

#define DCC_DEFINE_LISTENERS(name_, ...) \
    static dcc_status_t DCC_LISTENERS_SETUP_NAME(name_)(dcc_app_t *app, void *user_data) { \
        (void)user_data; \
        dcc_sugar_listener_list_t dcc_sugar_listeners = DCC_LISTENERS(__VA_ARGS__); \
        return dcc_sugar_listeners_register((app), &dcc_sugar_listeners); \
    }

#define DCC_DEFINE_PUBLIC_LISTENERS(name_, ...) \
    dcc_status_t DCC_LISTENERS_SETUP_NAME(name_)(dcc_app_t *app, void *user_data) { \
        (void)user_data; \
        dcc_sugar_listener_list_t dcc_sugar_listeners = DCC_LISTENERS(__VA_ARGS__); \
        return dcc_sugar_listeners_register((app), &dcc_sugar_listeners); \
    }

#define DCC_LOAD_LISTENERS(app_, name_) \
    DCC_LISTENERS_SETUP_NAME(name_)((app_), NULL)

#define DCC_LOAD_LISTENERS_DATA(app_, name_, user_data_) \
    DCC_LISTENERS_SETUP_NAME(name_)((app_), (user_data_))

#define DCC_LISTENERS_MODULE(name_, user_data_) \
    DCC_APP_MODULE(#name_, DCC_LISTENERS_SETUP_NAME(name_), (user_data_))

#define DCC_LISTENER_MODULE(name_, user_data_) \
    DCC_LISTENERS_MODULE(name_, (user_data_))

#define DCC_LISTEN_DECORATED_SLASH(handler_) \
    DCC_LISTEN_SLASH_ROUTE(DCC_ROUTE(handler_))
#define DCC_LISTEN_SLASH_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_SLASH(handler_)
#define DCC_LISTEN_DECORATED_COMMAND(handler_) \
    DCC_LISTEN_DECORATED_SLASH(handler_)
#define DCC_LISTEN_COMMAND_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_COMMAND(handler_)

#define DCC_LISTEN_DECORATED_SUBCOMMAND(handler_) \
    DCC_LISTEN_SUBCOMMAND_ROUTE(DCC_ROUTE(handler_))
#define DCC_LISTEN_SUBCOMMAND_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_SUBCOMMAND(handler_)

#define DCC_LISTEN_DECORATED_AUTOCOMPLETE(handler_) \
    DCC_LISTEN_AUTOCOMPLETE_ROUTE(DCC_ROUTE(handler_))
#define DCC_LISTEN_AUTOCOMPLETE_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_AUTOCOMPLETE(handler_)

#define DCC_LISTEN_DECORATED_CONTEXT_MENU(handler_) \
    DCC_LISTEN_CONTEXT_MENU_ROUTE(DCC_ROUTE(handler_))
#define DCC_LISTEN_CONTEXT_MENU_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_CONTEXT_MENU(handler_)
#define DCC_LISTEN_DECORATED_USER_CONTEXT_MENU(handler_) \
    DCC_LISTEN_DECORATED_CONTEXT_MENU(handler_)
#define DCC_LISTEN_USER_CONTEXT_MENU_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_USER_CONTEXT_MENU(handler_)
#define DCC_LISTEN_DECORATED_MESSAGE_CONTEXT_MENU(handler_) \
    DCC_LISTEN_DECORATED_CONTEXT_MENU(handler_)
#define DCC_LISTEN_MESSAGE_CONTEXT_MENU_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_MESSAGE_CONTEXT_MENU(handler_)

#define DCC_LISTEN_DECORATED_COMPONENT(handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ROUTE(handler_))
#define DCC_LISTEN_COMPONENT_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_COMPONENT(handler_)
#define DCC_LISTEN_DECORATED_BUTTON(handler_) \
    DCC_LISTEN_DECORATED_COMPONENT(handler_)
#define DCC_LISTEN_BUTTON_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_BUTTON(handler_)
#define DCC_LISTEN_DECORATED_SELECT(handler_) \
    DCC_LISTEN_DECORATED_COMPONENT(handler_)
#define DCC_LISTEN_SELECT_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_SELECT(handler_)
#define DCC_LISTEN_DECORATED_MODAL(handler_) \
    DCC_LISTEN_DECORATED_COMPONENT(handler_)
#define DCC_LISTEN_MODAL_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_MODAL(handler_)

#define DCC_LISTEN_DECORATED_EVENT(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ROUTE(handler_))
#define DCC_LISTEN_EVENT_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_EVENT(handler_)
#define DCC_LISTEN_DECORATED_READY(handler_) \
    DCC_LISTEN_DECORATED_EVENT(handler_)
#define DCC_LISTEN_READY_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_READY(handler_)

#define DCC_LISTEN_DECORATED_MESSAGE_COMMAND(handler_) \
    DCC_LISTEN_MESSAGE_COMMAND_ROUTE(DCC_ROUTE(handler_))
#define DCC_LISTEN_MESSAGE_COMMAND_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_MESSAGE_COMMAND(handler_)
#define DCC_LISTEN_DECORATED_PREFIX_COMMAND(handler_) \
    DCC_LISTEN_DECORATED_MESSAGE_COMMAND(handler_)
#define DCC_LISTEN_PREFIX_COMMAND_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_PREFIX_COMMAND(handler_)

#define DCC_LISTEN_DECORATED_TASK(handler_) \
    DCC_LISTEN_TASK_ROUTE(DCC_ROUTE(handler_))
#define DCC_LISTEN_TASK_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_TASK(handler_)

#define DCC_LISTEN_DECORATED_TYPED_SLASH(handler_) \
    DCC_LISTEN_TYPED_SLASH(DCC_TYPED_ROUTE(handler_))
#define DCC_LISTEN_TYPED_SLASH_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_TYPED_SLASH(handler_)
#define DCC_LISTEN_DECORATED_TYPED_COMMAND(handler_) \
    DCC_LISTEN_DECORATED_TYPED_SLASH(handler_)
#define DCC_LISTEN_TYPED_COMMAND_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_TYPED_COMMAND(handler_)

#define DCC_LISTEN_DECORATED_TYPED_SUBCOMMAND(handler_) \
    DCC_LISTEN_TYPED_SUBCOMMAND(DCC_TYPED_ROUTE(handler_))
#define DCC_LISTEN_TYPED_SUBCOMMAND_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_TYPED_SUBCOMMAND(handler_)

#define DCC_LISTEN_DECORATED_TYPED_AUTOCOMPLETE(handler_) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE(DCC_TYPED_ROUTE(handler_))
#define DCC_LISTEN_TYPED_AUTOCOMPLETE_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_TYPED_AUTOCOMPLETE(handler_)

#define DCC_LISTEN_DECORATED_TYPED_MODAL(handler_) \
    DCC_LISTEN_TYPED_MODAL(DCC_TYPED_ROUTE(handler_))
#define DCC_LISTEN_TYPED_MODAL_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_TYPED_MODAL(handler_)

#define DCC_LISTEN_DECORATED_TYPED_BUTTON(handler_) \
    DCC_LISTEN_TYPED_BUTTON(DCC_TYPED_ROUTE(handler_))
#define DCC_LISTEN_TYPED_BUTTON_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_TYPED_BUTTON(handler_)

#define DCC_LISTEN_DECORATED_TYPED_SELECT(handler_) \
    DCC_LISTEN_TYPED_SELECT(DCC_TYPED_ROUTE(handler_))
#define DCC_LISTEN_TYPED_SELECT_DECORATED(handler_) \
    DCC_LISTEN_DECORATED_TYPED_SELECT(handler_)

#define DCC_LISTEN_SLASH_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_SLASH, \
        .slash = (route_) \
    })
#define DCC_LISTEN_COMMAND_SCHEMA(command_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_COMMAND_SCHEMA, \
        .command_schema = (command_) \
    })
#define DCC_LISTEN_COMMAND_BUILDER(command_) \
    DCC_LISTEN_COMMAND_SCHEMA(command_)
#define DCC_LISTEN_SCHEMA(command_) \
    DCC_LISTEN_COMMAND_SCHEMA(command_)
#define DCC_LISTEN_SLASH_BUILDER(builder_, handler_) \
    DCC_LISTEN_SLASH_ROUTE(DCC_SLASH_BUILDER_VALUE((builder_), (handler_)))
#define DCC_LISTEN_SLASH_BUILDER_DATA(builder_, handler_, user_data_) \
    DCC_LISTEN_SLASH_ROUTE(DCC_SLASH_BUILDER_VALUE_DATA((builder_), (handler_), (user_data_)))
#define DCC_LISTEN_SLASH_BUILDER_POLICY(builder_, handler_, policy_) \
    DCC_LISTEN_SLASH_ROUTE(DCC_SLASH_BUILDER_VALUE_POLICY((builder_), (handler_), (policy_)))
#define DCC_LISTEN_SLASH_BUILDER_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_LISTEN_SLASH_ROUTE( \
        DCC_SLASH_BUILDER_VALUE_DATA_POLICY((builder_), (handler_), (user_data_), (policy_)) \
    )
#define DCC_LISTEN_SLASH(name_, description_, handler_) \
    DCC_LISTEN_SLASH_ROUTE(DCC_ON_SLASH((name_), (description_), (handler_)))
#define DCC_LISTEN_SLASH_DATA(name_, description_, handler_, user_data_) \
    DCC_LISTEN_SLASH_ROUTE(DCC_ON_SLASH_DATA((name_), (description_), (handler_), (user_data_)))
#define DCC_LISTEN_SLASH_GUARDED(name_, description_, handler_, ...) \
    DCC_LISTEN_SLASH_ROUTE(DCC_ON_SLASH_GUARDED((name_), (description_), (handler_), __VA_ARGS__))
#define DCC_LISTEN_SLASH_DATA_GUARDED(name_, description_, handler_, user_data_, ...) \
    DCC_LISTEN_SLASH_ROUTE( \
        DCC_ON_SLASH_DATA_GUARDED((name_), (description_), (handler_), (user_data_), __VA_ARGS__) \
    )
#define DCC_LISTEN_COMMAND(name_, description_, handler_) \
    DCC_LISTEN_SLASH((name_), (description_), (handler_))
#define DCC_LISTEN_COMMAND_DATA(name_, description_, handler_, user_data_) \
    DCC_LISTEN_SLASH_DATA((name_), (description_), (handler_), (user_data_))

#define DCC_LISTEN_SUBCOMMAND_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_SUBCOMMAND, \
        .subcommand = (route_) \
    })
#define DCC_LISTEN_SUBCOMMAND(command_name_, subcommand_path_, handler_) \
    DCC_LISTEN_SUBCOMMAND_ROUTE(DCC_SUBCOMMAND((command_name_), (subcommand_path_), (handler_)))
#define DCC_LISTEN_SUBCOMMAND_DATA(command_name_, subcommand_path_, handler_, user_data_) \
    DCC_LISTEN_SUBCOMMAND_ROUTE(DCC_SUBCOMMAND_DATA((command_name_), (subcommand_path_), (handler_), (user_data_)))

#define DCC_LISTEN_AUTOCOMPLETE_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_AUTOCOMPLETE, \
        .autocomplete = (route_) \
    })
#define DCC_LISTEN_AUTOCOMPLETE(name_, handler_) \
    DCC_LISTEN_AUTOCOMPLETE_ROUTE(DCC_ON_AUTOCOMPLETE((name_), (handler_)))
#define DCC_LISTEN_AUTOCOMPLETE_DATA(name_, handler_, user_data_) \
    DCC_LISTEN_AUTOCOMPLETE_ROUTE(DCC_ON_AUTOCOMPLETE_DATA((name_), (handler_), (user_data_)))

#define DCC_LISTEN_CONTEXT_MENU_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_CONTEXT_MENU, \
        .context_menu = (route_) \
    })
#define DCC_LISTEN_USER_CONTEXT_MENU(name_, handler_) \
    DCC_LISTEN_CONTEXT_MENU_ROUTE(DCC_ON_USER_CONTEXT_MENU((name_), (handler_)))
#define DCC_LISTEN_USER_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_LISTEN_CONTEXT_MENU_ROUTE(DCC_ON_USER_CONTEXT_MENU_DATA((name_), (handler_), (user_data_)))
#define DCC_LISTEN_MESSAGE_CONTEXT_MENU(name_, handler_) \
    DCC_LISTEN_CONTEXT_MENU_ROUTE(DCC_ON_MESSAGE_CONTEXT_MENU((name_), (handler_)))
#define DCC_LISTEN_MESSAGE_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_LISTEN_CONTEXT_MENU_ROUTE(DCC_ON_MESSAGE_CONTEXT_MENU_DATA((name_), (handler_), (user_data_)))

#define DCC_LISTEN_COMPONENT_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_COMPONENT, \
        .component = (route_) \
    })
#define DCC_LISTEN_BUTTON(custom_id_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON((custom_id_), (handler_)))
#define DCC_LISTEN_BUTTON_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_DATA((custom_id_), (handler_), (user_data_)))
#define DCC_LISTEN_BUTTON_PREFIX(custom_id_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_PREFIX((custom_id_prefix_), (handler_)))
#define DCC_LISTEN_BUTTON_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_)))
#define DCC_LISTEN_BUTTON_NS(namespace_, action_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_NS(namespace_, action_, (handler_)))
#define DCC_LISTEN_BUTTON_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_NS_DATA(namespace_, action_, (handler_), (user_data_)))
#define DCC_LISTEN_BUTTON_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_NS_PREFIX(namespace_, action_prefix_, (handler_)))
#define DCC_LISTEN_BUTTON_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_BUTTON_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_)))
#define DCC_LISTEN_SELECT(custom_id_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT((custom_id_), (handler_)))
#define DCC_LISTEN_SELECT_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_DATA((custom_id_), (handler_), (user_data_)))
#define DCC_LISTEN_SELECT_PREFIX(custom_id_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_PREFIX((custom_id_prefix_), (handler_)))
#define DCC_LISTEN_SELECT_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_)))
#define DCC_LISTEN_SELECT_NS(namespace_, action_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_NS(namespace_, action_, (handler_)))
#define DCC_LISTEN_SELECT_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_NS_DATA(namespace_, action_, (handler_), (user_data_)))
#define DCC_LISTEN_SELECT_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_NS_PREFIX(namespace_, action_prefix_, (handler_)))
#define DCC_LISTEN_SELECT_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_SELECT_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_)))
#define DCC_LISTEN_MODAL(custom_id_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL((custom_id_), (handler_)))
#define DCC_LISTEN_MODAL_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_DATA((custom_id_), (handler_), (user_data_)))
#define DCC_LISTEN_MODAL_PREFIX(custom_id_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_PREFIX((custom_id_prefix_), (handler_)))
#define DCC_LISTEN_MODAL_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_)))
#define DCC_LISTEN_MODAL_NS(namespace_, action_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_NS(namespace_, action_, (handler_)))
#define DCC_LISTEN_MODAL_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_NS_DATA(namespace_, action_, (handler_), (user_data_)))
#define DCC_LISTEN_MODAL_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_NS_PREFIX(namespace_, action_prefix_, (handler_)))
#define DCC_LISTEN_MODAL_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_COMPONENT_ROUTE(DCC_ON_MODAL_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_)))

#define DCC_LISTEN_EVENT_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_EVENT, \
        .event = (route_) \
    })
#define DCC_LISTEN_EVENT(type_, handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_EVENT((type_), (handler_)))
#define DCC_LISTEN_EVENT_DATA(type_, handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_EVENT_DATA((type_), (handler_), (user_data_)))
#define DCC_LISTEN_READY(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_READY((handler_)))
#define DCC_LISTEN_READY_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_READY_DATA((handler_), (user_data_)))
#define DCC_LISTEN_READY_ONCE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_READY_ONCE((handler_)))
#define DCC_LISTEN_READY_ONCE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_READY_ONCE_DATA((handler_), (user_data_)))
#define DCC_LISTEN_MESSAGE_CREATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_CREATE((handler_)))
#define DCC_LISTEN_MESSAGE_CREATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_CREATE_DATA((handler_), (user_data_)))
#define DCC_LISTEN_MESSAGE_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_UPDATE((handler_)))
#define DCC_LISTEN_MESSAGE_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_UPDATE_DATA((handler_), (user_data_)))
#define DCC_LISTEN_MESSAGE_DELETE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_DELETE((handler_)))
#define DCC_LISTEN_MESSAGE_DELETE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MESSAGE_DELETE_DATA((handler_), (user_data_)))

#define DCC_LISTEN_GUILD_CREATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_CREATE(handler_))
#define DCC_LISTEN_GUILD_CREATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_CREATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_GUILD_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_UPDATE(handler_))
#define DCC_LISTEN_GUILD_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_GUILD_DELETE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_DELETE(handler_))
#define DCC_LISTEN_GUILD_DELETE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_DELETE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_MEMBER_JOIN(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_JOIN(handler_))
#define DCC_LISTEN_MEMBER_JOIN_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_JOIN_DATA(handler_, (user_data_)))
#define DCC_LISTEN_MEMBER_ADD(handler_) DCC_LISTEN_MEMBER_JOIN(handler_)
#define DCC_LISTEN_MEMBER_ADD_DATA(handler_, user_data_) \
    DCC_LISTEN_MEMBER_JOIN_DATA(handler_, (user_data_))
#define DCC_LISTEN_MEMBER_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_UPDATE(handler_))
#define DCC_LISTEN_MEMBER_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_MEMBER_REMOVE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_REMOVE(handler_))
#define DCC_LISTEN_MEMBER_REMOVE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_MEMBER_REMOVE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_ROLE_CREATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_CREATE(handler_))
#define DCC_LISTEN_ROLE_CREATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_CREATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_ROLE_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_UPDATE(handler_))
#define DCC_LISTEN_ROLE_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_ROLE_DELETE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_DELETE(handler_))
#define DCC_LISTEN_ROLE_DELETE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_ROLE_DELETE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_CHANNEL_CREATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_CREATE(handler_))
#define DCC_LISTEN_CHANNEL_CREATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_CREATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_CHANNEL_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_UPDATE(handler_))
#define DCC_LISTEN_CHANNEL_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_CHANNEL_DELETE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_DELETE(handler_))
#define DCC_LISTEN_CHANNEL_DELETE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_CHANNEL_DELETE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_THREAD_CREATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_CREATE(handler_))
#define DCC_LISTEN_THREAD_CREATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_CREATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_THREAD_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_UPDATE(handler_))
#define DCC_LISTEN_THREAD_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_THREAD_DELETE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_DELETE(handler_))
#define DCC_LISTEN_THREAD_DELETE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_THREAD_DELETE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_REACTION_ADD(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_ADD(handler_))
#define DCC_LISTEN_REACTION_ADD_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_ADD_DATA(handler_, (user_data_)))
#define DCC_LISTEN_REACTION_REMOVE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_REMOVE(handler_))
#define DCC_LISTEN_REACTION_REMOVE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_REMOVE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_REACTION_CLEAR(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_CLEAR(handler_))
#define DCC_LISTEN_REACTION_CLEAR_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_CLEAR_DATA(handler_, (user_data_)))
#define DCC_LISTEN_REACTION_EMOJI_CLEAR(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_EMOJI_CLEAR(handler_))
#define DCC_LISTEN_REACTION_EMOJI_CLEAR_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_REACTION_EMOJI_CLEAR_DATA(handler_, (user_data_)))

#define DCC_LISTEN_POLL_VOTE_ADD(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_POLL_VOTE_ADD(handler_))
#define DCC_LISTEN_POLL_VOTE_ADD_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_POLL_VOTE_ADD_DATA(handler_, (user_data_)))
#define DCC_LISTEN_POLL_VOTE_REMOVE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_POLL_VOTE_REMOVE(handler_))
#define DCC_LISTEN_POLL_VOTE_REMOVE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_POLL_VOTE_REMOVE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_VOICE_STATE_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_VOICE_STATE_UPDATE(handler_))
#define DCC_LISTEN_VOICE_STATE_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_VOICE_STATE_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_TYPING_START(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_TYPING_START(handler_))
#define DCC_LISTEN_TYPING_START_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_TYPING_START_DATA(handler_, (user_data_)))
#define DCC_LISTEN_PRESENCE_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_PRESENCE_UPDATE(handler_))
#define DCC_LISTEN_PRESENCE_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_PRESENCE_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_USER_UPDATE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_USER_UPDATE(handler_))
#define DCC_LISTEN_USER_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_USER_UPDATE_DATA(handler_, (user_data_)))
#define DCC_LISTEN_GUILD_BAN_ADD(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_BAN_ADD(handler_))
#define DCC_LISTEN_GUILD_BAN_ADD_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_BAN_ADD_DATA(handler_, (user_data_)))
#define DCC_LISTEN_GUILD_BAN_REMOVE(handler_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_BAN_REMOVE(handler_))
#define DCC_LISTEN_GUILD_BAN_REMOVE_DATA(handler_, user_data_) \
    DCC_LISTEN_EVENT_ROUTE(DCC_ON_GUILD_BAN_REMOVE_DATA(handler_, (user_data_)))

#define DCC_LISTEN_MESSAGE_COMMAND_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_MESSAGE_COMMAND, \
        .message_command = (route_) \
    })
#define DCC_LISTEN_MESSAGE_COMMAND(prefix_, name_, handler_) \
    DCC_LISTEN_MESSAGE_COMMAND_ROUTE(DCC_MESSAGE_COMMAND((prefix_), (name_), (handler_)))
#define DCC_LISTEN_MESSAGE_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_LISTEN_MESSAGE_COMMAND_ROUTE(DCC_MESSAGE_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_)))
#define DCC_LISTEN_PREFIX_COMMAND(prefix_, name_, handler_) \
    DCC_LISTEN_MESSAGE_COMMAND((prefix_), (name_), (handler_))
#define DCC_LISTEN_PREFIX_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_LISTEN_MESSAGE_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_))

#define DCC_LISTEN_TASK_ROUTE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TASK, \
        .task = (route_) \
    })
#define DCC_LISTEN_TASK_MS(interval_ms_, handler_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_LOOP_MS((interval_ms_), (handler_)))
#define DCC_LISTEN_TASK_MS_DATA(interval_ms_, handler_, user_data_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_LOOP_MS_DATA((interval_ms_), (handler_), (user_data_)))
#define DCC_LISTEN_TASK_SECONDS(interval_seconds_, handler_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_LOOP_SECONDS((interval_seconds_), (handler_)))
#define DCC_LISTEN_TASK_SECONDS_DATA(interval_seconds_, handler_, user_data_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_LOOP_SECONDS_DATA((interval_seconds_), (handler_), (user_data_)))
#define DCC_LISTEN_TASK_DAILY_KST(hhmm_, handler_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_DAILY_KST((hhmm_), (handler_)))
#define DCC_LISTEN_TASK_DAILY_KST_DATA(hhmm_, handler_, user_data_) \
    DCC_LISTEN_TASK_ROUTE(DCC_TASK_DAILY_KST_DATA((hhmm_), (handler_), (user_data_)))

#define DCC_LISTEN_TYPED_SLASH(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_SLASH, \
        .typed_slash = (route_) \
    })
#define DCC_LISTEN_TYPED_SLASH_PARAMS(name_, description_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SLASH(DCC_SLASH_ROUTE((name_), (description_), args_type_, (handler_), params_))
#define DCC_LISTEN_TYPED_SLASH_PARAMS_DATA(name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_DATA((name_), (description_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_TYPED_SLASH_PARAMS_VALIDATED(name_, description_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_VALIDATED((name_), (description_), args_type_, (handler_), params_, (validators_)) \
    )
#define DCC_LISTEN_TYPED_SLASH_PARAMS_DATA_VALIDATED( \
    name_, description_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_DATA_VALIDATED( \
            (name_), (description_), args_type_, (handler_), (user_data_), params_, (validators_) \
        ) \
    )
#define DCC_LISTEN_TYPED_SLASH_PARAMS_GUARDED(name_, description_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_GUARDED((name_), (description_), args_type_, (handler_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_TYPED_SLASH_PARAMS_DATA_GUARDED( \
    name_, description_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_DATA_GUARDED( \
            (name_), (description_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SLASH_FN(handler_, description_, args_type_, params_) \
    DCC_LISTEN_TYPED_SLASH(DCC_SLASH_ROUTE_FN(handler_, (description_), args_type_, params_))
#define DCC_LISTEN_TYPED_SLASH_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_FN_DATA(handler_, (description_), args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_TYPED_SLASH_FN_VALIDATED(handler_, description_, args_type_, params_, validators_) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_FN_VALIDATED(handler_, (description_), args_type_, params_, (validators_)) \
    )
#define DCC_LISTEN_TYPED_SLASH_FN_DATA_VALIDATED( \
    handler_, description_, args_type_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_FN_DATA_VALIDATED( \
            handler_, (description_), args_type_, (user_data_), params_, (validators_) \
        ) \
    )
#define DCC_LISTEN_TYPED_SLASH_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_TYPED_SLASH_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH( \
        DCC_SLASH_ROUTE_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_SLASH_PARAMS(name_, description_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SLASH_PARAMS((name_), (description_), args_type_, (handler_), params_)
#define DCC_LISTEN_SLASH_PARAMS_DATA(name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SLASH_PARAMS_DATA((name_), (description_), args_type_, (handler_), (user_data_), params_)
#define DCC_LISTEN_SLASH_PARAMS_VALIDATED(name_, description_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_TYPED_SLASH_PARAMS_VALIDATED((name_), (description_), args_type_, (handler_), params_, (validators_))
#define DCC_LISTEN_SLASH_PARAMS_DATA_VALIDATED( \
    name_, description_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_SLASH_PARAMS_DATA_VALIDATED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_LISTEN_SLASH_PARAMS_GUARDED(name_, description_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH_PARAMS_GUARDED((name_), (description_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_LISTEN_SLASH_PARAMS_DATA_GUARDED( \
    name_, description_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SLASH_PARAMS_DATA_GUARDED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SLASH_FN(handler_, description_, args_type_, params_) \
    DCC_LISTEN_TYPED_SLASH_FN(handler_, (description_), args_type_, params_)
#define DCC_LISTEN_SLASH_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SLASH_FN_DATA(handler_, (description_), args_type_, (user_data_), params_)
#define DCC_LISTEN_SLASH_FN_VALIDATED(handler_, description_, args_type_, params_, validators_) \
    DCC_LISTEN_TYPED_SLASH_FN_VALIDATED(handler_, (description_), args_type_, params_, (validators_))
#define DCC_LISTEN_SLASH_FN_DATA_VALIDATED(handler_, description_, args_type_, user_data_, params_, validators_) \
    DCC_LISTEN_TYPED_SLASH_FN_DATA_VALIDATED(handler_, (description_), args_type_, (user_data_), params_, (validators_))
#define DCC_LISTEN_SLASH_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__)
#define DCC_LISTEN_SLASH_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SLASH_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__)
#define DCC_LISTEN_COMMAND_PARAMS(name_, description_, args_type_, handler_, params_) \
    DCC_LISTEN_SLASH_PARAMS((name_), (description_), args_type_, (handler_), params_)
#define DCC_LISTEN_COMMAND_PARAMS_DATA(name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_SLASH_PARAMS_DATA((name_), (description_), args_type_, (handler_), (user_data_), params_)
#define DCC_LISTEN_COMMAND_PARAMS_VALIDATED(name_, description_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_SLASH_PARAMS_VALIDATED((name_), (description_), args_type_, (handler_), params_, (validators_))
#define DCC_LISTEN_COMMAND_PARAMS_DATA_VALIDATED( \
    name_, description_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_SLASH_PARAMS_DATA_VALIDATED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_LISTEN_COMMAND_PARAMS_GUARDED(name_, description_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_SLASH_PARAMS_GUARDED((name_), (description_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_LISTEN_COMMAND_PARAMS_DATA_GUARDED( \
    name_, description_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_SLASH_PARAMS_DATA_GUARDED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_COMMAND_FN(handler_, description_, args_type_, params_) \
    DCC_LISTEN_SLASH_FN(handler_, (description_), args_type_, params_)
#define DCC_LISTEN_COMMAND_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_LISTEN_SLASH_FN_DATA(handler_, (description_), args_type_, (user_data_), params_)
#define DCC_LISTEN_COMMAND_FN_VALIDATED(handler_, description_, args_type_, params_, validators_) \
    DCC_LISTEN_SLASH_FN_VALIDATED(handler_, (description_), args_type_, params_, (validators_))
#define DCC_LISTEN_COMMAND_FN_DATA_VALIDATED(handler_, description_, args_type_, user_data_, params_, validators_) \
    DCC_LISTEN_SLASH_FN_DATA_VALIDATED(handler_, (description_), args_type_, (user_data_), params_, (validators_))
#define DCC_LISTEN_COMMAND_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_LISTEN_SLASH_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__)
#define DCC_LISTEN_COMMAND_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_SLASH_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__)
#define DCC_LISTEN_COMMAND_PARAMS_FN(handler_, description_, args_type_, params_) \
    DCC_LISTEN_COMMAND_FN(handler_, (description_), args_type_, params_)
#define DCC_LISTEN_COMMAND_PARAMS_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_LISTEN_COMMAND_FN_DATA(handler_, (description_), args_type_, (user_data_), params_)
#define DCC_LISTEN_COMMAND_PARAMS_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_LISTEN_COMMAND_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__)
#define DCC_LISTEN_COMMAND_PARAMS_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_COMMAND_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__)
#define DCC_LISTEN_TYPED_SUBCOMMAND(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_SUBCOMMAND, \
        .typed_subcommand = (route_) \
    })
#define DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE((command_name_), (subcommand_path_), args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_DATA( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_DATA( \
            (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_GUARDED( \
    command_name_, subcommand_path_, args_type_, handler_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_GUARDED( \
            (command_name_), (subcommand_path_), args_type_, (handler_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_DATA_GUARDED( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_DATA_GUARDED( \
            (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS(command_name_, group_, subcommand_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN((command_name_), group_, subcommand_, args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_DATA( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_DATA( \
            (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_GUARDED( \
            (command_name_), group_, subcommand_, args_type_, (handler_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_DATA_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_DATA_GUARDED( \
            (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_FN(command_name_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND(DCC_SUBCOMMAND_ROUTE_FN((command_name_), handler_, args_type_, params_))
#define DCC_LISTEN_TYPED_SUBCOMMAND_FN_DATA(command_name_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_FN_DATA((command_name_), handler_, args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_FN_GUARDED(command_name_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_FN_GUARDED((command_name_), handler_, args_type_, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_FN_DATA_GUARDED(command_name_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_FN_DATA_GUARDED( \
            (command_name_), handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN(command_name_, group_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND(DCC_SUBCOMMAND_ROUTE_IN_FN((command_name_), group_, handler_, args_type_, params_))
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_DATA(command_name_, group_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_FN_DATA((command_name_), group_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_GUARDED(command_name_, group_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_FN_GUARDED((command_name_), group_, handler_, args_type_, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_DATA_GUARDED( \
    command_name_, group_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND( \
        DCC_SUBCOMMAND_ROUTE_IN_FN_DATA_GUARDED( \
            (command_name_), group_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_SUBCOMMAND_PARAMS(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS((command_name_), (subcommand_path_), args_type_, (handler_), params_)
#define DCC_LISTEN_SUBCOMMAND_PARAMS_DATA(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_DATA( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_LISTEN_SUBCOMMAND_PARAMS_GUARDED(command_name_, subcommand_path_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_GUARDED( \
        (command_name_), (subcommand_path_), args_type_, (handler_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SUBCOMMAND_PARAMS_DATA_GUARDED( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_PARAMS_DATA_GUARDED( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SUBCOMMAND_IN_PARAMS(command_name_, group_, subcommand_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS((command_name_), group_, subcommand_, args_type_, (handler_), params_)
#define DCC_LISTEN_SUBCOMMAND_IN_PARAMS_DATA( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_DATA( \
        (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_LISTEN_SUBCOMMAND_IN_PARAMS_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_GUARDED( \
        (command_name_), group_, subcommand_, args_type_, (handler_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SUBCOMMAND_IN_PARAMS_DATA_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_PARAMS_DATA_GUARDED( \
        (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SUBCOMMAND_FN(command_name_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_FN((command_name_), handler_, args_type_, params_)
#define DCC_LISTEN_SUBCOMMAND_FN_DATA(command_name_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_FN_DATA((command_name_), handler_, args_type_, (user_data_), params_)
#define DCC_LISTEN_SUBCOMMAND_FN_GUARDED(command_name_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND_FN_GUARDED((command_name_), handler_, args_type_, params_, __VA_ARGS__)
#define DCC_LISTEN_SUBCOMMAND_FN_DATA_GUARDED(command_name_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND_FN_DATA_GUARDED( \
        (command_name_), handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_SUBCOMMAND_IN_FN(command_name_, group_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN((command_name_), group_, handler_, args_type_, params_)
#define DCC_LISTEN_SUBCOMMAND_IN_FN_DATA(command_name_, group_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_DATA((command_name_), group_, handler_, args_type_, (user_data_), params_)
#define DCC_LISTEN_SUBCOMMAND_IN_FN_GUARDED(command_name_, group_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_GUARDED((command_name_), group_, handler_, args_type_, params_, __VA_ARGS__)
#define DCC_LISTEN_SUBCOMMAND_IN_FN_DATA_GUARDED( \
    command_name_, group_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_IN_FN_DATA_GUARDED( \
        (command_name_), group_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_LISTEN_TYPED_AUTOCOMPLETE(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_AUTOCOMPLETE, \
        .typed_autocomplete = (route_) \
    })
#define DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_PARAMS( \
    command_name_, subcommand_path_, args_type_, handler_, params_ \
) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS((command_name_), (subcommand_path_), args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA( \
            (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS( \
    command_name_, group_, subcommand_, args_type_, handler_, params_ \
) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_IN( \
            (command_name_), group_, subcommand_, args_type_, (handler_), params_ \
        ) \
    )
#define DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS_DATA( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_IN_DATA( \
            (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_PARAMS(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_PARAMS( \
        (command_name_), (subcommand_path_), args_type_, (handler_), params_ \
    )
#define DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS( \
    command_name_, group_, subcommand_, args_type_, handler_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS( \
        (command_name_), group_, subcommand_, args_type_, (handler_), params_ \
    )
#define DCC_LISTEN_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS_DATA( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SUBCOMMAND_AUTOCOMPLETE_IN_PARAMS_DATA( \
        (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_LISTEN_TYPED_MODAL(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_MODAL, \
        .typed_modal = (route_) \
    })
#define DCC_LISTEN_TYPED_BUTTON(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_BUTTON, \
        .typed_component = (route_) \
    })
#define DCC_LISTEN_TYPED_SELECT(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_SELECT, \
        .typed_component = (route_) \
    })
#define DCC_LISTEN_MODAL_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_MODAL(DCC_ON_MODAL_PARAMS((custom_id_), args_type_, (handler_), params_))
#define DCC_LISTEN_MODAL_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_MODAL(DCC_ON_MODAL_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_))
#define DCC_LISTEN_MODAL_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_)) \
    )
#define DCC_LISTEN_MODAL_PARAMS_DATA_VALIDATED( \
    custom_id_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PARAMS_DATA_VALIDATED( \
            (custom_id_), args_type_, (handler_), (user_data_), params_, (validators_) \
        ) \
    )
#define DCC_LISTEN_MODAL_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), NULL, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_MODAL(DCC_ON_MODAL_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_))
#define DCC_LISTEN_MODAL_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_MODAL_PREFIX_PARAMS_DATA_GUARDED( \
    custom_id_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PREFIX_PARAMS_DATA_GUARDED( \
            (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_MODAL_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_MODAL(DCC_ON_MODAL_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_))
#define DCC_LISTEN_MODAL_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_MODAL_NS_PARAMS_DATA_GUARDED( \
    namespace_, action_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PARAMS_DATA_GUARDED( \
            namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_MODAL_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_MODAL_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_MODAL_NS_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_MODAL(DCC_ON_MODAL_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_LISTEN_MODAL_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_MODAL_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_BUTTON_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_BUTTON(DCC_ON_BUTTON_PARAMS((custom_id_), args_type_, (handler_), params_))
#define DCC_LISTEN_BUTTON_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_BUTTON(DCC_ON_BUTTON_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_))
#define DCC_LISTEN_BUTTON_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_)) \
    )
#define DCC_LISTEN_BUTTON_PARAMS_DATA_VALIDATED( \
    custom_id_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PARAMS_DATA_VALIDATED( \
            (custom_id_), args_type_, (handler_), (user_data_), params_, (validators_) \
        ) \
    )
#define DCC_LISTEN_BUTTON_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), NULL, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_BUTTON_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_BUTTON_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_BUTTON(DCC_ON_BUTTON_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_))
#define DCC_LISTEN_BUTTON_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_BUTTON_PREFIX_PARAMS_DATA_GUARDED( \
    custom_id_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PREFIX_PARAMS_DATA_GUARDED( \
            (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_BUTTON_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_BUTTON(DCC_ON_BUTTON_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_))
#define DCC_LISTEN_BUTTON_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_BUTTON_NS_PARAMS_DATA_GUARDED( \
    namespace_, action_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PARAMS_DATA_GUARDED( \
            namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_BUTTON_NS_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_BUTTON(DCC_ON_BUTTON_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_LISTEN_BUTTON_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_BUTTON_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_SELECT_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SELECT(DCC_ON_SELECT_PARAMS((custom_id_), args_type_, (handler_), params_))
#define DCC_LISTEN_SELECT_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SELECT(DCC_ON_SELECT_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_))
#define DCC_LISTEN_SELECT_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_)) \
    )
#define DCC_LISTEN_SELECT_PARAMS_DATA_VALIDATED( \
    custom_id_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PARAMS_DATA_VALIDATED( \
            (custom_id_), args_type_, (handler_), (user_data_), params_, (validators_) \
        ) \
    )
#define DCC_LISTEN_SELECT_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), NULL, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_SELECT_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_SELECT_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SELECT(DCC_ON_SELECT_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_))
#define DCC_LISTEN_SELECT_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_SELECT_PREFIX_PARAMS_DATA_GUARDED( \
    custom_id_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PREFIX_PARAMS_DATA_GUARDED( \
            (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_SELECT_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SELECT(DCC_ON_SELECT_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_))
#define DCC_LISTEN_SELECT_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_SELECT_NS_PARAMS_DATA_GUARDED( \
    namespace_, action_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PARAMS_DATA_GUARDED( \
            namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_SELECT_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_SELECT_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_SELECT_NS_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_SELECT(DCC_ON_SELECT_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_LISTEN_SELECT_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_SELECT_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_MODAL_FLOW(flow_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_MODAL_FLOW, \
        .modal_flow = (flow_) \
    })
#define DCC_LISTEN_MODAL_FLOW_PARAMS(custom_id_, title_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW(DCC_MODAL_FLOW_PARAMS((custom_id_), (title_), args_type_, (handler_), params_, __VA_ARGS__))
#define DCC_LISTEN_MODAL_FLOW_PARAMS_DATA(custom_id_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_FLOW_PARAMS_DATA((custom_id_), (title_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_V2_FLOW_PARAMS(custom_id_, title_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW(DCC_MODAL_V2_FLOW_PARAMS((custom_id_), (title_), args_type_, (handler_), params_, __VA_ARGS__))
#define DCC_LISTEN_MODAL_V2_FLOW_PARAMS_DATA(custom_id_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_V2_FLOW_PARAMS_DATA((custom_id_), (title_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_V2_FLOW_NS_PARAMS(namespace_, action_, title_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_V2_FLOW_NS_PARAMS(namespace_, action_, (title_), args_type_, (handler_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_V2_FLOW_NS_PARAMS_DATA( \
    namespace_, action_, title_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_V2_FLOW_NS_PARAMS_DATA( \
            namespace_, action_, (title_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_MODAL_V2_FLOW_NS_PARAMS_FN(namespace_, title_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_V2_FLOW_NS_PARAMS_FN(namespace_, (title_), args_type_, handler_, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_V2_FLOW_NS_PARAMS_FN_DATA( \
    namespace_, title_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_V2_FLOW_NS_PARAMS_FN_DATA( \
            namespace_, (title_), args_type_, handler_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )

#define DCC_ROUTES(...) DCC_LISTENERS(__VA_ARGS__)
#define DCC_ROUTE_TABLE(...) DCC_ROUTES(__VA_ARGS__)
#define DCC_ROUTE_LIST(...) DCC_ROUTES(__VA_ARGS__)

#define DCC_ROUTES_REGISTER(app_, ...) DCC_LISTENERS_REGISTER((app_), __VA_ARGS__)
#define DCC_USE_ROUTES(app_, ...) DCC_ROUTES_REGISTER((app_), __VA_ARGS__)

#define DCC_DECLARE_ROUTES(name_) DCC_DECLARE_LISTENERS(name_)
#define DCC_DEFINE_ROUTES(name_, ...) DCC_DEFINE_LISTENERS(name_, __VA_ARGS__)
#define DCC_DEFINE_PUBLIC_ROUTES(name_, ...) DCC_DEFINE_PUBLIC_LISTENERS(name_, __VA_ARGS__)
#define DCC_LOAD_ROUTES(app_, name_) DCC_LOAD_LISTENERS((app_), name_)
#define DCC_LOAD_ROUTES_DATA(app_, name_, user_data_) DCC_LOAD_LISTENERS_DATA((app_), name_, (user_data_))
#define DCC_ROUTES_MODULE(name_, user_data_) DCC_LISTENERS_MODULE(name_, (user_data_))
#define DCC_ROUTE_MODULE(name_, user_data_) DCC_ROUTES_MODULE(name_, (user_data_))

#define DCC_ROUTE_DECORATED(handler_) DCC_LISTEN_DECORATED_SLASH(handler_)
#define DCC_ROUTE_DECORATED_COMMAND(handler_) DCC_LISTEN_DECORATED_COMMAND(handler_)
#define DCC_ROUTE_DECORATED_SLASH(handler_) DCC_LISTEN_DECORATED_SLASH(handler_)
#define DCC_ROUTE_DECORATED_SUBCOMMAND(handler_) DCC_LISTEN_DECORATED_SUBCOMMAND(handler_)
#define DCC_ROUTE_DECORATED_AUTOCOMPLETE(handler_) DCC_LISTEN_DECORATED_AUTOCOMPLETE(handler_)
#define DCC_ROUTE_DECORATED_USER_CONTEXT_MENU(handler_) DCC_LISTEN_DECORATED_USER_CONTEXT_MENU(handler_)
#define DCC_ROUTE_DECORATED_MESSAGE_CONTEXT_MENU(handler_) DCC_LISTEN_DECORATED_MESSAGE_CONTEXT_MENU(handler_)
#define DCC_ROUTE_DECORATED_COMPONENT(handler_) DCC_LISTEN_DECORATED_COMPONENT(handler_)
#define DCC_ROUTE_DECORATED_BUTTON(handler_) DCC_LISTEN_DECORATED_BUTTON(handler_)
#define DCC_ROUTE_DECORATED_SELECT(handler_) DCC_LISTEN_DECORATED_SELECT(handler_)
#define DCC_ROUTE_DECORATED_MODAL(handler_) DCC_LISTEN_DECORATED_MODAL(handler_)
#define DCC_ROUTE_DECORATED_EVENT(handler_) DCC_LISTEN_DECORATED_EVENT(handler_)
#define DCC_ROUTE_DECORATED_READY(handler_) DCC_LISTEN_DECORATED_READY(handler_)
#define DCC_ROUTE_DECORATED_PREFIX_COMMAND(handler_) DCC_LISTEN_DECORATED_PREFIX_COMMAND(handler_)
#define DCC_ROUTE_DECORATED_TASK(handler_) DCC_LISTEN_DECORATED_TASK(handler_)
#define DCC_ROUTE_DECORATED_TYPED_COMMAND(handler_) DCC_LISTEN_DECORATED_TYPED_COMMAND(handler_)
#define DCC_ROUTE_DECORATED_TYPED_SUBCOMMAND(handler_) DCC_LISTEN_DECORATED_TYPED_SUBCOMMAND(handler_)
#define DCC_ROUTE_DECORATED_TYPED_AUTOCOMPLETE(handler_) DCC_LISTEN_DECORATED_TYPED_AUTOCOMPLETE(handler_)
#define DCC_ROUTE_DECORATED_TYPED_MODAL(handler_) DCC_LISTEN_DECORATED_TYPED_MODAL(handler_)
#define DCC_ROUTE_DECORATED_TYPED_BUTTON(handler_) DCC_LISTEN_DECORATED_TYPED_BUTTON(handler_)
#define DCC_ROUTE_DECORATED_TYPED_SELECT(handler_) DCC_LISTEN_DECORATED_TYPED_SELECT(handler_)

#define DCC_ROUTE_COMMAND(name_, description_, handler_) DCC_LISTEN_COMMAND((name_), (description_), (handler_))
#define DCC_ROUTE_COMMAND_DATA(name_, description_, handler_, user_data_) \
    DCC_LISTEN_COMMAND_DATA((name_), (description_), (handler_), (user_data_))
#define DCC_ROUTE_COMMAND_GUARDED(name_, description_, handler_, ...) \
    DCC_LISTEN_SLASH_GUARDED((name_), (description_), (handler_), __VA_ARGS__)
#define DCC_ROUTE_COMMAND_DATA_GUARDED(name_, description_, handler_, user_data_, ...) \
    DCC_LISTEN_SLASH_DATA_GUARDED((name_), (description_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_ROUTE_SLASH(name_, description_, handler_) DCC_LISTEN_SLASH((name_), (description_), (handler_))
#define DCC_ROUTE_SLASH_DATA(name_, description_, handler_, user_data_) \
    DCC_LISTEN_SLASH_DATA((name_), (description_), (handler_), (user_data_))
#define DCC_ROUTE_SLASH_GUARDED(name_, description_, handler_, ...) \
    DCC_LISTEN_SLASH_GUARDED((name_), (description_), (handler_), __VA_ARGS__)
#define DCC_ROUTE_SLASH_DATA_GUARDED(name_, description_, handler_, user_data_, ...) \
    DCC_LISTEN_SLASH_DATA_GUARDED((name_), (description_), (handler_), (user_data_), __VA_ARGS__)

#define DCC_ROUTE_COMMAND_PARAMS(name_, description_, args_type_, handler_, params_) \
    DCC_LISTEN_COMMAND_PARAMS((name_), (description_), args_type_, (handler_), params_)
#define DCC_ROUTE_COMMAND_PARAMS_DATA(name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_COMMAND_PARAMS_DATA((name_), (description_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_COMMAND_PARAMS_VALIDATED(name_, description_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_COMMAND_PARAMS_VALIDATED((name_), (description_), args_type_, (handler_), params_, (validators_))
#define DCC_ROUTE_COMMAND_PARAMS_DATA_VALIDATED( \
    name_, description_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_COMMAND_PARAMS_DATA_VALIDATED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ROUTE_COMMAND_PARAMS_GUARDED(name_, description_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_COMMAND_PARAMS_GUARDED((name_), (description_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_ROUTE_COMMAND_PARAMS_DATA_GUARDED( \
    name_, description_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_COMMAND_PARAMS_DATA_GUARDED( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_COMMAND_FN(handler_, description_, args_type_, params_) \
    DCC_LISTEN_COMMAND_FN(handler_, (description_), args_type_, params_)
#define DCC_ROUTE_COMMAND_FN_DATA(handler_, description_, args_type_, user_data_, params_) \
    DCC_LISTEN_COMMAND_FN_DATA(handler_, (description_), args_type_, (user_data_), params_)
#define DCC_ROUTE_COMMAND_FN_VALIDATED(handler_, description_, args_type_, params_, validators_) \
    DCC_LISTEN_COMMAND_FN_VALIDATED(handler_, (description_), args_type_, params_, (validators_))
#define DCC_ROUTE_COMMAND_FN_DATA_VALIDATED(handler_, description_, args_type_, user_data_, params_, validators_) \
    DCC_LISTEN_COMMAND_FN_DATA_VALIDATED(handler_, (description_), args_type_, (user_data_), params_, (validators_))
#define DCC_ROUTE_COMMAND_FN_GUARDED(handler_, description_, args_type_, params_, ...) \
    DCC_LISTEN_COMMAND_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__)
#define DCC_ROUTE_COMMAND_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_COMMAND_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__)

#define DCC_ROUTE_TYPED_COMMAND(route_) DCC_LISTEN_TYPED_SLASH(route_)
#define DCC_ROUTE_TYPED_SLASH(route_) DCC_LISTEN_TYPED_SLASH(route_)
#define DCC_ROUTE_TYPED_SUBCOMMAND(route_) DCC_LISTEN_TYPED_SUBCOMMAND(route_)
#define DCC_ROUTE_TYPED_AUTOCOMPLETE(route_) DCC_LISTEN_TYPED_AUTOCOMPLETE(route_)
#define DCC_ROUTE_TYPED_MODAL(route_) DCC_LISTEN_TYPED_MODAL(route_)
#define DCC_ROUTE_TYPED_BUTTON(route_) DCC_LISTEN_TYPED_BUTTON(route_)
#define DCC_ROUTE_TYPED_SELECT(route_) DCC_LISTEN_TYPED_SELECT(route_)

#define DCC_ROUTE_SUBCOMMAND(command_name_, subcommand_path_, handler_) \
    DCC_LISTEN_SUBCOMMAND((command_name_), (subcommand_path_), (handler_))
#define DCC_ROUTE_SUBCOMMAND_DATA(command_name_, subcommand_path_, handler_, user_data_) \
    DCC_LISTEN_SUBCOMMAND_DATA((command_name_), (subcommand_path_), (handler_), (user_data_))
#define DCC_ROUTE_SUBCOMMAND_PARAMS(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_LISTEN_SUBCOMMAND_PARAMS((command_name_), (subcommand_path_), args_type_, (handler_), params_)
#define DCC_ROUTE_SUBCOMMAND_PARAMS_DATA(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_SUBCOMMAND_PARAMS_DATA( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_ROUTE_SUBCOMMAND_PARAMS_GUARDED(command_name_, subcommand_path_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_SUBCOMMAND_PARAMS_GUARDED( \
        (command_name_), (subcommand_path_), args_type_, (handler_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SUBCOMMAND_PARAMS_DATA_GUARDED( \
    command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_SUBCOMMAND_PARAMS_DATA_GUARDED( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SUBCOMMAND_IN_PARAMS(command_name_, group_, subcommand_, args_type_, handler_, params_) \
    DCC_LISTEN_SUBCOMMAND_IN_PARAMS((command_name_), group_, subcommand_, args_type_, (handler_), params_)
#define DCC_ROUTE_SUBCOMMAND_IN_PARAMS_DATA( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_SUBCOMMAND_IN_PARAMS_DATA( \
        (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_ \
    )
#define DCC_ROUTE_SUBCOMMAND_IN_PARAMS_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, params_, ... \
) \
    DCC_LISTEN_SUBCOMMAND_IN_PARAMS_GUARDED( \
        (command_name_), group_, subcommand_, args_type_, (handler_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SUBCOMMAND_IN_PARAMS_DATA_GUARDED( \
    command_name_, group_, subcommand_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_SUBCOMMAND_IN_PARAMS_DATA_GUARDED( \
        (command_name_), group_, subcommand_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SUBCOMMAND_FN(command_name_, handler_, args_type_, params_) \
    DCC_LISTEN_SUBCOMMAND_FN((command_name_), handler_, args_type_, params_)
#define DCC_ROUTE_SUBCOMMAND_FN_DATA(command_name_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_SUBCOMMAND_FN_DATA((command_name_), handler_, args_type_, (user_data_), params_)
#define DCC_ROUTE_SUBCOMMAND_FN_GUARDED(command_name_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_SUBCOMMAND_FN_GUARDED((command_name_), handler_, args_type_, params_, __VA_ARGS__)
#define DCC_ROUTE_SUBCOMMAND_FN_DATA_GUARDED(command_name_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_SUBCOMMAND_FN_DATA_GUARDED( \
        (command_name_), handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SUBCOMMAND_IN_FN(command_name_, group_, handler_, args_type_, params_) \
    DCC_LISTEN_SUBCOMMAND_IN_FN((command_name_), group_, handler_, args_type_, params_)
#define DCC_ROUTE_SUBCOMMAND_IN_FN_DATA(command_name_, group_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_SUBCOMMAND_IN_FN_DATA((command_name_), group_, handler_, args_type_, (user_data_), params_)
#define DCC_ROUTE_SUBCOMMAND_IN_FN_GUARDED(command_name_, group_, handler_, args_type_, params_, ...) \
    DCC_LISTEN_SUBCOMMAND_IN_FN_GUARDED((command_name_), group_, handler_, args_type_, params_, __VA_ARGS__)
#define DCC_ROUTE_SUBCOMMAND_IN_FN_DATA_GUARDED(command_name_, group_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_SUBCOMMAND_IN_FN_DATA_GUARDED( \
        (command_name_), group_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_ROUTE_AUTOCOMPLETE(command_name_, handler_) DCC_LISTEN_AUTOCOMPLETE((command_name_), (handler_))
#define DCC_ROUTE_AUTOCOMPLETE_DATA(command_name_, handler_, user_data_) \
    DCC_LISTEN_AUTOCOMPLETE_DATA((command_name_), (handler_), (user_data_))
#define DCC_ROUTE_AUTOCOMPLETE_PARAMS(command_name_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE(DCC_AUTOCOMPLETE_PARAMS((command_name_), args_type_, (handler_), params_))
#define DCC_ROUTE_AUTOCOMPLETE_PARAMS_DATA(command_name_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_AUTOCOMPLETE_PARAMS_DATA((command_name_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_ROUTE_AUTOCOMPLETE_PARAMS_GUARDED(command_name_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_AUTOCOMPLETE_PARAMS_DATA_GUARDED((command_name_), args_type_, (handler_), NULL, params_, __VA_ARGS__) \
    )
#define DCC_ROUTE_AUTOCOMPLETE_PARAMS_DATA_GUARDED(command_name_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_AUTOCOMPLETE( \
        DCC_AUTOCOMPLETE_PARAMS_DATA_GUARDED( \
            (command_name_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )

#define DCC_ROUTE_USER_CONTEXT_MENU(name_, handler_) DCC_LISTEN_USER_CONTEXT_MENU((name_), (handler_))
#define DCC_ROUTE_USER_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_LISTEN_USER_CONTEXT_MENU_DATA((name_), (handler_), (user_data_))
#define DCC_ROUTE_MESSAGE_CONTEXT_MENU(name_, handler_) DCC_LISTEN_MESSAGE_CONTEXT_MENU((name_), (handler_))
#define DCC_ROUTE_MESSAGE_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_LISTEN_MESSAGE_CONTEXT_MENU_DATA((name_), (handler_), (user_data_))

#define DCC_ROUTE_BUTTON(custom_id_, handler_) DCC_LISTEN_BUTTON((custom_id_), (handler_))
#define DCC_ROUTE_BUTTON_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_BUTTON_DATA((custom_id_), (handler_), (user_data_))
#define DCC_ROUTE_BUTTON_PREFIX(custom_id_prefix_, handler_) DCC_LISTEN_BUTTON_PREFIX((custom_id_prefix_), (handler_))
#define DCC_ROUTE_BUTTON_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_BUTTON_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_))
#define DCC_ROUTE_BUTTON_NS(namespace_, action_, handler_) DCC_LISTEN_BUTTON_NS(namespace_, action_, (handler_))
#define DCC_ROUTE_BUTTON_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_BUTTON_NS_DATA(namespace_, action_, (handler_), (user_data_))
#define DCC_ROUTE_BUTTON_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_BUTTON_NS_PREFIX(namespace_, action_prefix_, (handler_))
#define DCC_ROUTE_BUTTON_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_BUTTON_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_))

#define DCC_ROUTE_BUTTON_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_BUTTON_PARAMS((custom_id_), args_type_, (handler_), params_)
#define DCC_ROUTE_BUTTON_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_BUTTON_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_BUTTON_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_BUTTON_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_))
#define DCC_ROUTE_BUTTON_PARAMS_DATA_VALIDATED(custom_id_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_LISTEN_BUTTON_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), (user_data_), params_, (validators_))
#define DCC_ROUTE_BUTTON_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_BUTTON_PARAMS_GUARDED((custom_id_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_ROUTE_BUTTON_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_BUTTON_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__)
#define DCC_ROUTE_BUTTON_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_BUTTON_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_)
#define DCC_ROUTE_BUTTON_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_BUTTON_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_BUTTON_PREFIX_PARAMS_DATA_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_BUTTON_PREFIX_PARAMS_DATA_GUARDED( \
        (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_BUTTON_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_BUTTON_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_)
#define DCC_ROUTE_BUTTON_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_BUTTON_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_BUTTON_NS_PARAMS_DATA_GUARDED(namespace_, action_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_BUTTON_NS_PARAMS_DATA_GUARDED( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_BUTTON_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_)
#define DCC_ROUTE_BUTTON_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS_DATA(namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_ROUTE_SELECT(custom_id_, handler_) DCC_LISTEN_SELECT((custom_id_), (handler_))
#define DCC_ROUTE_SELECT_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_SELECT_DATA((custom_id_), (handler_), (user_data_))
#define DCC_ROUTE_SELECT_PREFIX(custom_id_prefix_, handler_) DCC_LISTEN_SELECT_PREFIX((custom_id_prefix_), (handler_))
#define DCC_ROUTE_SELECT_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_SELECT_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_))
#define DCC_ROUTE_SELECT_NS(namespace_, action_, handler_) DCC_LISTEN_SELECT_NS(namespace_, action_, (handler_))
#define DCC_ROUTE_SELECT_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_SELECT_NS_DATA(namespace_, action_, (handler_), (user_data_))
#define DCC_ROUTE_SELECT_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_SELECT_NS_PREFIX(namespace_, action_prefix_, (handler_))
#define DCC_ROUTE_SELECT_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_SELECT_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_))
#define DCC_ROUTE_SELECT_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_SELECT_PARAMS((custom_id_), args_type_, (handler_), params_)
#define DCC_ROUTE_SELECT_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_SELECT_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_SELECT_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_SELECT_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_))
#define DCC_ROUTE_SELECT_PARAMS_DATA_VALIDATED(custom_id_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_LISTEN_SELECT_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), (user_data_), params_, (validators_))
#define DCC_ROUTE_SELECT_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_SELECT_PARAMS_GUARDED((custom_id_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_ROUTE_SELECT_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_SELECT_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__)
#define DCC_ROUTE_SELECT_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_SELECT_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_)
#define DCC_ROUTE_SELECT_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_SELECT_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_SELECT_PREFIX_PARAMS_DATA_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_SELECT_PREFIX_PARAMS_DATA_GUARDED( \
        (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SELECT_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_SELECT_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_)
#define DCC_ROUTE_SELECT_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_SELECT_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_SELECT_NS_PARAMS_DATA_GUARDED(namespace_, action_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_SELECT_NS_PARAMS_DATA_GUARDED( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_SELECT_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_SELECT_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_)
#define DCC_ROUTE_SELECT_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_SELECT_NS_PREFIX_PARAMS_DATA(namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_ROUTE_MODAL(custom_id_, handler_) DCC_LISTEN_MODAL((custom_id_), (handler_))
#define DCC_ROUTE_MODAL_DATA(custom_id_, handler_, user_data_) \
    DCC_LISTEN_MODAL_DATA((custom_id_), (handler_), (user_data_))
#define DCC_ROUTE_MODAL_PREFIX(custom_id_prefix_, handler_) DCC_LISTEN_MODAL_PREFIX((custom_id_prefix_), (handler_))
#define DCC_ROUTE_MODAL_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_LISTEN_MODAL_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_))
#define DCC_ROUTE_MODAL_NS(namespace_, action_, handler_) DCC_LISTEN_MODAL_NS(namespace_, action_, (handler_))
#define DCC_ROUTE_MODAL_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_LISTEN_MODAL_NS_DATA(namespace_, action_, (handler_), (user_data_))
#define DCC_ROUTE_MODAL_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_LISTEN_MODAL_NS_PREFIX(namespace_, action_prefix_, (handler_))
#define DCC_ROUTE_MODAL_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_LISTEN_MODAL_NS_PREFIX_DATA(namespace_, action_prefix_, (handler_), (user_data_))
#define DCC_ROUTE_MODAL_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_MODAL_PARAMS((custom_id_), args_type_, (handler_), params_)
#define DCC_ROUTE_MODAL_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_MODAL_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_MODAL_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_MODAL_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_))
#define DCC_ROUTE_MODAL_PARAMS_DATA_VALIDATED(custom_id_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_LISTEN_MODAL_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), (user_data_), params_, (validators_))
#define DCC_ROUTE_MODAL_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_PARAMS_GUARDED((custom_id_), args_type_, (handler_), params_, __VA_ARGS__)
#define DCC_ROUTE_MODAL_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_MODAL_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__)
#define DCC_ROUTE_MODAL_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_MODAL_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_)
#define DCC_ROUTE_MODAL_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_MODAL_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_MODAL_PREFIX_PARAMS_DATA_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_MODAL_PREFIX_PARAMS_DATA_GUARDED( \
        (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_MODAL_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_MODAL_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_)
#define DCC_ROUTE_MODAL_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_MODAL_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_MODAL_NS_PARAMS_DATA_GUARDED(namespace_, action_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_MODAL_NS_PARAMS_DATA_GUARDED( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_MODAL_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_MODAL_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_)
#define DCC_ROUTE_MODAL_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_MODAL_NS_PREFIX_PARAMS_DATA(namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_)
#define DCC_ROUTE_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )
#define DCC_ROUTE_MODAL_FLOW(flow_) DCC_LISTEN_MODAL_FLOW(flow_)
#define DCC_ROUTE_MODAL_V2_FLOW_NS_PARAMS(namespace_, action_, title_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_V2_FLOW_NS_PARAMS(namespace_, action_, (title_), args_type_, (handler_), params_, __VA_ARGS__)

#define DCC_ROUTE_EVENT(type_, handler_) DCC_LISTEN_EVENT((type_), (handler_))
#define DCC_ROUTE_EVENT_DATA(type_, handler_, user_data_) DCC_LISTEN_EVENT_DATA((type_), (handler_), (user_data_))
#define DCC_ROUTE_READY(handler_) DCC_LISTEN_READY(handler_)
#define DCC_ROUTE_READY_DATA(handler_, user_data_) DCC_LISTEN_READY_DATA(handler_, (user_data_))
#define DCC_ROUTE_READY_ONCE(handler_) DCC_LISTEN_READY_ONCE(handler_)
#define DCC_ROUTE_READY_ONCE_DATA(handler_, user_data_) DCC_LISTEN_READY_ONCE_DATA(handler_, (user_data_))
#define DCC_ROUTE_MESSAGE_CREATE(handler_) DCC_LISTEN_MESSAGE_CREATE(handler_)
#define DCC_ROUTE_MESSAGE_CREATE_DATA(handler_, user_data_) DCC_LISTEN_MESSAGE_CREATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_MEMBER_JOIN(handler_) DCC_LISTEN_MEMBER_JOIN(handler_)
#define DCC_ROUTE_MEMBER_JOIN_DATA(handler_, user_data_) DCC_LISTEN_MEMBER_JOIN_DATA(handler_, (user_data_))
#define DCC_ROUTE_MEMBER_ADD(handler_) DCC_ROUTE_MEMBER_JOIN(handler_)
#define DCC_ROUTE_MEMBER_ADD_DATA(handler_, user_data_) DCC_ROUTE_MEMBER_JOIN_DATA(handler_, (user_data_))
#define DCC_ROUTE_MEMBER_UPDATE(handler_) DCC_LISTEN_MEMBER_UPDATE(handler_)
#define DCC_ROUTE_MEMBER_UPDATE_DATA(handler_, user_data_) DCC_LISTEN_MEMBER_UPDATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_MEMBER_REMOVE(handler_) DCC_LISTEN_MEMBER_REMOVE(handler_)
#define DCC_ROUTE_MEMBER_REMOVE_DATA(handler_, user_data_) DCC_LISTEN_MEMBER_REMOVE_DATA(handler_, (user_data_))
#define DCC_ROUTE_GUILD_CREATE(handler_) DCC_LISTEN_GUILD_CREATE(handler_)
#define DCC_ROUTE_GUILD_CREATE_DATA(handler_, user_data_) DCC_LISTEN_GUILD_CREATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_GUILD_UPDATE(handler_) DCC_LISTEN_GUILD_UPDATE(handler_)
#define DCC_ROUTE_GUILD_UPDATE_DATA(handler_, user_data_) DCC_LISTEN_GUILD_UPDATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_GUILD_DELETE(handler_) DCC_LISTEN_GUILD_DELETE(handler_)
#define DCC_ROUTE_GUILD_DELETE_DATA(handler_, user_data_) DCC_LISTEN_GUILD_DELETE_DATA(handler_, (user_data_))
#define DCC_ROUTE_CHANNEL_CREATE(handler_) DCC_LISTEN_CHANNEL_CREATE(handler_)
#define DCC_ROUTE_CHANNEL_CREATE_DATA(handler_, user_data_) DCC_LISTEN_CHANNEL_CREATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_CHANNEL_UPDATE(handler_) DCC_LISTEN_CHANNEL_UPDATE(handler_)
#define DCC_ROUTE_CHANNEL_UPDATE_DATA(handler_, user_data_) DCC_LISTEN_CHANNEL_UPDATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_CHANNEL_DELETE(handler_) DCC_LISTEN_CHANNEL_DELETE(handler_)
#define DCC_ROUTE_CHANNEL_DELETE_DATA(handler_, user_data_) DCC_LISTEN_CHANNEL_DELETE_DATA(handler_, (user_data_))
#define DCC_ROUTE_THREAD_CREATE(handler_) DCC_LISTEN_THREAD_CREATE(handler_)
#define DCC_ROUTE_THREAD_CREATE_DATA(handler_, user_data_) DCC_LISTEN_THREAD_CREATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_THREAD_UPDATE(handler_) DCC_LISTEN_THREAD_UPDATE(handler_)
#define DCC_ROUTE_THREAD_UPDATE_DATA(handler_, user_data_) DCC_LISTEN_THREAD_UPDATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_THREAD_DELETE(handler_) DCC_LISTEN_THREAD_DELETE(handler_)
#define DCC_ROUTE_THREAD_DELETE_DATA(handler_, user_data_) DCC_LISTEN_THREAD_DELETE_DATA(handler_, (user_data_))
#define DCC_ROUTE_REACTION_ADD(handler_) DCC_LISTEN_REACTION_ADD(handler_)
#define DCC_ROUTE_REACTION_ADD_DATA(handler_, user_data_) DCC_LISTEN_REACTION_ADD_DATA(handler_, (user_data_))
#define DCC_ROUTE_REACTION_REMOVE(handler_) DCC_LISTEN_REACTION_REMOVE(handler_)
#define DCC_ROUTE_REACTION_REMOVE_DATA(handler_, user_data_) DCC_LISTEN_REACTION_REMOVE_DATA(handler_, (user_data_))
#define DCC_ROUTE_VOICE_STATE_UPDATE(handler_) DCC_LISTEN_VOICE_STATE_UPDATE(handler_)
#define DCC_ROUTE_VOICE_STATE_UPDATE_DATA(handler_, user_data_) \
    DCC_LISTEN_VOICE_STATE_UPDATE_DATA(handler_, (user_data_))
#define DCC_ROUTE_TYPING_START(handler_) DCC_LISTEN_TYPING_START(handler_)
#define DCC_ROUTE_TYPING_START_DATA(handler_, user_data_) DCC_LISTEN_TYPING_START_DATA(handler_, (user_data_))
#define DCC_ROUTE_PRESENCE_UPDATE(handler_) DCC_LISTEN_PRESENCE_UPDATE(handler_)
#define DCC_ROUTE_PRESENCE_UPDATE_DATA(handler_, user_data_) DCC_LISTEN_PRESENCE_UPDATE_DATA(handler_, (user_data_))

#define DCC_ROUTE_PREFIX_COMMAND(prefix_, name_, handler_) DCC_LISTEN_PREFIX_COMMAND((prefix_), (name_), (handler_))
#define DCC_ROUTE_PREFIX_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_LISTEN_PREFIX_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_))
#define DCC_ROUTE_MESSAGE_COMMAND(prefix_, name_, handler_) DCC_LISTEN_MESSAGE_COMMAND((prefix_), (name_), (handler_))
#define DCC_ROUTE_MESSAGE_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_LISTEN_MESSAGE_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_))

#define DCC_ROUTE_TASK_MS(interval_ms_, handler_) DCC_LISTEN_TASK_MS((interval_ms_), (handler_))
#define DCC_ROUTE_TASK_MS_DATA(interval_ms_, handler_, user_data_) \
    DCC_LISTEN_TASK_MS_DATA((interval_ms_), (handler_), (user_data_))
#define DCC_ROUTE_TASK_SECONDS(interval_seconds_, handler_) DCC_LISTEN_TASK_SECONDS((interval_seconds_), (handler_))
#define DCC_ROUTE_TASK_SECONDS_DATA(interval_seconds_, handler_, user_data_) \
    DCC_LISTEN_TASK_SECONDS_DATA((interval_seconds_), (handler_), (user_data_))
#define DCC_ROUTE_TASK_DAILY_KST(hhmm_, handler_) DCC_LISTEN_TASK_DAILY_KST((hhmm_), (handler_))
#define DCC_ROUTE_TASK_DAILY_KST_DATA(hhmm_, handler_, user_data_) \
    DCC_LISTEN_TASK_DAILY_KST_DATA((hhmm_), (handler_), (user_data_))

#define DCC_ROUTE_COMMAND_SCHEMA(command_) DCC_LISTEN_COMMAND_SCHEMA(command_)
#define DCC_ROUTE_COMMAND_BUILDER(command_) DCC_LISTEN_COMMAND_BUILDER(command_)
#define DCC_ROUTE_SCHEMA(command_) DCC_LISTEN_SCHEMA(command_)

#endif
