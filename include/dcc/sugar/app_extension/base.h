#ifndef DCC_SUGAR_APP_EXTENSION_BASE_H
#define DCC_SUGAR_APP_EXTENSION_BASE_H

#include <dcc/app.h>
#include <dcc/sugar/route_policy.h>

#define DCC_APP_EXTENSION(name_) \
    ((dcc_app_extension_t){ \
        .size = sizeof(dcc_app_extension_t), \
        .name = (name_) \
    })

#define DCC_COG(name_, ...) \
    ((dcc_app_extension_t){ \
        .size = sizeof(dcc_app_extension_t), \
        .name = (name_), \
        __VA_ARGS__ \
    })

#define DCC_COG_MIDDLEWARE(...) \
    .middlewares = (dcc_app_extension_middleware_t[]){ __VA_ARGS__ }, \
    .middleware_count = sizeof((dcc_app_extension_middleware_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_middleware_t)

#define DCC_COG_SLASHES(...) \
    .slash_commands = (dcc_app_extension_slash_command_t[]){ __VA_ARGS__ }, \
    .slash_command_count = sizeof((dcc_app_extension_slash_command_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_slash_command_t)

#define DCC_COG_COMMANDS(...) \
    DCC_COG_SLASHES(__VA_ARGS__)

#define DCC_COG_SUBCOMMANDS(...) \
    .subcommands = (dcc_app_extension_subcommand_t[]){ __VA_ARGS__ }, \
    .subcommand_count = sizeof((dcc_app_extension_subcommand_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_subcommand_t)

#define DCC_COG_AUTOCOMPLETES(...) \
    .autocompletes = (dcc_app_extension_autocomplete_t[]){ __VA_ARGS__ }, \
    .autocomplete_count = sizeof((dcc_app_extension_autocomplete_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_autocomplete_t)

#define DCC_COG_CONTEXT_MENUS(...) \
    .context_menus = (dcc_app_extension_context_menu_t[]){ __VA_ARGS__ }, \
    .context_menu_count = sizeof((dcc_app_extension_context_menu_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_context_menu_t)

#define DCC_COG_COMPONENTS(...) \
    .component_routes = (dcc_app_extension_component_route_t[]){ __VA_ARGS__ }, \
    .component_route_count = sizeof((dcc_app_extension_component_route_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_component_route_t)

#define DCC_COG_TYPED_SLASHES(...) \
    .typed_slash_commands = (dcc_app_typed_slash_command_t[]){ __VA_ARGS__ }, \
    .typed_slash_command_count = sizeof((dcc_app_typed_slash_command_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_slash_command_t)

#define DCC_COG_TYPED_COMMANDS(...) \
    DCC_COG_TYPED_SLASHES(__VA_ARGS__)
#define DCC_COG_COMMAND_ROUTES(...) \
    DCC_COG_TYPED_COMMANDS(__VA_ARGS__)

#define DCC_COG_TYPED_SUBCOMMANDS(...) \
    .typed_subcommands = (dcc_app_typed_subcommand_t[]){ __VA_ARGS__ }, \
    .typed_subcommand_count = sizeof((dcc_app_typed_subcommand_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_subcommand_t)
#define DCC_COG_SUBCOMMAND_ROUTES(...) \
    DCC_COG_TYPED_SUBCOMMANDS(__VA_ARGS__)

#define DCC_COG_TYPED_AUTOCOMPLETES(...) \
    .typed_autocompletes = (dcc_app_typed_autocomplete_t[]){ __VA_ARGS__ }, \
    .typed_autocomplete_count = sizeof((dcc_app_typed_autocomplete_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_autocomplete_t)
#define DCC_COG_AUTOCOMPLETE_ROUTES(...) \
    DCC_COG_TYPED_AUTOCOMPLETES(__VA_ARGS__)

#define DCC_COG_TYPED_MODALS(...) \
    .typed_modals = (dcc_app_typed_modal_t[]){ __VA_ARGS__ }, \
    .typed_modal_count = sizeof((dcc_app_typed_modal_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_modal_t)
#define DCC_COG_MODAL_ROUTES(...) \
    DCC_COG_TYPED_MODALS(__VA_ARGS__)

#define DCC_COG_TYPED_BUTTONS(...) \
    .typed_buttons = (dcc_app_typed_component_t[]){ __VA_ARGS__ }, \
    .typed_button_count = sizeof((dcc_app_typed_component_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_component_t)
#define DCC_COG_BUTTON_ROUTES(...) \
    DCC_COG_TYPED_BUTTONS(__VA_ARGS__)

#define DCC_COG_TYPED_SELECTS(...) \
    .typed_selects = (dcc_app_typed_component_t[]){ __VA_ARGS__ }, \
    .typed_select_count = sizeof((dcc_app_typed_component_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_component_t)
#define DCC_COG_SELECT_ROUTES(...) \
    DCC_COG_TYPED_SELECTS(__VA_ARGS__)

#define DCC_COG_VIEWS(...) \
    .views = (dcc_app_view_t[]){ __VA_ARGS__ }, \
    .view_count = sizeof((dcc_app_view_t[]){ __VA_ARGS__ }) / sizeof(dcc_app_view_t)

#define DCC_COG_EVENTS(...) \
    .events = (dcc_app_extension_event_t[]){ __VA_ARGS__ }, \
    .event_count = sizeof((dcc_app_extension_event_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_event_t)

#define DCC_COG_MESSAGE_COMMANDS(...) \
    .message_commands = (dcc_app_extension_message_command_t[]){ __VA_ARGS__ }, \
    .message_command_count = sizeof((dcc_app_extension_message_command_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_message_command_t)

#define DCC_COG_TASKS(...) \
    .tasks = (dcc_app_extension_task_t[]){ __VA_ARGS__ }, \
    .task_count = sizeof((dcc_app_extension_task_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_task_t)

#endif /* DCC_SUGAR_APP_EXTENSION_BASE_H */
