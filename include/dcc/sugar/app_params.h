#ifndef DCC_SUGAR_APP_PARAMS_H
#define DCC_SUGAR_APP_PARAMS_H

#include <dcc/sugar/app_typed.h>

#define DCC_MODAL_PARAM_BIND(type_, kind_, ...) DCC_MODAL_PARAM_BIND_##kind_(type_, __VA_ARGS__),

#define DCC_MODAL_PARAM_BIND_STRING(type_, field_, custom_id_, fallback_) \
    DCC_FORM_ARG_STRING(type_, field_, custom_id_, fallback_)
#define DCC_MODAL_PARAM_BIND_REQUIRED_STRING(type_, field_, custom_id_) \
    DCC_FORM_ARG_REQUIRED_STRING(type_, field_, custom_id_)
#define DCC_MODAL_PARAM_BIND_BOOL(type_, field_, custom_id_, fallback_) \
    DCC_FORM_ARG_BOOL(type_, field_, custom_id_, fallback_)
#define DCC_MODAL_PARAM_BIND_REQUIRED_BOOL(type_, field_, custom_id_) \
    DCC_FORM_ARG_REQUIRED_BOOL(type_, field_, custom_id_)
#define DCC_MODAL_PARAM_BIND_VALUES(type_, field_, count_field_, custom_id_, fallback_values_, fallback_count_) \
    DCC_FORM_ARG_VALUES(type_, field_, count_field_, custom_id_, fallback_values_, fallback_count_)
#define DCC_MODAL_PARAM_BIND_REQUIRED_VALUES(type_, field_, count_field_, custom_id_) \
    DCC_FORM_ARG_REQUIRED_VALUES(type_, field_, count_field_, custom_id_)

#define DCC_MODAL_PARAMS_BINDINGS(args_type_, params_) DCC_FORM_ARGS(params_(DCC_MODAL_PARAM_BIND, args_type_))

#define DCC_ON_MODAL_PARAMS_DATA_VALIDATED(custom_id_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_TYPED_MODAL((custom_id_), args_type_, (handler_), (user_data_), \
        DCC_MODAL_PARAMS_BINDINGS(args_type_, params_), (validators_))
#define DCC_ON_MODAL_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_ON_MODAL_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), NULL, params_, (validators_))
#define DCC_ON_MODAL_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_ON_MODAL_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_MODAL_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_ON_MODAL_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_MODAL_PARAMS_DATA_POLICY(custom_id_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_MODAL_POLICY((custom_id_), args_type_, (handler_), (user_data_), \
        DCC_MODAL_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_))
#define DCC_ON_MODAL_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_MODAL_PARAMS_DATA_POLICY( \
        (custom_id_), args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_MODAL_PREFIX_PARAMS_DATA_VALIDATED(custom_id_prefix_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_TYPED_MODAL_PREFIX((custom_id_prefix_), args_type_, (handler_), (user_data_), \
        DCC_MODAL_PARAMS_BINDINGS(args_type_, params_), (validators_))
#define DCC_ON_MODAL_PREFIX_PARAMS_VALIDATED(custom_id_prefix_, args_type_, handler_, params_, validators_) \
    DCC_ON_MODAL_PREFIX_PARAMS_DATA_VALIDATED((custom_id_prefix_), args_type_, (handler_), NULL, params_, (validators_))
#define DCC_ON_MODAL_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_ON_MODAL_PREFIX_PARAMS_DATA_VALIDATED((custom_id_prefix_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_MODAL_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_ON_MODAL_PREFIX_PARAMS_VALIDATED((custom_id_prefix_), args_type_, (handler_), params_, DCC_NO_VALIDATORS())

#define DCC_MODAL_FLOW_PARAMS_DATA_VALIDATED(custom_id_, title_, args_type_, handler_, user_data_, params_, validators_, ...) \
    DCC_MODAL_FLOW((custom_id_), (title_), args_type_, (handler_), (user_data_), \
        DCC_MODAL_PARAMS_BINDINGS(args_type_, params_), (validators_), __VA_ARGS__)
#define DCC_MODAL_FLOW_PARAMS_VALIDATED(custom_id_, title_, args_type_, handler_, params_, validators_, ...) \
    DCC_MODAL_FLOW_PARAMS_DATA_VALIDATED((custom_id_), (title_), args_type_, (handler_), NULL, params_, (validators_), __VA_ARGS__)
#define DCC_MODAL_FLOW_PARAMS_DATA(custom_id_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_MODAL_FLOW_PARAMS_DATA_VALIDATED((custom_id_), (title_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__)
#define DCC_MODAL_FLOW_PARAMS(custom_id_, title_, args_type_, handler_, params_, ...) \
    DCC_MODAL_FLOW_PARAMS_VALIDATED((custom_id_), (title_), args_type_, (handler_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__)

#define DCC_MODAL_V2_FLOW_PARAMS_DATA_VALIDATED(custom_id_, title_, args_type_, handler_, user_data_, params_, validators_, ...) \
    DCC_MODAL_V2_FLOW((custom_id_), (title_), args_type_, (handler_), (user_data_), \
        DCC_MODAL_PARAMS_BINDINGS(args_type_, params_), (validators_), __VA_ARGS__)
#define DCC_MODAL_V2_FLOW_PARAMS_VALIDATED(custom_id_, title_, args_type_, handler_, params_, validators_, ...) \
    DCC_MODAL_V2_FLOW_PARAMS_DATA_VALIDATED((custom_id_), (title_), args_type_, (handler_), NULL, params_, (validators_), __VA_ARGS__)
#define DCC_MODAL_V2_FLOW_PARAMS_DATA(custom_id_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_MODAL_V2_FLOW_PARAMS_DATA_VALIDATED((custom_id_), (title_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__)
#define DCC_MODAL_V2_FLOW_PARAMS(custom_id_, title_, args_type_, handler_, params_, ...) \
    DCC_MODAL_V2_FLOW_PARAMS_VALIDATED((custom_id_), (title_), args_type_, (handler_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__)

#define DCC_COMPONENT_PARAM_BIND(type_, kind_, ...) DCC_COMPONENT_PARAM_BIND_##kind_(type_, __VA_ARGS__),

#define DCC_COMPONENT_PARAM_BIND_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_) \
    DCC_COMPONENT_ARG_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_)
#define DCC_COMPONENT_PARAM_BIND_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_) \
    DCC_COMPONENT_ARG_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_)
#define DCC_COMPONENT_PARAM_BIND_FIRST_VALUE(type_, field_, fallback_) \
    DCC_COMPONENT_ARG_FIRST_VALUE(type_, field_, fallback_)
#define DCC_COMPONENT_PARAM_BIND_REQUIRED_FIRST_VALUE(type_, field_) \
    DCC_COMPONENT_ARG_REQUIRED_FIRST_VALUE(type_, field_)
#define DCC_COMPONENT_PARAM_BIND_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_) \
    DCC_COMPONENT_ARG_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_)
