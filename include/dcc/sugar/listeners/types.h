#ifndef DCC_SUGAR_LISTENERS_TYPES_H
#define DCC_SUGAR_LISTENERS_TYPES_H

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


#endif
