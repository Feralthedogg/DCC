#ifndef DCC_SUGAR_APP_EXTENSION_EVENTS_H
#define DCC_SUGAR_APP_EXTENSION_EVENTS_H

#include <dcc/sugar/app_extension/base.h>

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

#endif /* DCC_SUGAR_APP_EXTENSION_EVENTS_H */
