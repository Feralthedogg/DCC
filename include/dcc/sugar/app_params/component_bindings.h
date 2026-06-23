#ifndef DCC_SUGAR_APP_PARAMS_COMPONENT_BINDINGS_H
#define DCC_SUGAR_APP_PARAMS_COMPONENT_BINDINGS_H

#include <dcc/sugar/app_params/base.h>

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

#endif /* DCC_SUGAR_APP_PARAMS_COMPONENT_BINDINGS_H */
