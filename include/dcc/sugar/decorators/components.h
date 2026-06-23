#ifndef DCC_SUGAR_DECORATORS_COMPONENTS_H
#define DCC_SUGAR_DECORATORS_COMPONENTS_H

#include <dcc/sugar/decorators/base.h>

#define DCC_DECORATE_COMPONENT_ROUTE_AS(handler_, route_type_, custom_id_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_component_route_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_component_route_t), \
        .type = (route_type_), \
        .custom_id = (custom_id_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY(handler_, route_type_, custom_id_, user_data_, policy_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_component_route_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_component_route_t), \
        .type = (route_type_), \
        .custom_id = (custom_id_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_BUTTON(handler_, custom_id_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS(handler_, DCC_APP_EXTENSION_BUTTON, (custom_id_))

#define DCC_DECORATE_BUTTON_DATA(handler_, custom_id_, user_data_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_BUTTON, \
        (custom_id_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_BUTTON_POLICY(handler_, custom_id_, policy_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY(handler_, DCC_APP_EXTENSION_BUTTON, (custom_id_), NULL, (policy_))

#define DCC_DECORATE_BUTTON_GUARDED(handler_, custom_id_, ...) \
    DCC_DECORATE_BUTTON_POLICY(handler_, (custom_id_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_DECORATE_BUTTON_DATA_GUARDED(handler_, custom_id_, user_data_, ...) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_BUTTON, \
        (custom_id_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_BUTTON_FN(handler_) \
    DCC_DECORATE_BUTTON(handler_, #handler_)

#define DCC_DECORATE_BUTTON_FN_DATA(handler_, user_data_) \
    DCC_DECORATE_BUTTON_DATA(handler_, #handler_, (user_data_))

#define DCC_DECORATE_BUTTON_FN_GUARDED(handler_, ...) \
    DCC_DECORATE_BUTTON_GUARDED(handler_, #handler_, __VA_ARGS__)

#define DCC_DECORATE_BUTTON_PREFIX(handler_, custom_id_prefix_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS(handler_, DCC_APP_EXTENSION_BUTTON_PREFIX, (custom_id_prefix_))

#define DCC_DECORATE_BUTTON_PREFIX_DATA(handler_, custom_id_prefix_, user_data_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_BUTTON_PREFIX, \
        (custom_id_prefix_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_BUTTON_PREFIX_GUARDED(handler_, custom_id_prefix_, ...) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_BUTTON_PREFIX, \
        (custom_id_prefix_), \
        NULL, \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_BUTTON_NS(handler_, namespace_, action_) \
    DCC_DECORATE_BUTTON(handler_, DCC_COMPONENT_ID(namespace_, action_))

#define DCC_DECORATE_BUTTON_NS_DATA(handler_, namespace_, action_, user_data_) \
    DCC_DECORATE_BUTTON_DATA(handler_, DCC_COMPONENT_ID(namespace_, action_), (user_data_))

#define DCC_DECORATE_BUTTON_NS_GUARDED(handler_, namespace_, action_, ...) \
    DCC_DECORATE_BUTTON_GUARDED(handler_, DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)

#define DCC_DECORATE_BUTTON_NS_DATA_GUARDED(handler_, namespace_, action_, user_data_, ...) \
    DCC_DECORATE_BUTTON_DATA_GUARDED( \
        handler_, \
        DCC_COMPONENT_ID(namespace_, action_), \
        (user_data_), \
        __VA_ARGS__ \
    )

#define DCC_DECORATE_BUTTON_NS_PREFIX(handler_, namespace_, action_prefix_) \
    DCC_DECORATE_BUTTON_PREFIX(handler_, DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_))

#define DCC_DECORATE_BUTTON_NS_PREFIX_DATA(handler_, namespace_, action_prefix_, user_data_) \
    DCC_DECORATE_BUTTON_PREFIX_DATA( \
        handler_, \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (user_data_) \
    )

#define DCC_DECORATE_BUTTON_NS_PREFIX_GUARDED(handler_, namespace_, action_prefix_, ...) \
    DCC_DECORATE_BUTTON_PREFIX_GUARDED( \
        handler_, \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        __VA_ARGS__ \
    )

#define DCC_DECORATE_SELECT(handler_, custom_id_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS(handler_, DCC_APP_EXTENSION_SELECT, (custom_id_))

#define DCC_DECORATE_SELECT_DATA(handler_, custom_id_, user_data_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_SELECT, \
        (custom_id_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_SELECT_GUARDED(handler_, custom_id_, ...) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_SELECT, \
        (custom_id_), \
        NULL, \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_SELECT_DATA_GUARDED(handler_, custom_id_, user_data_, ...) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_SELECT, \
        (custom_id_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_SELECT_FN(handler_) \
    DCC_DECORATE_SELECT(handler_, #handler_)

#define DCC_DECORATE_SELECT_PREFIX(handler_, custom_id_prefix_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS(handler_, DCC_APP_EXTENSION_SELECT_PREFIX, (custom_id_prefix_))

#define DCC_DECORATE_SELECT_PREFIX_DATA(handler_, custom_id_prefix_, user_data_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_SELECT_PREFIX, \
        (custom_id_prefix_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_SELECT_PREFIX_GUARDED(handler_, custom_id_prefix_, ...) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_SELECT_PREFIX, \
        (custom_id_prefix_), \
        NULL, \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_SELECT_NS(handler_, namespace_, action_) \
    DCC_DECORATE_SELECT(handler_, DCC_COMPONENT_ID(namespace_, action_))

#define DCC_DECORATE_SELECT_NS_DATA(handler_, namespace_, action_, user_data_) \
    DCC_DECORATE_SELECT_DATA(handler_, DCC_COMPONENT_ID(namespace_, action_), (user_data_))

#define DCC_DECORATE_SELECT_NS_GUARDED(handler_, namespace_, action_, ...) \
    DCC_DECORATE_SELECT_GUARDED(handler_, DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)

#define DCC_DECORATE_SELECT_NS_DATA_GUARDED(handler_, namespace_, action_, user_data_, ...) \
    DCC_DECORATE_SELECT_DATA_GUARDED( \
        handler_, \
        DCC_COMPONENT_ID(namespace_, action_), \
        (user_data_), \
        __VA_ARGS__ \
    )

#define DCC_DECORATE_SELECT_NS_PREFIX(handler_, namespace_, action_prefix_) \
    DCC_DECORATE_SELECT_PREFIX(handler_, DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_))

#define DCC_DECORATE_SELECT_NS_PREFIX_DATA(handler_, namespace_, action_prefix_, user_data_) \
    DCC_DECORATE_SELECT_PREFIX_DATA( \
        handler_, \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (user_data_) \
    )

#define DCC_DECORATE_SELECT_NS_PREFIX_GUARDED(handler_, namespace_, action_prefix_, ...) \
    DCC_DECORATE_SELECT_PREFIX_GUARDED( \
        handler_, \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        __VA_ARGS__ \
    )

#define DCC_DECORATE_MODAL(handler_, custom_id_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS(handler_, DCC_APP_EXTENSION_MODAL, (custom_id_))

#define DCC_DECORATE_MODAL_DATA(handler_, custom_id_, user_data_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_MODAL, \
        (custom_id_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_MODAL_GUARDED(handler_, custom_id_, ...) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_MODAL, \
        (custom_id_), \
        NULL, \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_MODAL_DATA_GUARDED(handler_, custom_id_, user_data_, ...) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_MODAL, \
        (custom_id_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_MODAL_FN(handler_) \
    DCC_DECORATE_MODAL(handler_, #handler_)

#define DCC_DECORATE_MODAL_PREFIX(handler_, custom_id_prefix_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS(handler_, DCC_APP_EXTENSION_MODAL_PREFIX, (custom_id_prefix_))

#define DCC_DECORATE_MODAL_PREFIX_DATA(handler_, custom_id_prefix_, user_data_) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_MODAL_PREFIX, \
        (custom_id_prefix_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_MODAL_PREFIX_GUARDED(handler_, custom_id_prefix_, ...) \
    DCC_DECORATE_COMPONENT_ROUTE_AS_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_MODAL_PREFIX, \
        (custom_id_prefix_), \
        NULL, \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_MODAL_NS(handler_, namespace_, action_) \
    DCC_DECORATE_MODAL(handler_, DCC_COMPONENT_ID(namespace_, action_))

#define DCC_DECORATE_MODAL_NS_DATA(handler_, namespace_, action_, user_data_) \
    DCC_DECORATE_MODAL_DATA(handler_, DCC_COMPONENT_ID(namespace_, action_), (user_data_))

#define DCC_DECORATE_MODAL_NS_GUARDED(handler_, namespace_, action_, ...) \
    DCC_DECORATE_MODAL_GUARDED(handler_, DCC_COMPONENT_ID(namespace_, action_), __VA_ARGS__)

#define DCC_DECORATE_MODAL_NS_DATA_GUARDED(handler_, namespace_, action_, user_data_, ...) \
    DCC_DECORATE_MODAL_DATA_GUARDED( \
        handler_, \
        DCC_COMPONENT_ID(namespace_, action_), \
        (user_data_), \
        __VA_ARGS__ \
    )

#define DCC_DECORATE_MODAL_NS_PREFIX(handler_, namespace_, action_prefix_) \
    DCC_DECORATE_MODAL_PREFIX(handler_, DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_))

#define DCC_DECORATE_MODAL_NS_PREFIX_DATA(handler_, namespace_, action_prefix_, user_data_) \
    DCC_DECORATE_MODAL_PREFIX_DATA( \
        handler_, \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (user_data_) \
    )

#define DCC_DECORATE_MODAL_NS_PREFIX_GUARDED(handler_, namespace_, action_prefix_, ...) \
    DCC_DECORATE_MODAL_PREFIX_GUARDED( \
        handler_, \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        __VA_ARGS__ \
    )


#endif /* DCC_SUGAR_DECORATORS_COMPONENTS_H */
