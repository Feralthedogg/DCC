#ifndef DCC_SUGAR_APP_PARAMS_MODAL_BINDINGS_H
#define DCC_SUGAR_APP_PARAMS_MODAL_BINDINGS_H

#include <dcc/sugar/app_params/base.h>

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

#endif /* DCC_SUGAR_APP_PARAMS_MODAL_BINDINGS_H */
