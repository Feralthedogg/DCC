#ifndef DCC_SUGAR_APP_REGISTRATION_CONTEXT_MENUS_H
#define DCC_SUGAR_APP_REGISTRATION_CONTEXT_MENUS_H

#include <dcc/sugar/app_registration/base.h>

#define DCC_APP_ON_USER_CONTEXT_MENU(app_, name_, handler_) \
    dcc_app_user_context_menu((app_), (name_), (handler_), NULL)
#define DCC_APP_ON_USER_CONTEXT_MENU_DATA(app_, name_, handler_, user_data_) \
    dcc_app_user_context_menu((app_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU(app_, name_, handler_) \
    dcc_app_message_context_menu((app_), (name_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA(app_, name_, handler_, user_data_) \
    dcc_app_message_context_menu((app_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_USER_CONTEXT_MENU_FN(app_, handler_) \
    DCC_APP_ON_USER_CONTEXT_MENU((app_), #handler_, (handler_))
#define DCC_APP_ON_USER_CONTEXT_MENU_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_USER_CONTEXT_MENU_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU((app_), #handler_, (handler_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA((app_), #handler_, (handler_), (user_data_))

#endif /* DCC_SUGAR_APP_REGISTRATION_CONTEXT_MENUS_H */
