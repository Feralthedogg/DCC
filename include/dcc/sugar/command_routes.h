/* Included by <dcc/sugar.h>. */
#define DCC_APP_ON_SLASH(app_, name_, description_, handler_) \
    dcc_app_slash((app_), (name_), (description_), (handler_), NULL)
#define DCC_APP_ON_SLASH_DATA(app_, name_, description_, handler_, user_data_) \
    dcc_app_slash((app_), (name_), (description_), (handler_), (user_data_))
#define DCC_APP_ON_SLASH_BUILDER(app_, builder_, handler_) \
    dcc_app_slash_builder((app_), (builder_), (handler_), NULL)
#define DCC_APP_ON_SLASH_BUILDER_DATA(app_, builder_, handler_, user_data_) \
    dcc_app_slash_builder((app_), (builder_), (handler_), (user_data_))
#define DCC_APP_ON_COMMAND(app_, name_, description_, handler_) \
    DCC_APP_ON_SLASH((app_), (name_), (description_), (handler_))
#define DCC_APP_ON_COMMAND_DATA(app_, name_, description_, handler_, user_data_) \
    DCC_APP_ON_SLASH_DATA((app_), (name_), (description_), (handler_), (user_data_))
#define DCC_APP_ON_COMMAND_BUILDER(app_, builder_, handler_) \
    DCC_APP_ON_SLASH_BUILDER((app_), (builder_), (handler_))
#define DCC_APP_ON_COMMAND_BUILDER_DATA(app_, builder_, handler_, user_data_) \
    DCC_APP_ON_SLASH_BUILDER_DATA((app_), (builder_), (handler_), (user_data_))
#define DCC_APP_ON_SLASH_FN(app_, handler_, description_) \
    DCC_APP_ON_SLASH((app_), #handler_, (description_), (handler_))
#define DCC_APP_ON_SLASH_FN_DATA(app_, handler_, description_, user_data_) \
    DCC_APP_ON_SLASH_DATA((app_), #handler_, (description_), (handler_), (user_data_))
#define DCC_APP_ON_COMMAND_FN(app_, handler_, description_) \
    DCC_APP_ON_SLASH_FN((app_), handler_, (description_))
#define DCC_APP_ON_COMMAND_FN_DATA(app_, handler_, description_, user_data_) \
    DCC_APP_ON_SLASH_FN_DATA((app_), handler_, (description_), (user_data_))
#define DCC_APP_ON_SUBCOMMAND(app_, command_name_, subcommand_path_, handler_) \
    dcc_app_subcommand((app_), (command_name_), (subcommand_path_), (handler_), NULL)
#define DCC_APP_ON_SUBCOMMAND_DATA(app_, command_name_, subcommand_path_, handler_, user_data_) \
    dcc_app_subcommand((app_), (command_name_), (subcommand_path_), (handler_), (user_data_))
#define DCC_APP_ON_AUTOCOMPLETE(app_, name_, handler_) \
    dcc_app_autocomplete((app_), (name_), (handler_), NULL)
#define DCC_APP_ON_AUTOCOMPLETE_DATA(app_, name_, handler_, user_data_) \
    dcc_app_autocomplete((app_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_AUTOCOMPLETE_FN(app_, handler_) \
    DCC_APP_ON_AUTOCOMPLETE((app_), #handler_, (handler_))
#define DCC_APP_ON_AUTOCOMPLETE_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_AUTOCOMPLETE_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_TYPED_SLASH(app_, route_) \
    dcc_app_slash_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_slash_command_t, (route_)))
#define DCC_APP_TYPED_SUBCOMMAND(app_, route_) \
    dcc_app_subcommand_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_subcommand_t, (route_)))
#define DCC_APP_TYPED_AUTOCOMPLETE(app_, route_) \
    dcc_app_autocomplete_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_autocomplete_t, (route_)))
#define DCC_APP_TYPED_MODAL(app_, route_) \
    dcc_app_modal_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_modal_t, (route_)))
#define DCC_APP_MODAL_FLOW(app_, flow_) \
    dcc_app_modal_flow_register((app_), DCC_SUGAR_PTR(dcc_app_modal_flow_t, (flow_)))
#define DCC_APP_TYPED_BUTTON(app_, route_) \
    dcc_app_button_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_component_t, (route_)))
#define DCC_APP_TYPED_SELECT(app_, route_) \
    dcc_app_select_typed((app_), DCC_SUGAR_PTR(dcc_app_typed_component_t, (route_)))
#define DCC_APP_REGISTER_SLASH(app_, route_) \
    DCC_APP_TYPED_SLASH((app_), (route_))
#define DCC_APP_REGISTER_COMMAND(app_, route_) \
    DCC_APP_TYPED_SLASH((app_), (route_))
#define DCC_APP_REGISTER_SUBCOMMAND(app_, route_) \
    DCC_APP_TYPED_SUBCOMMAND((app_), (route_))
#define DCC_APP_REGISTER_AUTOCOMPLETE(app_, route_) \
    DCC_APP_TYPED_AUTOCOMPLETE((app_), (route_))
#define DCC_APP_REGISTER_MODAL(app_, route_) \
    DCC_APP_TYPED_MODAL((app_), (route_))
#define DCC_APP_REGISTER_MODAL_FLOW(app_, flow_) \
    DCC_APP_MODAL_FLOW((app_), (flow_))
#define DCC_APP_REGISTER_BUTTON(app_, route_) \
    DCC_APP_TYPED_BUTTON((app_), (route_))
#define DCC_APP_REGISTER_SELECT(app_, route_) \
    DCC_APP_TYPED_SELECT((app_), (route_))
#define DCC_APP_SLASH_ROUTE(app_, name_, description_, args_type_, handler_, params_) \
    DCC_APP_TYPED_SLASH((app_), DCC_SLASH_ROUTE((name_), (description_), args_type_, (handler_), params_))