#define DCC_COMPONENT_PARAM_BIND_REQUIRED_VALUES(type_, field_, count_field_) \
    DCC_COMPONENT_ARG_REQUIRED_VALUES(type_, field_, count_field_)

#define DCC_COMPONENT_PARAMS_BINDINGS(args_type_, params_) DCC_COMPONENT_ARGS(params_(DCC_COMPONENT_PARAM_BIND, args_type_))

#define DCC_ON_BUTTON_PARAMS_DATA_VALIDATED(custom_id_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_TYPED_BUTTON((custom_id_), args_type_, (handler_), (user_data_), \
        DCC_COMPONENT_PARAMS_BINDINGS(args_type_, params_), (validators_))
#define DCC_ON_BUTTON_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_ON_BUTTON_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), NULL, params_, (validators_))
#define DCC_ON_BUTTON_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_ON_BUTTON_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_BUTTON_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_ON_BUTTON_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_BUTTON_PARAMS_DATA_POLICY(custom_id_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_BUTTON_POLICY((custom_id_), args_type_, (handler_), (user_data_), \
        DCC_COMPONENT_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_))
#define DCC_ON_BUTTON_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_BUTTON_PARAMS_DATA_POLICY( \
        (custom_id_), args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_BUTTON_PREFIX_PARAMS_DATA_VALIDATED(custom_id_prefix_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_TYPED_BUTTON_PREFIX((custom_id_prefix_), args_type_, (handler_), (user_data_), \
        DCC_COMPONENT_PARAMS_BINDINGS(args_type_, params_), (validators_))
#define DCC_ON_BUTTON_PREFIX_PARAMS_VALIDATED(custom_id_prefix_, args_type_, handler_, params_, validators_) \
    DCC_ON_BUTTON_PREFIX_PARAMS_DATA_VALIDATED((custom_id_prefix_), args_type_, (handler_), NULL, params_, (validators_))
#define DCC_ON_BUTTON_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_ON_BUTTON_PREFIX_PARAMS_DATA_VALIDATED((custom_id_prefix_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_BUTTON_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_ON_BUTTON_PREFIX_PARAMS_VALIDATED((custom_id_prefix_), args_type_, (handler_), params_, DCC_NO_VALIDATORS())

#define DCC_ON_SELECT_PARAMS_DATA_VALIDATED(custom_id_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_TYPED_SELECT((custom_id_), args_type_, (handler_), (user_data_), \
        DCC_COMPONENT_PARAMS_BINDINGS(args_type_, params_), (validators_))
#define DCC_ON_SELECT_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_ON_SELECT_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), NULL, params_, (validators_))
#define DCC_ON_SELECT_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_ON_SELECT_PARAMS_DATA_VALIDATED((custom_id_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_SELECT_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_ON_SELECT_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_SELECT_PARAMS_DATA_POLICY(custom_id_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_SELECT_POLICY((custom_id_), args_type_, (handler_), (user_data_), \
        DCC_COMPONENT_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_))
