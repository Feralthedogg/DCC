#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_TYPED_SLASH_CORE_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_TYPED_SLASH_CORE_H

#include <dcc/sugar/app_typed/args_and_slash/binding_lists.h>

#define DCC_TYPED_SLASH(command_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_SLASH_POLICY(command_, args_type_, handler_, user_data_, bindings_, validators_, policy_) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_SLASH_ROUTE(command_, args_type_, handler_, user_data_, out_route_, bindings_, validators_) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_TYPED_SLASH_ROUTE_POLICY( \
    command_, \
    args_type_, \
    handler_, \
    user_data_, \
    out_route_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_slash_command_t){ \
        .size = sizeof(dcc_app_typed_slash_command_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_TYPED_SLASH_CORE_H */
