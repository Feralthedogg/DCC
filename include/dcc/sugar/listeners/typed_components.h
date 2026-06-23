#ifndef DCC_SUGAR_LISTENERS_TYPED_COMPONENTS_H
#define DCC_SUGAR_LISTENERS_TYPED_COMPONENTS_H

#include <dcc/sugar/listeners/types.h>

#define DCC_LISTEN_TYPED_MODAL(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_MODAL, \
        .typed_modal = (route_) \
    })
#define DCC_LISTEN_TYPED_BUTTON(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_BUTTON, \
        .typed_component = (route_) \
    })
#define DCC_LISTEN_TYPED_SELECT(route_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_TYPED_SELECT, \
        .typed_component = (route_) \
    })
#define DCC_LISTEN_MODAL_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_MODAL(DCC_ON_MODAL_PARAMS((custom_id_), args_type_, (handler_), params_))
#define DCC_LISTEN_MODAL_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_MODAL(DCC_ON_MODAL_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_))
#define DCC_LISTEN_MODAL_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_)) \
    )
#define DCC_LISTEN_MODAL_PARAMS_DATA_VALIDATED( \
    custom_id_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PARAMS_DATA_VALIDATED( \
            (custom_id_), args_type_, (handler_), (user_data_), params_, (validators_) \
        ) \
    )
