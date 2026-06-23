#ifndef DCC_SUGAR_APP_EXTENSION_H
#define DCC_SUGAR_APP_EXTENSION_H

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

#define DCC_EXTENSION_MIDDLEWARE(middleware_, user_data_) \
    ((dcc_app_extension_middleware_t){ \
        .size = sizeof(dcc_app_extension_middleware_t), \
        .middleware = (middleware_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_SLASH(name_, description_, handler_, user_data_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (name_), \
        .description = (description_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_SLASH_ROUTE(name_, description_, handler_, user_data_, out_route_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (name_), \
        .description = (description_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_EXTENSION_SLASH_POLICY(name_, description_, handler_, user_data_, policy_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (name_), \
        .description = (description_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_EXTENSION_SLASH_ROUTE_POLICY(name_, description_, handler_, user_data_, out_route_, policy_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (name_), \
        .description = (description_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

#define DCC_EXTENSION_SLASH_BUILDER(builder_, handler_, user_data_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .builder = (builder_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_SLASH_BUILDER_ROUTE(builder_, handler_, user_data_, out_route_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .builder = (builder_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_EXTENSION_SLASH_BUILDER_POLICY(builder_, handler_, user_data_, policy_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .builder = (builder_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_EXTENSION_SLASH_BUILDER_ROUTE_POLICY(builder_, handler_, user_data_, out_route_, policy_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .builder = (builder_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

#define DCC_SLASH(name_, description_, handler_) \
    DCC_EXTENSION_SLASH((name_), (description_), (handler_), NULL)

#define DCC_SLASH_DATA(name_, description_, handler_, user_data_) \
    DCC_EXTENSION_SLASH((name_), (description_), (handler_), (user_data_))

#define DCC_SLASH_POLICY(name_, description_, handler_, policy_) \
    DCC_EXTENSION_SLASH_POLICY((name_), (description_), (handler_), NULL, (policy_))

#define DCC_SLASH_DATA_POLICY(name_, description_, handler_, user_data_, policy_) \
    DCC_EXTENSION_SLASH_POLICY((name_), (description_), (handler_), (user_data_), (policy_))

#define DCC_SLASH_HANDLER(name_, description_, handler_) \
    DCC_SLASH((name_), (description_), (handler_))

#define DCC_SLASH_HANDLER_DATA(name_, description_, handler_, user_data_) \
    DCC_SLASH_DATA((name_), (description_), (handler_), (user_data_))

#define DCC_SLASH_BUILDER(builder_, handler_) \
    DCC_EXTENSION_SLASH_BUILDER((builder_), (handler_), NULL)

#define DCC_SLASH_BUILDER_DATA(builder_, handler_, user_data_) \
    DCC_EXTENSION_SLASH_BUILDER((builder_), (handler_), (user_data_))

#define DCC_SLASH_BUILDER_VALUE(builder_, handler_) \
    DCC_SLASH_BUILDER(&((dcc_application_command_builder_t[]){ (builder_) })[0], (handler_))

#define DCC_SLASH_BUILDER_VALUE_DATA(builder_, handler_, user_data_) \
    DCC_SLASH_BUILDER_DATA(&((dcc_application_command_builder_t[]){ (builder_) })[0], (handler_), (user_data_))

#define DCC_SLASH_BUILDER_POLICY(builder_, handler_, policy_) \
    DCC_EXTENSION_SLASH_BUILDER_POLICY((builder_), (handler_), NULL, (policy_))

#define DCC_SLASH_BUILDER_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_EXTENSION_SLASH_BUILDER_POLICY((builder_), (handler_), (user_data_), (policy_))

#define DCC_SLASH_BUILDER_VALUE_POLICY(builder_, handler_, policy_) \
    DCC_SLASH_BUILDER_POLICY(&((dcc_application_command_builder_t[]){ (builder_) })[0], (handler_), (policy_))

#define DCC_SLASH_BUILDER_VALUE_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_SLASH_BUILDER_DATA_POLICY( \
        &((dcc_application_command_builder_t[]){ (builder_) })[0], \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_COMMAND(name_, description_, handler_) \
    DCC_SLASH((name_), (description_), (handler_))

#define DCC_COMMAND_DATA(name_, description_, handler_, user_data_) \
    DCC_SLASH_DATA((name_), (description_), (handler_), (user_data_))

#define DCC_COMMAND_POLICY(name_, description_, handler_, policy_) \
    DCC_SLASH_POLICY((name_), (description_), (handler_), (policy_))

#define DCC_COMMAND_DATA_POLICY(name_, description_, handler_, user_data_, policy_) \
    DCC_SLASH_DATA_POLICY((name_), (description_), (handler_), (user_data_), (policy_))

#define DCC_COMMAND_BUILDER(builder_, handler_) \
    DCC_SLASH_BUILDER((builder_), (handler_))

#define DCC_COMMAND_BUILDER_DATA(builder_, handler_, user_data_) \
    DCC_SLASH_BUILDER_DATA((builder_), (handler_), (user_data_))

#define DCC_COMMAND_BUILDER_POLICY(builder_, handler_, policy_) \
    DCC_SLASH_BUILDER_POLICY((builder_), (handler_), (policy_))

#define DCC_COMMAND_BUILDER_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_SLASH_BUILDER_DATA_POLICY((builder_), (handler_), (user_data_), (policy_))

#define DCC_ON_SLASH(name_, description_, handler_) \
    DCC_SLASH((name_), (description_), (handler_))

#define DCC_ON_SLASH_DATA(name_, description_, handler_, user_data_) \
    DCC_SLASH_DATA((name_), (description_), (handler_), (user_data_))

#define DCC_ON_SLASH_POLICY(name_, description_, handler_, policy_) \
    DCC_SLASH_POLICY((name_), (description_), (handler_), (policy_))

#define DCC_ON_SLASH_DATA_POLICY(name_, description_, handler_, user_data_, policy_) \
    DCC_SLASH_DATA_POLICY((name_), (description_), (handler_), (user_data_), (policy_))

#define DCC_ON_COMMAND(name_, description_, handler_) \
    DCC_COMMAND((name_), (description_), (handler_))

#define DCC_ON_COMMAND_DATA(name_, description_, handler_, user_data_) \
    DCC_COMMAND_DATA((name_), (description_), (handler_), (user_data_))

#define DCC_ON_COMMAND_POLICY(name_, description_, handler_, policy_) \
    DCC_COMMAND_POLICY((name_), (description_), (handler_), (policy_))

#define DCC_ON_COMMAND_DATA_POLICY(name_, description_, handler_, user_data_, policy_) \
    DCC_COMMAND_DATA_POLICY((name_), (description_), (handler_), (user_data_), (policy_))

#define DCC_ON_COMMAND_BUILDER(builder_, handler_) \
    DCC_COMMAND_BUILDER((builder_), (handler_))

#define DCC_ON_COMMAND_BUILDER_DATA(builder_, handler_, user_data_) \
    DCC_COMMAND_BUILDER_DATA((builder_), (handler_), (user_data_))

#define DCC_ON_COMMAND_BUILDER_POLICY(builder_, handler_, policy_) \
    DCC_COMMAND_BUILDER_POLICY((builder_), (handler_), (policy_))

#define DCC_ON_COMMAND_BUILDER_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_COMMAND_BUILDER_DATA_POLICY((builder_), (handler_), (user_data_), (policy_))

#define DCC_EXTENSION_SUBCOMMAND(command_name_, subcommand_path_, handler_, user_data_) \
    ((dcc_app_extension_subcommand_t){ \
        .size = sizeof(dcc_app_extension_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_SUBCOMMAND_POLICY(command_name_, subcommand_path_, handler_, user_data_, policy_) \
    ((dcc_app_extension_subcommand_t){ \
        .size = sizeof(dcc_app_extension_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_SUBCOMMAND(command_name_, subcommand_path_, handler_) \
    DCC_EXTENSION_SUBCOMMAND((command_name_), (subcommand_path_), (handler_), NULL)

#define DCC_SUBCOMMAND_DATA(command_name_, subcommand_path_, handler_, user_data_) \
    DCC_EXTENSION_SUBCOMMAND((command_name_), (subcommand_path_), (handler_), (user_data_))

#define DCC_SUBCOMMAND_POLICY(command_name_, subcommand_path_, handler_, policy_) \
    DCC_EXTENSION_SUBCOMMAND_POLICY((command_name_), (subcommand_path_), (handler_), NULL, (policy_))

#define DCC_SUBCOMMAND_DATA_POLICY(command_name_, subcommand_path_, handler_, user_data_, policy_) \
    DCC_EXTENSION_SUBCOMMAND_POLICY((command_name_), (subcommand_path_), (handler_), (user_data_), (policy_))

#define DCC_ON_SUBCOMMAND(command_name_, subcommand_path_, handler_) \
    DCC_SUBCOMMAND((command_name_), (subcommand_path_), (handler_))

#define DCC_ON_SUBCOMMAND_DATA(command_name_, subcommand_path_, handler_, user_data_) \
    DCC_SUBCOMMAND_DATA((command_name_), (subcommand_path_), (handler_), (user_data_))

#define DCC_ON_SUBCOMMAND_POLICY(command_name_, subcommand_path_, handler_, policy_) \
    DCC_SUBCOMMAND_POLICY((command_name_), (subcommand_path_), (handler_), (policy_))

#define DCC_ON_SUBCOMMAND_DATA_POLICY(command_name_, subcommand_path_, handler_, user_data_, policy_) \
    DCC_SUBCOMMAND_DATA_POLICY((command_name_), (subcommand_path_), (handler_), (user_data_), (policy_))

#define DCC_EXTENSION_AUTOCOMPLETE(name_, handler_, user_data_) \
    ((dcc_app_extension_autocomplete_t){ \
        .size = sizeof(dcc_app_extension_autocomplete_t), \
        .name = (name_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_AUTOCOMPLETE_POLICY(name_, handler_, user_data_, policy_) \
    ((dcc_app_extension_autocomplete_t){ \
        .size = sizeof(dcc_app_extension_autocomplete_t), \
        .name = (name_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_AUTOCOMPLETE_HANDLER(name_, handler_) \
    DCC_EXTENSION_AUTOCOMPLETE((name_), (handler_), NULL)

#define DCC_AUTOCOMPLETE_HANDLER_DATA(name_, handler_, user_data_) \
    DCC_EXTENSION_AUTOCOMPLETE((name_), (handler_), (user_data_))

#define DCC_AUTOCOMPLETE_HANDLER_POLICY(name_, handler_, policy_) \
    DCC_EXTENSION_AUTOCOMPLETE_POLICY((name_), (handler_), NULL, (policy_))

#define DCC_AUTOCOMPLETE_HANDLER_DATA_POLICY(name_, handler_, user_data_, policy_) \
    DCC_EXTENSION_AUTOCOMPLETE_POLICY((name_), (handler_), (user_data_), (policy_))

#define DCC_ON_AUTOCOMPLETE(name_, handler_) \
    DCC_AUTOCOMPLETE_HANDLER((name_), (handler_))

#define DCC_ON_AUTOCOMPLETE_DATA(name_, handler_, user_data_) \
    DCC_AUTOCOMPLETE_HANDLER_DATA((name_), (handler_), (user_data_))

#define DCC_ON_AUTOCOMPLETE_POLICY(name_, handler_, policy_) \
    DCC_AUTOCOMPLETE_HANDLER_POLICY((name_), (handler_), (policy_))

#define DCC_ON_AUTOCOMPLETE_DATA_POLICY(name_, handler_, user_data_, policy_) \
    DCC_AUTOCOMPLETE_HANDLER_DATA_POLICY((name_), (handler_), (user_data_), (policy_))

#define DCC_EXTENSION_CONTEXT_MENU(type_, name_, handler_, user_data_) \
    ((dcc_app_extension_context_menu_t){ \
        .size = sizeof(dcc_app_extension_context_menu_t), \
        .type = (type_), \
        .name = (name_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_CONTEXT_MENU_ROUTE(type_, name_, handler_, user_data_, out_route_) \
    ((dcc_app_extension_context_menu_t){ \
        .size = sizeof(dcc_app_extension_context_menu_t), \
        .type = (type_), \
        .name = (name_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_EXTENSION_CONTEXT_MENU_POLICY(type_, name_, handler_, user_data_, policy_) \
    ((dcc_app_extension_context_menu_t){ \
        .size = sizeof(dcc_app_extension_context_menu_t), \
        .type = (type_), \
        .name = (name_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_EXTENSION_CONTEXT_MENU_ROUTE_POLICY(type_, name_, handler_, user_data_, out_route_, policy_) \
    ((dcc_app_extension_context_menu_t){ \
        .size = sizeof(dcc_app_extension_context_menu_t), \
        .type = (type_), \
        .name = (name_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

#define DCC_USER_CONTEXT_MENU(name_, handler_) \
    DCC_EXTENSION_CONTEXT_MENU(DCC_APP_EXTENSION_USER_CONTEXT_MENU, (name_), (handler_), NULL)
#define DCC_USER_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_EXTENSION_CONTEXT_MENU(DCC_APP_EXTENSION_USER_CONTEXT_MENU, (name_), (handler_), (user_data_))
#define DCC_USER_CONTEXT_MENU_POLICY(name_, handler_, policy_) \
    DCC_EXTENSION_CONTEXT_MENU_POLICY(DCC_APP_EXTENSION_USER_CONTEXT_MENU, (name_), (handler_), NULL, (policy_))
#define DCC_USER_CONTEXT_MENU_DATA_POLICY(name_, handler_, user_data_, policy_) \
    DCC_EXTENSION_CONTEXT_MENU_POLICY(DCC_APP_EXTENSION_USER_CONTEXT_MENU, (name_), (handler_), (user_data_), (policy_))
#define DCC_USER_CONTEXT_MENU_ROUTE(name_, handler_, out_route_) \
    DCC_EXTENSION_CONTEXT_MENU_ROUTE(DCC_APP_EXTENSION_USER_CONTEXT_MENU, (name_), (handler_), NULL, (out_route_))
#define DCC_USER_CONTEXT_MENU_ROUTE_DATA(name_, handler_, user_data_, out_route_) \
    DCC_EXTENSION_CONTEXT_MENU_ROUTE(DCC_APP_EXTENSION_USER_CONTEXT_MENU, (name_), (handler_), (user_data_), (out_route_))
#define DCC_USER_CONTEXT_MENU_ROUTE_POLICY(name_, handler_, out_route_, policy_) \
    DCC_EXTENSION_CONTEXT_MENU_ROUTE_POLICY( \
        DCC_APP_EXTENSION_USER_CONTEXT_MENU, \
        (name_), \
        (handler_), \
        NULL, \
        (out_route_), \
        (policy_) \
    )
#define DCC_USER_CONTEXT_MENU_ROUTE_DATA_POLICY(name_, handler_, user_data_, out_route_, policy_) \
    DCC_EXTENSION_CONTEXT_MENU_ROUTE_POLICY( \
        DCC_APP_EXTENSION_USER_CONTEXT_MENU, \
        (name_), \
        (handler_), \
        (user_data_), \
        (out_route_), \
        (policy_) \
    )

#define DCC_MESSAGE_CONTEXT_MENU(name_, handler_) \
    DCC_EXTENSION_CONTEXT_MENU(DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, (name_), (handler_), NULL)
#define DCC_MESSAGE_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_EXTENSION_CONTEXT_MENU(DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, (name_), (handler_), (user_data_))
#define DCC_MESSAGE_CONTEXT_MENU_POLICY(name_, handler_, policy_) \
    DCC_EXTENSION_CONTEXT_MENU_POLICY(DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, (name_), (handler_), NULL, (policy_))
#define DCC_MESSAGE_CONTEXT_MENU_DATA_POLICY(name_, handler_, user_data_, policy_) \
    DCC_EXTENSION_CONTEXT_MENU_POLICY(DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, (name_), (handler_), (user_data_), (policy_))
#define DCC_MESSAGE_CONTEXT_MENU_ROUTE(name_, handler_, out_route_) \
    DCC_EXTENSION_CONTEXT_MENU_ROUTE(DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, (name_), (handler_), NULL, (out_route_))
#define DCC_MESSAGE_CONTEXT_MENU_ROUTE_DATA(name_, handler_, user_data_, out_route_) \
    DCC_EXTENSION_CONTEXT_MENU_ROUTE(DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, (name_), (handler_), (user_data_), (out_route_))
#define DCC_MESSAGE_CONTEXT_MENU_ROUTE_POLICY(name_, handler_, out_route_, policy_) \
    DCC_EXTENSION_CONTEXT_MENU_ROUTE_POLICY( \
        DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, \
        (name_), \
        (handler_), \
        NULL, \
        (out_route_), \
        (policy_) \
    )
#define DCC_MESSAGE_CONTEXT_MENU_ROUTE_DATA_POLICY(name_, handler_, user_data_, out_route_, policy_) \
    DCC_EXTENSION_CONTEXT_MENU_ROUTE_POLICY( \
        DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, \
        (name_), \
        (handler_), \
        (user_data_), \
        (out_route_), \
        (policy_) \
    )

#define DCC_ON_USER_CONTEXT_MENU(name_, handler_) \
    DCC_USER_CONTEXT_MENU((name_), (handler_))
#define DCC_ON_USER_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_USER_CONTEXT_MENU_DATA((name_), (handler_), (user_data_))
#define DCC_ON_USER_CONTEXT_MENU_POLICY(name_, handler_, policy_) \
    DCC_USER_CONTEXT_MENU_POLICY((name_), (handler_), (policy_))
#define DCC_ON_USER_CONTEXT_MENU_DATA_POLICY(name_, handler_, user_data_, policy_) \
    DCC_USER_CONTEXT_MENU_DATA_POLICY((name_), (handler_), (user_data_), (policy_))

#define DCC_ON_MESSAGE_CONTEXT_MENU(name_, handler_) \
    DCC_MESSAGE_CONTEXT_MENU((name_), (handler_))
#define DCC_ON_MESSAGE_CONTEXT_MENU_DATA(name_, handler_, user_data_) \
    DCC_MESSAGE_CONTEXT_MENU_DATA((name_), (handler_), (user_data_))
#define DCC_ON_MESSAGE_CONTEXT_MENU_POLICY(name_, handler_, policy_) \
    DCC_MESSAGE_CONTEXT_MENU_POLICY((name_), (handler_), (policy_))
#define DCC_ON_MESSAGE_CONTEXT_MENU_DATA_POLICY(name_, handler_, user_data_, policy_) \
    DCC_MESSAGE_CONTEXT_MENU_DATA_POLICY((name_), (handler_), (user_data_), (policy_))

#define DCC_EXTENSION_COMPONENT_ROUTE(type_, custom_id_, handler_, user_data_) \
    ((dcc_app_extension_component_route_t){ \
        .size = sizeof(dcc_app_extension_component_route_t), \
        .type = (type_), \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_COMPONENT_ROUTE_ID(type_, custom_id_, handler_, user_data_, out_route_) \
    ((dcc_app_extension_component_route_t){ \
        .size = sizeof(dcc_app_extension_component_route_t), \
        .type = (type_), \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_EXTENSION_COMPONENT_ROUTE_POLICY(type_, custom_id_, handler_, user_data_, policy_) \
    ((dcc_app_extension_component_route_t){ \
        .size = sizeof(dcc_app_extension_component_route_t), \
        .type = (type_), \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_EXTENSION_COMPONENT_ROUTE_ID_POLICY(type_, custom_id_, handler_, user_data_, out_route_, policy_) \
    ((dcc_app_extension_component_route_t){ \
        .size = sizeof(dcc_app_extension_component_route_t), \
        .type = (type_), \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

#define DCC_EXTENSION_BUTTON(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_BUTTON, (custom_id_), (handler_), (user_data_))
#define DCC_EXTENSION_BUTTON_PREFIX(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_BUTTON_PREFIX, (custom_id_prefix_), (handler_), (user_data_))
#define DCC_EXTENSION_SELECT(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_SELECT, (custom_id_), (handler_), (user_data_))
#define DCC_EXTENSION_SELECT_PREFIX(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_SELECT_PREFIX, (custom_id_prefix_), (handler_), (user_data_))
#define DCC_EXTENSION_MODAL(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_MODAL, (custom_id_), (handler_), (user_data_))
#define DCC_EXTENSION_MODAL_PREFIX(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_COMPONENT_ROUTE(DCC_APP_EXTENSION_MODAL_PREFIX, (custom_id_prefix_), (handler_), (user_data_))

#define DCC_EXTENSION_BUTTON_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY(DCC_APP_EXTENSION_BUTTON, (custom_id_), (handler_), (user_data_), (policy_))
#define DCC_EXTENSION_BUTTON_PREFIX_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY( \
        DCC_APP_EXTENSION_BUTTON_PREFIX, \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )
#define DCC_EXTENSION_SELECT_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY(DCC_APP_EXTENSION_SELECT, (custom_id_), (handler_), (user_data_), (policy_))
#define DCC_EXTENSION_SELECT_PREFIX_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY( \
        DCC_APP_EXTENSION_SELECT_PREFIX, \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )
#define DCC_EXTENSION_MODAL_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY(DCC_APP_EXTENSION_MODAL, (custom_id_), (handler_), (user_data_), (policy_))
#define DCC_EXTENSION_MODAL_PREFIX_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_COMPONENT_ROUTE_POLICY( \
        DCC_APP_EXTENSION_MODAL_PREFIX, \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_BUTTON_HANDLER(custom_id_, handler_) \
    DCC_EXTENSION_BUTTON((custom_id_), (handler_), NULL)
#define DCC_BUTTON_HANDLER_DATA(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_BUTTON((custom_id_), (handler_), (user_data_))
#define DCC_BUTTON_HANDLER_POLICY(custom_id_, handler_, policy_) \
    DCC_EXTENSION_BUTTON_POLICY((custom_id_), (handler_), NULL, (policy_))
#define DCC_BUTTON_HANDLER_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_BUTTON_POLICY((custom_id_), (handler_), (user_data_), (policy_))
#define DCC_BUTTON_PREFIX_HANDLER(custom_id_prefix_, handler_) \
    DCC_EXTENSION_BUTTON_PREFIX((custom_id_prefix_), (handler_), NULL)
#define DCC_BUTTON_PREFIX_HANDLER_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_BUTTON_PREFIX((custom_id_prefix_), (handler_), (user_data_))
#define DCC_BUTTON_PREFIX_HANDLER_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_EXTENSION_BUTTON_PREFIX_POLICY((custom_id_prefix_), (handler_), NULL, (policy_))
#define DCC_BUTTON_PREFIX_HANDLER_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_BUTTON_PREFIX_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))
#define DCC_SELECT_HANDLER(custom_id_, handler_) \
    DCC_EXTENSION_SELECT((custom_id_), (handler_), NULL)
#define DCC_SELECT_HANDLER_DATA(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_SELECT((custom_id_), (handler_), (user_data_))
#define DCC_SELECT_HANDLER_POLICY(custom_id_, handler_, policy_) \
    DCC_EXTENSION_SELECT_POLICY((custom_id_), (handler_), NULL, (policy_))
#define DCC_SELECT_HANDLER_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_SELECT_POLICY((custom_id_), (handler_), (user_data_), (policy_))
#define DCC_SELECT_PREFIX_HANDLER(custom_id_prefix_, handler_) \
    DCC_EXTENSION_SELECT_PREFIX((custom_id_prefix_), (handler_), NULL)
#define DCC_SELECT_PREFIX_HANDLER_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_SELECT_PREFIX((custom_id_prefix_), (handler_), (user_data_))
#define DCC_SELECT_PREFIX_HANDLER_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_EXTENSION_SELECT_PREFIX_POLICY((custom_id_prefix_), (handler_), NULL, (policy_))
#define DCC_SELECT_PREFIX_HANDLER_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_SELECT_PREFIX_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))
#define DCC_MODAL_HANDLER(custom_id_, handler_) \
    DCC_EXTENSION_MODAL((custom_id_), (handler_), NULL)
#define DCC_MODAL_HANDLER_DATA(custom_id_, handler_, user_data_) \
    DCC_EXTENSION_MODAL((custom_id_), (handler_), (user_data_))
#define DCC_MODAL_HANDLER_POLICY(custom_id_, handler_, policy_) \
    DCC_EXTENSION_MODAL_POLICY((custom_id_), (handler_), NULL, (policy_))
#define DCC_MODAL_HANDLER_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_EXTENSION_MODAL_POLICY((custom_id_), (handler_), (user_data_), (policy_))
#define DCC_MODAL_PREFIX_HANDLER(custom_id_prefix_, handler_) \
    DCC_EXTENSION_MODAL_PREFIX((custom_id_prefix_), (handler_), NULL)
#define DCC_MODAL_PREFIX_HANDLER_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_EXTENSION_MODAL_PREFIX((custom_id_prefix_), (handler_), (user_data_))
#define DCC_MODAL_PREFIX_HANDLER_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_EXTENSION_MODAL_PREFIX_POLICY((custom_id_prefix_), (handler_), NULL, (policy_))
#define DCC_MODAL_PREFIX_HANDLER_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_EXTENSION_MODAL_PREFIX_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_BUTTON(custom_id_, handler_) \
    DCC_BUTTON_HANDLER((custom_id_), (handler_))

#define DCC_BUTTON_DATA(custom_id_, handler_, user_data_) \
    DCC_BUTTON_HANDLER_DATA((custom_id_), (handler_), (user_data_))

#define DCC_BUTTON_POLICY(custom_id_, handler_, policy_) \
    DCC_BUTTON_HANDLER_POLICY((custom_id_), (handler_), (policy_))

#define DCC_BUTTON_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_BUTTON_HANDLER_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_BUTTON_PREFIX(custom_id_prefix_, handler_) \
    DCC_BUTTON_PREFIX_HANDLER((custom_id_prefix_), (handler_))

#define DCC_BUTTON_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_BUTTON_PREFIX_HANDLER_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_BUTTON_PREFIX_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_BUTTON_PREFIX_HANDLER_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_BUTTON_PREFIX_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_BUTTON_PREFIX_HANDLER_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_SELECT(custom_id_, handler_) \
    DCC_SELECT_HANDLER((custom_id_), (handler_))

#define DCC_SELECT_DATA(custom_id_, handler_, user_data_) \
    DCC_SELECT_HANDLER_DATA((custom_id_), (handler_), (user_data_))

#define DCC_SELECT_POLICY(custom_id_, handler_, policy_) \
    DCC_SELECT_HANDLER_POLICY((custom_id_), (handler_), (policy_))

#define DCC_SELECT_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_SELECT_HANDLER_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_SELECT_PREFIX(custom_id_prefix_, handler_) \
    DCC_SELECT_PREFIX_HANDLER((custom_id_prefix_), (handler_))

#define DCC_SELECT_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_SELECT_PREFIX_HANDLER_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_SELECT_PREFIX_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_SELECT_PREFIX_HANDLER_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_SELECT_PREFIX_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_SELECT_PREFIX_HANDLER_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_MODAL_ROUTE(custom_id_, handler_) \
    DCC_MODAL_HANDLER((custom_id_), (handler_))

#define DCC_MODAL_ROUTE_DATA(custom_id_, handler_, user_data_) \
    DCC_MODAL_HANDLER_DATA((custom_id_), (handler_), (user_data_))

#define DCC_MODAL_ROUTE_POLICY(custom_id_, handler_, policy_) \
    DCC_MODAL_HANDLER_POLICY((custom_id_), (handler_), (policy_))

#define DCC_MODAL_ROUTE_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_MODAL_HANDLER_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_MODAL_PREFIX_ROUTE(custom_id_prefix_, handler_) \
    DCC_MODAL_PREFIX_HANDLER((custom_id_prefix_), (handler_))

#define DCC_MODAL_PREFIX_ROUTE_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_MODAL_PREFIX_HANDLER_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_MODAL_PREFIX_ROUTE_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_MODAL_PREFIX_HANDLER_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_MODAL_PREFIX_ROUTE_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_MODAL_PREFIX_HANDLER_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_ON_BUTTON(custom_id_, handler_) \
    DCC_BUTTON((custom_id_), (handler_))

#define DCC_ON_BUTTON_DATA(custom_id_, handler_, user_data_) \
    DCC_BUTTON_DATA((custom_id_), (handler_), (user_data_))

#define DCC_ON_BUTTON_POLICY(custom_id_, handler_, policy_) \
    DCC_BUTTON_POLICY((custom_id_), (handler_), (policy_))

#define DCC_ON_BUTTON_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_BUTTON_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_ON_BUTTON_PREFIX(custom_id_prefix_, handler_) \
    DCC_BUTTON_PREFIX((custom_id_prefix_), (handler_))

#define DCC_ON_BUTTON_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_BUTTON_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_ON_BUTTON_PREFIX_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_BUTTON_PREFIX_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_ON_BUTTON_PREFIX_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_BUTTON_PREFIX_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_ON_SELECT(custom_id_, handler_) \
    DCC_SELECT((custom_id_), (handler_))

#define DCC_ON_SELECT_DATA(custom_id_, handler_, user_data_) \
    DCC_SELECT_DATA((custom_id_), (handler_), (user_data_))

#define DCC_ON_SELECT_POLICY(custom_id_, handler_, policy_) \
    DCC_SELECT_POLICY((custom_id_), (handler_), (policy_))

#define DCC_ON_SELECT_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_SELECT_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_ON_SELECT_PREFIX(custom_id_prefix_, handler_) \
    DCC_SELECT_PREFIX((custom_id_prefix_), (handler_))

#define DCC_ON_SELECT_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_SELECT_PREFIX_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_ON_SELECT_PREFIX_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_SELECT_PREFIX_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_ON_SELECT_PREFIX_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_SELECT_PREFIX_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#define DCC_ON_MODAL(custom_id_, handler_) \
    DCC_MODAL_ROUTE((custom_id_), (handler_))

#define DCC_ON_MODAL_DATA(custom_id_, handler_, user_data_) \
    DCC_MODAL_ROUTE_DATA((custom_id_), (handler_), (user_data_))

#define DCC_ON_MODAL_POLICY(custom_id_, handler_, policy_) \
    DCC_MODAL_ROUTE_POLICY((custom_id_), (handler_), (policy_))

#define DCC_ON_MODAL_DATA_POLICY(custom_id_, handler_, user_data_, policy_) \
    DCC_MODAL_ROUTE_DATA_POLICY((custom_id_), (handler_), (user_data_), (policy_))

#define DCC_ON_MODAL_PREFIX(custom_id_prefix_, handler_) \
    DCC_MODAL_PREFIX_ROUTE((custom_id_prefix_), (handler_))

#define DCC_ON_MODAL_PREFIX_DATA(custom_id_prefix_, handler_, user_data_) \
    DCC_MODAL_PREFIX_ROUTE_DATA((custom_id_prefix_), (handler_), (user_data_))

#define DCC_ON_MODAL_PREFIX_POLICY(custom_id_prefix_, handler_, policy_) \
    DCC_MODAL_PREFIX_ROUTE_POLICY((custom_id_prefix_), (handler_), (policy_))

#define DCC_ON_MODAL_PREFIX_DATA_POLICY(custom_id_prefix_, handler_, user_data_, policy_) \
    DCC_MODAL_PREFIX_ROUTE_DATA_POLICY((custom_id_prefix_), (handler_), (user_data_), (policy_))

#ifndef DCC_COMPONENT_ID
#define DCC_COMPONENT_ID(namespace_, action_) namespace_ "." action_
#endif
#ifndef DCC_COMPONENT_PREFIX
#define DCC_COMPONENT_PREFIX(namespace_) namespace_ "."
#endif
#ifndef DCC_COMPONENT_ACTION_PREFIX
#define DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_) namespace_ "." action_prefix_
#endif

#define DCC_ON_BUTTON_NS(namespace_, action_, handler_) \
    DCC_ON_BUTTON(DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_ON_BUTTON_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_ON_BUTTON_DATA(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_ON_BUTTON_NS_POLICY(namespace_, action_, handler_, policy_) \
    DCC_ON_BUTTON_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_ON_BUTTON_NS_DATA_POLICY(namespace_, action_, handler_, user_data_, policy_) \
    DCC_ON_BUTTON_DATA_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_), (policy_))
#define DCC_ON_BUTTON_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_ON_BUTTON_PREFIX(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_ON_BUTTON_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_ON_BUTTON_PREFIX_DATA(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (user_data_))
#define DCC_ON_BUTTON_NS_PREFIX_POLICY(namespace_, action_prefix_, handler_, policy_) \
    DCC_ON_BUTTON_PREFIX_POLICY(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (policy_))
#define DCC_ON_BUTTON_NS_PREFIX_DATA_POLICY(namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_ON_BUTTON_PREFIX_DATA_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_ON_SELECT_NS(namespace_, action_, handler_) \
    DCC_ON_SELECT(DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_ON_SELECT_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_ON_SELECT_DATA(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_ON_SELECT_NS_POLICY(namespace_, action_, handler_, policy_) \
    DCC_ON_SELECT_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_ON_SELECT_NS_DATA_POLICY(namespace_, action_, handler_, user_data_, policy_) \
    DCC_ON_SELECT_DATA_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_), (policy_))
#define DCC_ON_SELECT_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_ON_SELECT_PREFIX(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_ON_SELECT_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_ON_SELECT_PREFIX_DATA(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (user_data_))
#define DCC_ON_SELECT_NS_PREFIX_POLICY(namespace_, action_prefix_, handler_, policy_) \
    DCC_ON_SELECT_PREFIX_POLICY(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (policy_))
#define DCC_ON_SELECT_NS_PREFIX_DATA_POLICY(namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_ON_SELECT_PREFIX_DATA_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_ON_MODAL_NS(namespace_, action_, handler_) \
    DCC_ON_MODAL(DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_ON_MODAL_NS_DATA(namespace_, action_, handler_, user_data_) \
    DCC_ON_MODAL_DATA(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_ON_MODAL_NS_POLICY(namespace_, action_, handler_, policy_) \
    DCC_ON_MODAL_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_ON_MODAL_NS_DATA_POLICY(namespace_, action_, handler_, user_data_, policy_) \
    DCC_ON_MODAL_DATA_POLICY(DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_), (policy_))
#define DCC_ON_MODAL_NS_PREFIX(namespace_, action_prefix_, handler_) \
    DCC_ON_MODAL_PREFIX(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_ON_MODAL_NS_PREFIX_DATA(namespace_, action_prefix_, handler_, user_data_) \
    DCC_ON_MODAL_PREFIX_DATA(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (user_data_))
#define DCC_ON_MODAL_NS_PREFIX_POLICY(namespace_, action_prefix_, handler_, policy_) \
    DCC_ON_MODAL_PREFIX_POLICY(DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_), (policy_))
#define DCC_ON_MODAL_NS_PREFIX_DATA_POLICY(namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_ON_MODAL_PREFIX_DATA_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_ON_SLASH_GUARDED(name_, description_, handler_, ...) \
    DCC_ON_SLASH_POLICY((name_), (description_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_SLASH_DATA_GUARDED(name_, description_, handler_, user_data_, ...) \
    DCC_ON_SLASH_DATA_POLICY( \
        (name_), \
        (description_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_COMMAND_GUARDED(name_, description_, handler_, ...) \
    DCC_ON_COMMAND_POLICY((name_), (description_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_COMMAND_DATA_GUARDED(name_, description_, handler_, user_data_, ...) \
    DCC_ON_COMMAND_DATA_POLICY( \
        (name_), \
        (description_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_SUBCOMMAND_GUARDED(command_name_, subcommand_path_, handler_, ...) \
    DCC_ON_SUBCOMMAND_POLICY( \
        (command_name_), \
        (subcommand_path_), \
        (handler_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_SUBCOMMAND_DATA_GUARDED(command_name_, subcommand_path_, handler_, user_data_, ...) \
    DCC_ON_SUBCOMMAND_DATA_POLICY( \
        (command_name_), \
        (subcommand_path_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_AUTOCOMPLETE_GUARDED(name_, handler_, ...) \
    DCC_ON_AUTOCOMPLETE_POLICY((name_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_AUTOCOMPLETE_DATA_GUARDED(name_, handler_, user_data_, ...) \
    DCC_ON_AUTOCOMPLETE_DATA_POLICY((name_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_USER_CONTEXT_MENU_GUARDED(name_, handler_, ...) \
    DCC_ON_USER_CONTEXT_MENU_POLICY((name_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_USER_CONTEXT_MENU_DATA_GUARDED(name_, handler_, user_data_, ...) \
    DCC_ON_USER_CONTEXT_MENU_DATA_POLICY((name_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_MESSAGE_CONTEXT_MENU_GUARDED(name_, handler_, ...) \
    DCC_ON_MESSAGE_CONTEXT_MENU_POLICY((name_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_MESSAGE_CONTEXT_MENU_DATA_GUARDED(name_, handler_, user_data_, ...) \
    DCC_ON_MESSAGE_CONTEXT_MENU_DATA_POLICY((name_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_BUTTON_GUARDED(custom_id_, handler_, ...) \
    DCC_ON_BUTTON_POLICY((custom_id_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_BUTTON_DATA_GUARDED(custom_id_, handler_, user_data_, ...) \
    DCC_ON_BUTTON_DATA_POLICY((custom_id_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_BUTTON_PREFIX_GUARDED(custom_id_prefix_, handler_, ...) \
    DCC_ON_BUTTON_PREFIX_POLICY((custom_id_prefix_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_BUTTON_PREFIX_DATA_GUARDED(custom_id_prefix_, handler_, user_data_, ...) \
    DCC_ON_BUTTON_PREFIX_DATA_POLICY( \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_BUTTON_NS_GUARDED(namespace_, action_, handler_, ...) \
    DCC_ON_BUTTON_NS_POLICY(namespace_, action_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_BUTTON_NS_DATA_GUARDED(namespace_, action_, handler_, user_data_, ...) \
    DCC_ON_BUTTON_NS_DATA_POLICY(namespace_, action_, (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_BUTTON_NS_PREFIX_GUARDED(namespace_, action_prefix_, handler_, ...) \
    DCC_ON_BUTTON_NS_PREFIX_POLICY(namespace_, action_prefix_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_BUTTON_NS_PREFIX_DATA_GUARDED(namespace_, action_prefix_, handler_, user_data_, ...) \
    DCC_ON_BUTTON_NS_PREFIX_DATA_POLICY( \
        namespace_, \
        action_prefix_, \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_SELECT_GUARDED(custom_id_, handler_, ...) \
    DCC_ON_SELECT_POLICY((custom_id_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_SELECT_DATA_GUARDED(custom_id_, handler_, user_data_, ...) \
    DCC_ON_SELECT_DATA_POLICY((custom_id_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_SELECT_PREFIX_GUARDED(custom_id_prefix_, handler_, ...) \
    DCC_ON_SELECT_PREFIX_POLICY((custom_id_prefix_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_SELECT_PREFIX_DATA_GUARDED(custom_id_prefix_, handler_, user_data_, ...) \
    DCC_ON_SELECT_PREFIX_DATA_POLICY( \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_SELECT_NS_GUARDED(namespace_, action_, handler_, ...) \
    DCC_ON_SELECT_NS_POLICY(namespace_, action_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_SELECT_NS_DATA_GUARDED(namespace_, action_, handler_, user_data_, ...) \
    DCC_ON_SELECT_NS_DATA_POLICY(namespace_, action_, (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_SELECT_NS_PREFIX_GUARDED(namespace_, action_prefix_, handler_, ...) \
    DCC_ON_SELECT_NS_PREFIX_POLICY(namespace_, action_prefix_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_SELECT_NS_PREFIX_DATA_GUARDED(namespace_, action_prefix_, handler_, user_data_, ...) \
    DCC_ON_SELECT_NS_PREFIX_DATA_POLICY( \
        namespace_, \
        action_prefix_, \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_MODAL_GUARDED(custom_id_, handler_, ...) \
    DCC_ON_MODAL_POLICY((custom_id_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_MODAL_DATA_GUARDED(custom_id_, handler_, user_data_, ...) \
    DCC_ON_MODAL_DATA_POLICY((custom_id_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_MODAL_PREFIX_GUARDED(custom_id_prefix_, handler_, ...) \
    DCC_ON_MODAL_PREFIX_POLICY((custom_id_prefix_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_MODAL_PREFIX_DATA_GUARDED(custom_id_prefix_, handler_, user_data_, ...) \
    DCC_ON_MODAL_PREFIX_DATA_POLICY( \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_MODAL_NS_GUARDED(namespace_, action_, handler_, ...) \
    DCC_ON_MODAL_NS_POLICY(namespace_, action_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_MODAL_NS_DATA_GUARDED(namespace_, action_, handler_, user_data_, ...) \
    DCC_ON_MODAL_NS_DATA_POLICY(namespace_, action_, (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_MODAL_NS_PREFIX_GUARDED(namespace_, action_prefix_, handler_, ...) \
    DCC_ON_MODAL_NS_PREFIX_POLICY(namespace_, action_prefix_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_ON_MODAL_NS_PREFIX_DATA_GUARDED(namespace_, action_prefix_, handler_, user_data_, ...) \
    DCC_ON_MODAL_NS_PREFIX_DATA_POLICY( \
        namespace_, \
        action_prefix_, \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_COMMAND_GUARDED(name_, description_, handler_, ...) \
    DCC_ON_COMMAND_GUARDED((name_), (description_), (handler_), __VA_ARGS__)
#define DCC_COMMAND_DATA_GUARDED(name_, description_, handler_, user_data_, ...) \
    DCC_ON_COMMAND_DATA_GUARDED((name_), (description_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_SUBCOMMAND_GUARDED(command_name_, subcommand_path_, handler_, ...) \
    DCC_ON_SUBCOMMAND_GUARDED((command_name_), (subcommand_path_), (handler_), __VA_ARGS__)
#define DCC_SUBCOMMAND_DATA_GUARDED(command_name_, subcommand_path_, handler_, user_data_, ...) \
    DCC_ON_SUBCOMMAND_DATA_GUARDED((command_name_), (subcommand_path_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_AUTOCOMPLETE_GUARDED(name_, handler_, ...) \
    DCC_ON_AUTOCOMPLETE_GUARDED((name_), (handler_), __VA_ARGS__)
#define DCC_AUTOCOMPLETE_DATA_GUARDED(name_, handler_, user_data_, ...) \
    DCC_ON_AUTOCOMPLETE_DATA_GUARDED((name_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_USER_CONTEXT_MENU_GUARDED(name_, handler_, ...) \
    DCC_ON_USER_CONTEXT_MENU_GUARDED((name_), (handler_), __VA_ARGS__)
#define DCC_USER_CONTEXT_MENU_DATA_GUARDED(name_, handler_, user_data_, ...) \
    DCC_ON_USER_CONTEXT_MENU_DATA_GUARDED((name_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_MESSAGE_CONTEXT_MENU_GUARDED(name_, handler_, ...) \
    DCC_ON_MESSAGE_CONTEXT_MENU_GUARDED((name_), (handler_), __VA_ARGS__)
#define DCC_MESSAGE_CONTEXT_MENU_DATA_GUARDED(name_, handler_, user_data_, ...) \
    DCC_ON_MESSAGE_CONTEXT_MENU_DATA_GUARDED((name_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_BUTTON_GUARDED(custom_id_, handler_, ...) \
    DCC_ON_BUTTON_GUARDED((custom_id_), (handler_), __VA_ARGS__)
#define DCC_BUTTON_DATA_GUARDED(custom_id_, handler_, user_data_, ...) \
    DCC_ON_BUTTON_DATA_GUARDED((custom_id_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_BUTTON_PREFIX_GUARDED(custom_id_prefix_, handler_, ...) \
    DCC_ON_BUTTON_PREFIX_GUARDED((custom_id_prefix_), (handler_), __VA_ARGS__)
#define DCC_BUTTON_PREFIX_DATA_GUARDED(custom_id_prefix_, handler_, user_data_, ...) \
    DCC_ON_BUTTON_PREFIX_DATA_GUARDED((custom_id_prefix_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_SELECT_GUARDED(custom_id_, handler_, ...) \
    DCC_ON_SELECT_GUARDED((custom_id_), (handler_), __VA_ARGS__)
#define DCC_SELECT_DATA_GUARDED(custom_id_, handler_, user_data_, ...) \
    DCC_ON_SELECT_DATA_GUARDED((custom_id_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_SELECT_PREFIX_GUARDED(custom_id_prefix_, handler_, ...) \
    DCC_ON_SELECT_PREFIX_GUARDED((custom_id_prefix_), (handler_), __VA_ARGS__)
#define DCC_SELECT_PREFIX_DATA_GUARDED(custom_id_prefix_, handler_, user_data_, ...) \
    DCC_ON_SELECT_PREFIX_DATA_GUARDED((custom_id_prefix_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_MODAL_GUARDED(custom_id_, handler_, ...) \
    DCC_ON_MODAL_GUARDED((custom_id_), (handler_), __VA_ARGS__)
#define DCC_MODAL_DATA_GUARDED(custom_id_, handler_, user_data_, ...) \
    DCC_ON_MODAL_DATA_GUARDED((custom_id_), (handler_), (user_data_), __VA_ARGS__)
#define DCC_MODAL_PREFIX_GUARDED(custom_id_prefix_, handler_, ...) \
    DCC_ON_MODAL_PREFIX_GUARDED((custom_id_prefix_), (handler_), __VA_ARGS__)
#define DCC_MODAL_PREFIX_DATA_GUARDED(custom_id_prefix_, handler_, user_data_, ...) \
    DCC_ON_MODAL_PREFIX_DATA_GUARDED((custom_id_prefix_), (handler_), (user_data_), __VA_ARGS__)

#define DCC_EXTENSION_EVENT(type_, handler_, user_data_) \
    ((dcc_app_extension_event_t){ \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_EVENT, \
        .event_type = (type_), \
        .event_handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_ON_EVENT(type_, handler_) \
    DCC_EXTENSION_EVENT((type_), (handler_), NULL)

#define DCC_ON_EVENT_DATA(type_, handler_, user_data_) \
    DCC_EXTENSION_EVENT((type_), (handler_), (user_data_))

#define DCC_EVENT(type_, handler_) \
    DCC_ON_EVENT((type_), (handler_))

#define DCC_EVENT_DATA(type_, handler_, user_data_) \
    DCC_ON_EVENT_DATA((type_), (handler_), (user_data_))

#define DCC_EXTENSION_READY(handler_, user_data_) \
    ((dcc_app_extension_event_t){ \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_READY, \
        .ready_handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_READY_ONCE(handler_, user_data_) \
    ((dcc_app_extension_event_t){ \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_READY, \
        .ready_handler = (handler_), \
        .user_data = (user_data_), \
        .once = 1U \
    })

#define DCC_ON_READY(handler_) \
    DCC_EXTENSION_READY((handler_), NULL)

#define DCC_ON_READY_DATA(handler_, user_data_) \
    DCC_EXTENSION_READY((handler_), (user_data_))

#define DCC_READY(handler_) \
    DCC_ON_READY((handler_))

#define DCC_READY_DATA(handler_, user_data_) \
    DCC_ON_READY_DATA((handler_), (user_data_))

#define DCC_ON_READY_ONCE(handler_) \
    DCC_EXTENSION_READY_ONCE((handler_), NULL)

#define DCC_ON_READY_ONCE_DATA(handler_, user_data_) \
    DCC_EXTENSION_READY_ONCE((handler_), (user_data_))

#define DCC_READY_ONCE(handler_) \
    DCC_ON_READY_ONCE((handler_))

#define DCC_READY_ONCE_DATA(handler_, user_data_) \
    DCC_ON_READY_ONCE_DATA((handler_), (user_data_))

#define DCC_EXTENSION_MESSAGE_CREATE(handler_, user_data_) \
    ((dcc_app_extension_event_t){ \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_CREATE, \
        .message_handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_ON_MESSAGE(handler_) \
    DCC_EXTENSION_MESSAGE_CREATE((handler_), NULL)

#define DCC_ON_MESSAGE_DATA(handler_, user_data_) \
    DCC_EXTENSION_MESSAGE_CREATE((handler_), (user_data_))

#define DCC_MESSAGE_EVENT(handler_) \
    DCC_ON_MESSAGE((handler_))

#define DCC_MESSAGE_EVENT_DATA(handler_, user_data_) \
    DCC_ON_MESSAGE_DATA((handler_), (user_data_))

#define DCC_ON_MESSAGE_CREATE(handler_) \
    DCC_EXTENSION_MESSAGE_CREATE((handler_), NULL)

#define DCC_ON_MESSAGE_CREATE_DATA(handler_, user_data_) \
    DCC_EXTENSION_MESSAGE_CREATE((handler_), (user_data_))

#define DCC_MESSAGE_CREATE(handler_) \
    DCC_ON_MESSAGE_CREATE((handler_))

#define DCC_MESSAGE_CREATE_DATA(handler_, user_data_) \
    DCC_ON_MESSAGE_CREATE_DATA((handler_), (user_data_))

#define DCC_EXTENSION_MESSAGE_UPDATE(handler_, user_data_) \
    ((dcc_app_extension_event_t){ \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_UPDATE, \
        .message_handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_ON_MESSAGE_UPDATE(handler_) \
    DCC_EXTENSION_MESSAGE_UPDATE((handler_), NULL)

#define DCC_ON_MESSAGE_UPDATE_DATA(handler_, user_data_) \
    DCC_EXTENSION_MESSAGE_UPDATE((handler_), (user_data_))

#define DCC_MESSAGE_UPDATE(handler_) \
    DCC_ON_MESSAGE_UPDATE((handler_))

#define DCC_MESSAGE_UPDATE_DATA(handler_, user_data_) \
    DCC_ON_MESSAGE_UPDATE_DATA((handler_), (user_data_))

#define DCC_EXTENSION_MESSAGE_DELETE(handler_, user_data_) \
    ((dcc_app_extension_event_t){ \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_DELETE, \
        .message_handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_ON_MESSAGE_DELETE(handler_) \
    DCC_EXTENSION_MESSAGE_DELETE((handler_), NULL)

#define DCC_ON_MESSAGE_DELETE_DATA(handler_, user_data_) \
    DCC_EXTENSION_MESSAGE_DELETE((handler_), (user_data_))

#define DCC_MESSAGE_DELETE_EVENT(handler_) \
    DCC_ON_MESSAGE_DELETE((handler_))

#define DCC_MESSAGE_DELETE_EVENT_DATA(handler_, user_data_) \
    DCC_ON_MESSAGE_DELETE_DATA((handler_), (user_data_))

#define DCC_EXTENSION_MESSAGE_COMMAND(prefix_, name_, handler_, user_data_) \
    ((dcc_app_extension_message_command_t){ \
        .size = sizeof(dcc_app_extension_message_command_t), \
        .prefix = (prefix_), \
        .name = (name_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_MESSAGE_COMMAND(prefix_, name_, handler_) \
    DCC_EXTENSION_MESSAGE_COMMAND((prefix_), (name_), (handler_), NULL)

#define DCC_MESSAGE_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_EXTENSION_MESSAGE_COMMAND((prefix_), (name_), (handler_), (user_data_))

#define DCC_PREFIX_COMMAND(prefix_, name_, handler_) \
    DCC_MESSAGE_COMMAND((prefix_), (name_), (handler_))

#define DCC_PREFIX_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_MESSAGE_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_))

#define DCC_TEXT_COMMAND(prefix_, name_, handler_) \
    DCC_PREFIX_COMMAND((prefix_), (name_), (handler_))

#define DCC_TEXT_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_PREFIX_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_))

#define DCC_ON_MESSAGE_COMMAND(prefix_, name_, handler_) \
    DCC_MESSAGE_COMMAND((prefix_), (name_), (handler_))

#define DCC_ON_MESSAGE_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_MESSAGE_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_))

#define DCC_ON_PREFIX_COMMAND(prefix_, name_, handler_) \
    DCC_PREFIX_COMMAND((prefix_), (name_), (handler_))

#define DCC_ON_PREFIX_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_PREFIX_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_))

#define DCC_ON_TEXT_COMMAND(prefix_, name_, handler_) \
    DCC_TEXT_COMMAND((prefix_), (name_), (handler_))

#define DCC_ON_TEXT_COMMAND_DATA(prefix_, name_, handler_, user_data_) \
    DCC_TEXT_COMMAND_DATA((prefix_), (name_), (handler_), (user_data_))

#define DCC_MESSAGE_COMMAND_FN_ROUTE(prefix_, handler_) \
    DCC_MESSAGE_COMMAND((prefix_), #handler_, (handler_))

#define DCC_MESSAGE_COMMAND_FN_ROUTE_DATA(prefix_, handler_, user_data_) \
    DCC_MESSAGE_COMMAND_DATA((prefix_), #handler_, (handler_), (user_data_))

#define DCC_PREFIX_COMMAND_FN_ROUTE(prefix_, handler_) \
    DCC_MESSAGE_COMMAND_FN_ROUTE((prefix_), handler_)

#define DCC_PREFIX_COMMAND_FN_ROUTE_DATA(prefix_, handler_, user_data_) \
    DCC_MESSAGE_COMMAND_FN_ROUTE_DATA((prefix_), handler_, (user_data_))

#define DCC_TEXT_COMMAND_FN_ROUTE(prefix_, handler_) \
    DCC_PREFIX_COMMAND_FN_ROUTE((prefix_), handler_)

#define DCC_TEXT_COMMAND_FN_ROUTE_DATA(prefix_, handler_, user_data_) \
    DCC_PREFIX_COMMAND_FN_ROUTE_DATA((prefix_), handler_, (user_data_))

#define DCC_ON_MESSAGE_COMMAND_FN(prefix_, handler_) \
    DCC_MESSAGE_COMMAND_FN_ROUTE((prefix_), handler_)

#define DCC_ON_MESSAGE_COMMAND_FN_DATA(prefix_, handler_, user_data_) \
    DCC_MESSAGE_COMMAND_FN_ROUTE_DATA((prefix_), handler_, (user_data_))

#define DCC_ON_PREFIX_COMMAND_FN(prefix_, handler_) \
    DCC_PREFIX_COMMAND_FN_ROUTE((prefix_), handler_)

#define DCC_ON_PREFIX_COMMAND_FN_DATA(prefix_, handler_, user_data_) \
    DCC_PREFIX_COMMAND_FN_ROUTE_DATA((prefix_), handler_, (user_data_))

#define DCC_ON_TEXT_COMMAND_FN(prefix_, handler_) \
    DCC_TEXT_COMMAND_FN_ROUTE((prefix_), handler_)

#define DCC_ON_TEXT_COMMAND_FN_DATA(prefix_, handler_, user_data_) \
    DCC_TEXT_COMMAND_FN_ROUTE_DATA((prefix_), handler_, (user_data_))

#define DCC_EXTENSION_TASK_MS(interval_ms_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_MS, \
        .interval = (interval_ms_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_MS(interval_ms_, handler_) \
    DCC_EXTENSION_TASK_MS((interval_ms_), (handler_), NULL)

#define DCC_EVERY_MS_DATA(interval_ms_, handler_, user_data_) \
    DCC_EXTENSION_TASK_MS((interval_ms_), (handler_), (user_data_))

#define DCC_TASK_LOOP_MS(interval_ms_, handler_) \
    DCC_EVERY_MS((interval_ms_), (handler_))

#define DCC_TASK_LOOP_MS_DATA(interval_ms_, handler_, user_data_) \
    DCC_EVERY_MS_DATA((interval_ms_), (handler_), (user_data_))

#define DCC_EXTENSION_TASK_SECONDS(interval_seconds_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_SECONDS, \
        .interval = (interval_seconds_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_SECONDS(interval_seconds_, handler_) \
    DCC_EXTENSION_TASK_SECONDS((interval_seconds_), (handler_), NULL)

#define DCC_EVERY_SECONDS_DATA(interval_seconds_, handler_, user_data_) \
    DCC_EXTENSION_TASK_SECONDS((interval_seconds_), (handler_), (user_data_))

#define DCC_TASK_LOOP_SECONDS(interval_seconds_, handler_) \
    DCC_EVERY_SECONDS((interval_seconds_), (handler_))

#define DCC_TASK_LOOP_SECONDS_DATA(interval_seconds_, handler_, user_data_) \
    DCC_EVERY_SECONDS_DATA((interval_seconds_), (handler_), (user_data_))

#define DCC_EXTENSION_TASK_MINUTES(interval_minutes_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_MINUTES, \
        .interval = (interval_minutes_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_MINUTES(interval_minutes_, handler_) \
    DCC_EXTENSION_TASK_MINUTES((interval_minutes_), (handler_), NULL)

#define DCC_EVERY_MINUTES_DATA(interval_minutes_, handler_, user_data_) \
    DCC_EXTENSION_TASK_MINUTES((interval_minutes_), (handler_), (user_data_))

#define DCC_TASK_LOOP_MINUTES(interval_minutes_, handler_) \
    DCC_EVERY_MINUTES((interval_minutes_), (handler_))

#define DCC_TASK_LOOP_MINUTES_DATA(interval_minutes_, handler_, user_data_) \
    DCC_EVERY_MINUTES_DATA((interval_minutes_), (handler_), (user_data_))

#define DCC_EXTENSION_TASK_HOURS(interval_hours_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_HOURS, \
        .interval = (interval_hours_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_HOURS(interval_hours_, handler_) \
    DCC_EXTENSION_TASK_HOURS((interval_hours_), (handler_), NULL)

#define DCC_EVERY_HOURS_DATA(interval_hours_, handler_, user_data_) \
    DCC_EXTENSION_TASK_HOURS((interval_hours_), (handler_), (user_data_))

#define DCC_TASK_LOOP_HOURS(interval_hours_, handler_) \
    DCC_EVERY_HOURS((interval_hours_), (handler_))

#define DCC_TASK_LOOP_HOURS_DATA(interval_hours_, handler_, user_data_) \
    DCC_EVERY_HOURS_DATA((interval_hours_), (handler_), (user_data_))

#define DCC_EXTENSION_TASK_KST(hhmm_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_KST, \
        .hhmm = (hhmm_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_KST(hhmm_, handler_) \
    DCC_EXTENSION_TASK_KST((hhmm_), (handler_), NULL)

#define DCC_EVERY_KST_DATA(hhmm_, handler_, user_data_) \
    DCC_EXTENSION_TASK_KST((hhmm_), (handler_), (user_data_))

#define DCC_TASK_LOOP_KST(hhmm_, handler_) \
    DCC_EVERY_KST((hhmm_), (handler_))

#define DCC_TASK_LOOP_KST_DATA(hhmm_, handler_, user_data_) \
    DCC_EVERY_KST_DATA((hhmm_), (handler_), (user_data_))

#define DCC_TASK_DAILY_KST(hhmm_, handler_) \
    DCC_TASK_LOOP_KST((hhmm_), (handler_))

#define DCC_TASK_DAILY_KST_DATA(hhmm_, handler_, user_data_) \
    DCC_TASK_LOOP_KST_DATA((hhmm_), (handler_), (user_data_))

#define DCC_EXTENSION_TASK_DAY_AT_KST(hour_, minute_, handler_, user_data_) \
    ((dcc_app_extension_task_t){ \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST, \
        .hour = (hour_), \
        .minute = (minute_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EVERY_DAY_AT_KST(hour_, minute_, handler_) \
    DCC_EXTENSION_TASK_DAY_AT_KST((hour_), (minute_), (handler_), NULL)

#define DCC_EVERY_DAY_AT_KST_DATA(hour_, minute_, handler_, user_data_) \
    DCC_EXTENSION_TASK_DAY_AT_KST((hour_), (minute_), (handler_), (user_data_))

#define DCC_TASK_LOOP_DAY_AT_KST(hour_, minute_, handler_) \
    DCC_EVERY_DAY_AT_KST((hour_), (minute_), (handler_))

#define DCC_TASK_LOOP_DAY_AT_KST_DATA(hour_, minute_, handler_, user_data_) \
    DCC_EVERY_DAY_AT_KST_DATA((hour_), (minute_), (handler_), (user_data_))

#define DCC_TASK_DAILY_AT_KST(hour_, minute_, handler_) \
    DCC_TASK_LOOP_DAY_AT_KST((hour_), (minute_), (handler_))

#define DCC_TASK_DAILY_AT_KST_DATA(hour_, minute_, handler_, user_data_) \
    DCC_TASK_LOOP_DAY_AT_KST_DATA((hour_), (minute_), (handler_), (user_data_))

#endif
