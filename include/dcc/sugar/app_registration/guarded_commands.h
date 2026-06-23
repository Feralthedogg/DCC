#ifndef DCC_SUGAR_APP_REGISTRATION_GUARDED_COMMANDS_H
#define DCC_SUGAR_APP_REGISTRATION_GUARDED_COMMANDS_H

#include <dcc/sugar/app_registration/policy_namespaced.h>

#define DCC_APP_ON_SLASH_GUARDED(app_, name_, description_, handler_, ...) \
    DCC_APP_ON_SLASH_POLICY((app_), (name_), (description_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SLASH_DATA_GUARDED(app_, name_, description_, handler_, user_data_, ...) \
    DCC_APP_ON_SLASH_DATA_POLICY( \
        (app_), \
        (name_), \
        (description_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_SLASH_BUILDER_GUARDED(app_, builder_, handler_, ...) \
    DCC_APP_ON_SLASH_BUILDER_POLICY((app_), (builder_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SLASH_BUILDER_DATA_GUARDED(app_, builder_, handler_, user_data_, ...) \
    DCC_APP_ON_SLASH_BUILDER_DATA_POLICY( \
        (app_), \
        (builder_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_COMMAND_GUARDED(app_, name_, description_, handler_, ...) \
    DCC_APP_ON_SLASH_GUARDED((app_), (name_), (description_), (handler_), __VA_ARGS__)

#define DCC_APP_ON_COMMAND_DATA_GUARDED(app_, name_, description_, handler_, user_data_, ...) \
    DCC_APP_ON_SLASH_DATA_GUARDED((app_), (name_), (description_), (handler_), (user_data_), __VA_ARGS__)

#define DCC_APP_ON_COMMAND_BUILDER_GUARDED(app_, builder_, handler_, ...) \
    DCC_APP_ON_SLASH_BUILDER_GUARDED((app_), (builder_), (handler_), __VA_ARGS__)

#define DCC_APP_ON_COMMAND_BUILDER_DATA_GUARDED(app_, builder_, handler_, user_data_, ...) \
    DCC_APP_ON_SLASH_BUILDER_DATA_GUARDED((app_), (builder_), (handler_), (user_data_), __VA_ARGS__)

#define DCC_APP_ON_SUBCOMMAND_GUARDED(app_, command_name_, subcommand_path_, handler_, ...) \
    DCC_APP_ON_SUBCOMMAND_POLICY( \
        (app_), \
        (command_name_), \
        (subcommand_path_), \
        (handler_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_SUBCOMMAND_DATA_GUARDED(app_, command_name_, subcommand_path_, handler_, user_data_, ...) \
    DCC_APP_ON_SUBCOMMAND_DATA_POLICY( \
        (app_), \
        (command_name_), \
        (subcommand_path_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_AUTOCOMPLETE_GUARDED(app_, name_, handler_, ...) \
    DCC_APP_ON_AUTOCOMPLETE_POLICY((app_), (name_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_AUTOCOMPLETE_DATA_GUARDED(app_, name_, handler_, user_data_, ...) \
    DCC_APP_ON_AUTOCOMPLETE_DATA_POLICY((app_), (name_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_USER_CONTEXT_MENU_GUARDED(app_, name_, handler_, ...) \
    DCC_APP_ON_USER_CONTEXT_MENU_POLICY((app_), (name_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_USER_CONTEXT_MENU_DATA_GUARDED(app_, name_, handler_, user_data_, ...) \
    DCC_APP_ON_USER_CONTEXT_MENU_DATA_POLICY((app_), (name_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_GUARDED(app_, name_, handler_, ...) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU_POLICY((app_), (name_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA_GUARDED(app_, name_, handler_, user_data_, ...) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA_POLICY((app_), (name_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#endif /* DCC_SUGAR_APP_REGISTRATION_GUARDED_COMMANDS_H */
