#ifndef DCC_SUGAR_APP_MODULE_H
#define DCC_SUGAR_APP_MODULE_H

#include <dcc/app.h>

#define DCC_APP_MODULE(name_, setup_, user_data_) \
    ((dcc_app_module_t){ \
        .size = sizeof(dcc_app_module_t), \
        .name = (name_), \
        .setup = (setup_), \
        .user_data = (user_data_) \
    })

#define DCC_APP_MODULE_WITH_CLEANUP(name_, setup_, user_data_, cleanup_) \
    ((dcc_app_module_t){ \
        .size = sizeof(dcc_app_module_t), \
        .name = (name_), \
        .setup = (setup_), \
        .user_data = (user_data_), \
        .cleanup = (cleanup_) \
    })

#endif
