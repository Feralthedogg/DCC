#ifndef DCC_SUGAR_DECORATORS_MESSAGE_COMMANDS_H
#define DCC_SUGAR_DECORATORS_MESSAGE_COMMANDS_H

#include <dcc/sugar/decorators/base.h>

#define DCC_DECORATE_MESSAGE_COMMAND(handler_, prefix_, command_name_) \
    static void handler_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const char *args, \
        const dcc_event_t *event, \
        void *user_data \
    ); \
    static const dcc_app_extension_message_command_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_message_command_t), \
        .prefix = (prefix_), \
        .name = (command_name_), \
        .handler = (handler_) \
    }; \
    static void handler_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const char *args, \
        const dcc_event_t *event, \
        void *user_data \
    )

#define DCC_DECORATE_MESSAGE_COMMAND_DATA(handler_, prefix_, command_name_, user_data_) \
    static void handler_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const char *args, \
        const dcc_event_t *event, \
        void *user_data \
    ); \
    static const dcc_app_extension_message_command_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_message_command_t), \
        .prefix = (prefix_), \
        .name = (command_name_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const char *args, \
        const dcc_event_t *event, \
        void *user_data \
    )

#define DCC_DECORATE_MESSAGE_COMMAND_FN(handler_, prefix_) \
    DCC_DECORATE_MESSAGE_COMMAND(handler_, (prefix_), #handler_)

#define DCC_DECORATE_MESSAGE_COMMAND_FN_DATA(handler_, prefix_, user_data_) \
    DCC_DECORATE_MESSAGE_COMMAND_DATA(handler_, (prefix_), #handler_, (user_data_))

#define DCC_DECORATE_PREFIX_COMMAND(handler_, prefix_, command_name_) \
    DCC_DECORATE_MESSAGE_COMMAND(handler_, (prefix_), (command_name_))

#define DCC_DECORATE_PREFIX_COMMAND_DATA(handler_, prefix_, command_name_, user_data_) \
    DCC_DECORATE_MESSAGE_COMMAND_DATA(handler_, (prefix_), (command_name_), (user_data_))

#define DCC_DECORATE_PREFIX_COMMAND_FN(handler_, prefix_) \
    DCC_DECORATE_MESSAGE_COMMAND_FN(handler_, (prefix_))

#define DCC_DECORATE_PREFIX_COMMAND_FN_DATA(handler_, prefix_, user_data_) \
    DCC_DECORATE_MESSAGE_COMMAND_FN_DATA(handler_, (prefix_), (user_data_))

#define DCC_DECORATE_TEXT_COMMAND(handler_, prefix_, command_name_) \
    DCC_DECORATE_PREFIX_COMMAND(handler_, (prefix_), (command_name_))

#define DCC_DECORATE_TEXT_COMMAND_FN(handler_, prefix_) \
    DCC_DECORATE_PREFIX_COMMAND_FN(handler_, (prefix_))


#endif /* DCC_SUGAR_DECORATORS_MESSAGE_COMMANDS_H */
