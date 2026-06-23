#ifndef DCC_SUGAR_APP_PARAMS_MODAL_NAMESPACE_H
#define DCC_SUGAR_APP_PARAMS_MODAL_NAMESPACE_H

#include <dcc/sugar/app_params/modal_bindings.h>

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

#endif /* DCC_SUGAR_APP_PARAMS_MODAL_NAMESPACE_H */
