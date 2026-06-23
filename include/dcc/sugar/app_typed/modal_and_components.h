/* Included by <dcc/sugar/app_typed.h>. */
#define DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_ \
) \
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE_POLICY( \
    command_name_, \
    subcommand_path_, \
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
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE_ROUTE( \
    command_name_, \
    subcommand_path_, \
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
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE_ROUTE_POLICY( \
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
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
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

#define DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE_BUILDER( \
    command_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_ \
) \
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
        .command = (command_), \
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE_BUILDER_POLICY( \
    command_, \
    subcommand_path_, \
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
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE_BUILDER_ROUTE( \
    command_, \
    subcommand_path_, \
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
        .subcommand_path = (subcommand_path_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .out_route = (out_route_) \
    })

#define DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE_BUILDER_ROUTE_POLICY( \
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
    ((dcc_app_typed_autocomplete_t){ \
        .size = sizeof(dcc_app_typed_autocomplete_t), \
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

#define DCC_TYPED_MODAL(custom_id_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_modal_t){ \
        .size = sizeof(dcc_app_typed_modal_t), \
        .custom_id = (custom_id_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_MODAL_POLICY(custom_id_, args_type_, handler_, user_data_, bindings_, validators_, policy_) \
    ((dcc_app_typed_modal_t){ \
        .size = sizeof(dcc_app_typed_modal_t), \
        .custom_id = (custom_id_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_MODAL_PREFIX(custom_id_prefix_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_modal_t){ \
        .size = sizeof(dcc_app_typed_modal_t), \
        .custom_id = (custom_id_prefix_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .prefix = 1U \
    })

#define DCC_TYPED_MODAL_PREFIX_POLICY( \
    custom_id_prefix_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_modal_t){ \
        .size = sizeof(dcc_app_typed_modal_t), \
        .custom_id = (custom_id_prefix_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .prefix = 1U, \
        .policy = (policy_) \
    })

#define DCC_MODAL_FLOW(custom_id_, title_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    ((dcc_app_modal_flow_t){ \
        .size = sizeof(dcc_app_modal_flow_t), \
        .modal = { \
            .custom_id = (custom_id_), \
            .title = (title_), \
            .components = (dcc_component_builder_t[]){ __VA_ARGS__ }, \
            .components_count = sizeof((dcc_component_builder_t[]){ __VA_ARGS__ }) / \
                sizeof(dcc_component_builder_t), \
            .has_custom_id = 1U, \
            .has_title = 1U \
        }, \
        .route = DCC_TYPED_MODAL((custom_id_), args_type_, (handler_), (user_data_), (bindings_), (validators_)) \
    })

#define DCC_MODAL_FLOW_POLICY(custom_id_, title_, args_type_, handler_, user_data_, bindings_, validators_, policy_, ...) \
    ((dcc_app_modal_flow_t){ \
        .size = sizeof(dcc_app_modal_flow_t), \
        .modal = { \
            .custom_id = (custom_id_), \
            .title = (title_), \
            .components = (dcc_component_builder_t[]){ __VA_ARGS__ }, \
            .components_count = sizeof((dcc_component_builder_t[]){ __VA_ARGS__ }) / \
                sizeof(dcc_component_builder_t), \
            .has_custom_id = 1U, \
            .has_title = 1U \
        }, \
        .route = DCC_TYPED_MODAL_POLICY( \
            (custom_id_), \
            args_type_, \
            (handler_), \
            (user_data_), \
            (bindings_), \
            (validators_), \
            (policy_) \
        ) \
    })

#define DCC_MODAL_V2_FLOW(custom_id_, title_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    ((dcc_app_modal_flow_t){ \
        .size = sizeof(dcc_app_modal_flow_t), \
        .modal = { \
            .custom_id = (custom_id_), \
            .title = (title_), \
            .components_v2 = (dcc_component_v2_builder_t[]){ __VA_ARGS__ }, \
            .components_v2_count = sizeof((dcc_component_v2_builder_t[]){ __VA_ARGS__ }) / \
                sizeof(dcc_component_v2_builder_t), \
            .has_custom_id = 1U, \
            .has_title = 1U \
        }, \
        .route = DCC_TYPED_MODAL((custom_id_), args_type_, (handler_), (user_data_), (bindings_), (validators_)) \
    })

#define DCC_MODAL_V2_FLOW_POLICY( \
    custom_id_, \
    title_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    policy_, \
    ... \
) \
    ((dcc_app_modal_flow_t){ \
        .size = sizeof(dcc_app_modal_flow_t), \
        .modal = { \
            .custom_id = (custom_id_), \
            .title = (title_), \
            .components_v2 = (dcc_component_v2_builder_t[]){ __VA_ARGS__ }, \
            .components_v2_count = sizeof((dcc_component_v2_builder_t[]){ __VA_ARGS__ }) / \
                sizeof(dcc_component_v2_builder_t), \
            .has_custom_id = 1U, \
            .has_title = 1U \
        }, \
        .route = DCC_TYPED_MODAL_POLICY( \
            (custom_id_), \
            args_type_, \
            (handler_), \
            (user_data_), \
            (bindings_), \
            (validators_), \
            (policy_) \
        ) \
    })

#define DCC_TYPED_BUTTON(custom_id_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_component_t){ \
        .size = sizeof(dcc_app_typed_component_t), \
        .custom_id = (custom_id_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_BUTTON_POLICY(custom_id_, args_type_, handler_, user_data_, bindings_, validators_, policy_) \
    ((dcc_app_typed_component_t){ \
        .size = sizeof(dcc_app_typed_component_t), \
        .custom_id = (custom_id_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_BUTTON_PREFIX(custom_id_prefix_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_component_t){ \
        .size = sizeof(dcc_app_typed_component_t), \
        .custom_id = (custom_id_prefix_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .prefix = 1U \
    })

#define DCC_TYPED_BUTTON_PREFIX_POLICY( \
    custom_id_prefix_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_component_t){ \
        .size = sizeof(dcc_app_typed_component_t), \
        .custom_id = (custom_id_prefix_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .prefix = 1U, \
        .policy = (policy_) \
    })

#define DCC_TYPED_SELECT(custom_id_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_component_t){ \
        .size = sizeof(dcc_app_typed_component_t), \
        .custom_id = (custom_id_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_TYPED_SELECT_POLICY(custom_id_, args_type_, handler_, user_data_, bindings_, validators_, policy_) \
    ((dcc_app_typed_component_t){ \
        .size = sizeof(dcc_app_typed_component_t), \
        .custom_id = (custom_id_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    })

#define DCC_TYPED_SELECT_PREFIX(custom_id_prefix_, args_type_, handler_, user_data_, bindings_, validators_) \
    ((dcc_app_typed_component_t){ \
        .size = sizeof(dcc_app_typed_component_t), \
        .custom_id = (custom_id_prefix_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .prefix = 1U \
    })

#define DCC_TYPED_SELECT_PREFIX_POLICY( \
    custom_id_prefix_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    policy_ \
) \
    ((dcc_app_typed_component_t){ \
        .size = sizeof(dcc_app_typed_component_t), \
        .custom_id = (custom_id_prefix_), \
        .args_size = sizeof(args_type_), \
        .bindings = (bindings_), \
        .validators = (validators_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .prefix = 1U, \
        .policy = (policy_) \
    })

#define DCC_TYPED_SLASH_GUARDED(command_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_SLASH_POLICY( \
        (command_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_TYPED_SUBCOMMAND_GUARDED( \
    command_name_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    ... \
) \
    DCC_TYPED_SUBCOMMAND_POLICY( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_TYPED_SUBCOMMAND_BUILDER_GUARDED( \
    command_, \
    subcommand_path_, \
    args_type_, \
    handler_, \
    user_data_, \
    bindings_, \
    validators_, \
    ... \
) \
    DCC_TYPED_SUBCOMMAND_BUILDER_POLICY( \
        (command_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_TYPED_AUTOCOMPLETE_GUARDED(command_name_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_AUTOCOMPLETE_POLICY( \
        (command_name_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_TYPED_AUTOCOMPLETE_BUILDER_GUARDED(command_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_AUTOCOMPLETE_BUILDER_POLICY( \
        (command_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_TYPED_MODAL_GUARDED(custom_id_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_MODAL_POLICY( \
        (custom_id_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_TYPED_MODAL_PREFIX_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_MODAL_PREFIX_POLICY( \
        (custom_id_prefix_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_TYPED_BUTTON_GUARDED(custom_id_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_BUTTON_POLICY( \
        (custom_id_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_TYPED_BUTTON_PREFIX_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_BUTTON_PREFIX_POLICY( \
        (custom_id_prefix_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_TYPED_SELECT_GUARDED(custom_id_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_SELECT_POLICY( \
        (custom_id_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_TYPED_SELECT_PREFIX_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_SELECT_PREFIX_POLICY( \
        (custom_id_prefix_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_TYPED_MODAL_NS(namespace_, action_, args_type_, handler_, user_data_, bindings_, validators_) \
    DCC_TYPED_MODAL(DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), (bindings_), (validators_))
#define DCC_TYPED_MODAL_NS_POLICY(namespace_, action_, args_type_, handler_, user_data_, bindings_, validators_, policy_) \
    DCC_TYPED_MODAL_POLICY( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), (bindings_), (validators_), (policy_) \
    )
#define DCC_TYPED_MODAL_NS_GUARDED(namespace_, action_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_MODAL_GUARDED( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), (bindings_), (validators_), __VA_ARGS__ \
    )
#define DCC_TYPED_MODAL_NS_PREFIX(namespace_, action_prefix_, args_type_, handler_, user_data_, bindings_, validators_) \
    DCC_TYPED_MODAL_PREFIX( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), args_type_, (handler_), (user_data_), (bindings_), (validators_) \
    )
#define DCC_TYPED_MODAL_NS_PREFIX_POLICY( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, bindings_, validators_, policy_ \
) \
    DCC_TYPED_MODAL_PREFIX_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        (policy_) \
    )
#define DCC_TYPED_MODAL_NS_PREFIX_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, bindings_, validators_, ... \
) \
    DCC_TYPED_MODAL_PREFIX_GUARDED( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        __VA_ARGS__ \
    )

#define DCC_TYPED_BUTTON_NS(namespace_, action_, args_type_, handler_, user_data_, bindings_, validators_) \
    DCC_TYPED_BUTTON(DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), (bindings_), (validators_))
#define DCC_TYPED_BUTTON_NS_POLICY(namespace_, action_, args_type_, handler_, user_data_, bindings_, validators_, policy_) \
    DCC_TYPED_BUTTON_POLICY( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), (bindings_), (validators_), (policy_) \
    )
#define DCC_TYPED_BUTTON_NS_GUARDED(namespace_, action_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_BUTTON_GUARDED( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), (bindings_), (validators_), __VA_ARGS__ \
    )
#define DCC_TYPED_BUTTON_NS_PREFIX(namespace_, action_prefix_, args_type_, handler_, user_data_, bindings_, validators_) \
    DCC_TYPED_BUTTON_PREFIX( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), args_type_, (handler_), (user_data_), (bindings_), (validators_) \
    )
#define DCC_TYPED_BUTTON_NS_PREFIX_POLICY( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, bindings_, validators_, policy_ \
) \
    DCC_TYPED_BUTTON_PREFIX_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        (policy_) \
    )
#define DCC_TYPED_BUTTON_NS_PREFIX_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, bindings_, validators_, ... \
) \
    DCC_TYPED_BUTTON_PREFIX_GUARDED( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        __VA_ARGS__ \
    )

#define DCC_TYPED_SELECT_NS(namespace_, action_, args_type_, handler_, user_data_, bindings_, validators_) \
    DCC_TYPED_SELECT(DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), (bindings_), (validators_))
#define DCC_TYPED_SELECT_NS_POLICY(namespace_, action_, args_type_, handler_, user_data_, bindings_, validators_, policy_) \
    DCC_TYPED_SELECT_POLICY( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), (bindings_), (validators_), (policy_) \
    )
#define DCC_TYPED_SELECT_NS_GUARDED(namespace_, action_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_TYPED_SELECT_GUARDED( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), (bindings_), (validators_), __VA_ARGS__ \
    )
#define DCC_TYPED_SELECT_NS_PREFIX(namespace_, action_prefix_, args_type_, handler_, user_data_, bindings_, validators_) \
    DCC_TYPED_SELECT_PREFIX( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), args_type_, (handler_), (user_data_), (bindings_), (validators_) \
    )
#define DCC_TYPED_SELECT_NS_PREFIX_POLICY( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, bindings_, validators_, policy_ \
) \
    DCC_TYPED_SELECT_PREFIX_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        (policy_) \
    )
#define DCC_TYPED_SELECT_NS_PREFIX_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, bindings_, validators_, ... \
) \
    DCC_TYPED_SELECT_PREFIX_GUARDED( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        (bindings_), \
        (validators_), \
        __VA_ARGS__ \
    )

