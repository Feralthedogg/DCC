#ifndef DCC_SUGAR_APP_REGISTRATION_EVENTS_H
#define DCC_SUGAR_APP_REGISTRATION_EVENTS_H

#include <dcc/sugar/app_registration/base.h>

#define DCC_APP_ON_EVENT(app_, type_, handler_) dcc_app_on((app_), (type_), (handler_), NULL)
#define DCC_APP_ON_EVENT_DATA(app_, type_, handler_, user_data_) \
    dcc_app_on((app_), (type_), (handler_), (user_data_))
#define DCC_APP_ON_EVENT_FN(app_, type_, handler_) \
    DCC_APP_ON_EVENT((app_), (type_), (handler_))
#define DCC_APP_ON_EVENT_FN_DATA(app_, type_, handler_, user_data_) \
    DCC_APP_ON_EVENT_DATA((app_), (type_), (handler_), (user_data_))

#define DCC_APP_ON_MESSAGE(app_, handler_) dcc_app_on_message_create((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_create((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE((app_), (handler_))
#define DCC_APP_ON_MESSAGE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_DATA((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CREATE(app_, handler_) \
    dcc_app_on_message_create((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_CREATE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_create((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CREATE_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE_CREATE((app_), (handler_))
#define DCC_APP_ON_MESSAGE_CREATE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_CREATE_DATA((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_UPDATE(app_, handler_) \
    dcc_app_on_message_update((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_UPDATE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_update((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_UPDATE_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE_UPDATE((app_), (handler_))
#define DCC_APP_ON_MESSAGE_UPDATE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_UPDATE_DATA((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_DELETE(app_, handler_) \
    dcc_app_on_message_delete((app_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_DELETE_DATA(app_, handler_, user_data_) \
    dcc_app_on_message_delete((app_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_DELETE_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE_DELETE((app_), (handler_))
#define DCC_APP_ON_MESSAGE_DELETE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_DELETE_DATA((app_), (handler_), (user_data_))

#endif /* DCC_SUGAR_APP_REGISTRATION_EVENTS_H */
