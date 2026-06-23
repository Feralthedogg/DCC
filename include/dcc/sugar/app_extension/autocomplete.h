#ifndef DCC_SUGAR_APP_EXTENSION_AUTOCOMPLETE_H
#define DCC_SUGAR_APP_EXTENSION_AUTOCOMPLETE_H

#include <dcc/sugar/app_extension/base.h>

#define DCC_EXTENSION_AUTOCOMPLETE(name_, handler_, user_data_) \
    ((dcc_app_extension_autocomplete_t){ \
        .size = sizeof(dcc_app_extension_autocomplete_t), \
        .name = (name_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_AUTOCOMPLETE_POLICY(name_, handler_, user_data_, policy_) \
    ((dcc_app_extension_autocomplete_t){ \
        .size = sizeof(dcc_app_extension_autocomplete_t), \
        .name = (name_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_AUTOCOMPLETE_HANDLER(name_, handler_) \
    DCC_EXTENSION_AUTOCOMPLETE((name_), (handler_), NULL)

#define DCC_AUTOCOMPLETE_HANDLER_DATA(name_, handler_, user_data_) \
    DCC_EXTENSION_AUTOCOMPLETE((name_), (handler_), (user_data_))

#define DCC_AUTOCOMPLETE_HANDLER_POLICY(name_, handler_, policy_) \
    DCC_EXTENSION_AUTOCOMPLETE_POLICY((name_), (handler_), NULL, (policy_))

#define DCC_AUTOCOMPLETE_HANDLER_DATA_POLICY(name_, handler_, user_data_, policy_) \
    DCC_EXTENSION_AUTOCOMPLETE_POLICY((name_), (handler_), (user_data_), (policy_))

#define DCC_ON_AUTOCOMPLETE(name_, handler_) \
    DCC_AUTOCOMPLETE_HANDLER((name_), (handler_))

#define DCC_ON_AUTOCOMPLETE_DATA(name_, handler_, user_data_) \
    DCC_AUTOCOMPLETE_HANDLER_DATA((name_), (handler_), (user_data_))

#define DCC_ON_AUTOCOMPLETE_POLICY(name_, handler_, policy_) \
    DCC_AUTOCOMPLETE_HANDLER_POLICY((name_), (handler_), (policy_))

#define DCC_ON_AUTOCOMPLETE_DATA_POLICY(name_, handler_, user_data_, policy_) \
    DCC_AUTOCOMPLETE_HANDLER_DATA_POLICY((name_), (handler_), (user_data_), (policy_))

#endif /* DCC_SUGAR_APP_EXTENSION_AUTOCOMPLETE_H */