#define DCC_LISTEN_MODAL_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), NULL, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_MODAL(DCC_ON_MODAL_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_))
#define DCC_LISTEN_MODAL_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_MODAL_PREFIX_PARAMS_DATA_GUARDED( \
    custom_id_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_PREFIX_PARAMS_DATA_GUARDED( \
            (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_MODAL_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_MODAL(DCC_ON_MODAL_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_))
#define DCC_LISTEN_MODAL_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_MODAL_NS_PARAMS_DATA_GUARDED( \
    namespace_, action_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PARAMS_DATA_GUARDED( \
            namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_MODAL_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_MODAL_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PREFIX_PARAMS_DATA_GUARDED( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_MODAL_NS_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_MODAL(DCC_ON_MODAL_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_LISTEN_MODAL_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_MODAL_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_MODAL( \
        DCC_ON_MODAL_NS_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_BUTTON_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_BUTTON(DCC_ON_BUTTON_PARAMS((custom_id_), args_type_, (handler_), params_))
#define DCC_LISTEN_BUTTON_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_BUTTON(DCC_ON_BUTTON_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_))
#define DCC_LISTEN_BUTTON_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_)) \
    )
#define DCC_LISTEN_BUTTON_PARAMS_DATA_VALIDATED( \
    custom_id_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PARAMS_DATA_VALIDATED( \
            (custom_id_), args_type_, (handler_), (user_data_), params_, (validators_) \
        ) \
    )
#define DCC_LISTEN_BUTTON_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), NULL, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_BUTTON_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_BUTTON_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_BUTTON(DCC_ON_BUTTON_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_))
#define DCC_LISTEN_BUTTON_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_BUTTON_PREFIX_PARAMS_DATA_GUARDED( \
    custom_id_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_PREFIX_PARAMS_DATA_GUARDED( \
            (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_BUTTON_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_BUTTON(DCC_ON_BUTTON_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_))
#define DCC_LISTEN_BUTTON_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_BUTTON_NS_PARAMS_DATA_GUARDED( \
    namespace_, action_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PARAMS_DATA_GUARDED( \
            namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_BUTTON_NS_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_BUTTON(DCC_ON_BUTTON_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_LISTEN_BUTTON_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_BUTTON_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_BUTTON( \
        DCC_ON_BUTTON_NS_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_SELECT_PARAMS(custom_id_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SELECT(DCC_ON_SELECT_PARAMS((custom_id_), args_type_, (handler_), params_))
#define DCC_LISTEN_SELECT_PARAMS_DATA(custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SELECT(DCC_ON_SELECT_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_))
#define DCC_LISTEN_SELECT_PARAMS_VALIDATED(custom_id_, args_type_, handler_, params_, validators_) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PARAMS_VALIDATED((custom_id_), args_type_, (handler_), params_, (validators_)) \
    )
#define DCC_LISTEN_SELECT_PARAMS_DATA_VALIDATED( \
    custom_id_, args_type_, handler_, user_data_, params_, validators_ \
) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PARAMS_DATA_VALIDATED( \
            (custom_id_), args_type_, (handler_), (user_data_), params_, (validators_) \
        ) \
    )
#define DCC_LISTEN_SELECT_PARAMS_GUARDED(custom_id_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), NULL, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_SELECT_PARAMS_DATA_GUARDED(custom_id_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PARAMS_DATA_GUARDED((custom_id_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_SELECT_PREFIX_PARAMS(custom_id_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SELECT(DCC_ON_SELECT_PREFIX_PARAMS((custom_id_prefix_), args_type_, (handler_), params_))
#define DCC_LISTEN_SELECT_PREFIX_PARAMS_DATA(custom_id_prefix_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PREFIX_PARAMS_DATA((custom_id_prefix_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_SELECT_PREFIX_PARAMS_DATA_GUARDED( \
    custom_id_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_PREFIX_PARAMS_DATA_GUARDED( \
            (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_SELECT_NS_PARAMS(namespace_, action_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SELECT(DCC_ON_SELECT_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_))
#define DCC_LISTEN_SELECT_NS_PARAMS_DATA(namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_LISTEN_SELECT_NS_PARAMS_DATA_GUARDED( \
    namespace_, action_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PARAMS_DATA_GUARDED( \
            namespace_, action_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_SELECT_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, handler_, params_) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PREFIX_PARAMS(namespace_, action_prefix_, args_type_, (handler_), params_) \
    )
#define DCC_LISTEN_SELECT_NS_PREFIX_PARAMS_DATA( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_ \
) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_ \
        ) \
    )
#define DCC_LISTEN_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED( \
    namespace_, action_prefix_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_SELECT_NS_PARAMS_FN(namespace_, handler_, args_type_, params_) \
    DCC_LISTEN_TYPED_SELECT(DCC_ON_SELECT_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_LISTEN_SELECT_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, user_data_, params_) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_LISTEN_SELECT_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_LISTEN_TYPED_SELECT( \
        DCC_ON_SELECT_NS_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_MODAL_FLOW(flow_) \
    ((dcc_sugar_listener_t){ \
        .size = sizeof(dcc_sugar_listener_t), \
        .kind = DCC_SUGAR_LISTENER_MODAL_FLOW, \
        .modal_flow = (flow_) \
    })
#define DCC_LISTEN_MODAL_FLOW_PARAMS(custom_id_, title_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW(DCC_MODAL_FLOW_PARAMS((custom_id_), (title_), args_type_, (handler_), params_, __VA_ARGS__))
#define DCC_LISTEN_MODAL_FLOW_PARAMS_DATA(custom_id_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_FLOW_PARAMS_DATA((custom_id_), (title_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_V2_FLOW_PARAMS(custom_id_, title_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW(DCC_MODAL_V2_FLOW_PARAMS((custom_id_), (title_), args_type_, (handler_), params_, __VA_ARGS__))
#define DCC_LISTEN_MODAL_V2_FLOW_PARAMS_DATA(custom_id_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_V2_FLOW_PARAMS_DATA((custom_id_), (title_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_V2_FLOW_NS_PARAMS(namespace_, action_, title_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_V2_FLOW_NS_PARAMS(namespace_, action_, (title_), args_type_, (handler_), params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_V2_FLOW_NS_PARAMS_DATA( \
    namespace_, action_, title_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_V2_FLOW_NS_PARAMS_DATA( \
            namespace_, action_, (title_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_LISTEN_MODAL_V2_FLOW_NS_PARAMS_FN(namespace_, title_, args_type_, handler_, params_, ...) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_V2_FLOW_NS_PARAMS_FN(namespace_, (title_), args_type_, handler_, params_, __VA_ARGS__) \
    )
#define DCC_LISTEN_MODAL_V2_FLOW_NS_PARAMS_FN_DATA( \
    namespace_, title_, args_type_, handler_, user_data_, params_, ... \
) \
    DCC_LISTEN_MODAL_FLOW( \
        DCC_MODAL_V2_FLOW_NS_PARAMS_FN_DATA( \
            namespace_, (title_), args_type_, handler_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )


#endif
