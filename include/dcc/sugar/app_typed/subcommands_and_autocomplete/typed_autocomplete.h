#ifndef DCC_SUGAR_APP_TYPED_SUBCOMMANDS_AND_AUTOCOMPLETE_TYPED_AUTOCOMPLETE_H
#define DCC_SUGAR_APP_TYPED_SUBCOMMANDS_AND_AUTOCOMPLETE_TYPED_AUTOCOMPLETE_H

#include <dcc/sugar/app_typed/subcommands_and_autocomplete/base.h>

#define DCC_TYPED_AUTOCOMPLETE(command_name_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
        .command_name = (command_name_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_AUTOCOMPLETE_POLICY( \
    command_name_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
        .command_name = (command_name_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_AUTOCOMPLETE_ROUTE( \
    command_name_, \
    args_type_, \
    handler_, \
    user_data_, \
    out_route_, \
    bindings_, \
    validators_ \
) \
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
        .command_name = (command_name_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_TYPED_AUTOCOMPLETE_ROUTE_POLICY( \
    command_name_, \
    args_type_, \
    handler_, \
    user_data_, \
    out_route_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
        .command_name = (command_name_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_AUTOCOMPLETE_BUILDER(command_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_AUTOCOMPLETE_BUILDER_POLICY( \
    command_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_AUTOCOMPLETE_BUILDER_ROUTE( \
    command_, \
    args_type_, \
    handler_, \
    user_data_, \
    out_route_, \
    bindings_, \
    validators_ \
) \
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_TYPED_AUTOCOMPLETE_BUILDER_ROUTE_POLICY( \
    command_, \
    args_type_, \
    handler_, \
    user_data_, \
    out_route_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
        .command = (command_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

#endif /* DCC_SUGAR_APP_TYPED_SUBCOMMANDS_AND_AUTOCOMPLETE_TYPED_AUTOCOMPLETE_H */
