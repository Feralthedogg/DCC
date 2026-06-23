#ifndef DCC_SUGAR_APP_EXTENSION_CONTEXT_MENUS_H
#define DCC_SUGAR_APP_EXTENSION_CONTEXT_MENUS_H

#include <dcc/sugar/app_extension/base.h>

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

#endif /* DCC_SUGAR_APP_EXTENSION_CONTEXT_MENUS_H */
