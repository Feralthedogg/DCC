#ifndef DCC_SUGAR_APP_REGISTRATION_MESSAGE_COMMANDS_H
#define DCC_SUGAR_APP_REGISTRATION_MESSAGE_COMMANDS_H

#include <dcc/sugar/app_registration/events.h>

#define DCC_APP_ON_MESSAGE_COMMAND(app_, prefix_, name_, handler_) \
    dcc_app_on_message_command((app_), (prefix_), (name_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_COMMAND_DATA(app_, prefix_, name_, handler_, user_data_) \
    dcc_app_on_message_command((app_), (prefix_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_PREFIX_COMMAND(app_, prefix_, name_, handler_) \
    DCC_APP_ON_MESSAGE_COMMAND((app_), (prefix_), (name_), (handler_))
#define DCC_APP_ON_PREFIX_COMMAND_DATA(app_, prefix_, name_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_COMMAND_DATA((app_), (prefix_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_TEXT_COMMAND(app_, prefix_, name_, handler_) \
    DCC_APP_ON_PREFIX_COMMAND((app_), (prefix_), (name_), (handler_))
#define DCC_APP_ON_TEXT_COMMAND_DATA(app_, prefix_, name_, handler_, user_data_) \
    DCC_APP_ON_PREFIX_COMMAND_DATA((app_), (prefix_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_COMMAND_FN(app_, prefix_, handler_) \
    DCC_APP_ON_MESSAGE_COMMAND((app_), (prefix_), #handler_, (handler_))
#define DCC_APP_ON_MESSAGE_COMMAND_FN_DATA(app_, prefix_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_COMMAND_DATA((app_), (prefix_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_PREFIX_COMMAND_FN(app_, prefix_, handler_) \
    DCC_APP_ON_MESSAGE_COMMAND_FN((app_), (prefix_), handler_)
#define DCC_APP_ON_PREFIX_COMMAND_FN_DATA(app_, prefix_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_COMMAND_FN_DATA((app_), (prefix_), handler_, (user_data_))
#define DCC_APP_ON_TEXT_COMMAND_FN(app_, prefix_, handler_) \
    DCC_APP_ON_PREFIX_COMMAND_FN((app_), (prefix_), handler_)
#define DCC_APP_ON_TEXT_COMMAND_FN_DATA(app_, prefix_, handler_, user_data_) \
    DCC_APP_ON_PREFIX_COMMAND_FN_DATA((app_), (prefix_), handler_, (user_data_))

#endif /* DCC_SUGAR_APP_REGISTRATION_MESSAGE_COMMANDS_H */
