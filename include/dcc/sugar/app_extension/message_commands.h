#ifndef DCC_SUGAR_APP_EXTENSION_MESSAGE_COMMANDS_H
#define DCC_SUGAR_APP_EXTENSION_MESSAGE_COMMANDS_H

#include <dcc/sugar/app_extension/base.h>

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

#endif /* DCC_SUGAR_APP_EXTENSION_MESSAGE_COMMANDS_H */
