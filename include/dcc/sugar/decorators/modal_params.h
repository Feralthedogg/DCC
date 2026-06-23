#ifndef DCC_SUGAR_DECORATORS_MODAL_PARAMS_H
#define DCC_SUGAR_DECORATORS_MODAL_PARAMS_H

#include <dcc/sugar/decorators/base.h>

#define DCC_DECORATE_TYPED_MODAL_PARAMS_DATA_POLICY(handler_, custom_id_, args_type_, arg_, user_data_, params_, policy_) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_modal_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_MODAL_PARAMS_DATA_POLICY( \
            (custom_id_), args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )

#define DCC_DECORATE_TYPED_MODAL_PARAMS_DATA(handler_, custom_id_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_TYPED_MODAL_PARAMS_DATA_POLICY( \
        handler_, (custom_id_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_TYPED_MODAL_PARAMS_POLICY(handler_, custom_id_, args_type_, arg_, params_, policy_) \
    DCC_DECORATE_TYPED_MODAL_PARAMS_DATA_POLICY(handler_, (custom_id_), args_type_, arg_, NULL, params_, (policy_))
#define DCC_DECORATE_TYPED_MODAL_PARAMS_GUARDED(handler_, custom_id_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_TYPED_MODAL_PARAMS_POLICY( \
        handler_, (custom_id_), args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_DECORATE_TYPED_MODAL_PARAMS_DATA_GUARDED(handler_, custom_id_, args_type_, arg_, user_data_, params_, ...) \
    DCC_DECORATE_TYPED_MODAL_PARAMS_DATA_POLICY( \
        handler_, (custom_id_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_DECORATE_TYPED_MODAL_PARAMS(handler_, custom_id_, args_type_, arg_, params_) \
    DCC_DECORATE_TYPED_MODAL_PARAMS_DATA(handler_, (custom_id_), args_type_, arg_, NULL, params_)

#define DCC_DECORATE_MODAL_PARAMS(handler_, custom_id_, args_type_, arg_, params_) \
    DCC_DECORATE_TYPED_MODAL_PARAMS(handler_, (custom_id_), args_type_, arg_, params_)
#define DCC_DECORATE_MODAL_PARAMS_DATA(handler_, custom_id_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_TYPED_MODAL_PARAMS_DATA(handler_, (custom_id_), args_type_, arg_, (user_data_), params_)
#define DCC_DECORATE_MODAL_PARAMS_POLICY(handler_, custom_id_, args_type_, arg_, params_, policy_) \
    DCC_DECORATE_TYPED_MODAL_PARAMS_POLICY(handler_, (custom_id_), args_type_, arg_, params_, (policy_))
#define DCC_DECORATE_MODAL_PARAMS_GUARDED(handler_, custom_id_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_TYPED_MODAL_PARAMS_GUARDED(handler_, (custom_id_), args_type_, arg_, params_, __VA_ARGS__)
#define DCC_DECORATE_MODAL_PARAMS_DATA_GUARDED(handler_, custom_id_, args_type_, arg_, user_data_, params_, ...) \
    DCC_DECORATE_TYPED_MODAL_PARAMS_DATA_GUARDED( \
        handler_, (custom_id_), args_type_, arg_, (user_data_), params_, __VA_ARGS__ \
    )

#define DCC_DECORATE_MODAL_PREFIX_PARAMS_DATA_POLICY( \
    handler_, custom_id_prefix_, args_type_, arg_, user_data_, params_, policy_ \
) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_modal_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_MODAL_PREFIX_PARAMS_DATA_POLICY( \
            (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )
#define DCC_DECORATE_MODAL_PREFIX_PARAMS(handler_, custom_id_prefix_, args_type_, arg_, params_) \
    DCC_DECORATE_MODAL_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, NULL, params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_MODAL_PREFIX_PARAMS_DATA(handler_, custom_id_prefix_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_MODAL_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_MODAL_PREFIX_PARAMS_POLICY(handler_, custom_id_prefix_, args_type_, arg_, params_, policy_) \
    DCC_DECORATE_MODAL_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, NULL, params_, (policy_) \
    )
#define DCC_DECORATE_MODAL_PREFIX_PARAMS_GUARDED(handler_, custom_id_prefix_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_MODAL_PREFIX_PARAMS_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_DECORATE_MODAL_PREFIX_PARAMS_DATA_GUARDED( \
    handler_, custom_id_prefix_, args_type_, arg_, user_data_, params_, ... \
) \
    DCC_DECORATE_MODAL_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_MODAL_NS_PARAMS_DATA_POLICY( \
    handler_, namespace_, action_, args_type_, arg_, user_data_, params_, policy_ \
) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_modal_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_MODAL_NS_PARAMS_DATA_POLICY( \
            namespace_, action_, args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )
#define DCC_DECORATE_MODAL_NS_PARAMS(handler_, namespace_, action_, args_type_, arg_, params_) \
    DCC_DECORATE_MODAL_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, NULL, params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_MODAL_NS_PARAMS_DATA(handler_, namespace_, action_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_MODAL_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_MODAL_NS_PARAMS_POLICY(handler_, namespace_, action_, args_type_, arg_, params_, policy_) \
    DCC_DECORATE_MODAL_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, NULL, params_, (policy_) \
    )
#define DCC_DECORATE_MODAL_NS_PARAMS_GUARDED(handler_, namespace_, action_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_MODAL_NS_PARAMS_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_DECORATE_MODAL_NS_PARAMS_DATA_GUARDED( \
    handler_, namespace_, action_, args_type_, arg_, user_data_, params_, ... \
) \
    DCC_DECORATE_MODAL_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_MODAL_NS_PREFIX_PARAMS_DATA_POLICY( \
    handler_, namespace_, action_prefix_, args_type_, arg_, user_data_, params_, policy_ \
) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_modal_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_POLICY( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )
#define DCC_DECORATE_MODAL_NS_PREFIX_PARAMS(handler_, namespace_, action_prefix_, args_type_, arg_, params_) \
    DCC_DECORATE_MODAL_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, NULL, params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_MODAL_NS_PREFIX_PARAMS_DATA( \
    handler_, namespace_, action_prefix_, args_type_, arg_, user_data_, params_ \
) \
    DCC_DECORATE_MODAL_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_MODAL_NS_PREFIX_PARAMS_POLICY( \
    handler_, namespace_, action_prefix_, args_type_, arg_, params_, policy_ \
) \
    DCC_DECORATE_MODAL_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, NULL, params_, (policy_) \
    )
#define DCC_DECORATE_MODAL_NS_PREFIX_PARAMS_GUARDED( \
    handler_, namespace_, action_prefix_, args_type_, arg_, params_, ... \
) \
    DCC_DECORATE_MODAL_NS_PREFIX_PARAMS_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_DECORATE_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED( \
    handler_, namespace_, action_prefix_, args_type_, arg_, user_data_, params_, ... \
) \
    DCC_DECORATE_MODAL_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )


#endif /* DCC_SUGAR_DECORATORS_MODAL_PARAMS_H */
