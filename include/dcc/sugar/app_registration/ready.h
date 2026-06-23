#ifndef DCC_SUGAR_APP_REGISTRATION_READY_H
#define DCC_SUGAR_APP_REGISTRATION_READY_H

#include <dcc/sugar/app_registration/base.h>

#define DCC_APP_ON_READY(app_, handler_) \
    dcc_app_on_ready((app_), (handler_), NULL)

#define DCC_APP_ON_READY_DATA(app_, handler_, user_data_) \
    dcc_app_on_ready((app_), (handler_), (user_data_))

#define DCC_APP_ON_READY_FN(app_, handler_) \
    DCC_APP_ON_READY((app_), (handler_))

#define DCC_APP_ON_READY_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_READY_DATA((app_), (handler_), (user_data_))

#define DCC_APP_ON_READY_ONCE(app_, handler_) \
    dcc_app_on_ready_once((app_), (handler_), NULL)

#define DCC_APP_ON_READY_ONCE_DATA(app_, handler_, user_data_) \
    dcc_app_on_ready_once((app_), (handler_), (user_data_))

#define DCC_APP_ON_READY_ONCE_FN(app_, handler_) \
    DCC_APP_ON_READY_ONCE((app_), (handler_))

#define DCC_APP_ON_READY_ONCE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_READY_ONCE_DATA((app_), (handler_), (user_data_))

#endif /* DCC_SUGAR_APP_REGISTRATION_READY_H */
