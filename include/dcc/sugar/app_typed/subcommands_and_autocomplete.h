/* Included by <dcc/sugar/app_typed.h>. */
#define DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    params_, \
    validators_ \
) \
    DCC_TYPED_SUBCOMMAND( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), \
        (validators_) \
    )

#define DCC_ON_SUBCOMMAND_PARAMS_VALIDATED( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    params_, \
    validators_ \
) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        NULL, \
        params_, \
        (validators_) \
    )

#define DCC_ON_SUBCOMMAND_PARAMS_DATA(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_SUBCOMMAND_PARAMS(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS_VALIDATED( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_VALIDATED( \
    command_name_, \
    handler_, \
    args_type_, \
    user_data_, \
    params_, \
    validators_ \
) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        #handler_, \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        (validators_) \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN_VALIDATED(command_name_, handler_, args_type_, params_, validators_) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        #handler_, \
        args_type_, \
        (handler_), \
        NULL, \
        params_, \
        (validators_) \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN_DATA(command_name_, handler_, args_type_, user_data_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        #handler_, \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN(command_name_, handler_, args_type_, params_) \
    DCC_ON_SUBCOMMAND_PARAMS_VALIDATED( \
        (command_name_), \
        #handler_, \
        args_type_, \
        (handler_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_SUBCOMMAND_BUILDER_PARAMS_DATA_VALIDATED( \
    command_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    params_, \
    validators_ \
) \
    DCC_TYPED_SUBCOMMAND_BUILDER( \
        (command_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), \
        (validators_) \
    )

#define DCC_ON_SUBCOMMAND_BUILDER_PARAMS_VALIDATED( \
    command_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    params_, \
    validators_ \
) \
    DCC_ON_SUBCOMMAND_BUILDER_PARAMS_DATA_VALIDATED( \
        (command_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        NULL, \
        params_, \
        (validators_) \
    )

#define DCC_ON_SUBCOMMAND_BUILDER_PARAMS_DATA(command_, subcommand_path_, args_type_, handler_, user_data_, params_) \
    DCC_ON_SUBCOMMAND_BUILDER_PARAMS_DATA_VALIDATED( \
        (command_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_SUBCOMMAND_BUILDER_PARAMS(command_, subcommand_path_, args_type_, handler_, params_) \
    DCC_ON_SUBCOMMAND_BUILDER_PARAMS_VALIDATED( \
        (command_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_AUTOCOMPLETE_PARAMS_DATA_VALIDATED( \
    command_name_, \
    args_type_, \
    handler_, \
    user_data_, \
    params_, \
    validators_ \
) \
    DCC_TYPED_AUTOCOMPLETE( \
        (command_name_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), \
        (validators_) \
    )

#define DCC_ON_AUTOCOMPLETE_PARAMS_VALIDATED(command_name_, args_type_, handler_, params_, validators_) \
    DCC_ON_AUTOCOMPLETE_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        args_type_, \
        (handler_), \
        NULL, \
        params_, \
        (validators_) \
    )

#define DCC_ON_AUTOCOMPLETE_PARAMS_DATA(command_name_, args_type_, handler_, user_data_, params_) \
    DCC_ON_AUTOCOMPLETE_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_AUTOCOMPLETE_PARAMS(command_name_, args_type_, handler_, params_) \
    DCC_ON_AUTOCOMPLETE_PARAMS_VALIDATED( \
        (command_name_), \
        args_type_, \
        (handler_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )
#define DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_VALIDATED(handler_, args_type_, user_data_, params_, validators_) \
    DCC_ON_AUTOCOMPLETE_PARAMS_DATA_VALIDATED( \
        #handler_, args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_AUTOCOMPLETE_PARAMS_FN_VALIDATED(handler_, args_type_, params_, validators_) \
    DCC_ON_AUTOCOMPLETE_PARAMS_DATA_VALIDATED( \
        #handler_, args_type_, (handler_), NULL, params_, (validators_) \
    )
#define DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA(handler_, args_type_, user_data_, params_) \
    DCC_ON_AUTOCOMPLETE_PARAMS_DATA_VALIDATED( \
        #handler_, args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS() \
    )
#define DCC_ON_AUTOCOMPLETE_PARAMS_FN(handler_, args_type_, params_) \
    DCC_ON_AUTOCOMPLETE_PARAMS_VALIDATED( \
        #handler_, args_type_, (handler_), params_, DCC_NO_VALIDATORS() \
    )

#define DCC_ON_AUTOCOMPLETE_BUILDER_PARAMS_DATA_VALIDATED( \
    command_, \
    args_type_, \
    handler_, \
    user_data_, \
    params_, \
    validators_ \
) \
    DCC_TYPED_AUTOCOMPLETE_BUILDER( \
        (command_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), \
        (validators_) \
    )

#define DCC_ON_AUTOCOMPLETE_BUILDER_PARAMS_VALIDATED(command_, args_type_, handler_, params_, validators_) \
    DCC_ON_AUTOCOMPLETE_BUILDER_PARAMS_DATA_VALIDATED( \
        (command_), \
        args_type_, \
        (handler_), \
        NULL, \
        params_, \
        (validators_) \
    )

#define DCC_ON_AUTOCOMPLETE_BUILDER_PARAMS_DATA(command_, args_type_, handler_, user_data_, params_) \
    DCC_ON_AUTOCOMPLETE_BUILDER_PARAMS_DATA_VALIDATED( \
        (command_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_AUTOCOMPLETE_BUILDER_PARAMS(command_, args_type_, handler_, params_) \
    DCC_ON_AUTOCOMPLETE_BUILDER_PARAMS_VALIDATED( \
        (command_), \
        args_type_, \
        (handler_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA_VALIDATED( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    params_, \
    validators_ \
) \
    DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), \
        (validators_) \
    )

#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_VALIDATED( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    params_, \
    validators_ \
) \
    DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        NULL, \
        params_, \
        (validators_) \
    )

#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    params_ \
) \
    DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA_VALIDATED( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS(command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_VALIDATED( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS_DATA_VALIDATED( \
    command_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    params_, \
    validators_ \
) \
    DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE_BUILDER( \
        (command_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), \
        (validators_) \
    )

#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS_VALIDATED( \
    command_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    params_, \
    validators_ \
) \
    DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS_DATA_VALIDATED( \
        (command_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        NULL, \
        params_, \
        (validators_) \
    )

#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS_DATA( \
    command_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    params_ \
) \
    DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS_DATA_VALIDATED( \
        (command_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS(command_, subcommand_path_, args_type_, handler_, params_) \
    DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS_VALIDATED( \
        (command_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        params_, \
        DCC_NO_VALIDATORS() \
    )

#define DCC_TYPED_SUBCOMMAND(command_name_, subcommand_path_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_subcommand_t){ \
        .size = sizeof(dcc_app_typed_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_SUBCOMMAND_POLICY( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_subcommand_t){ \
        .size = sizeof(dcc_app_typed_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_SUBCOMMAND_ROUTE( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    out_route_, \
    bindings_, \
    validators_ \
) \
    ((dcc_app_typed_subcommand_t){ \
        .size = sizeof(dcc_app_typed_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_TYPED_SUBCOMMAND_ROUTE_POLICY( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    out_route_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_subcommand_t){ \
        .size = sizeof(dcc_app_typed_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_SUBCOMMAND_BUILDER(command_, subcommand_path_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_subcommand_t){ \
        .size = sizeof(dcc_app_typed_subcommand_t), \
        .command = (command_), \
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_SUBCOMMAND_BUILDER_POLICY( \
    command_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_subcommand_t){ \
        .size = sizeof(dcc_app_typed_subcommand_t), \
        .command = (command_), \
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_SUBCOMMAND_BUILDER_ROUTE( \
    command_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    out_route_, \
    bindings_, \
    validators_ \
) \
    ((dcc_app_typed_subcommand_t){ \
        .size = sizeof(dcc_app_typed_subcommand_t), \
        .command = (command_), \
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_TYPED_SUBCOMMAND_BUILDER_ROUTE_POLICY( \
    command_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    out_route_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_subcommand_t){ \
        .size = sizeof(dcc_app_typed_subcommand_t), \
        .command = (command_), \
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_), \
        .policy = (policy_) \
    })

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