#define DCC_ON_SELECT_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_SELECT_PARAMS_DATA_POLICY( \
        (custom_id_), args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_SLASH_PARAMS_DATA_POLICY(name_, description_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_SLASH_OPTIONS_DATA_POLICY( \
        (name_), (description_), args_type_, (handler_), (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_), \
        params_(DCC_SLASH_PARAM_OPTION, args_type_) \
    )
#define DCC_ON_SLASH_PARAMS_DATA_GUARDED(name_, description_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_SLASH_PARAMS_DATA_POLICY( \
        (name_), (description_), args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_SLASH_PARAMS_FN_DATA_POLICY(handler_, description_, args_type_, user_data_, params_, policy_) \
    DCC_ON_SLASH_PARAMS_DATA_POLICY( \
        #handler_, (description_), args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_SLASH_PARAMS_FN_DATA_GUARDED(handler_, description_, args_type_, user_data_, params_, ...) \
    DCC_ON_SLASH_PARAMS_FN_DATA_POLICY( \
        handler_, (description_), args_type_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_SUBCOMMAND_PARAMS_DATA_POLICY(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_SUBCOMMAND_POLICY( \
        (command_name_), (subcommand_path_), args_type_, (handler_), (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_) \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_DATA_GUARDED(command_name_, subcommand_path_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_POLICY( \
        (command_name_), \
        (subcommand_path_), \
        args_type_, \
        (handler_), \
        (user_data_), \
        params_, \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_POLICY(command_name_, handler_, args_type_, user_data_, params_, policy_) \
    DCC_ON_SUBCOMMAND_PARAMS_DATA_POLICY( \
        (command_name_), #handler_, args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_GUARDED(command_name_, handler_, args_type_, user_data_, params_, ...) \
    DCC_ON_SUBCOMMAND_PARAMS_FN_DATA_POLICY( \
        (command_name_), handler_, args_type_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_TYPED_SLASH_BUILDER_DATA_POLICY(command_, args_type_, handler_, user_data_, bindings_, validators_, policy_) \
    DCC_TYPED_SLASH_POLICY((command_), args_type_, (handler_), (user_data_), (bindings_), (validators_), (policy_))
#define DCC_ON_TYPED_SLASH_BUILDER_DATA_GUARDED(command_, args_type_, handler_, user_data_, bindings_, validators_, ...) \
    DCC_ON_TYPED_SLASH_BUILDER_DATA_POLICY( \
        (command_), args_type_, (handler_), (user_data_), (bindings_), (validators_), DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_AUTOCOMPLETE_PARAMS_DATA_POLICY(command_name_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_AUTOCOMPLETE_POLICY( \
        (command_name_), args_type_, (handler_), (user_data_), \
        DCC_SLASH_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_) \
    )
#define DCC_ON_AUTOCOMPLETE_PARAMS_DATA_GUARDED(command_name_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_AUTOCOMPLETE_PARAMS_DATA_POLICY( \
        (command_name_), args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_POLICY(handler_, args_type_, user_data_, params_, policy_) \
    DCC_ON_AUTOCOMPLETE_PARAMS_DATA_POLICY( \
        #handler_, args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_GUARDED(handler_, args_type_, user_data_, params_, ...) \
    DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_POLICY( \
        handler_, args_type_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_SELECT_PREFIX_PARAMS_DATA_VALIDATED(custom_id_prefix_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_TYPED_SELECT_PREFIX((custom_id_prefix_), args_type_, (handler_), (user_data_), \
        DCC_COMPONENT_PARAMS_BINDINGS(args_type_, params_), (validators_))
#define DCC_ON_SELECT_PREFIX_PARAMS_VALIDATED(custom_id_prefix_, args_type_, handler_, params_, validators_) \
    DCC_ON_SELECT_PREFIX_PARAMS_DATA_VALIDATED((custom_id_prefix_), args_type_, (handler_), NULL, params_, (validators_))
#define DCC_ON_SELECT_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_ON_SELECT_PREFIX_PARAMS_DATA_VALIDATED((custom_id_prefix_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_SELECT_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_ON_SELECT_PREFIX_PARAMS_VALIDATED((custom_id_prefix_), args_type_, (handler_), params_, DCC_NO_VALIDATORS())

#define DCC_ON_MODAL_PREFIX_PARAMS_DATA_POLICY(custom_id_prefix_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_MODAL_PREFIX_POLICY((custom_id_prefix_), args_type_, (handler_), (user_data_), \
        DCC_MODAL_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_))
#define DCC_ON_MODAL_PREFIX_PARAMS_DATA_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_MODAL_PREFIX_PARAMS_DATA_POLICY( \
        (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_BUTTON_PREFIX_PARAMS_DATA_POLICY(custom_id_prefix_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_BUTTON_PREFIX_POLICY((custom_id_prefix_), args_type_, (handler_), (user_data_), \
        DCC_COMPONENT_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_))
#define DCC_ON_BUTTON_PREFIX_PARAMS_DATA_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_BUTTON_PREFIX_PARAMS_DATA_POLICY( \
        (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_SELECT_PREFIX_PARAMS_DATA_POLICY(custom_id_prefix_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_TYPED_SELECT_PREFIX_POLICY((custom_id_prefix_), args_type_, (handler_), (user_data_), \
        DCC_COMPONENT_PARAMS_BINDINGS(args_type_, params_), DCC_NO_VALIDATORS(), (policy_))
#define DCC_ON_SELECT_PREFIX_PARAMS_DATA_GUARDED(custom_id_prefix_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_SELECT_PREFIX_PARAMS_DATA_POLICY( \
        (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_ON_MODAL_NS_PARAMS_DATA_VALIDATED(namespace_, action_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_ON_MODAL_PARAMS_DATA_VALIDATED( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_MODAL_NS_PARAMS_VALIDATED(namespace_, action_, args_type_, handler_, params_, validators_) \
    DCC_ON_MODAL_NS_PARAMS_DATA_VALIDATED(namespace_, action_, args_type_, (handler_), NULL, params_, (validators_))
#define DCC_ON_MODAL_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_ON_MODAL_NS_PARAMS_DATA_VALIDATED( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS() \
    )
#define DCC_ON_MODAL_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_ON_MODAL_NS_PARAMS_VALIDATED(namespace_, action_, args_type_, (handler_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_MODAL_NS_PARAMS_DATA_POLICY(namespace_, action_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_ON_MODAL_PARAMS_DATA_POLICY( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_MODAL_NS_PARAMS_DATA_GUARDED(namespace_, action_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_MODAL_NS_PARAMS_DATA_POLICY( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_MODAL_NS_PARAMS_FN_VALIDATED(namespace_, handler_, args_type_, params_, validators_) \
    DCC_ON_MODAL_NS_PARAMS_VALIDATED(namespace_, #handler_, args_type_, (handler_), params_, (validators_))
#define DCC_ON_MODAL_NS_PARAMS_FN_DATA_VALIDATED(namespace_, handler_, args_type_, user_data_, params_, validators_) \
    DCC_ON_MODAL_NS_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_MODAL_NS_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_ON_MODAL_NS_PARAMS(namespace_, #handler_, args_type_, (handler_), params_)
#define DCC_ON_MODAL_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_ON_MODAL_NS_PARAMS_DATA(namespace_, #handler_, args_type_, (handler_), (user_data_), params_)
#define DCC_ON_MODAL_NS_PARAMS_FN_DATA_POLICY(namespace_, handler_, args_type_, user_data_, params_, policy_) \
    DCC_ON_MODAL_NS_PARAMS_DATA_POLICY( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_MODAL_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_ON_MODAL_NS_PARAMS_DATA_GUARDED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_VALIDATED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_ON_MODAL_PREFIX_PARAMS_DATA_VALIDATED( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_MODAL_NS_PREFIX_PARAMS_VALIDATED(namespace_, action_prefix_, args_type_, handler_, params_, validators_) \
    DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_VALIDATED( \
        namespace_, action_prefix_, args_type_, (handler_), NULL, params_, (validators_) \
    )
#define DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA(namespace_, action_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_VALIDATED( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS() \
    )
#define DCC_ON_MODAL_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_ON_MODAL_NS_PREFIX_PARAMS_VALIDATED( \
        namespace_, action_prefix_, args_type_, (handler_), params_, DCC_NO_VALIDATORS() \
    )
#define DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_POLICY( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, policy_ \
) \
    DCC_ON_MODAL_PREFIX_PARAMS_DATA_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_POLICY( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_VALIDATED(namespace_, handler_, args_type_, params_, validators_) \
    DCC_ON_MODAL_NS_PREFIX_PARAMS_VALIDATED(namespace_, #handler_, args_type_, (handler_), params_, (validators_))
#define DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA_VALIDATED( \
    namespace_, handler_, args_type_, user_data_, params_, validators_ \
) \
    DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_MODAL_NS_PREFIX_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_ON_MODAL_NS_PREFIX_PARAMS(namespace_, #handler_, args_type_, (handler_), params_)
#define DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA(namespace_, #handler_, args_type_, (handler_), (user_data_), params_)
#define DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA_POLICY(namespace_, handler_, args_type_, user_data_, params_, policy_) \
    DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_POLICY( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
    namespace_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_MODAL_FLOW_NS_PARAMS_DATA_VALIDATED( \
    namespace_, action_, title_, args_type_, handler_, user_data_, params_, validators_, ... \
) \
    DCC_MODAL_FLOW_PARAMS_DATA_VALIDATED( \
        DCC_COMPONENT_ID(namespace_, action_), (title_), args_type_, (handler_), (user_data_), params_, (validators_), __VA_ARGS__ \
    )
#define DCC_MODAL_FLOW_NS_PARAMS_VALIDATED(namespace_, action_, title_, args_type_, handler_, params_, validators_, ...) \
    DCC_MODAL_FLOW_NS_PARAMS_DATA_VALIDATED( \
        namespace_, action_, (title_), args_type_, (handler_), NULL, params_, (validators_), __VA_ARGS__ \
    )
#define DCC_MODAL_FLOW_NS_PARAMS_DATA(namespace_, action_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_MODAL_FLOW_NS_PARAMS_DATA_VALIDATED( \
        namespace_, action_, (title_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__ \
    )
#define DCC_MODAL_FLOW_NS_PARAMS(namespace_, action_, title_, args_type_, handler_, params_, ...) \
    DCC_MODAL_FLOW_NS_PARAMS_VALIDATED( \
        namespace_, action_, (title_), args_type_, (handler_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__ \
    )
#define DCC_MODAL_FLOW_NS_PARAMS_FN_DATA_VALIDATED( \
    namespace_, title_, args_type_, handler_, user_data_, params_, validators_, ... \
) \
    DCC_MODAL_FLOW_NS_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, (title_), args_type_, (handler_), (user_data_), params_, (validators_), __VA_ARGS__ \
    )
#define DCC_MODAL_FLOW_NS_PARAMS_FN_VALIDATED( \
    namespace_, title_, args_type_, handler_, params_, validators_, ... \
) \
    DCC_MODAL_FLOW_NS_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, (title_), args_type_, (handler_), NULL, params_, (validators_), __VA_ARGS__ \
    )
#define DCC_MODAL_FLOW_NS_PARAMS_FN_DATA(namespace_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_MODAL_FLOW_NS_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, (title_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__ \
    )
#define DCC_MODAL_FLOW_NS_PARAMS_FN(namespace_, title_, args_type_, handler_, params_, ...) \
    DCC_MODAL_FLOW_NS_PARAMS_VALIDATED( \
        namespace_, #handler_, (title_), args_type_, (handler_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__ \
    )

#define DCC_MODAL_V2_FLOW_NS_PARAMS_DATA_VALIDATED( \
    namespace_, action_, title_, args_type_, handler_, user_data_, params_, validators_, ... \
) \
    DCC_MODAL_V2_FLOW_PARAMS_DATA_VALIDATED( \
        DCC_COMPONENT_ID(namespace_, action_), (title_), args_type_, (handler_), (user_data_), params_, (validators_), __VA_ARGS__ \
    )
#define DCC_MODAL_V2_FLOW_NS_PARAMS_VALIDATED(namespace_, action_, title_, args_type_, handler_, params_, validators_, ...) \
    DCC_MODAL_V2_FLOW_NS_PARAMS_DATA_VALIDATED( \
        namespace_, action_, (title_), args_type_, (handler_), NULL, params_, (validators_), __VA_ARGS__ \
    )
#define DCC_MODAL_V2_FLOW_NS_PARAMS_DATA(namespace_, action_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_MODAL_V2_FLOW_NS_PARAMS_DATA_VALIDATED( \
        namespace_, action_, (title_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__ \
    )
#define DCC_MODAL_V2_FLOW_NS_PARAMS(namespace_, action_, title_, args_type_, handler_, params_, ...) \
    DCC_MODAL_V2_FLOW_NS_PARAMS_VALIDATED( \
        namespace_, action_, (title_), args_type_, (handler_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__ \
    )
#define DCC_MODAL_V2_FLOW_NS_PARAMS_FN_DATA_VALIDATED( \
    namespace_, title_, args_type_, handler_, user_data_, params_, validators_, ... \
) \
    DCC_MODAL_V2_FLOW_NS_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, (title_), args_type_, (handler_), (user_data_), params_, (validators_), __VA_ARGS__ \
    )
#define DCC_MODAL_V2_FLOW_NS_PARAMS_FN_VALIDATED( \
    namespace_, title_, args_type_, handler_, params_, validators_, ... \
) \
    DCC_MODAL_V2_FLOW_NS_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, (title_), args_type_, (handler_), NULL, params_, (validators_), __VA_ARGS__ \
    )
#define DCC_MODAL_V2_FLOW_NS_PARAMS_FN_DATA(namespace_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_MODAL_V2_FLOW_NS_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, (title_), args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__ \
    )
#define DCC_MODAL_V2_FLOW_NS_PARAMS_FN(namespace_, title_, args_type_, handler_, params_, ...) \
    DCC_MODAL_V2_FLOW_NS_PARAMS_VALIDATED( \
        namespace_, #handler_, (title_), args_type_, (handler_), params_, DCC_NO_VALIDATORS(), __VA_ARGS__ \
    )

#define DCC_ON_BUTTON_NS_PARAMS_DATA_VALIDATED(namespace_, action_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_ON_BUTTON_PARAMS_DATA_VALIDATED( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_BUTTON_NS_PARAMS_VALIDATED(namespace_, action_, args_type_, handler_, params_, validators_) \
    DCC_ON_BUTTON_NS_PARAMS_DATA_VALIDATED(namespace_, action_, args_type_, (handler_), NULL, params_, (validators_))
#define DCC_ON_BUTTON_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_ON_BUTTON_NS_PARAMS_DATA_VALIDATED( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS() \
    )
#define DCC_ON_BUTTON_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_ON_BUTTON_NS_PARAMS_VALIDATED(namespace_, action_, args_type_, (handler_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_BUTTON_NS_PARAMS_DATA_POLICY(namespace_, action_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_ON_BUTTON_PARAMS_DATA_POLICY( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_BUTTON_NS_PARAMS_DATA_GUARDED(namespace_, action_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_BUTTON_NS_PARAMS_DATA_POLICY( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_BUTTON_NS_PARAMS_FN_VALIDATED(namespace_, handler_, args_type_, params_, validators_) \
    DCC_ON_BUTTON_NS_PARAMS_VALIDATED(namespace_, #handler_, args_type_, (handler_), params_, (validators_))
#define DCC_ON_BUTTON_NS_PARAMS_FN_DATA_VALIDATED(namespace_, handler_, args_type_, user_data_, params_, validators_) \
    DCC_ON_BUTTON_NS_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_BUTTON_NS_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_ON_BUTTON_NS_PARAMS(namespace_, #handler_, args_type_, (handler_), params_)
#define DCC_ON_BUTTON_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_ON_BUTTON_NS_PARAMS_DATA(namespace_, #handler_, args_type_, (handler_), (user_data_), params_)
#define DCC_ON_BUTTON_NS_PARAMS_FN_DATA_POLICY(namespace_, handler_, args_type_, user_data_, params_, policy_) \
    DCC_ON_BUTTON_NS_PARAMS_DATA_POLICY( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_BUTTON_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_ON_BUTTON_NS_PARAMS_DATA_GUARDED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_VALIDATED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_ON_BUTTON_PREFIX_PARAMS_DATA_VALIDATED( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_BUTTON_NS_PREFIX_PARAMS_VALIDATED(namespace_, action_prefix_, args_type_, handler_, params_, validators_) \
    DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_VALIDATED( \
        namespace_, action_prefix_, args_type_, (handler_), NULL, params_, (validators_) \
    )
#define DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA(namespace_, action_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_VALIDATED( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS() \
    )
#define DCC_ON_BUTTON_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_ON_BUTTON_NS_PREFIX_PARAMS_VALIDATED( \
        namespace_, action_prefix_, args_type_, (handler_), params_, DCC_NO_VALIDATORS() \
    )
#define DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, policy_ \
) \
    DCC_ON_BUTTON_PREFIX_PARAMS_DATA_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_VALIDATED(namespace_, handler_, args_type_, params_, validators_) \
    DCC_ON_BUTTON_NS_PREFIX_PARAMS_VALIDATED(namespace_, #handler_, args_type_, (handler_), params_, (validators_))
#define DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA_VALIDATED( \
    namespace_, handler_, args_type_, user_data_, params_, validators_ \
) \
    DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_ON_BUTTON_NS_PREFIX_PARAMS(namespace_, #handler_, args_type_, (handler_), params_)
#define DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA(namespace_, #handler_, args_type_, (handler_), (user_data_), params_)
#define DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA_POLICY(namespace_, handler_, args_type_, user_data_, params_, policy_) \
    DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
    namespace_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_ON_SELECT_NS_PARAMS_DATA_VALIDATED(namespace_, action_, args_type_, handler_, user_data_, params_, validators_) \
    DCC_ON_SELECT_PARAMS_DATA_VALIDATED( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_SELECT_NS_PARAMS_VALIDATED(namespace_, action_, args_type_, handler_, params_, validators_) \
    DCC_ON_SELECT_NS_PARAMS_DATA_VALIDATED(namespace_, action_, args_type_, (handler_), NULL, params_, (validators_))
#define DCC_ON_SELECT_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_ON_SELECT_NS_PARAMS_DATA_VALIDATED( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS() \
    )
#define DCC_ON_SELECT_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_ON_SELECT_NS_PARAMS_VALIDATED(namespace_, action_, args_type_, (handler_), params_, DCC_NO_VALIDATORS())
#define DCC_ON_SELECT_NS_PARAMS_DATA_POLICY(namespace_, action_, args_type_, handler_, user_data_, params_, policy_) \
    DCC_ON_SELECT_PARAMS_DATA_POLICY( \
        DCC_COMPONENT_ID(namespace_, action_), args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_SELECT_NS_PARAMS_DATA_GUARDED(namespace_, action_, args_type_, handler_, user_data_, params_, ...) \
    DCC_ON_SELECT_NS_PARAMS_DATA_POLICY( \
        namespace_, action_, args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_SELECT_NS_PARAMS_FN_VALIDATED(namespace_, handler_, args_type_, params_, validators_) \
    DCC_ON_SELECT_NS_PARAMS_VALIDATED(namespace_, #handler_, args_type_, (handler_), params_, (validators_))
#define DCC_ON_SELECT_NS_PARAMS_FN_DATA_VALIDATED(namespace_, handler_, args_type_, user_data_, params_, validators_) \
    DCC_ON_SELECT_NS_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_SELECT_NS_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_ON_SELECT_NS_PARAMS(namespace_, #handler_, args_type_, (handler_), params_)
#define DCC_ON_SELECT_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_ON_SELECT_NS_PARAMS_DATA(namespace_, #handler_, args_type_, (handler_), (user_data_), params_)
#define DCC_ON_SELECT_NS_PARAMS_FN_DATA_POLICY(namespace_, handler_, args_type_, user_data_, params_, policy_) \
    DCC_ON_SELECT_NS_PARAMS_DATA_POLICY( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_SELECT_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_ON_SELECT_NS_PARAMS_DATA_GUARDED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_VALIDATED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_ON_SELECT_PREFIX_PARAMS_DATA_VALIDATED( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_SELECT_NS_PREFIX_PARAMS_VALIDATED(namespace_, action_prefix_, args_type_, handler_, params_, validators_) \
    DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_VALIDATED( \
        namespace_, action_prefix_, args_type_, (handler_), NULL, params_, (validators_) \
    )
#define DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA(namespace_, action_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_VALIDATED( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, DCC_NO_VALIDATORS() \
    )
#define DCC_ON_SELECT_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_ON_SELECT_NS_PREFIX_PARAMS_VALIDATED( \
        namespace_, action_prefix_, args_type_, (handler_), params_, DCC_NO_VALIDATORS() \
    )
#define DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_POLICY( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, policy_ \
) \
    DCC_ON_SELECT_PREFIX_PARAMS_DATA_POLICY( \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_POLICY( \
        namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_VALIDATED(namespace_, handler_, args_type_, params_, validators_) \
    DCC_ON_SELECT_NS_PREFIX_PARAMS_VALIDATED(namespace_, #handler_, args_type_, (handler_), params_, (validators_))
#define DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA_VALIDATED( \
    namespace_, handler_, args_type_, user_data_, params_, validators_ \
) \
    DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_VALIDATED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (validators_) \
    )
#define DCC_ON_SELECT_NS_PREFIX_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_ON_SELECT_NS_PREFIX_PARAMS(namespace_, #handler_, args_type_, (handler_), params_)
#define DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA(namespace_, #handler_, args_type_, (handler_), (user_data_), params_)
#define DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA_POLICY(namespace_, handler_, args_type_, user_data_, params_, policy_) \
    DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_POLICY( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, (policy_) \
    )
#define DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
    namespace_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED( \
        namespace_, #handler_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_MODAL_PARAMS_DATA_VALIDATED(...) DCC_ON_MODAL_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_PARAMS_VALIDATED(...) DCC_ON_MODAL_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_PARAMS_DATA(...) DCC_ON_MODAL_PARAMS_DATA(__VA_ARGS__)
#define DCC_MODAL_PARAMS(...) DCC_ON_MODAL_PARAMS(__VA_ARGS__)
#define DCC_MODAL_PARAMS_DATA_POLICY(...) DCC_ON_MODAL_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_MODAL_PARAMS_DATA_GUARDED(...) DCC_ON_MODAL_PARAMS_DATA_GUARDED(__VA_ARGS__)

#define DCC_MODAL_PREFIX_PARAMS_DATA_VALIDATED(...) DCC_ON_MODAL_PREFIX_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_PREFIX_PARAMS_VALIDATED(...) DCC_ON_MODAL_PREFIX_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_PREFIX_PARAMS_DATA(...) DCC_ON_MODAL_PREFIX_PARAMS_DATA(__VA_ARGS__)
#define DCC_MODAL_PREFIX_PARAMS(...) DCC_ON_MODAL_PREFIX_PARAMS(__VA_ARGS__)
#define DCC_MODAL_PREFIX_PARAMS_DATA_POLICY(...) DCC_ON_MODAL_PREFIX_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_MODAL_PREFIX_PARAMS_DATA_GUARDED(...) DCC_ON_MODAL_PREFIX_PARAMS_DATA_GUARDED(__VA_ARGS__)

#define DCC_BUTTON_PARAMS_DATA_VALIDATED(...) DCC_ON_BUTTON_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_PARAMS_VALIDATED(...) DCC_ON_BUTTON_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_PARAMS_DATA(...) DCC_ON_BUTTON_PARAMS_DATA(__VA_ARGS__)
#define DCC_BUTTON_PARAMS(...) DCC_ON_BUTTON_PARAMS(__VA_ARGS__)
#define DCC_BUTTON_PARAMS_DATA_POLICY(...) DCC_ON_BUTTON_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_BUTTON_PARAMS_DATA_GUARDED(...) DCC_ON_BUTTON_PARAMS_DATA_GUARDED(__VA_ARGS__)

#define DCC_BUTTON_PREFIX_PARAMS_DATA_VALIDATED(...) DCC_ON_BUTTON_PREFIX_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_PREFIX_PARAMS_VALIDATED(...) DCC_ON_BUTTON_PREFIX_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_PREFIX_PARAMS_DATA(...) DCC_ON_BUTTON_PREFIX_PARAMS_DATA(__VA_ARGS__)
#define DCC_BUTTON_PREFIX_PARAMS(...) DCC_ON_BUTTON_PREFIX_PARAMS(__VA_ARGS__)
#define DCC_BUTTON_PREFIX_PARAMS_DATA_POLICY(...) DCC_ON_BUTTON_PREFIX_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_BUTTON_PREFIX_PARAMS_DATA_GUARDED(...) DCC_ON_BUTTON_PREFIX_PARAMS_DATA_GUARDED(__VA_ARGS__)

#define DCC_SELECT_PARAMS_DATA_VALIDATED(...) DCC_ON_SELECT_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_PARAMS_VALIDATED(...) DCC_ON_SELECT_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_PARAMS_DATA(...) DCC_ON_SELECT_PARAMS_DATA(__VA_ARGS__)
#define DCC_SELECT_PARAMS(...) DCC_ON_SELECT_PARAMS(__VA_ARGS__)
#define DCC_SELECT_PARAMS_DATA_POLICY(...) DCC_ON_SELECT_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_SELECT_PARAMS_DATA_GUARDED(...) DCC_ON_SELECT_PARAMS_DATA_GUARDED(__VA_ARGS__)

#define DCC_SELECT_PREFIX_PARAMS_DATA_VALIDATED(...) DCC_ON_SELECT_PREFIX_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_PREFIX_PARAMS_VALIDATED(...) DCC_ON_SELECT_PREFIX_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_PREFIX_PARAMS_DATA(...) DCC_ON_SELECT_PREFIX_PARAMS_DATA(__VA_ARGS__)
#define DCC_SELECT_PREFIX_PARAMS(...) DCC_ON_SELECT_PREFIX_PARAMS(__VA_ARGS__)
#define DCC_SELECT_PREFIX_PARAMS_DATA_POLICY(...) DCC_ON_SELECT_PREFIX_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_SELECT_PREFIX_PARAMS_DATA_GUARDED(...) DCC_ON_SELECT_PREFIX_PARAMS_DATA_GUARDED(__VA_ARGS__)

#define DCC_AUTOCOMPLETE_PARAMS_DATA_VALIDATED(...) DCC_ON_AUTOCOMPLETE_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_PARAMS_VALIDATED(...) DCC_ON_AUTOCOMPLETE_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_PARAMS_DATA(...) DCC_ON_AUTOCOMPLETE_PARAMS_DATA(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_PARAMS(...) DCC_ON_AUTOCOMPLETE_PARAMS(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_PARAMS_DATA_POLICY(...) DCC_ON_AUTOCOMPLETE_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_PARAMS_DATA_GUARDED(...) DCC_ON_AUTOCOMPLETE_PARAMS_DATA_GUARDED(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_PARAMS_FN_DATA_VALIDATED(...) DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_VALIDATED(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_PARAMS_FN_VALIDATED(...) DCC_ON_AUTOCOMPLETE_PARAMS_FN_VALIDATED(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_PARAMS_FN_DATA(...) DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_PARAMS_FN(...) DCC_ON_AUTOCOMPLETE_PARAMS_FN(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_PARAMS_FN_DATA_POLICY(...) DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_POLICY(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_PARAMS_FN_DATA_GUARDED(...) DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_GUARDED(__VA_ARGS__)

#define DCC_AUTOCOMPLETE_BUILDER_PARAMS_DATA_VALIDATED(...) \
    DCC_ON_AUTOCOMPLETE_BUILDER_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_BUILDER_PARAMS_VALIDATED(...) DCC_ON_AUTOCOMPLETE_BUILDER_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_BUILDER_PARAMS_DATA(...) DCC_ON_AUTOCOMPLETE_BUILDER_PARAMS_DATA(__VA_ARGS__)
#define DCC_AUTOCOMPLETE_BUILDER_PARAMS(...) DCC_ON_AUTOCOMPLETE_BUILDER_PARAMS(__VA_ARGS__)

#define DCC_MODAL_NS_PARAMS_DATA_VALIDATED(...) DCC_ON_MODAL_NS_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_NS_PARAMS_VALIDATED(...) DCC_ON_MODAL_NS_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_NS_PARAMS_DATA(...) DCC_ON_MODAL_NS_PARAMS_DATA(__VA_ARGS__)
#define DCC_MODAL_NS_PARAMS(...) DCC_ON_MODAL_NS_PARAMS(__VA_ARGS__)
#define DCC_MODAL_NS_PARAMS_DATA_POLICY(...) DCC_ON_MODAL_NS_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_MODAL_NS_PARAMS_DATA_GUARDED(...) DCC_ON_MODAL_NS_PARAMS_DATA_GUARDED(__VA_ARGS__)
#define DCC_MODAL_NS_PARAMS_FN_VALIDATED(...) DCC_ON_MODAL_NS_PARAMS_FN_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_NS_PARAMS_FN_DATA_VALIDATED(...) DCC_ON_MODAL_NS_PARAMS_FN_DATA_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_NS_PARAMS_FN(...) DCC_ON_MODAL_NS_PARAMS_FN(__VA_ARGS__)
#define DCC_MODAL_NS_PARAMS_FN_DATA(...) DCC_ON_MODAL_NS_PARAMS_FN_DATA(__VA_ARGS__)
#define DCC_MODAL_NS_PARAMS_FN_DATA_POLICY(...) DCC_ON_MODAL_NS_PARAMS_FN_DATA_POLICY(__VA_ARGS__)
#define DCC_MODAL_NS_PARAMS_FN_DATA_GUARDED(...) DCC_ON_MODAL_NS_PARAMS_FN_DATA_GUARDED(__VA_ARGS__)

#define DCC_MODAL_NS_PREFIX_PARAMS_DATA_VALIDATED(...) DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_NS_PREFIX_PARAMS_VALIDATED(...) DCC_ON_MODAL_NS_PREFIX_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_NS_PREFIX_PARAMS_DATA(...) DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA(__VA_ARGS__)
#define DCC_MODAL_NS_PREFIX_PARAMS(...) DCC_ON_MODAL_NS_PREFIX_PARAMS(__VA_ARGS__)
#define DCC_MODAL_NS_PREFIX_PARAMS_DATA_POLICY(...) DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED(...) DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED(__VA_ARGS__)
#define DCC_MODAL_NS_PREFIX_PARAMS_FN_VALIDATED(...) DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_NS_PREFIX_PARAMS_FN_DATA_VALIDATED(...) \
    DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA_VALIDATED(__VA_ARGS__)
#define DCC_MODAL_NS_PREFIX_PARAMS_FN(...) DCC_ON_MODAL_NS_PREFIX_PARAMS_FN(__VA_ARGS__)
#define DCC_MODAL_NS_PREFIX_PARAMS_FN_DATA(...) DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA(__VA_ARGS__)
#define DCC_MODAL_NS_PREFIX_PARAMS_FN_DATA_POLICY(...) DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA_POLICY(__VA_ARGS__)
#define DCC_MODAL_NS_PREFIX_PARAMS_FN_DATA_GUARDED(...) DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA_GUARDED(__VA_ARGS__)

#define DCC_BUTTON_NS_PARAMS_DATA_VALIDATED(...) DCC_ON_BUTTON_NS_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_NS_PARAMS_VALIDATED(...) DCC_ON_BUTTON_NS_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_NS_PARAMS_DATA(...) DCC_ON_BUTTON_NS_PARAMS_DATA(__VA_ARGS__)
#define DCC_BUTTON_NS_PARAMS(...) DCC_ON_BUTTON_NS_PARAMS(__VA_ARGS__)
#define DCC_BUTTON_NS_PARAMS_DATA_POLICY(...) DCC_ON_BUTTON_NS_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_BUTTON_NS_PARAMS_DATA_GUARDED(...) DCC_ON_BUTTON_NS_PARAMS_DATA_GUARDED(__VA_ARGS__)
#define DCC_BUTTON_NS_PARAMS_FN_VALIDATED(...) DCC_ON_BUTTON_NS_PARAMS_FN_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_NS_PARAMS_FN_DATA_VALIDATED(...) DCC_ON_BUTTON_NS_PARAMS_FN_DATA_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_NS_PARAMS_FN(...) DCC_ON_BUTTON_NS_PARAMS_FN(__VA_ARGS__)
#define DCC_BUTTON_NS_PARAMS_FN_DATA(...) DCC_ON_BUTTON_NS_PARAMS_FN_DATA(__VA_ARGS__)
#define DCC_BUTTON_NS_PARAMS_FN_DATA_POLICY(...) DCC_ON_BUTTON_NS_PARAMS_FN_DATA_POLICY(__VA_ARGS__)
#define DCC_BUTTON_NS_PARAMS_FN_DATA_GUARDED(...) DCC_ON_BUTTON_NS_PARAMS_FN_DATA_GUARDED(__VA_ARGS__)

#define DCC_BUTTON_NS_PREFIX_PARAMS_DATA_VALIDATED(...) DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_NS_PREFIX_PARAMS_VALIDATED(...) DCC_ON_BUTTON_NS_PREFIX_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_NS_PREFIX_PARAMS_DATA(...) DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA(__VA_ARGS__)
#define DCC_BUTTON_NS_PREFIX_PARAMS(...) DCC_ON_BUTTON_NS_PREFIX_PARAMS(__VA_ARGS__)
#define DCC_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY(...) DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED(...) DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED(__VA_ARGS__)
#define DCC_BUTTON_NS_PREFIX_PARAMS_FN_VALIDATED(...) DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_NS_PREFIX_PARAMS_FN_DATA_VALIDATED(...) \
    DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA_VALIDATED(__VA_ARGS__)
#define DCC_BUTTON_NS_PREFIX_PARAMS_FN(...) DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN(__VA_ARGS__)
#define DCC_BUTTON_NS_PREFIX_PARAMS_FN_DATA(...) DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA(__VA_ARGS__)
#define DCC_BUTTON_NS_PREFIX_PARAMS_FN_DATA_POLICY(...) DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA_POLICY(__VA_ARGS__)
#define DCC_BUTTON_NS_PREFIX_PARAMS_FN_DATA_GUARDED(...) DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA_GUARDED(__VA_ARGS__)

#define DCC_SELECT_NS_PARAMS_DATA_VALIDATED(...) DCC_ON_SELECT_NS_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_NS_PARAMS_VALIDATED(...) DCC_ON_SELECT_NS_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_NS_PARAMS_DATA(...) DCC_ON_SELECT_NS_PARAMS_DATA(__VA_ARGS__)
#define DCC_SELECT_NS_PARAMS(...) DCC_ON_SELECT_NS_PARAMS(__VA_ARGS__)
#define DCC_SELECT_NS_PARAMS_DATA_POLICY(...) DCC_ON_SELECT_NS_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_SELECT_NS_PARAMS_DATA_GUARDED(...) DCC_ON_SELECT_NS_PARAMS_DATA_GUARDED(__VA_ARGS__)
#define DCC_SELECT_NS_PARAMS_FN_VALIDATED(...) DCC_ON_SELECT_NS_PARAMS_FN_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_NS_PARAMS_FN_DATA_VALIDATED(...) DCC_ON_SELECT_NS_PARAMS_FN_DATA_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_NS_PARAMS_FN(...) DCC_ON_SELECT_NS_PARAMS_FN(__VA_ARGS__)
#define DCC_SELECT_NS_PARAMS_FN_DATA(...) DCC_ON_SELECT_NS_PARAMS_FN_DATA(__VA_ARGS__)
#define DCC_SELECT_NS_PARAMS_FN_DATA_POLICY(...) DCC_ON_SELECT_NS_PARAMS_FN_DATA_POLICY(__VA_ARGS__)
#define DCC_SELECT_NS_PARAMS_FN_DATA_GUARDED(...) DCC_ON_SELECT_NS_PARAMS_FN_DATA_GUARDED(__VA_ARGS__)

#define DCC_SELECT_NS_PREFIX_PARAMS_DATA_VALIDATED(...) DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_NS_PREFIX_PARAMS_VALIDATED(...) DCC_ON_SELECT_NS_PREFIX_PARAMS_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_NS_PREFIX_PARAMS_DATA(...) DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA(__VA_ARGS__)
#define DCC_SELECT_NS_PREFIX_PARAMS(...) DCC_ON_SELECT_NS_PREFIX_PARAMS(__VA_ARGS__)
#define DCC_SELECT_NS_PREFIX_PARAMS_DATA_POLICY(...) DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_POLICY(__VA_ARGS__)
#define DCC_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED(...) DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED(__VA_ARGS__)
#define DCC_SELECT_NS_PREFIX_PARAMS_FN_VALIDATED(...) DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_NS_PREFIX_PARAMS_FN_DATA_VALIDATED(...) \
    DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA_VALIDATED(__VA_ARGS__)
#define DCC_SELECT_NS_PREFIX_PARAMS_FN(...) DCC_ON_SELECT_NS_PREFIX_PARAMS_FN(__VA_ARGS__)
#define DCC_SELECT_NS_PREFIX_PARAMS_FN_DATA(...) DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA(__VA_ARGS__)
#define DCC_SELECT_NS_PREFIX_PARAMS_FN_DATA_POLICY(...) DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA_POLICY(__VA_ARGS__)
#define DCC_SELECT_NS_PREFIX_PARAMS_FN_DATA_GUARDED(...) DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA_GUARDED(__VA_ARGS__)

#endif
