#ifndef DCC_SUGAR_APP_EXTENSION_SLASH_H
#define DCC_SUGAR_APP_EXTENSION_SLASH_H

#include <dcc/sugar/app_extension/base.h>

#define DCC_EXTENSION_MIDDLEWARE(middleware_, user_data_) \
    ((dcc_app_extension_middleware_t){ \
        .size = sizeof(dcc_app_extension_middleware_t), \
        .middleware = (middleware_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_SLASH(name_, description_, handler_, user_data_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (name_), \
        .description = (description_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_SLASH_ROUTE(name_, description_, handler_, user_data_, out_route_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (name_), \
        .description = (description_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_EXTENSION_SLASH_POLICY(name_, description_, handler_, user_data_, policy_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (name_), \
        .description = (description_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_EXTENSION_SLASH_ROUTE_POLICY(name_, description_, handler_, user_data_, out_route_, policy_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (name_), \
        .description = (description_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

#define DCC_EXTENSION_SLASH_BUILDER(builder_, handler_, user_data_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .builder = (builder_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_EXTENSION_SLASH_BUILDER_ROUTE(builder_, handler_, user_data_, out_route_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .builder = (builder_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_EXTENSION_SLASH_BUILDER_POLICY(builder_, handler_, user_data_, policy_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .builder = (builder_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_EXTENSION_SLASH_BUILDER_ROUTE_POLICY(builder_, handler_, user_data_, out_route_, policy_) \
    ((dcc_app_extension_slash_command_t){ \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .builder = (builder_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

#define DCC_SLASH(name_, description_, handler_) \
    DCC_EXTENSION_SLASH((name_), (description_), (handler_), NULL)

#define DCC_SLASH_DATA(name_, description_, handler_, user_data_) \
    DCC_EXTENSION_SLASH((name_), (description_), (handler_), (user_data_))

#define DCC_SLASH_POLICY(name_, description_, handler_, policy_) \
    DCC_EXTENSION_SLASH_POLICY((name_), (description_), (handler_), NULL, (policy_))

#define DCC_SLASH_DATA_POLICY(name_, description_, handler_, user_data_, policy_) \
    DCC_EXTENSION_SLASH_POLICY((name_), (description_), (handler_), (user_data_), (policy_))

#define DCC_SLASH_HANDLER(name_, description_, handler_) \
    DCC_SLASH((name_), (description_), (handler_))

#define DCC_SLASH_HANDLER_DATA(name_, description_, handler_, user_data_) \
    DCC_SLASH_DATA((name_), (description_), (handler_), (user_data_))

#define DCC_SLASH_BUILDER(builder_, handler_) \
    DCC_EXTENSION_SLASH_BUILDER((builder_), (handler_), NULL)

#define DCC_SLASH_BUILDER_DATA(builder_, handler_, user_data_) \
    DCC_EXTENSION_SLASH_BUILDER((builder_), (handler_), (user_data_))

#define DCC_SLASH_BUILDER_VALUE(builder_, handler_) \
    DCC_SLASH_BUILDER(&((dcc_application_command_builder_t[]){ (builder_) })[0], (handler_))

#define DCC_SLASH_BUILDER_VALUE_DATA(builder_, handler_, user_data_) \
    DCC_SLASH_BUILDER_DATA(&((dcc_application_command_builder_t[]){ (builder_) })[0], (handler_), (user_data_))

#define DCC_SLASH_BUILDER_POLICY(builder_, handler_, policy_) \
    DCC_EXTENSION_SLASH_BUILDER_POLICY((builder_), (handler_), NULL, (policy_))

#define DCC_SLASH_BUILDER_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_EXTENSION_SLASH_BUILDER_POLICY((builder_), (handler_), (user_data_), (policy_))

#define DCC_SLASH_BUILDER_VALUE_POLICY(builder_, handler_, policy_) \
    DCC_SLASH_BUILDER_POLICY(&((dcc_application_command_builder_t[]){ (builder_) })[0], (handler_), (policy_))

#define DCC_SLASH_BUILDER_VALUE_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_SLASH_BUILDER_DATA_POLICY( \
        &((dcc_application_command_builder_t[]){ (builder_) })[0], \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_COMMAND(name_, description_, handler_) \
    DCC_SLASH((name_), (description_), (handler_))

#define DCC_COMMAND_DATA(name_, description_, handler_, user_data_) \
    DCC_SLASH_DATA((name_), (description_), (handler_), (user_data_))

#define DCC_COMMAND_POLICY(name_, description_, handler_, policy_) \
    DCC_SLASH_POLICY((name_), (description_), (handler_), (policy_))

#define DCC_COMMAND_DATA_POLICY(name_, description_, handler_, user_data_, policy_) \
    DCC_SLASH_DATA_POLICY((name_), (description_), (handler_), (user_data_), (policy_))

#define DCC_COMMAND_BUILDER(builder_, handler_) \
    DCC_SLASH_BUILDER((builder_), (handler_))

#define DCC_COMMAND_BUILDER_DATA(builder_, handler_, user_data_) \
    DCC_SLASH_BUILDER_DATA((builder_), (handler_), (user_data_))

#define DCC_COMMAND_BUILDER_VALUE(builder_, handler_) \
    DCC_SLASH_BUILDER_VALUE((builder_), (handler_))

#define DCC_COMMAND_BUILDER_VALUE_DATA(builder_, handler_, user_data_) \
    DCC_SLASH_BUILDER_VALUE_DATA((builder_), (handler_), (user_data_))

#define DCC_COMMAND_BUILDER_POLICY(builder_, handler_, policy_) \
    DCC_SLASH_BUILDER_POLICY((builder_), (handler_), (policy_))

#define DCC_COMMAND_BUILDER_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_SLASH_BUILDER_DATA_POLICY((builder_), (handler_), (user_data_), (policy_))

#define DCC_COMMAND_BUILDER_VALUE_POLICY(builder_, handler_, policy_) \
    DCC_SLASH_BUILDER_VALUE_POLICY((builder_), (handler_), (policy_))

#define DCC_COMMAND_BUILDER_VALUE_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_SLASH_BUILDER_VALUE_DATA_POLICY((builder_), (handler_), (user_data_), (policy_))

#define DCC_ON_SLASH(name_, description_, handler_) \
    DCC_SLASH((name_), (description_), (handler_))

#define DCC_ON_SLASH_DATA(name_, description_, handler_, user_data_) \
    DCC_SLASH_DATA((name_), (description_), (handler_), (user_data_))

#define DCC_ON_SLASH_POLICY(name_, description_, handler_, policy_) \
    DCC_SLASH_POLICY((name_), (description_), (handler_), (policy_))

#define DCC_ON_SLASH_DATA_POLICY(name_, description_, handler_, user_data_, policy_) \
    DCC_SLASH_DATA_POLICY((name_), (description_), (handler_), (user_data_), (policy_))

#define DCC_ON_COMMAND(name_, description_, handler_) \
    DCC_COMMAND((name_), (description_), (handler_))

#define DCC_ON_COMMAND_DATA(name_, description_, handler_, user_data_) \
    DCC_COMMAND_DATA((name_), (description_), (handler_), (user_data_))

#define DCC_ON_COMMAND_POLICY(name_, description_, handler_, policy_) \
    DCC_COMMAND_POLICY((name_), (description_), (handler_), (policy_))

#define DCC_ON_COMMAND_DATA_POLICY(name_, description_, handler_, user_data_, policy_) \
    DCC_COMMAND_DATA_POLICY((name_), (description_), (handler_), (user_data_), (policy_))

#define DCC_ON_COMMAND_BUILDER(builder_, handler_) \
    DCC_COMMAND_BUILDER((builder_), (handler_))

#define DCC_ON_COMMAND_BUILDER_DATA(builder_, handler_, user_data_) \
    DCC_COMMAND_BUILDER_DATA((builder_), (handler_), (user_data_))

#define DCC_ON_COMMAND_BUILDER_POLICY(builder_, handler_, policy_) \
    DCC_COMMAND_BUILDER_POLICY((builder_), (handler_), (policy_))

#define DCC_ON_COMMAND_BUILDER_DATA_POLICY(builder_, handler_, user_data_, policy_) \
    DCC_COMMAND_BUILDER_DATA_POLICY((builder_), (handler_), (user_data_), (policy_))

#endif /* DCC_SUGAR_APP_EXTENSION_SLASH_H */
