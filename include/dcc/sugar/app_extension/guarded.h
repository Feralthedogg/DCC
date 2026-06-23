#ifndef DCC_SUGAR_APP_EXTENSION_GUARDED_H
#define DCC_SUGAR_APP_EXTENSION_GUARDED_H

#include <dcc/sugar/app_extension/slash.h>
#include <dcc/sugar/app_extension/subcommands.h>
#include <dcc/sugar/app_extension/autocomplete.h>
#include <dcc/sugar/app_extension/context_menus.h>
#include <dcc/sugar/app_extension/components.h>

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

#endif /* DCC_SUGAR_APP_EXTENSION_GUARDED_H */