#define DCC_APP_SLASH_ROUTE_DATA(app_, name_, description_, args_type_, handler_, user_data_, params_) \
    DCC_APP_TYPED_SLASH( \
        (app_), DCC_SLASH_ROUTE_DATA((name_), (description_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_APP_SLASH_ROUTE_VALIDATED(app_, name_, description_, args_type_, handler_, params_, validators_) \
    DCC_APP_TYPED_SLASH( \
        (app_), DCC_SLASH_ROUTE_VALIDATED((name_), (description_), args_type_, (handler_), params_, (validators_)) \
    )
#define DCC_APP_SLASH_ROUTE_DATA_VALIDATED( \
    app_, name_, description_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_APP_TYPED_SLASH( \
        (app_), \
        DCC_SLASH_ROUTE_DATA_VALIDATED( \
            (name_), (description_), args_type_, (handler_), (user_data_), params_, (validators_) \
        ) \
    )
#define DCC_APP_SLASH_ROUTE_DATA_POLICY(app_, name_, description_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_APP_TYPED_SLASH( \
        (app_), \
        DCC_SLASH_ROUTE_DATA_POLICY((name_), (description_), args_type_, (handler_), (user_data_), params_, (policy_)) \
    )
#define DCC_APP_SLASH_ROUTE_GUARDED(app_, name_, description_, args_type_, handler_, params_, ...) \
    DCC_APP_TYPED_SLASH( \
        (app_), DCC_SLASH_ROUTE_GUARDED((name_), (description_), args_type_, (handler_), params_, __VA_ARGS__) \
    )
#define DCC_APP_SLASH_ROUTE_DATA_GUARDED(app_, name_, description_, args_type_, handler_, user_data_, params_, ...) \
    DCC_APP_TYPED_SLASH( \
        (app_), \
        DCC_SLASH_ROUTE_DATA_GUARDED((name_), (description_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_SLASH_ROUTE_FN(app_, handler_, description_, args_type_, params_) \
    DCC_APP_TYPED_SLASH((app_), DCC_SLASH_ROUTE_FN(handler_, (description_), args_type_, params_))
#define DCC_APP_SLASH_ROUTE_FN_DATA(app_, handler_, description_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_SLASH( \
        (app_), DCC_SLASH_ROUTE_FN_DATA(handler_, (description_), args_type_, (user_data_), params_) \
    )
#define DCC_APP_SLASH_ROUTE_FN_VALIDATED(app_, handler_, description_, args_type_, params_, validators_) \
    DCC_APP_TYPED_SLASH( \
        (app_), DCC_SLASH_ROUTE_FN_VALIDATED(handler_, (description_), args_type_, params_, (validators_)) \
    )
#define DCC_APP_SLASH_ROUTE_FN_DATA_VALIDATED( \
    app_, handler_, description_, args_type_, user_data_, params_, validators_ \
) \
    DCC_APP_TYPED_SLASH( \
        (app_), \
        DCC_SLASH_ROUTE_FN_DATA_VALIDATED(handler_, (description_), args_type_, (user_data_), params_, (validators_)) \
    )
#define DCC_APP_SLASH_ROUTE_FN_DATA_POLICY(app_, handler_, description_, args_type_, user_data_, params_, policy_) \
    DCC_APP_TYPED_SLASH( \
        (app_), DCC_SLASH_ROUTE_FN_DATA_POLICY(handler_, (description_), args_type_, (user_data_), params_, (policy_)) \
    )
#define DCC_APP_SLASH_ROUTE_FN_GUARDED(app_, handler_, description_, args_type_, params_, ...) \
    DCC_APP_TYPED_SLASH( \
        (app_), DCC_SLASH_ROUTE_FN_GUARDED(handler_, (description_), args_type_, params_, __VA_ARGS__) \
    )
#define DCC_APP_SLASH_ROUTE_FN_DATA_GUARDED(app_, handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_APP_TYPED_SLASH( \
        (app_), \
        DCC_SLASH_ROUTE_FN_DATA_GUARDED(handler_, (description_), args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_SUBCOMMAND_ROUTE(app_, command_name_, subcommand_path_, args_type_, handler_, params_) \
    DCC_APP_TYPED_SUBCOMMAND( \
        (app_), DCC_SUBCOMMAND_ROUTE((command_name_), (subcommand_path_), args_type_, (handler_), params_) \
    )
#define DCC_APP_SUBCOMMAND_ROUTE_DATA(app_, command_name_, subcommand_path_, args_type_, handler_, user_data_, params_) \
    DCC_APP_TYPED_SUBCOMMAND( \
        (app_), \
        DCC_SUBCOMMAND_ROUTE_DATA((command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_APP_SUBCOMMAND_ROUTE_FN(app_, command_name_, handler_, args_type_, params_) \
    DCC_APP_TYPED_SUBCOMMAND((app_), DCC_SUBCOMMAND_ROUTE_FN((command_name_), handler_, args_type_, params_))
#define DCC_APP_SUBCOMMAND_ROUTE_FN_DATA(app_, command_name_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_SUBCOMMAND( \
        (app_), DCC_SUBCOMMAND_ROUTE_FN_DATA((command_name_), handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_SUBCOMMAND_ROUTE_FN_DATA_GUARDED(app_, command_name_, handler_, args_type_, user_data_, params_, ...) \
    DCC_APP_TYPED_SUBCOMMAND( \
        (app_), \
        DCC_SUBCOMMAND_ROUTE_FN_DATA_GUARDED((command_name_), handler_, args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_SUBCOMMAND_ROUTE_IN_FN_DATA( \
    app_, command_name_, group_, handler_, args_type_, user_data_, params_ \
) \
    DCC_APP_TYPED_SUBCOMMAND( \
        (app_), \
        DCC_SUBCOMMAND_ROUTE_IN_FN_DATA((command_name_), group_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_AUTOCOMPLETE_PARAMS(app_, command_name_, args_type_, handler_, params_) \
    DCC_APP_TYPED_AUTOCOMPLETE( \
        (app_), DCC_ON_AUTOCOMPLETE_PARAMS((command_name_), args_type_, (handler_), params_) \
    )
#define DCC_APP_AUTOCOMPLETE_PARAMS_DATA(app_, command_name_, args_type_, handler_, user_data_, params_) \
    DCC_APP_TYPED_AUTOCOMPLETE( \
        (app_), DCC_ON_AUTOCOMPLETE_PARAMS_DATA((command_name_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_APP_AUTOCOMPLETE_PARAMS_FN(app_, handler_, args_type_, params_) \
    DCC_APP_TYPED_AUTOCOMPLETE((app_), DCC_ON_AUTOCOMPLETE_PARAMS_FN(handler_, args_type_, params_))
#define DCC_APP_AUTOCOMPLETE_PARAMS_FN_DATA(app_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_AUTOCOMPLETE( \
        (app_), DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA(handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_AUTOCOMPLETE_PARAMS_FN_DATA_GUARDED(app_, handler_, args_type_, user_data_, params_, ...) \
    DCC_APP_TYPED_AUTOCOMPLETE( \
        (app_), DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_GUARDED(handler_, args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_MODAL_PARAMS(app_, custom_id_, args_type_, handler_, params_) \
    DCC_APP_TYPED_MODAL((app_), DCC_ON_MODAL_PARAMS((custom_id_), args_type_, (handler_), params_))
#define DCC_APP_MODAL_PARAMS_DATA(app_, custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_APP_TYPED_MODAL( \
        (app_), DCC_ON_MODAL_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_APP_MODAL_NS_PARAMS(app_, namespace_, action_, args_type_, handler_, params_) \
    DCC_APP_TYPED_MODAL((app_), DCC_ON_MODAL_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_))
#define DCC_APP_MODAL_NS_PARAMS_DATA(app_, namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_APP_TYPED_MODAL( \
        (app_), DCC_ON_MODAL_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_APP_MODAL_NS_PARAMS_FN(app_, namespace_, handler_, args_type_, params_) \
    DCC_APP_TYPED_MODAL((app_), DCC_ON_MODAL_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_APP_MODAL_NS_PARAMS_FN_DATA(app_, namespace_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_MODAL( \
        (app_), DCC_ON_MODAL_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_MODAL_NS_PARAMS_FN_DATA_GUARDED(app_, namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_APP_TYPED_MODAL( \
        (app_), \
        DCC_ON_MODAL_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_MODAL_NS_PREFIX_PARAMS_FN_DATA( \
    app_, namespace_, handler_, args_type_, user_data_, params_ \
) \
    DCC_APP_TYPED_MODAL( \
        (app_), DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_MODAL_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
    app_, namespace_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_APP_TYPED_MODAL( \
        (app_), \
        DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_APP_MODAL_FLOW_NS_PARAMS_FN_DATA(app_, namespace_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_APP_MODAL_FLOW( \
        (app_), DCC_MODAL_FLOW_NS_PARAMS_FN_DATA(namespace_, (title_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_MODAL_V2_FLOW_NS_PARAMS_FN_DATA(app_, namespace_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_APP_MODAL_FLOW( \
        (app_), \
        DCC_MODAL_V2_FLOW_NS_PARAMS_FN_DATA(namespace_, (title_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_BUTTON_NS_PARAMS_FN(app_, namespace_, handler_, args_type_, params_) \
    DCC_APP_TYPED_BUTTON((app_), DCC_ON_BUTTON_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_APP_BUTTON_NS_PARAMS_FN_DATA(app_, namespace_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_BUTTON( \
        (app_), DCC_ON_BUTTON_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_BUTTON_NS_PARAMS_FN_DATA_GUARDED(app_, namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_APP_TYPED_BUTTON( \
        (app_), \
        DCC_ON_BUTTON_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_BUTTON_NS_PREFIX_PARAMS_FN_DATA(app_, namespace_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_BUTTON( \
        (app_), DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_BUTTON_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
    app_, namespace_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_APP_TYPED_BUTTON( \
        (app_), \
        DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_APP_SELECT_NS_PARAMS_FN(app_, namespace_, handler_, args_type_, params_) \
    DCC_APP_TYPED_SELECT((app_), DCC_ON_SELECT_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_APP_SELECT_NS_PARAMS_FN_DATA(app_, namespace_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_SELECT( \
        (app_), DCC_ON_SELECT_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_SELECT_NS_PARAMS_FN_DATA_GUARDED(app_, namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_APP_TYPED_SELECT( \
        (app_), \
        DCC_ON_SELECT_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_SELECT_NS_PREFIX_PARAMS_FN_DATA(app_, namespace_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_SELECT( \
        (app_), DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_SELECT_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
    app_, namespace_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_APP_TYPED_SELECT( \
        (app_), \
        DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_APP_ON_USER_CONTEXT_MENU(app_, name_, handler_) \
    dcc_app_user_context_menu((app_), (name_), (handler_), NULL)
#define DCC_APP_ON_USER_CONTEXT_MENU_DATA(app_, name_, handler_, user_data_) \
    dcc_app_user_context_menu((app_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU(app_, name_, handler_) \
    dcc_app_message_context_menu((app_), (name_), (handler_), NULL)
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA(app_, name_, handler_, user_data_) \
    dcc_app_message_context_menu((app_), (name_), (handler_), (user_data_))
#define DCC_APP_ON_USER_CONTEXT_MENU_FN(app_, handler_) \
    DCC_APP_ON_USER_CONTEXT_MENU((app_), #handler_, (handler_))
#define DCC_APP_ON_USER_CONTEXT_MENU_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_USER_CONTEXT_MENU_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN(app_, handler_) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU((app_), #handler_, (handler_))
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_BUTTON(app_, custom_id_, handler_) \
    dcc_app_button((app_), (custom_id_), (handler_), NULL)
#define DCC_APP_ON_BUTTON_DATA(app_, custom_id_, handler_, user_data_) \
    dcc_app_button((app_), (custom_id_), (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_PREFIX(app_, custom_id_prefix_, handler_) \
    dcc_app_button_prefix((app_), (custom_id_prefix_), (handler_), NULL)
#define DCC_APP_ON_BUTTON_PREFIX_DATA(app_, custom_id_prefix_, handler_, user_data_) \
    dcc_app_button_prefix((app_), (custom_id_prefix_), (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_FN(app_, handler_) \
    DCC_APP_ON_BUTTON((app_), #handler_, (handler_))
#define DCC_APP_ON_BUTTON_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_PREFIX_FN(app_, handler_) \
    DCC_APP_ON_BUTTON_PREFIX((app_), #handler_, (handler_))
#define DCC_APP_ON_BUTTON_PREFIX_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_PREFIX_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_NS(app_, namespace_, action_, handler_) \
    DCC_APP_ON_BUTTON((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_APP_ON_BUTTON_NS_DATA(app_, namespace_, action_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_DATA((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_NS_PREFIX(app_, namespace_, action_prefix_, handler_) \
    DCC_APP_ON_BUTTON_PREFIX((app_), DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_APP_ON_BUTTON_NS_PREFIX_DATA(app_, namespace_, action_prefix_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_PREFIX_DATA( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_) \
    )
#define DCC_APP_ON_BUTTON_NS_FN(app_, namespace_, handler_) \
    DCC_APP_ON_BUTTON_NS((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_BUTTON_NS_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_NS_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_BUTTON_NS_PREFIX_FN(app_, namespace_, handler_) \
    DCC_APP_ON_BUTTON_NS_PREFIX((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_BUTTON_NS_PREFIX_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_BUTTON_NS_PREFIX_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_SELECT(app_, custom_id_, handler_) \
    dcc_app_select((app_), (custom_id_), (handler_), NULL)
#define DCC_APP_ON_SELECT_DATA(app_, custom_id_, handler_, user_data_) \
    dcc_app_select((app_), (custom_id_), (handler_), (user_data_))
#define DCC_APP_ON_SELECT_PREFIX(app_, custom_id_prefix_, handler_) \
    dcc_app_select_prefix((app_), (custom_id_prefix_), (handler_), NULL)
#define DCC_APP_ON_SELECT_PREFIX_DATA(app_, custom_id_prefix_, handler_, user_data_) \
    dcc_app_select_prefix((app_), (custom_id_prefix_), (handler_), (user_data_))
#define DCC_APP_ON_SELECT_FN(app_, handler_) \
    DCC_APP_ON_SELECT((app_), #handler_, (handler_))
#define DCC_APP_ON_SELECT_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_SELECT_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_SELECT_PREFIX_FN(app_, handler_) \
    DCC_APP_ON_SELECT_PREFIX((app_), #handler_, (handler_))
#define DCC_APP_ON_SELECT_PREFIX_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_SELECT_PREFIX_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_SELECT_NS(app_, namespace_, action_, handler_) \
    DCC_APP_ON_SELECT((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_APP_ON_SELECT_NS_DATA(app_, namespace_, action_, handler_, user_data_) \
    DCC_APP_ON_SELECT_DATA((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_APP_ON_SELECT_NS_PREFIX(app_, namespace_, action_prefix_, handler_) \
    DCC_APP_ON_SELECT_PREFIX((app_), DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_APP_ON_SELECT_NS_PREFIX_DATA(app_, namespace_, action_prefix_, handler_, user_data_) \
    DCC_APP_ON_SELECT_PREFIX_DATA( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_) \
    )
#define DCC_APP_ON_SELECT_NS_FN(app_, namespace_, handler_) \
    DCC_APP_ON_SELECT_NS((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_SELECT_NS_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_SELECT_NS_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_SELECT_NS_PREFIX_FN(app_, namespace_, handler_) \
    DCC_APP_ON_SELECT_NS_PREFIX((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_SELECT_NS_PREFIX_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_SELECT_NS_PREFIX_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MODAL(app_, custom_id_, handler_) \
    dcc_app_modal((app_), (custom_id_), (handler_), NULL)
#define DCC_APP_ON_MODAL_DATA(app_, custom_id_, handler_, user_data_) \
    dcc_app_modal((app_), (custom_id_), (handler_), (user_data_))
#define DCC_APP_ON_MODAL_PREFIX(app_, custom_id_prefix_, handler_) \
    dcc_app_modal_prefix((app_), (custom_id_prefix_), (handler_), NULL)
#define DCC_APP_ON_MODAL_PREFIX_DATA(app_, custom_id_prefix_, handler_, user_data_) \
    dcc_app_modal_prefix((app_), (custom_id_prefix_), (handler_), (user_data_))
#define DCC_APP_ON_MODAL_FN(app_, handler_) \
    DCC_APP_ON_MODAL((app_), #handler_, (handler_))
#define DCC_APP_ON_MODAL_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MODAL_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MODAL_PREFIX_FN(app_, handler_) \
    DCC_APP_ON_MODAL_PREFIX((app_), #handler_, (handler_))
#define DCC_APP_ON_MODAL_PREFIX_FN_DATA(app_, handler_, user_data_) \
    DCC_APP_ON_MODAL_PREFIX_DATA((app_), #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MODAL_NS(app_, namespace_, action_, handler_) \
    DCC_APP_ON_MODAL((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_))
#define DCC_APP_ON_MODAL_NS_DATA(app_, namespace_, action_, handler_, user_data_) \
    DCC_APP_ON_MODAL_DATA((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (user_data_))
#define DCC_APP_ON_MODAL_NS_PREFIX(app_, namespace_, action_prefix_, handler_) \
    DCC_APP_ON_MODAL_PREFIX((app_), DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), (handler_))
#define DCC_APP_ON_MODAL_NS_PREFIX_DATA(app_, namespace_, action_prefix_, handler_, user_data_) \
    DCC_APP_ON_MODAL_PREFIX_DATA( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_) \
    )
#define DCC_APP_ON_MODAL_NS_FN(app_, namespace_, handler_) \
    DCC_APP_ON_MODAL_NS((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_MODAL_NS_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_MODAL_NS_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_ON_MODAL_NS_PREFIX_FN(app_, namespace_, handler_) \
    DCC_APP_ON_MODAL_NS_PREFIX((app_), namespace_, #handler_, (handler_))
#define DCC_APP_ON_MODAL_NS_PREFIX_FN_DATA(app_, namespace_, handler_, user_data_) \
    DCC_APP_ON_MODAL_NS_PREFIX_DATA((app_), namespace_, #handler_, (handler_), (user_data_))
#define DCC_APP_SEND(app_, channel_id_, message_) \
    dcc_app_send((app_), (channel_id_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)
#define DCC_APP_SEND_CB(app_, channel_id_, message_, cb_, user_data_) \
    dcc_app_send((app_), (channel_id_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))
#define DCC_APP_SEND_WITH_ID(app_, channel_id_, message_, cb_, user_data_) \
    dcc_app_send_with_id((app_), (channel_id_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))
#define DCC_APP_SEND_ID(app_, channel_id_, message_, cb_, user_data_) \
    DCC_APP_SEND_WITH_ID((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_APP_SEND_THREAD_PARAMS(app_, channel_id_, message_, thread_, cb_, user_data_) \
    dcc_app_send_with_thread( \
        (app_), \
        (channel_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (thread_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_SEND_THREAD(app_, channel_id_, message_, thread_name_) \
    dcc_app_send_with_thread_name( \
        (app_), \
        (channel_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (thread_name_), \
        NULL, \
        NULL \
    )
#define DCC_APP_SEND_THREAD_CB(app_, channel_id_, message_, thread_name_, cb_, user_data_) \
    dcc_app_send_with_thread_name( \
        (app_), \
        (channel_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (thread_name_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_SEND_TEXT(app_, channel_id_, content_) \
    dcc_app_send_text((app_), (channel_id_), (content_), NULL, NULL)
#define DCC_APP_SEND_TEXT_CB(app_, channel_id_, content_, cb_, user_data_) \
    dcc_app_send_text((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_APP_SEND_TEXT_WITH_ID(app_, channel_id_, content_, cb_, user_data_) \
    dcc_app_send_text_with_id((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_APP_SEND_TEXT_ID(app_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_SEND_TEXT_WITH_ID((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_APP_SEND_TEXT_THREAD(app_, channel_id_, content_, thread_name_) \
    dcc_app_send_text_with_thread((app_), (channel_id_), (content_), (thread_name_), NULL, NULL)
#define DCC_APP_SEND_TEXT_THREAD_CB(app_, channel_id_, content_, thread_name_, cb_, user_data_) \
    dcc_app_send_text_with_thread((app_), (channel_id_), (content_), (thread_name_), (cb_), (user_data_))
#define DCC_APP_SEND_V2(app_, channel_id_, ...) \
    DCC_APP_SEND((app_), (channel_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_APP_SEND_V2_CB(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_CB((app_), (channel_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_APP_SEND_V2_WITH_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_WITH_ID((app_), (channel_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_APP_SEND_V2_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_APP_SEND_V2_THREAD(app_, channel_id_, thread_name_, ...) \
    DCC_APP_SEND_THREAD((app_), (channel_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (thread_name_))
#define DCC_APP_SEND_V2_THREAD_CB(app_, channel_id_, thread_name_, cb_, user_data_, ...) \
    DCC_APP_SEND_THREAD_CB( \
        (app_), \
        (channel_id_), \
        DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), \
        (thread_name_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_SEND_EPHEMERAL_V2(app_, channel_id_, ...) \
    DCC_APP_SEND((app_), (channel_id_), DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(__VA_ARGS__))
#define DCC_APP_SEND_UI(app_, channel_id_, ...) DCC_APP_SEND_V2((app_), (channel_id_), __VA_ARGS__)
#define DCC_APP_SEND_UI_WITH_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_APP_SEND_UI_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_UI_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_APP_SEND_UI_THREAD(app_, channel_id_, thread_name_, ...) \
    DCC_APP_SEND_V2_THREAD((app_), (channel_id_), (thread_name_), __VA_ARGS__)
#define DCC_APP_SEND_UI_THREAD_CB(app_, channel_id_, thread_name_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_THREAD_CB((app_), (channel_id_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_APP_SEND_EPHEMERAL_UI(app_, channel_id_, ...) \
    DCC_APP_SEND_EPHEMERAL_V2((app_), (channel_id_), __VA_ARGS__)
#define DCC_APP_GET_CHANNEL(app_, channel_id_) \
    dcc_app_get_channel((app_), (channel_id_), NULL, NULL)
#define DCC_APP_GET_CHANNEL_CB(app_, channel_id_, cb_, user_data_) \
    dcc_app_get_channel((app_), (channel_id_), (cb_), (user_data_))
#define DCC_APP_GET_GUILD_CHANNELS(app_, guild_id_) \
    dcc_app_get_guild_channels((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_CHANNELS_CB(app_, guild_id_, cb_, user_data_) \
    dcc_app_get_guild_channels((app_), (guild_id_), (cb_), (user_data_))
#define DCC_APP_CREATE_GUILD_CHANNEL(app_, guild_id_, json_body_) \
    dcc_app_create_guild_channel((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_CHANNEL_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    dcc_app_create_guild_channel((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_CREATE_GUILD_CHANNEL_PARAMS(app_, params_) \
    dcc_app_create_guild_channel_params((app_), (params_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_CHANNEL_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_create_guild_channel_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_EDIT_CHANNEL(app_, channel_id_, json_body_) \
    dcc_app_modify_channel((app_), (channel_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_CHANNEL_CB(app_, channel_id_, json_body_, cb_, user_data_) \
    dcc_app_modify_channel((app_), (channel_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_EDIT_CHANNEL_PARAMS(app_, params_) \
    dcc_app_modify_channel_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_CHANNEL_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_modify_channel_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_DELETE_CHANNEL(app_, channel_id_) \
    dcc_app_delete_channel((app_), (channel_id_), NULL, NULL)
#define DCC_APP_DELETE_CHANNEL_CB(app_, channel_id_, cb_, user_data_) \
    dcc_app_delete_channel((app_), (channel_id_), (cb_), (user_data_))
#define DCC_APP_TRIGGER_CHANNEL_TYPING(app_, channel_id_) \
    dcc_app_trigger_channel_typing((app_), (channel_id_), NULL, NULL)
#define DCC_APP_TRIGGER_CHANNEL_TYPING_CB(app_, channel_id_, cb_, user_data_) \
    dcc_app_trigger_channel_typing((app_), (channel_id_), (cb_), (user_data_))
#define DCC_APP_FOLLOW_NEWS_CHANNEL(app_, channel_id_, webhook_channel_id_) \
    dcc_app_follow_news_channel((app_), (channel_id_), (webhook_channel_id_), NULL, NULL)
#define DCC_APP_FOLLOW_NEWS_CHANNEL_CB(app_, channel_id_, webhook_channel_id_, cb_, user_data_) \
    dcc_app_follow_news_channel((app_), (channel_id_), (webhook_channel_id_), (cb_), (user_data_))
#define DCC_APP_SET_CHANNEL_VOICE_STATUS(app_, channel_id_, status_) \
    dcc_app_set_channel_voice_status((app_), (channel_id_), (status_), NULL, NULL)
#define DCC_APP_SET_CHANNEL_VOICE_STATUS_CB(app_, channel_id_, status_, cb_, user_data_) \
    dcc_app_set_channel_voice_status((app_), (channel_id_), (status_), (cb_), (user_data_))
#define DCC_APP_GET_CURRENT_USER_VOICE_STATE(app_, guild_id_) \
    dcc_app_get_current_user_voice_state((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_MY_VOICE_STATE(app_, guild_id_) \
    DCC_APP_GET_CURRENT_USER_VOICE_STATE((app_), (guild_id_))
#define DCC_APP_EDIT_CURRENT_USER_VOICE_STATE(app_, guild_id_, json_body_) \
    dcc_app_modify_current_user_voice_state((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_MY_VOICE_STATE(app_, guild_id_, json_body_) \
    DCC_APP_EDIT_CURRENT_USER_VOICE_STATE((app_), (guild_id_), (json_body_))
#define DCC_APP_EDIT_CURRENT_USER_VOICE_STATE_PARAMS(app_, params_) \
    dcc_app_modify_current_user_voice_state_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_MY_VOICE_STATE_PARAMS(app_, params_) \
    DCC_APP_EDIT_CURRENT_USER_VOICE_STATE_PARAMS((app_), (params_))
#define DCC_APP_GET_USER_VOICE_STATE(app_, guild_id_, user_id_) \
    dcc_app_get_user_voice_state((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_APP_EDIT_USER_VOICE_STATE(app_, guild_id_, user_id_, json_body_) \
    dcc_app_modify_user_voice_state((app_), (guild_id_), (user_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_USER_VOICE_STATE_PARAMS(app_, params_) \
    dcc_app_modify_user_voice_state_params((app_), (params_), NULL, NULL)
#define DCC_APP_GET_VOICE_REGIONS(app_) \
    dcc_app_get_voice_regions((app_), NULL, NULL)
#define DCC_APP_GET_GUILD_VOICE_REGIONS(app_, guild_id_) \
    dcc_app_get_guild_voice_regions((app_), (guild_id_), NULL, NULL)
#define DCC_APP_REORDER_GUILD_CHANNELS(app_, guild_id_, json_body_) \
    dcc_app_modify_guild_channel_positions((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_REORDER_GUILD_CHANNELS_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    dcc_app_modify_guild_channel_positions((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_REORDER_GUILD_CHANNELS_PARAMS(app_, params_) \
    dcc_app_modify_guild_channel_positions_params((app_), (params_), NULL, NULL)
#define DCC_APP_REORDER_GUILD_CHANNELS_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_modify_guild_channel_positions_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_GET_CHANNEL_INVITES(app_, channel_id_) \
    dcc_app_get_channel_invites((app_), (channel_id_), NULL, NULL)
#define DCC_APP_GET_CHANNEL_INVITES_CB(app_, channel_id_, cb_, user_data_) \
    dcc_app_get_channel_invites((app_), (channel_id_), (cb_), (user_data_))
#define DCC_APP_CREATE_CHANNEL_INVITE(app_, channel_id_, json_body_) \
    dcc_app_create_channel_invite((app_), (channel_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_CHANNEL_INVITE_CB(app_, channel_id_, json_body_, cb_, user_data_) \
    dcc_app_create_channel_invite((app_), (channel_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_CREATE_CHANNEL_INVITE_PARAMS(app_, params_) \
    dcc_app_create_channel_invite_params((app_), (params_), NULL, NULL)
#define DCC_APP_CREATE_CHANNEL_INVITE_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_create_channel_invite_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_EDIT_CHANNEL_PERMISSION(app_, channel_id_, overwrite_id_, json_body_) \
    dcc_app_modify_channel_permission((app_), (channel_id_), (overwrite_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_CHANNEL_PERMISSION_CB(app_, channel_id_, overwrite_id_, json_body_, cb_, user_data_) \
    dcc_app_modify_channel_permission((app_), (channel_id_), (overwrite_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_EDIT_CHANNEL_PERMISSION_PARAMS(app_, channel_id_, overwrite_) \
    dcc_app_modify_channel_permission_params((app_), (channel_id_), (overwrite_), NULL, NULL)
#define DCC_APP_EDIT_CHANNEL_PERMISSION_PARAMS_CB(app_, channel_id_, overwrite_, cb_, user_data_) \
    dcc_app_modify_channel_permission_params((app_), (channel_id_), (overwrite_), (cb_), (user_data_))
#define DCC_APP_DELETE_CHANNEL_PERMISSION(app_, channel_id_, overwrite_id_) \
    dcc_app_delete_channel_permission((app_), (channel_id_), (overwrite_id_), NULL, NULL)
#define DCC_APP_DELETE_CHANNEL_PERMISSION_CB(app_, channel_id_, overwrite_id_, cb_, user_data_) \
    dcc_app_delete_channel_permission((app_), (channel_id_), (overwrite_id_), (cb_), (user_data_))
#define DCC_APP_GET_GUILD_ROLES(app_, guild_id_) \
    dcc_app_get_guild_roles((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_ROLES_CB(app_, guild_id_, cb_, user_data_) \
    dcc_app_get_guild_roles((app_), (guild_id_), (cb_), (user_data_))
#define DCC_APP_CREATE_GUILD_ROLE(app_, guild_id_, json_body_) \
    dcc_app_create_guild_role((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_ROLE_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    dcc_app_create_guild_role((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_CREATE_GUILD_ROLE_PARAMS(app_, params_) \
    dcc_app_create_guild_role_params((app_), (params_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_ROLE_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_create_guild_role_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_EDIT_GUILD_ROLE(app_, guild_id_, role_id_, json_body_) \
    dcc_app_modify_guild_role((app_), (guild_id_), (role_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_ROLE_CB(app_, guild_id_, role_id_, json_body_, cb_, user_data_) \
    dcc_app_modify_guild_role((app_), (guild_id_), (role_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_EDIT_GUILD_ROLE_PARAMS(app_, params_) \
    dcc_app_modify_guild_role_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_ROLE_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_modify_guild_role_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_REORDER_GUILD_ROLES(app_, guild_id_, positions_, position_count_) \
    dcc_app_modify_guild_role_positions((app_), (guild_id_), (positions_), (position_count_), NULL, NULL)
#define DCC_APP_REORDER_GUILD_ROLES_CB(app_, guild_id_, positions_, position_count_, cb_, user_data_) \
    dcc_app_modify_guild_role_positions( \
        (app_), \
        (guild_id_), \
        (positions_), \
        (position_count_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_DELETE_GUILD_ROLE(app_, guild_id_, role_id_) \
    dcc_app_delete_guild_role((app_), (guild_id_), (role_id_), NULL, NULL)
#define DCC_APP_DELETE_GUILD_ROLE_CB(app_, guild_id_, role_id_, cb_, user_data_) \
    dcc_app_delete_guild_role((app_), (guild_id_), (role_id_), (cb_), (user_data_))
#define DCC_APP_EDIT_MESSAGE(app_, channel_id_, message_id_, message_) \
    dcc_app_edit_message( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_MESSAGE_CB(app_, channel_id_, message_id_, message_, cb_, user_data_) \
    dcc_app_edit_message( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_EDIT_V2(app_, channel_id_, message_id_, ...) \
    DCC_APP_EDIT_MESSAGE((app_), (channel_id_), (message_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_APP_EDIT_UI(app_, channel_id_, message_id_, ...) \
    DCC_APP_EDIT_V2((app_), (channel_id_), (message_id_), __VA_ARGS__)
#define DCC_APP_EDIT_V2_CB(app_, channel_id_, message_id_, cb_, user_data_, ...) \
    DCC_APP_EDIT_MESSAGE_CB( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_EDIT_TEXT(app_, channel_id_, message_id_, content_) \
    dcc_app_edit_message_text((app_), (channel_id_), (message_id_), (content_), NULL, NULL)
#define DCC_APP_EDIT_TEXT_CB(app_, channel_id_, message_id_, content_, cb_, user_data_) \
    dcc_app_edit_message_text((app_), (channel_id_), (message_id_), (content_), (cb_), (user_data_))
#define DCC_APP_DELETE_MESSAGE(app_, channel_id_, message_id_) \
    dcc_app_delete_message((app_), (channel_id_), (message_id_), NULL, NULL)
#define DCC_APP_DELETE_MESSAGE_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    dcc_app_delete_message((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_APP_CROSSPOST_MESSAGE(app_, channel_id_, message_id_) \
    dcc_app_crosspost_message((app_), (channel_id_), (message_id_), NULL, NULL)
#define DCC_APP_CROSSPOST_MESSAGE_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    dcc_app_crosspost_message((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_APP_PIN_MESSAGE(app_, channel_id_, message_id_) \
    dcc_app_pin_message((app_), (channel_id_), (message_id_), NULL, NULL)
#define DCC_APP_PIN_MESSAGE_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    dcc_app_pin_message((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_APP_UNPIN_MESSAGE(app_, channel_id_, message_id_) \
    dcc_app_unpin_message((app_), (channel_id_), (message_id_), NULL, NULL)
#define DCC_APP_UNPIN_MESSAGE_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    dcc_app_unpin_message((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_APP_ADD_MESSAGE_REACTION(app_, channel_id_, message_id_, reaction_) \
    dcc_app_add_message_reaction((app_), (channel_id_), (message_id_), (reaction_), NULL, NULL)
#define DCC_APP_ADD_MESSAGE_REACTION_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    dcc_app_add_message_reaction((app_), (channel_id_), (message_id_), (reaction_), (cb_), (user_data_))
#define DCC_APP_DELETE_OWN_MESSAGE_REACTION(app_, channel_id_, message_id_, reaction_) \
    dcc_app_delete_own_message_reaction((app_), (channel_id_), (message_id_), (reaction_), NULL, NULL)
#define DCC_APP_DELETE_OWN_MESSAGE_REACTION_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    dcc_app_delete_own_message_reaction((app_), (channel_id_), (message_id_), (reaction_), (cb_), (user_data_))
#define DCC_APP_DELETE_USER_MESSAGE_REACTION(app_, channel_id_, message_id_, reaction_, user_id_) \
    dcc_app_delete_user_message_reaction((app_), (channel_id_), (message_id_), (reaction_), (user_id_), NULL, NULL)
#define DCC_APP_DELETE_USER_MESSAGE_REACTION_CB( \
    app_, \
    channel_id_, \
    message_id_, \
    reaction_, \
    user_id_, \
    cb_, \
    user_data_ \
) \
    dcc_app_delete_user_message_reaction( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (reaction_), \
        (user_id_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_CLEAR_MESSAGE_REACTIONS(app_, channel_id_, message_id_) \
    dcc_app_delete_all_message_reactions((app_), (channel_id_), (message_id_), NULL, NULL)
#define DCC_APP_CLEAR_MESSAGE_REACTIONS_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    dcc_app_delete_all_message_reactions((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_APP_CLEAR_MESSAGE_REACTIONS_FOR_EMOJI(app_, channel_id_, message_id_, reaction_) \
    dcc_app_delete_all_message_reactions_for_emoji((app_), (channel_id_), (message_id_), (reaction_), NULL, NULL)
#define DCC_APP_CLEAR_MESSAGE_REACTIONS_FOR_EMOJI_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    dcc_app_delete_all_message_reactions_for_emoji( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (reaction_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_THREAD_FROM_MESSAGE(app_, channel_id_, message_id_, name_) \
    dcc_app_create_thread_from_message_name((app_), (channel_id_), (message_id_), (name_), NULL, NULL)
#define DCC_APP_THREAD_FROM_MESSAGE_CB(app_, channel_id_, message_id_, name_, cb_, user_data_) \
    dcc_app_create_thread_from_message_name( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (name_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_THREAD_FROM_MESSAGE_PARAMS(app_, channel_id_, message_id_, params_) \
    dcc_app_create_thread_from_message((app_), (channel_id_), (message_id_), (params_), NULL, NULL)
#define DCC_APP_THREAD_FROM_MESSAGE_PARAMS_CB(app_, channel_id_, message_id_, params_, cb_, user_data_) \
    dcc_app_create_thread_from_message((app_), (channel_id_), (message_id_), (params_), (cb_), (user_data_))
#define DCC_APP_ARCHIVE_THREAD(app_, thread_id_) \
    dcc_app_archive_thread((app_), (thread_id_), NULL, NULL)
#define DCC_APP_ARCHIVE_THREAD_CB(app_, thread_id_, cb_, user_data_) \
    dcc_app_archive_thread((app_), (thread_id_), (cb_), (user_data_))
#define DCC_APP_LOCK_THREAD(app_, thread_id_) \
    dcc_app_lock_thread((app_), (thread_id_), NULL, NULL)
#define DCC_APP_LOCK_THREAD_CB(app_, thread_id_, cb_, user_data_) \
    dcc_app_lock_thread((app_), (thread_id_), (cb_), (user_data_))
#define DCC_APP_UNLOCK_THREAD(app_, thread_id_) \
    dcc_app_unlock_thread((app_), (thread_id_), NULL, NULL)
#define DCC_APP_UNLOCK_THREAD_CB(app_, thread_id_, cb_, user_data_) \
    dcc_app_unlock_thread((app_), (thread_id_), (cb_), (user_data_))

#define DCC_CHANNEL_SEND(app_, channel_id_, message_) \
    DCC_APP_SEND((app_), (channel_id_), (message_))
#define DCC_CHANNEL_SEND_CB(app_, channel_id_, message_, cb_, user_data_) \
    DCC_APP_SEND_CB((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_WITH_ID(app_, channel_id_, message_, cb_, user_data_) \
    DCC_APP_SEND_WITH_ID((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_ID(app_, channel_id_, message_, cb_, user_data_) \
    DCC_CHANNEL_SEND_WITH_ID((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_THREAD(app_, channel_id_, message_, thread_name_) \
    DCC_APP_SEND_THREAD((app_), (channel_id_), (message_), (thread_name_))
#define DCC_CHANNEL_SEND_THREAD_CB(app_, channel_id_, message_, thread_name_, cb_, user_data_) \
    DCC_APP_SEND_THREAD_CB((app_), (channel_id_), (message_), (thread_name_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_THREAD_PARAMS(app_, channel_id_, message_, thread_, cb_, user_data_) \
    DCC_APP_SEND_THREAD_PARAMS((app_), (channel_id_), (message_), (thread_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_TEXT(app_, channel_id_, content_) \
    DCC_APP_SEND_TEXT((app_), (channel_id_), (content_))
#define DCC_CHANNEL_SEND_TEXT_CB(app_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_SEND_TEXT_CB((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_TEXT_WITH_ID(app_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_SEND_TEXT_WITH_ID((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_TEXT_ID(app_, channel_id_, content_, cb_, user_data_) \
    DCC_CHANNEL_SEND_TEXT_WITH_ID((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_TEXT_THREAD(app_, channel_id_, content_, thread_name_) \
    DCC_APP_SEND_TEXT_THREAD((app_), (channel_id_), (content_), (thread_name_))
#define DCC_CHANNEL_SEND_TEXT_THREAD_CB(app_, channel_id_, content_, thread_name_, cb_, user_data_) \
    DCC_APP_SEND_TEXT_THREAD_CB((app_), (channel_id_), (content_), (thread_name_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_SAFE(app_, channel_id_, content_) \
    DCC_APP_SEND_SAFE((app_), (channel_id_), (content_))
#define DCC_CHANNEL_SEND_SAFE_CB(app_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_SEND_SAFE_CB((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_V2(app_, channel_id_, ...) \
    DCC_APP_SEND_V2((app_), (channel_id_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_V2_WITH_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_V2_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_CHANNEL_SEND_V2_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_V2_THREAD(app_, channel_id_, thread_name_, ...) \
    DCC_APP_SEND_V2_THREAD((app_), (channel_id_), (thread_name_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_V2_THREAD_CB(app_, channel_id_, thread_name_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_THREAD_CB((app_), (channel_id_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI(app_, channel_id_, ...) \
    DCC_APP_SEND_UI((app_), (channel_id_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI_WITH_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_UI_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_CHANNEL_SEND_UI_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI_THREAD(app_, channel_id_, thread_name_, ...) \
    DCC_APP_SEND_UI_THREAD((app_), (channel_id_), (thread_name_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI_THREAD_CB(app_, channel_id_, thread_name_, cb_, user_data_, ...) \
    DCC_APP_SEND_UI_THREAD_CB((app_), (channel_id_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_FETCH(app_, channel_id_) \
    DCC_APP_GET_CHANNEL((app_), (channel_id_))
#define DCC_CHANNEL_FETCH_CB(app_, channel_id_, cb_, user_data_) \
    DCC_APP_GET_CHANNEL_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_GET(app_, channel_id_) \
    DCC_CHANNEL_FETCH((app_), (channel_id_))
#define DCC_CHANNEL_GET_CB(app_, channel_id_, cb_, user_data_) \
    DCC_CHANNEL_FETCH_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_EDIT(app_, channel_id_, json_body_) \
    DCC_APP_EDIT_CHANNEL((app_), (channel_id_), (json_body_))
#define DCC_CHANNEL_EDIT_CB(app_, channel_id_, json_body_, cb_, user_data_) \
    DCC_APP_EDIT_CHANNEL_CB((app_), (channel_id_), (json_body_), (cb_), (user_data_))
#define DCC_CHANNEL_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_CHANNEL_PARAMS((app_), (params_))
#define DCC_CHANNEL_EDIT_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_EDIT_CHANNEL_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_CHANNEL_DELETE(app_, channel_id_) \
    DCC_APP_DELETE_CHANNEL((app_), (channel_id_))
#define DCC_CHANNEL_DELETE_CB(app_, channel_id_, cb_, user_data_) \
    DCC_APP_DELETE_CHANNEL_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_TYPING(app_, channel_id_) \
    DCC_APP_TRIGGER_CHANNEL_TYPING((app_), (channel_id_))
#define DCC_CHANNEL_TYPING_CB(app_, channel_id_, cb_, user_data_) \
    DCC_APP_TRIGGER_CHANNEL_TYPING_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_TRIGGER_TYPING(app_, channel_id_) \
    DCC_CHANNEL_TYPING((app_), (channel_id_))
#define DCC_CHANNEL_TRIGGER_TYPING_CB(app_, channel_id_, cb_, user_data_) \
    DCC_CHANNEL_TYPING_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_FOLLOW(app_, channel_id_, webhook_channel_id_) \
    DCC_APP_FOLLOW_NEWS_CHANNEL((app_), (channel_id_), (webhook_channel_id_))
#define DCC_CHANNEL_FOLLOW_CB(app_, channel_id_, webhook_channel_id_, cb_, user_data_) \
    DCC_APP_FOLLOW_NEWS_CHANNEL_CB((app_), (channel_id_), (webhook_channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_SET_VOICE_STATUS(app_, channel_id_, status_) \
    DCC_APP_SET_CHANNEL_VOICE_STATUS((app_), (channel_id_), (status_))
#define DCC_CHANNEL_SET_VOICE_STATUS_CB(app_, channel_id_, status_, cb_, user_data_) \
    DCC_APP_SET_CHANNEL_VOICE_STATUS_CB((app_), (channel_id_), (status_), (cb_), (user_data_))
#define DCC_MY_VOICE_STATE_FETCH(app_, guild_id_) \
    DCC_APP_GET_MY_VOICE_STATE((app_), (guild_id_))
#define DCC_MY_VOICE_STATE_EDIT(app_, guild_id_, json_body_) \
    DCC_APP_EDIT_MY_VOICE_STATE((app_), (guild_id_), (json_body_))
#define DCC_MY_VOICE_STATE_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_MY_VOICE_STATE_PARAMS((app_), (params_))
#define DCC_USER_VOICE_STATE_FETCH(app_, guild_id_, user_id_) \
    DCC_APP_GET_USER_VOICE_STATE((app_), (guild_id_), (user_id_))
#define DCC_USER_VOICE_STATE_EDIT(app_, guild_id_, user_id_, json_body_) \
    DCC_APP_EDIT_USER_VOICE_STATE((app_), (guild_id_), (user_id_), (json_body_))
#define DCC_USER_VOICE_STATE_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_USER_VOICE_STATE_PARAMS((app_), (params_))
#define DCC_VOICE_REGIONS_FETCH(app_) \
    DCC_APP_GET_VOICE_REGIONS((app_))
#define DCC_GUILD_VOICE_REGIONS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_VOICE_REGIONS((app_), (guild_id_))
#define DCC_CHANNEL_INVITES_FETCH(app_, channel_id_) \
    DCC_APP_GET_CHANNEL_INVITES((app_), (channel_id_))
#define DCC_CHANNEL_INVITES_FETCH_CB(app_, channel_id_, cb_, user_data_) \
    DCC_APP_GET_CHANNEL_INVITES_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_INVITE_CREATE(app_, channel_id_, json_body_) \
    DCC_APP_CREATE_CHANNEL_INVITE((app_), (channel_id_), (json_body_))
#define DCC_CHANNEL_INVITE_CREATE_CB(app_, channel_id_, json_body_, cb_, user_data_) \
    DCC_APP_CREATE_CHANNEL_INVITE_CB((app_), (channel_id_), (json_body_), (cb_), (user_data_))
#define DCC_CHANNEL_INVITE_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_CHANNEL_INVITE_PARAMS((app_), (params_))
#define DCC_CHANNEL_INVITE_CREATE_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_CREATE_CHANNEL_INVITE_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_CHANNEL_PERMISSION_EDIT(app_, channel_id_, overwrite_id_, json_body_) \
    DCC_APP_EDIT_CHANNEL_PERMISSION((app_), (channel_id_), (overwrite_id_), (json_body_))
#define DCC_CHANNEL_PERMISSION_EDIT_CB(app_, channel_id_, overwrite_id_, json_body_, cb_, user_data_) \
    DCC_APP_EDIT_CHANNEL_PERMISSION_CB((app_), (channel_id_), (overwrite_id_), (json_body_), (cb_), (user_data_))
#define DCC_CHANNEL_PERMISSION_EDIT_PARAMS(app_, channel_id_, overwrite_) \
    DCC_APP_EDIT_CHANNEL_PERMISSION_PARAMS((app_), (channel_id_), (overwrite_))
#define DCC_CHANNEL_PERMISSION_EDIT_PARAMS_CB(app_, channel_id_, overwrite_, cb_, user_data_) \
    DCC_APP_EDIT_CHANNEL_PERMISSION_PARAMS_CB((app_), (channel_id_), (overwrite_), (cb_), (user_data_))
#define DCC_CHANNEL_PERMISSION_DELETE(app_, channel_id_, overwrite_id_) \
    DCC_APP_DELETE_CHANNEL_PERMISSION((app_), (channel_id_), (overwrite_id_))
#define DCC_CHANNEL_PERMISSION_DELETE_CB(app_, channel_id_, overwrite_id_, cb_, user_data_) \
    DCC_APP_DELETE_CHANNEL_PERMISSION_CB((app_), (channel_id_), (overwrite_id_), (cb_), (user_data_))
#define DCC_GUILD_CHANNELS_REORDER(app_, guild_id_, json_body_) \
    DCC_APP_REORDER_GUILD_CHANNELS((app_), (guild_id_), (json_body_))
#define DCC_GUILD_CHANNELS_REORDER_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    DCC_APP_REORDER_GUILD_CHANNELS_CB((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_GUILD_CHANNELS_REORDER_PARAMS(app_, params_) \
    DCC_APP_REORDER_GUILD_CHANNELS_PARAMS((app_), (params_))
#define DCC_GUILD_CHANNELS_REORDER_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_REORDER_GUILD_CHANNELS_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_GUILD_CHANNELS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_CHANNELS((app_), (guild_id_))
#define DCC_GUILD_CHANNELS_FETCH_CB(app_, guild_id_, cb_, user_data_) \
    DCC_APP_GET_GUILD_CHANNELS_CB((app_), (guild_id_), (cb_), (user_data_))
#define DCC_GUILD_CHANNEL_CREATE(app_, guild_id_, json_body_) \
    DCC_APP_CREATE_GUILD_CHANNEL((app_), (guild_id_), (json_body_))
#define DCC_GUILD_CHANNEL_CREATE_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    DCC_APP_CREATE_GUILD_CHANNEL_CB((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_GUILD_CHANNEL_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_CHANNEL_PARAMS((app_), (params_))
#define DCC_GUILD_CHANNEL_CREATE_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_CREATE_GUILD_CHANNEL_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_GUILD_ROLES_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_ROLES((app_), (guild_id_))
#define DCC_GUILD_ROLES_FETCH_CB(app_, guild_id_, cb_, user_data_) \
    DCC_APP_GET_GUILD_ROLES_CB((app_), (guild_id_), (cb_), (user_data_))
#define DCC_GUILD_ROLES_REORDER(app_, guild_id_, positions_, position_count_) \
    DCC_APP_REORDER_GUILD_ROLES((app_), (guild_id_), (positions_), (position_count_))
#define DCC_GUILD_ROLES_REORDER_CB(app_, guild_id_, positions_, position_count_, cb_, user_data_) \
    DCC_APP_REORDER_GUILD_ROLES_CB((app_), (guild_id_), (positions_), (position_count_), (cb_), (user_data_))
#define DCC_GUILD_ROLE_CREATE(app_, guild_id_, json_body_) \
    DCC_APP_CREATE_GUILD_ROLE((app_), (guild_id_), (json_body_))
#define DCC_GUILD_ROLE_CREATE_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    DCC_APP_CREATE_GUILD_ROLE_CB((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_GUILD_ROLE_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_ROLE_PARAMS((app_), (params_))
#define DCC_GUILD_ROLE_CREATE_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_CREATE_GUILD_ROLE_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_ROLE_EDIT(app_, guild_id_, role_id_, json_body_) \
    DCC_APP_EDIT_GUILD_ROLE((app_), (guild_id_), (role_id_), (json_body_))
#define DCC_ROLE_EDIT_CB(app_, guild_id_, role_id_, json_body_, cb_, user_data_) \
    DCC_APP_EDIT_GUILD_ROLE_CB((app_), (guild_id_), (role_id_), (json_body_), (cb_), (user_data_))
#define DCC_ROLE_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_GUILD_ROLE_PARAMS((app_), (params_))
#define DCC_ROLE_EDIT_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_EDIT_GUILD_ROLE_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_ROLE_DELETE(app_, guild_id_, role_id_) \
    DCC_APP_DELETE_GUILD_ROLE((app_), (guild_id_), (role_id_))
#define DCC_ROLE_DELETE_CB(app_, guild_id_, role_id_, cb_, user_data_) \
    DCC_APP_DELETE_GUILD_ROLE_CB((app_), (guild_id_), (role_id_), (cb_), (user_data_))

#define DCC_APP_ADD_MEMBER_ROLE(app_, guild_id_, user_id_, role_id_) \
    dcc_app_add_member_role((app_), (guild_id_), (user_id_), (role_id_), NULL, NULL)
#define DCC_APP_ADD_MEMBER_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    dcc_app_add_member_role((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_APP_REMOVE_MEMBER_ROLE(app_, guild_id_, user_id_, role_id_) \
    dcc_app_remove_member_role((app_), (guild_id_), (user_id_), (role_id_), NULL, NULL)
#define DCC_APP_REMOVE_MEMBER_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    dcc_app_remove_member_role((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_MEMBER_ADD_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_APP_ADD_MEMBER_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_MEMBER_ADD_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_APP_ADD_MEMBER_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_MEMBER_REMOVE_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_APP_REMOVE_MEMBER_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_MEMBER_REMOVE_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_APP_REMOVE_MEMBER_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_ADD_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_MEMBER_ADD_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_GUILD_MEMBER_ADD_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_MEMBER_ADD_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_REMOVE_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_MEMBER_REMOVE_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_GUILD_MEMBER_REMOVE_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_MEMBER_REMOVE_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_MEMBER_FETCH(app_, guild_id_, user_id_) \
    dcc_app_get_member((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_MEMBER_FETCH_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_get_member((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_MEMBER_TIMEOUT(app_, guild_id_, user_id_, until_) \
    dcc_app_set_member_timeout((app_), (guild_id_), (user_id_), (until_), NULL, NULL)
#define DCC_MEMBER_TIMEOUT_CB(app_, guild_id_, user_id_, until_, cb_, user_data_) \
    dcc_app_set_member_timeout((app_), (guild_id_), (user_id_), (until_), (cb_), (user_data_))
#define DCC_MEMBER_CLEAR_TIMEOUT(app_, guild_id_, user_id_) \
    dcc_app_clear_member_timeout((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_MEMBER_CLEAR_TIMEOUT_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_clear_member_timeout((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_MEMBER_MOVE(app_, guild_id_, user_id_, channel_id_) \
    dcc_app_move_member((app_), (guild_id_), (user_id_), (channel_id_), NULL, NULL)
#define DCC_MEMBER_MOVE_CB(app_, guild_id_, user_id_, channel_id_, cb_, user_data_) \
    dcc_app_move_member((app_), (guild_id_), (user_id_), (channel_id_), (cb_), (user_data_))
#define DCC_MEMBER_KICK(app_, guild_id_, user_id_) \
    dcc_app_remove_member((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_MEMBER_KICK_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_remove_member((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_FETCH(app_, guild_id_, user_id_) \
    DCC_MEMBER_FETCH((app_), (guild_id_), (user_id_))
#define DCC_GUILD_MEMBER_FETCH_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    DCC_MEMBER_FETCH_CB((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_TIMEOUT(app_, guild_id_, user_id_, until_) \
    DCC_MEMBER_TIMEOUT((app_), (guild_id_), (user_id_), (until_))
#define DCC_GUILD_MEMBER_TIMEOUT_CB(app_, guild_id_, user_id_, until_, cb_, user_data_) \
    DCC_MEMBER_TIMEOUT_CB((app_), (guild_id_), (user_id_), (until_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_CLEAR_TIMEOUT(app_, guild_id_, user_id_) \
    DCC_MEMBER_CLEAR_TIMEOUT((app_), (guild_id_), (user_id_))
#define DCC_GUILD_MEMBER_CLEAR_TIMEOUT_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    DCC_MEMBER_CLEAR_TIMEOUT_CB((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_MOVE(app_, guild_id_, user_id_, channel_id_) \
    DCC_MEMBER_MOVE((app_), (guild_id_), (user_id_), (channel_id_))
#define DCC_GUILD_MEMBER_MOVE_CB(app_, guild_id_, user_id_, channel_id_, cb_, user_data_) \
    DCC_MEMBER_MOVE_CB((app_), (guild_id_), (user_id_), (channel_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_KICK(app_, guild_id_, user_id_) \
    DCC_MEMBER_KICK((app_), (guild_id_), (user_id_))
#define DCC_GUILD_MEMBER_KICK_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    DCC_MEMBER_KICK_CB((app_), (guild_id_), (user_id_), (cb_), (user_data_))

#define DCC_MESSAGE_EDIT(app_, channel_id_, message_id_, message_) \
    DCC_APP_EDIT_MESSAGE((app_), (channel_id_), (message_id_), (message_))
#define DCC_MESSAGE_EDIT_CB(app_, channel_id_, message_id_, message_, cb_, user_data_) \
    DCC_APP_EDIT_MESSAGE_CB((app_), (channel_id_), (message_id_), (message_), (cb_), (user_data_))
#define DCC_MESSAGE_EDIT_TEXT(app_, channel_id_, message_id_, content_) \
    DCC_APP_EDIT_TEXT((app_), (channel_id_), (message_id_), (content_))
#define DCC_MESSAGE_EDIT_TEXT_CB(app_, channel_id_, message_id_, content_, cb_, user_data_) \
    DCC_APP_EDIT_TEXT_CB((app_), (channel_id_), (message_id_), (content_), (cb_), (user_data_))
#define DCC_MESSAGE_EDIT_V2(app_, channel_id_, message_id_, ...) \
    DCC_APP_EDIT_V2((app_), (channel_id_), (message_id_), __VA_ARGS__)
#define DCC_MESSAGE_EDIT_UI(app_, channel_id_, message_id_, ...) \
    DCC_APP_EDIT_UI((app_), (channel_id_), (message_id_), __VA_ARGS__)
#define DCC_MESSAGE_DELETE(app_, channel_id_, message_id_) \
    DCC_APP_DELETE_MESSAGE((app_), (channel_id_), (message_id_))
#define DCC_MESSAGE_DELETE_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    DCC_APP_DELETE_MESSAGE_CB((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_MESSAGE_CROSSPOST(app_, channel_id_, message_id_) \
    DCC_APP_CROSSPOST_MESSAGE((app_), (channel_id_), (message_id_))
#define DCC_MESSAGE_CROSSPOST_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    DCC_APP_CROSSPOST_MESSAGE_CB((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_MESSAGE_PIN(app_, channel_id_, message_id_) \
    DCC_APP_PIN_MESSAGE((app_), (channel_id_), (message_id_))
#define DCC_MESSAGE_PIN_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    DCC_APP_PIN_MESSAGE_CB((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_MESSAGE_UNPIN(app_, channel_id_, message_id_) \
    DCC_APP_UNPIN_MESSAGE((app_), (channel_id_), (message_id_))
#define DCC_MESSAGE_UNPIN_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    DCC_APP_UNPIN_MESSAGE_CB((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_MESSAGE_REACT(app_, channel_id_, message_id_, reaction_) \
    DCC_APP_ADD_MESSAGE_REACTION((app_), (channel_id_), (message_id_), (reaction_))
#define DCC_MESSAGE_REACT_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    DCC_APP_ADD_MESSAGE_REACTION_CB((app_), (channel_id_), (message_id_), (reaction_), (cb_), (user_data_))
#define DCC_MESSAGE_UNREACT(app_, channel_id_, message_id_, reaction_) \
    DCC_APP_DELETE_OWN_MESSAGE_REACTION((app_), (channel_id_), (message_id_), (reaction_))
#define DCC_MESSAGE_UNREACT_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    DCC_APP_DELETE_OWN_MESSAGE_REACTION_CB((app_), (channel_id_), (message_id_), (reaction_), (cb_), (user_data_))
#define DCC_MESSAGE_REMOVE_REACTION(app_, channel_id_, message_id_, reaction_, user_id_) \
    DCC_APP_DELETE_USER_MESSAGE_REACTION((app_), (channel_id_), (message_id_), (reaction_), (user_id_))
#define DCC_MESSAGE_REMOVE_REACTION_CB(app_, channel_id_, message_id_, reaction_, user_id_, cb_, user_data_) \
    DCC_APP_DELETE_USER_MESSAGE_REACTION_CB( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (reaction_), \
        (user_id_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_MESSAGE_CLEAR_REACTIONS(app_, channel_id_, message_id_) \
    DCC_APP_CLEAR_MESSAGE_REACTIONS((app_), (channel_id_), (message_id_))
#define DCC_MESSAGE_CLEAR_REACTIONS_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    DCC_APP_CLEAR_MESSAGE_REACTIONS_CB((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_MESSAGE_CLEAR_REACTION_EMOJI(app_, channel_id_, message_id_, reaction_) \
    DCC_APP_CLEAR_MESSAGE_REACTIONS_FOR_EMOJI((app_), (channel_id_), (message_id_), (reaction_))
#define DCC_MESSAGE_CLEAR_REACTION_EMOJI_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    DCC_APP_CLEAR_MESSAGE_REACTIONS_FOR_EMOJI_CB( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (reaction_), \
        (cb_), \
        (user_data_) \
    )

#define DCC_MESSAGE_THREAD(app_, channel_id_, message_id_, name_) \
    DCC_APP_THREAD_FROM_MESSAGE((app_), (channel_id_), (message_id_), (name_))
#define DCC_MESSAGE_THREAD_CB(app_, channel_id_, message_id_, name_, cb_, user_data_) \
    DCC_APP_THREAD_FROM_MESSAGE_CB((app_), (channel_id_), (message_id_), (name_), (cb_), (user_data_))
#define DCC_MESSAGE_THREAD_PARAMS(app_, channel_id_, message_id_, params_) \
    DCC_APP_THREAD_FROM_MESSAGE_PARAMS((app_), (channel_id_), (message_id_), (params_))
#define DCC_MESSAGE_THREAD_PARAMS_CB(app_, channel_id_, message_id_, params_, cb_, user_data_) \
    DCC_APP_THREAD_FROM_MESSAGE_PARAMS_CB( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (params_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_THREAD_FROM_MESSAGE(app_, channel_id_, message_id_, name_) \
    DCC_MESSAGE_THREAD((app_), (channel_id_), (message_id_), (name_))
#define DCC_THREAD_FROM_MESSAGE_PARAMS(app_, channel_id_, message_id_, params_) \
    DCC_MESSAGE_THREAD_PARAMS((app_), (channel_id_), (message_id_), (params_))
#define DCC_THREAD_ARCHIVE(app_, thread_id_) \
    DCC_APP_ARCHIVE_THREAD((app_), (thread_id_))
#define DCC_THREAD_ARCHIVE_CB(app_, thread_id_, cb_, user_data_) \
    DCC_APP_ARCHIVE_THREAD_CB((app_), (thread_id_), (cb_), (user_data_))
#define DCC_THREAD_LOCK(app_, thread_id_) \
    DCC_APP_LOCK_THREAD((app_), (thread_id_))
#define DCC_THREAD_LOCK_CB(app_, thread_id_, cb_, user_data_) \
    DCC_APP_LOCK_THREAD_CB((app_), (thread_id_), (cb_), (user_data_))
#define DCC_THREAD_UNLOCK(app_, thread_id_) \
    DCC_APP_UNLOCK_THREAD((app_), (thread_id_))
#define DCC_THREAD_UNLOCK_CB(app_, thread_id_, cb_, user_data_) \
    DCC_APP_UNLOCK_THREAD_CB((app_), (thread_id_), (cb_), (user_data_))
