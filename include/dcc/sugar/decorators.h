#ifndef DCC_SUGAR_DECORATORS_H
#define DCC_SUGAR_DECORATORS_H

#include <dcc/sugar/app_params.h>
#include <dcc/sugar/events.h>

#define DCC_DECORATED_ROUTE_NAME_(name_) name_ ## _dcc_route
#define DCC_DECORATED_ROUTE_NAME(name_) DCC_DECORATED_ROUTE_NAME_(name_)
#define DCC_ROUTE(name_) DCC_DECORATED_ROUTE_NAME(name_)
#define DCC_TYPED_ROUTE(name_) DCC_ROUTE(name_)

#define DCC_DECORATE_TYPED_HANDLER_IMPL(route_type_, handler_, args_type_, arg_, route_init_) \
    static void handler_(dcc_ctx_t *ctx, void *args, void *user_data); \
    static void handler_ ## _typed(dcc_ctx_t *ctx, const args_type_ *arg_, void *user_data); \
    static const route_type_ DCC_DECORATED_ROUTE_NAME(handler_) = route_init_; \
    static void handler_(dcc_ctx_t *ctx, void *args, void *user_data) { \
        if (args == NULL) { \
            return; \
        } \
        handler_ ## _typed((ctx), (const args_type_ *)args, (user_data)); \
    } \
    static void handler_ ## _typed(dcc_ctx_t *ctx, const args_type_ *arg_, void *user_data)

#define DCC_DECORATE_TYPED_SLASH_PARAMS_AS_DATA_POLICY( \
    handler_, \
    command_name_, \
    description_, \
    args_type_, \
    arg_, \
    user_data_, \
    params_, \
    policy_ \
) \
    static void handler_(dcc_ctx_t *ctx, void *args, void *user_data); \
    static void handler_ ## _typed(dcc_ctx_t *ctx, const args_type_ *arg_, void *user_data); \
    static const dcc_app_typed_slash_command_t DCC_DECORATED_ROUTE_NAME(handler_) = \
        DCC_ON_SLASH_PARAMS_DATA_POLICY( \
            (command_name_), \
            (description_), \
            args_type_, \
            (handler_), \
            (user_data_), \
            params_, \
            (policy_) \
        ); \
    static void handler_(dcc_ctx_t *ctx, void *args, void *user_data) { \
        if (args == NULL) { \
            return; \
        } \
        handler_ ## _typed((ctx), (const args_type_ *)args, (user_data)); \
    } \
    static void handler_ ## _typed(dcc_ctx_t *ctx, const args_type_ *arg_, void *user_data)

#define DCC_DECORATE_TYPED_SLASH_PARAMS_AS_DATA( \
    handler_, \
    command_name_, \
    description_, \
    args_type_, \
    arg_, \
    user_data_, \
    params_ \
) \
    DCC_DECORATE_TYPED_SLASH_PARAMS_AS_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (description_), \
        args_type_, \
        arg_, \
        (user_data_), \
        params_, \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_TYPED_SLASH_PARAMS_AS_POLICY( \
    handler_, \
    command_name_, \
    description_, \
    args_type_, \
    arg_, \
    params_, \
    policy_ \
) \
    DCC_DECORATE_TYPED_SLASH_PARAMS_AS_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (description_), \
        args_type_, \
        arg_, \
        NULL, \
        params_, \
        (policy_) \
    )

#define DCC_DECORATE_TYPED_SLASH_PARAMS_AS_GUARDED( \
    handler_, \
    command_name_, \
    description_, \
    args_type_, \
    arg_, \
    params_, \
    ... \
) \
    DCC_DECORATE_TYPED_SLASH_PARAMS_AS_POLICY( \
        handler_, \
        (command_name_), \
        (description_), \
        args_type_, \
        arg_, \
        params_, \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_TYPED_SLASH_PARAMS_AS_DATA_GUARDED( \
    handler_, \
    command_name_, \
    description_, \
    args_type_, \
    arg_, \
    user_data_, \
    params_, \
    ... \
) \
    DCC_DECORATE_TYPED_SLASH_PARAMS_AS_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (description_), \
        args_type_, \
        arg_, \
        (user_data_), \
        params_, \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_TYPED_SLASH_PARAMS(handler_, description_, args_type_, arg_, params_) \
    DCC_DECORATE_TYPED_SLASH_PARAMS_AS_DATA( \
        handler_, \
        #handler_, \
        (description_), \
        args_type_, \
        arg_, \
        NULL, \
        params_ \
    )

#define DCC_DECORATE_TYPED_SLASH_PARAMS_DATA(handler_, description_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_TYPED_SLASH_PARAMS_AS_DATA( \
        handler_, \
        #handler_, \
        (description_), \
        args_type_, \
        arg_, \
        (user_data_), \
        params_ \
    )

#define DCC_DECORATE_TYPED_SLASH_PARAMS_GUARDED(handler_, description_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_TYPED_SLASH_PARAMS_AS_GUARDED( \
        handler_, \
        #handler_, \
        (description_), \
        args_type_, \
        arg_, \
        params_, \
        __VA_ARGS__ \
    )

#define DCC_DECORATE_TYPED_SLASH_PARAMS_DATA_GUARDED( \
    handler_, \
    description_, \
    args_type_, \
    arg_, \
    user_data_, \
    params_, \
    ... \
) \
    DCC_DECORATE_TYPED_SLASH_PARAMS_AS_DATA_GUARDED( \
        handler_, \
        #handler_, \
        (description_), \
        args_type_, \
        arg_, \
        (user_data_), \
        params_, \
        __VA_ARGS__ \
    )

#define DCC_DECORATE_SLASH_PARAMS(handler_, description_, args_type_, arg_, params_) \
    DCC_DECORATE_TYPED_SLASH_PARAMS(handler_, (description_), args_type_, arg_, params_)
#define DCC_DECORATE_SLASH_PARAMS_DATA(handler_, description_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_TYPED_SLASH_PARAMS_DATA(handler_, (description_), args_type_, arg_, (user_data_), params_)
#define DCC_DECORATE_SLASH_PARAMS_GUARDED(handler_, description_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_TYPED_SLASH_PARAMS_GUARDED(handler_, (description_), args_type_, arg_, params_, __VA_ARGS__)
#define DCC_DECORATE_SLASH_PARAMS_DATA_GUARDED( \
    handler_, \
    description_, \
    args_type_, \
    arg_, \
    user_data_, \
    params_, \
    ... \
) \
    DCC_DECORATE_TYPED_SLASH_PARAMS_DATA_GUARDED( \
        handler_, \
        (description_), \
        args_type_, \
        arg_, \
        (user_data_), \
        params_, \
        __VA_ARGS__ \
    )

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

#define DCC_DECORATE_BUTTON_PARAMS_DATA_POLICY(handler_, custom_id_, args_type_, arg_, user_data_, params_, policy_) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_component_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_BUTTON_PARAMS_DATA_POLICY( \
            (custom_id_), args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )
#define DCC_DECORATE_BUTTON_PARAMS(handler_, custom_id_, args_type_, arg_, params_) \
    DCC_DECORATE_BUTTON_PARAMS_DATA_POLICY( \
        handler_, (custom_id_), args_type_, arg_, NULL, params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_BUTTON_PARAMS_DATA(handler_, custom_id_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_BUTTON_PARAMS_DATA_POLICY( \
        handler_, (custom_id_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_BUTTON_PARAMS_POLICY(handler_, custom_id_, args_type_, arg_, params_, policy_) \
    DCC_DECORATE_BUTTON_PARAMS_DATA_POLICY(handler_, (custom_id_), args_type_, arg_, NULL, params_, (policy_))
#define DCC_DECORATE_BUTTON_PARAMS_GUARDED(handler_, custom_id_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_BUTTON_PARAMS_POLICY(handler_, (custom_id_), args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__))
#define DCC_DECORATE_BUTTON_PARAMS_DATA_GUARDED(handler_, custom_id_, args_type_, arg_, user_data_, params_, ...) \
    DCC_DECORATE_BUTTON_PARAMS_DATA_POLICY( \
        handler_, (custom_id_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_BUTTON_PREFIX_PARAMS_DATA_POLICY( \
    handler_, custom_id_prefix_, args_type_, arg_, user_data_, params_, policy_ \
) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_component_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_BUTTON_PREFIX_PARAMS_DATA_POLICY( \
            (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )
#define DCC_DECORATE_BUTTON_PREFIX_PARAMS(handler_, custom_id_prefix_, args_type_, arg_, params_) \
    DCC_DECORATE_BUTTON_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, NULL, params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_BUTTON_PREFIX_PARAMS_DATA(handler_, custom_id_prefix_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_BUTTON_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_BUTTON_PREFIX_PARAMS_POLICY(handler_, custom_id_prefix_, args_type_, arg_, params_, policy_) \
    DCC_DECORATE_BUTTON_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, NULL, params_, (policy_) \
    )
#define DCC_DECORATE_BUTTON_PREFIX_PARAMS_GUARDED(handler_, custom_id_prefix_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_BUTTON_PREFIX_PARAMS_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_DECORATE_BUTTON_PREFIX_PARAMS_DATA_GUARDED( \
    handler_, custom_id_prefix_, args_type_, arg_, user_data_, params_, ... \
) \
    DCC_DECORATE_BUTTON_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_BUTTON_NS_PARAMS_DATA_POLICY( \
    handler_, namespace_, action_, args_type_, arg_, user_data_, params_, policy_ \
) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_component_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_BUTTON_NS_PARAMS_DATA_POLICY( \
            namespace_, action_, args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )
#define DCC_DECORATE_BUTTON_NS_PARAMS(handler_, namespace_, action_, args_type_, arg_, params_) \
    DCC_DECORATE_BUTTON_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, NULL, params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_BUTTON_NS_PARAMS_DATA(handler_, namespace_, action_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_BUTTON_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_BUTTON_NS_PARAMS_POLICY(handler_, namespace_, action_, args_type_, arg_, params_, policy_) \
    DCC_DECORATE_BUTTON_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, NULL, params_, (policy_) \
    )
#define DCC_DECORATE_BUTTON_NS_PARAMS_GUARDED(handler_, namespace_, action_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_BUTTON_NS_PARAMS_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_DECORATE_BUTTON_NS_PARAMS_DATA_GUARDED( \
    handler_, namespace_, action_, args_type_, arg_, user_data_, params_, ... \
) \
    DCC_DECORATE_BUTTON_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY( \
    handler_, namespace_, action_prefix_, args_type_, arg_, user_data_, params_, policy_ \
) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_component_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )
#define DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS(handler_, namespace_, action_prefix_, args_type_, arg_, params_) \
    DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, NULL, params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_DATA( \
    handler_, namespace_, action_prefix_, args_type_, arg_, user_data_, params_ \
) \
    DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_POLICY( \
    handler_, namespace_, action_prefix_, args_type_, arg_, params_, policy_ \
) \
    DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, NULL, params_, (policy_) \
    )
#define DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_GUARDED( \
    handler_, namespace_, action_prefix_, args_type_, arg_, params_, ... \
) \
    DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED( \
    handler_, namespace_, action_prefix_, args_type_, arg_, user_data_, params_, ... \
) \
    DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_SELECT_PARAMS_DATA_POLICY(handler_, custom_id_, args_type_, arg_, user_data_, params_, policy_) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_component_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_SELECT_PARAMS_DATA_POLICY( \
            (custom_id_), args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )
#define DCC_DECORATE_SELECT_PARAMS(handler_, custom_id_, args_type_, arg_, params_) \
    DCC_DECORATE_SELECT_PARAMS_DATA_POLICY( \
        handler_, (custom_id_), args_type_, arg_, NULL, params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_SELECT_PARAMS_DATA(handler_, custom_id_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_SELECT_PARAMS_DATA_POLICY( \
        handler_, (custom_id_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_SELECT_PARAMS_POLICY(handler_, custom_id_, args_type_, arg_, params_, policy_) \
    DCC_DECORATE_SELECT_PARAMS_DATA_POLICY(handler_, (custom_id_), args_type_, arg_, NULL, params_, (policy_))
#define DCC_DECORATE_SELECT_PARAMS_GUARDED(handler_, custom_id_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_SELECT_PARAMS_POLICY(handler_, (custom_id_), args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__))
#define DCC_DECORATE_SELECT_PARAMS_DATA_GUARDED(handler_, custom_id_, args_type_, arg_, user_data_, params_, ...) \
    DCC_DECORATE_SELECT_PARAMS_DATA_POLICY( \
        handler_, (custom_id_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_SELECT_PREFIX_PARAMS_DATA_POLICY( \
    handler_, custom_id_prefix_, args_type_, arg_, user_data_, params_, policy_ \
) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_component_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_SELECT_PREFIX_PARAMS_DATA_POLICY( \
            (custom_id_prefix_), args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )
#define DCC_DECORATE_SELECT_PREFIX_PARAMS(handler_, custom_id_prefix_, args_type_, arg_, params_) \
    DCC_DECORATE_SELECT_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, NULL, params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_SELECT_PREFIX_PARAMS_DATA(handler_, custom_id_prefix_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_SELECT_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_SELECT_PREFIX_PARAMS_POLICY(handler_, custom_id_prefix_, args_type_, arg_, params_, policy_) \
    DCC_DECORATE_SELECT_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, NULL, params_, (policy_) \
    )
#define DCC_DECORATE_SELECT_PREFIX_PARAMS_GUARDED(handler_, custom_id_prefix_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_SELECT_PREFIX_PARAMS_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_DECORATE_SELECT_PREFIX_PARAMS_DATA_GUARDED( \
    handler_, custom_id_prefix_, args_type_, arg_, user_data_, params_, ... \
) \
    DCC_DECORATE_SELECT_PREFIX_PARAMS_DATA_POLICY( \
        handler_, (custom_id_prefix_), args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_SELECT_NS_PARAMS_DATA_POLICY( \
    handler_, namespace_, action_, args_type_, arg_, user_data_, params_, policy_ \
) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_component_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_SELECT_NS_PARAMS_DATA_POLICY( \
            namespace_, action_, args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )
#define DCC_DECORATE_SELECT_NS_PARAMS(handler_, namespace_, action_, args_type_, arg_, params_) \
    DCC_DECORATE_SELECT_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, NULL, params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_SELECT_NS_PARAMS_DATA(handler_, namespace_, action_, args_type_, arg_, user_data_, params_) \
    DCC_DECORATE_SELECT_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_SELECT_NS_PARAMS_POLICY(handler_, namespace_, action_, args_type_, arg_, params_, policy_) \
    DCC_DECORATE_SELECT_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, NULL, params_, (policy_) \
    )
#define DCC_DECORATE_SELECT_NS_PARAMS_GUARDED(handler_, namespace_, action_, args_type_, arg_, params_, ...) \
    DCC_DECORATE_SELECT_NS_PARAMS_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_DECORATE_SELECT_NS_PARAMS_DATA_GUARDED( \
    handler_, namespace_, action_, args_type_, arg_, user_data_, params_, ... \
) \
    DCC_DECORATE_SELECT_NS_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_SELECT_NS_PREFIX_PARAMS_DATA_POLICY( \
    handler_, namespace_, action_prefix_, args_type_, arg_, user_data_, params_, policy_ \
) \
    DCC_DECORATE_TYPED_HANDLER_IMPL( \
        dcc_app_typed_component_t, \
        handler_, \
        args_type_, \
        arg_, \
        DCC_ON_SELECT_NS_PREFIX_PARAMS_DATA_POLICY( \
            namespace_, action_prefix_, args_type_, (handler_), (user_data_), params_, (policy_) \
        ) \
    )
#define DCC_DECORATE_SELECT_NS_PREFIX_PARAMS(handler_, namespace_, action_prefix_, args_type_, arg_, params_) \
    DCC_DECORATE_SELECT_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, NULL, params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_SELECT_NS_PREFIX_PARAMS_DATA( \
    handler_, namespace_, action_prefix_, args_type_, arg_, user_data_, params_ \
) \
    DCC_DECORATE_SELECT_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_NO_POLICY() \
    )
#define DCC_DECORATE_SELECT_NS_PREFIX_PARAMS_POLICY( \
    handler_, namespace_, action_prefix_, args_type_, arg_, params_, policy_ \
) \
    DCC_DECORATE_SELECT_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, NULL, params_, (policy_) \
    )
#define DCC_DECORATE_SELECT_NS_PREFIX_PARAMS_GUARDED( \
    handler_, namespace_, action_prefix_, args_type_, arg_, params_, ... \
) \
    DCC_DECORATE_SELECT_NS_PREFIX_PARAMS_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )
#define DCC_DECORATE_SELECT_NS_PREFIX_PARAMS_DATA_GUARDED( \
    handler_, namespace_, action_prefix_, args_type_, arg_, user_data_, params_, ... \
) \
    DCC_DECORATE_SELECT_NS_PREFIX_PARAMS_DATA_POLICY( \
        handler_, namespace_, action_prefix_, args_type_, arg_, (user_data_), params_, DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_SLASH_AS(handler_, command_name_, description_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_slash_command_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (command_name_), \
        .description = (description_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_SLASH_AS_DATA_POLICY(handler_, command_name_, description_, user_data_, policy_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_slash_command_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_slash_command_t), \
        .name = (command_name_), \
        .description = (description_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_SLASH_AS_DATA(handler_, command_name_, description_, user_data_) \
    DCC_DECORATE_SLASH_AS_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (description_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_SLASH_AS_POLICY(handler_, command_name_, description_, policy_) \
    DCC_DECORATE_SLASH_AS_DATA_POLICY(handler_, (command_name_), (description_), NULL, (policy_))

#define DCC_DECORATE_SLASH_AS_GUARDED(handler_, command_name_, description_, ...) \
    DCC_DECORATE_SLASH_AS_POLICY(handler_, (command_name_), (description_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_DECORATE_SLASH_AS_DATA_GUARDED(handler_, command_name_, description_, user_data_, ...) \
    DCC_DECORATE_SLASH_AS_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (description_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_SLASH(handler_, description_) \
    DCC_DECORATE_SLASH_AS(handler_, #handler_, (description_))

#define DCC_DECORATE_SLASH_DATA(handler_, description_, user_data_) \
    DCC_DECORATE_SLASH_AS_DATA(handler_, #handler_, (description_), (user_data_))

#define DCC_DECORATE_SLASH_POLICY(handler_, description_, policy_) \
    DCC_DECORATE_SLASH_AS_POLICY(handler_, #handler_, (description_), (policy_))

#define DCC_DECORATE_SLASH_GUARDED(handler_, description_, ...) \
    DCC_DECORATE_SLASH_AS_GUARDED(handler_, #handler_, (description_), __VA_ARGS__)

#define DCC_DECORATE_SLASH_DATA_GUARDED(handler_, description_, user_data_, ...) \
    DCC_DECORATE_SLASH_AS_DATA_GUARDED(handler_, #handler_, (description_), (user_data_), __VA_ARGS__)

#define DCC_DECORATE_COMMAND_AS(handler_, command_name_, description_) \
    DCC_DECORATE_SLASH_AS(handler_, (command_name_), (description_))

#define DCC_DECORATE_COMMAND(handler_, description_) \
    DCC_DECORATE_SLASH(handler_, (description_))

#define DCC_DECORATE_COMMAND_AS_DATA(handler_, command_name_, description_, user_data_) \
    DCC_DECORATE_SLASH_AS_DATA(handler_, (command_name_), (description_), (user_data_))

#define DCC_DECORATE_COMMAND_AS_POLICY(handler_, command_name_, description_, policy_) \
    DCC_DECORATE_SLASH_AS_POLICY(handler_, (command_name_), (description_), (policy_))

#define DCC_DECORATE_COMMAND_AS_GUARDED(handler_, command_name_, description_, ...) \
    DCC_DECORATE_SLASH_AS_GUARDED(handler_, (command_name_), (description_), __VA_ARGS__)

#define DCC_DECORATE_COMMAND_AS_DATA_GUARDED(handler_, command_name_, description_, user_data_, ...) \
    DCC_DECORATE_SLASH_AS_DATA_GUARDED( \
        handler_, \
        (command_name_), \
        (description_), \
        (user_data_), \
        __VA_ARGS__ \
    )

#define DCC_DECORATE_COMMAND_DATA(handler_, description_, user_data_) \
    DCC_DECORATE_SLASH_DATA(handler_, (description_), (user_data_))

#define DCC_DECORATE_COMMAND_POLICY(handler_, description_, policy_) \
    DCC_DECORATE_SLASH_POLICY(handler_, (description_), (policy_))

#define DCC_DECORATE_COMMAND_GUARDED(handler_, description_, ...) \
    DCC_DECORATE_SLASH_GUARDED(handler_, (description_), __VA_ARGS__)

#define DCC_DECORATE_COMMAND_DATA_GUARDED(handler_, description_, user_data_, ...) \
    DCC_DECORATE_SLASH_DATA_GUARDED(handler_, (description_), (user_data_), __VA_ARGS__)

#define DCC_DECORATE_SUBCOMMAND_AS(handler_, command_name_, subcommand_path_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_subcommand_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_SUBCOMMAND_DATA_POLICY(handler_, command_name_, subcommand_path_, user_data_, policy_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_subcommand_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_subcommand_t), \
        .command_name = (command_name_), \
        .subcommand_path = (subcommand_path_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_SUBCOMMAND(handler_, command_name_, subcommand_path_) \
    DCC_DECORATE_SUBCOMMAND_AS(handler_, (command_name_), (subcommand_path_))

#define DCC_DECORATE_SUBCOMMAND_DATA(handler_, command_name_, subcommand_path_, user_data_) \
    DCC_DECORATE_SUBCOMMAND_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (subcommand_path_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_SUBCOMMAND_POLICY(handler_, command_name_, subcommand_path_, policy_) \
    DCC_DECORATE_SUBCOMMAND_DATA_POLICY(handler_, (command_name_), (subcommand_path_), NULL, (policy_))

#define DCC_DECORATE_SUBCOMMAND_GUARDED(handler_, command_name_, subcommand_path_, ...) \
    DCC_DECORATE_SUBCOMMAND_POLICY(handler_, (command_name_), (subcommand_path_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_DECORATE_SUBCOMMAND_DATA_GUARDED(handler_, command_name_, subcommand_path_, user_data_, ...) \
    DCC_DECORATE_SUBCOMMAND_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (subcommand_path_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_AUTOCOMPLETE_AS(handler_, command_name_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_autocomplete_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_autocomplete_t), \
        .name = (command_name_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_AUTOCOMPLETE_AS_DATA_POLICY(handler_, command_name_, user_data_, policy_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_autocomplete_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_autocomplete_t), \
        .name = (command_name_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_AUTOCOMPLETE(handler_) \
    DCC_DECORATE_AUTOCOMPLETE_AS(handler_, #handler_)

#define DCC_DECORATE_AUTOCOMPLETE_AS_DATA(handler_, command_name_, user_data_) \
    DCC_DECORATE_AUTOCOMPLETE_AS_DATA_POLICY(handler_, (command_name_), (user_data_), DCC_ROUTE_NO_POLICY())

#define DCC_DECORATE_AUTOCOMPLETE_AS_POLICY(handler_, command_name_, policy_) \
    DCC_DECORATE_AUTOCOMPLETE_AS_DATA_POLICY(handler_, (command_name_), NULL, (policy_))

#define DCC_DECORATE_AUTOCOMPLETE_AS_GUARDED(handler_, command_name_, ...) \
    DCC_DECORATE_AUTOCOMPLETE_AS_POLICY(handler_, (command_name_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_DECORATE_AUTOCOMPLETE_AS_DATA_GUARDED(handler_, command_name_, user_data_, ...) \
    DCC_DECORATE_AUTOCOMPLETE_AS_DATA_POLICY( \
        handler_, \
        (command_name_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_DECORATE_AUTOCOMPLETE_DATA(handler_, user_data_) \
    DCC_DECORATE_AUTOCOMPLETE_AS_DATA(handler_, #handler_, (user_data_))

#define DCC_DECORATE_AUTOCOMPLETE_POLICY(handler_, policy_) \
    DCC_DECORATE_AUTOCOMPLETE_AS_POLICY(handler_, #handler_, (policy_))

#define DCC_DECORATE_AUTOCOMPLETE_GUARDED(handler_, ...) \
    DCC_DECORATE_AUTOCOMPLETE_AS_GUARDED(handler_, #handler_, __VA_ARGS__)

#define DCC_DECORATE_AUTOCOMPLETE_DATA_GUARDED(handler_, user_data_, ...) \
    DCC_DECORATE_AUTOCOMPLETE_AS_DATA_GUARDED(handler_, #handler_, (user_data_), __VA_ARGS__)

#define DCC_DECORATE_USER_CONTEXT_MENU_AS(handler_, menu_name_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_context_menu_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_context_menu_t), \
        .type = DCC_APP_EXTENSION_USER_CONTEXT_MENU, \
        .name = (menu_name_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_CONTEXT_MENU_DATA_POLICY(handler_, type_, menu_name_, user_data_, policy_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_context_menu_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_context_menu_t), \
        .type = (type_), \
        .name = (menu_name_), \
        .handler = (handler_), \
        .user_data = (user_data_), \
        .policy = (policy_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_USER_CONTEXT_MENU(handler_) \
    DCC_DECORATE_USER_CONTEXT_MENU_AS(handler_, #handler_)

#define DCC_DECORATE_USER_CONTEXT_MENU_AS_DATA(handler_, menu_name_, user_data_) \
    DCC_DECORATE_CONTEXT_MENU_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_USER_CONTEXT_MENU, \
        (menu_name_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_USER_CONTEXT_MENU_AS_POLICY(handler_, menu_name_, policy_) \
    DCC_DECORATE_CONTEXT_MENU_DATA_POLICY(handler_, DCC_APP_EXTENSION_USER_CONTEXT_MENU, (menu_name_), NULL, (policy_))

#define DCC_DECORATE_USER_CONTEXT_MENU_AS_GUARDED(handler_, menu_name_, ...) \
    DCC_DECORATE_USER_CONTEXT_MENU_AS_POLICY(handler_, (menu_name_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_DECORATE_USER_CONTEXT_MENU_DATA(handler_, user_data_) \
    DCC_DECORATE_USER_CONTEXT_MENU_AS_DATA(handler_, #handler_, (user_data_))

#define DCC_DECORATE_USER_CONTEXT_MENU_GUARDED(handler_, ...) \
    DCC_DECORATE_USER_CONTEXT_MENU_AS_GUARDED(handler_, #handler_, __VA_ARGS__)

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS(handler_, menu_name_) \
    static void handler_(dcc_ctx_t *ctx, void *user_data); \
    static const dcc_app_extension_context_menu_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_context_menu_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, \
        .name = (menu_name_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_ctx_t *ctx, void *user_data)

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU(handler_) \
    DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS(handler_, #handler_)

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_DATA(handler_, menu_name_, user_data_) \
    DCC_DECORATE_CONTEXT_MENU_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, \
        (menu_name_), \
        (user_data_), \
        DCC_ROUTE_NO_POLICY() \
    )

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_POLICY(handler_, menu_name_, policy_) \
    DCC_DECORATE_CONTEXT_MENU_DATA_POLICY( \
        handler_, \
        DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU, \
        (menu_name_), \
        NULL, \
        (policy_) \
    )

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_GUARDED(handler_, menu_name_, ...) \
    DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_POLICY(handler_, (menu_name_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_DATA(handler_, user_data_) \
    DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_DATA(handler_, #handler_, (user_data_))

#define DCC_DECORATE_MESSAGE_CONTEXT_MENU_GUARDED(handler_, ...) \
    DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS_GUARDED(handler_, #handler_, __VA_ARGS__)

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

#define DCC_DECORATE_EVENT(handler_, event_type_) \
    static void handler_(dcc_app_t *app, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_EVENT, \
        .event_type = (event_type_), \
        .event_handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_event_t *event, void *user_data)

#define DCC_DECORATE_EVENT_DATA(handler_, event_type_, user_data_) \
    static void handler_(dcc_app_t *app, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_EVENT, \
        .event_type = (event_type_), \
        .event_handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_event_t *event, void *user_data)

#define DCC_DECORATE_EVENT_PAYLOAD(handler_, event_type_, payload_type_, payload_name_, accessor_) \
    static void handler_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    ); \
    static void DCC_EVENT_DISPATCH_NAME(handler_)( \
        dcc_app_t *app, \
        const dcc_event_t *event, \
        void *user_data \
    ) { \
        const payload_type_ *payload_name_ = accessor_((event)); \
        if (payload_name_ != NULL) { \
            handler_((app), payload_name_, (event), (user_data)); \
        } \
    } \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_EVENT, \
        .event_type = (event_type_), \
        .event_handler = DCC_EVENT_DISPATCH_NAME(handler_) \
    }; \
    static void handler_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    )

#define DCC_DECORATE_EVENT_PAYLOAD_DATA( \
    handler_, \
    event_type_, \
    payload_type_, \
    payload_name_, \
    accessor_, \
    user_data_ \
) \
    static void handler_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    ); \
    static void DCC_EVENT_DISPATCH_NAME(handler_)( \
        dcc_app_t *app, \
        const dcc_event_t *event, \
        void *user_data \
    ) { \
        const payload_type_ *payload_name_ = accessor_((event)); \
        if (payload_name_ != NULL) { \
            handler_((app), payload_name_, (event), (user_data)); \
        } \
    } \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_EVENT, \
        .event_type = (event_type_), \
        .event_handler = DCC_EVENT_DISPATCH_NAME(handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_( \
        dcc_app_t *app, \
        const payload_type_ *payload_name_, \
        const dcc_event_t *event, \
        void *user_data \
    )

#define DCC_DECORATE_GUILD_CREATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_CREATE, dcc_guild_t, guild, dcc_event_guild)
#define DCC_DECORATE_GUILD_CREATE_DATA(handler_, user_data_) \
    DCC_DECORATE_EVENT_PAYLOAD_DATA( \
        handler_, DCC_EVENT_GUILD_CREATE, dcc_guild_t, guild, dcc_event_guild, (user_data_) \
    )
#define DCC_DECORATE_GUILD_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_UPDATE, dcc_guild_t, guild, dcc_event_guild)
#define DCC_DECORATE_GUILD_DELETE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_DELETE, dcc_guild_t, guild, dcc_event_guild)

#define DCC_DECORATE_MEMBER_JOIN(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_MEMBER_ADD, dcc_member_t, member, dcc_event_member)
#define DCC_DECORATE_MEMBER_JOIN_DATA(handler_, user_data_) \
    DCC_DECORATE_EVENT_PAYLOAD_DATA( \
        handler_, DCC_EVENT_GUILD_MEMBER_ADD, dcc_member_t, member, dcc_event_member, (user_data_) \
    )
#define DCC_DECORATE_MEMBER_ADD(handler_) DCC_DECORATE_MEMBER_JOIN(handler_)
#define DCC_DECORATE_MEMBER_ADD_DATA(handler_, user_data_) DCC_DECORATE_MEMBER_JOIN_DATA(handler_, (user_data_))
#define DCC_DECORATE_MEMBER_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_MEMBER_UPDATE, dcc_member_t, member, dcc_event_member)
#define DCC_DECORATE_MEMBER_REMOVE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_MEMBER_REMOVE, dcc_member_t, member, dcc_event_member)

#define DCC_DECORATE_ROLE_CREATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_ROLE_CREATE, dcc_role_t, role, dcc_event_role)
#define DCC_DECORATE_ROLE_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_ROLE_UPDATE, dcc_role_t, role, dcc_event_role)
#define DCC_DECORATE_ROLE_DELETE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_ROLE_DELETE, dcc_role_t, role, dcc_event_role)

#define DCC_DECORATE_CHANNEL_CREATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_CHANNEL_CREATE, dcc_channel_t, channel, dcc_event_channel)
#define DCC_DECORATE_CHANNEL_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_CHANNEL_UPDATE, dcc_channel_t, channel, dcc_event_channel)
#define DCC_DECORATE_CHANNEL_DELETE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_CHANNEL_DELETE, dcc_channel_t, channel, dcc_event_channel)
#define DCC_DECORATE_THREAD_CREATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_THREAD_CREATE, dcc_channel_t, thread, dcc_event_channel)
#define DCC_DECORATE_THREAD_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_THREAD_UPDATE, dcc_channel_t, thread, dcc_event_channel)
#define DCC_DECORATE_THREAD_DELETE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_THREAD_DELETE, dcc_channel_t, thread, dcc_event_channel)

#define DCC_DECORATE_REACTION_ADD(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_REACTION_ADD, \
        dcc_message_reaction_t, \
        reaction, \
        dcc_event_message_reaction \
    )
#define DCC_DECORATE_REACTION_REMOVE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_REACTION_REMOVE, \
        dcc_message_reaction_t, \
        reaction, \
        dcc_event_message_reaction \
    )
#define DCC_DECORATE_REACTION_CLEAR(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_REACTION_REMOVE_ALL, \
        dcc_message_reaction_t, \
        reaction, \
        dcc_event_message_reaction \
    )
#define DCC_DECORATE_REACTION_EMOJI_CLEAR(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_REACTION_REMOVE_EMOJI, \
        dcc_message_reaction_t, \
        reaction, \
        dcc_event_message_reaction \
    )

#define DCC_DECORATE_POLL_VOTE_ADD(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_POLL_VOTE_ADD, \
        dcc_poll_vote_t, \
        poll_vote, \
        dcc_event_poll_vote \
    )
#define DCC_DECORATE_POLL_VOTE_REMOVE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_MESSAGE_POLL_VOTE_REMOVE, \
        dcc_poll_vote_t, \
        poll_vote, \
        dcc_event_poll_vote \
    )

#define DCC_DECORATE_VOICE_STATE_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_VOICE_STATE_UPDATE, \
        dcc_voice_state_t, \
        voice_state, \
        dcc_event_voice_state \
    )
#define DCC_DECORATE_TYPING_START(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD( \
        handler_, \
        DCC_EVENT_TYPING_START, \
        dcc_typing_start_t, \
        typing, \
        dcc_event_typing_start \
    )
#define DCC_DECORATE_PRESENCE_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_PRESENCE_UPDATE, dcc_presence_t, presence, dcc_event_presence)
#define DCC_DECORATE_USER_UPDATE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_USER_UPDATE, dcc_user_t, user, dcc_event_user)
#define DCC_DECORATE_GUILD_BAN_ADD(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_BAN_ADD, dcc_guild_ban_t, ban, dcc_event_guild_ban)
#define DCC_DECORATE_GUILD_BAN_REMOVE(handler_) \
    DCC_DECORATE_EVENT_PAYLOAD(handler_, DCC_EVENT_GUILD_BAN_REMOVE, dcc_guild_ban_t, ban, dcc_event_guild_ban)

#define DCC_DECORATE_READY(handler_) \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_READY, \
        .ready_handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)

#define DCC_DECORATE_READY_DATA(handler_, user_data_) \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_READY, \
        .ready_handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)

#define DCC_DECORATE_READY_ONCE(handler_) \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_READY, \
        .ready_handler = (handler_), \
        .once = 1U \
    }; \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)

#define DCC_DECORATE_READY_ONCE_DATA(handler_, user_data_) \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_READY, \
        .ready_handler = (handler_), \
        .user_data = (user_data_), \
        .once = 1U \
    }; \
    static void handler_(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data)

#define DCC_DECORATE_MESSAGE_CREATE(handler_) \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_CREATE, \
        .message_handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data)

#define DCC_DECORATE_MESSAGE_CREATE_DATA(handler_, user_data_) \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_CREATE, \
        .message_handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data)

#define DCC_DECORATE_MESSAGE(handler_) DCC_DECORATE_MESSAGE_CREATE(handler_)
#define DCC_DECORATE_MESSAGE_DATA(handler_, user_data_) DCC_DECORATE_MESSAGE_CREATE_DATA(handler_, (user_data_))

#define DCC_DECORATE_MESSAGE_UPDATE(handler_) \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_UPDATE, \
        .message_handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data)

#define DCC_DECORATE_MESSAGE_DELETE(handler_) \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data); \
    static const dcc_app_extension_event_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_event_t), \
        .type = DCC_APP_EXTENSION_MESSAGE_DELETE, \
        .message_handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, const dcc_message_t *message, const dcc_event_t *event, void *user_data)

#define DCC_DECORATE_MESSAGE_COMMAND(handler_, prefix_, command_name_) \
    static void handler_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const char *args, \
        const dcc_event_t *event, \
        void *user_data \
    ); \
    static const dcc_app_extension_message_command_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_message_command_t), \
        .prefix = (prefix_), \
        .name = (command_name_), \
        .handler = (handler_) \
    }; \
    static void handler_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const char *args, \
        const dcc_event_t *event, \
        void *user_data \
    )

#define DCC_DECORATE_MESSAGE_COMMAND_DATA(handler_, prefix_, command_name_, user_data_) \
    static void handler_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const char *args, \
        const dcc_event_t *event, \
        void *user_data \
    ); \
    static const dcc_app_extension_message_command_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_message_command_t), \
        .prefix = (prefix_), \
        .name = (command_name_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_( \
        dcc_app_t *app, \
        const dcc_message_t *message, \
        const char *args, \
        const dcc_event_t *event, \
        void *user_data \
    )

#define DCC_DECORATE_MESSAGE_COMMAND_FN(handler_, prefix_) \
    DCC_DECORATE_MESSAGE_COMMAND(handler_, (prefix_), #handler_)

#define DCC_DECORATE_MESSAGE_COMMAND_FN_DATA(handler_, prefix_, user_data_) \
    DCC_DECORATE_MESSAGE_COMMAND_DATA(handler_, (prefix_), #handler_, (user_data_))

#define DCC_DECORATE_PREFIX_COMMAND(handler_, prefix_, command_name_) \
    DCC_DECORATE_MESSAGE_COMMAND(handler_, (prefix_), (command_name_))

#define DCC_DECORATE_PREFIX_COMMAND_DATA(handler_, prefix_, command_name_, user_data_) \
    DCC_DECORATE_MESSAGE_COMMAND_DATA(handler_, (prefix_), (command_name_), (user_data_))

#define DCC_DECORATE_PREFIX_COMMAND_FN(handler_, prefix_) \
    DCC_DECORATE_MESSAGE_COMMAND_FN(handler_, (prefix_))

#define DCC_DECORATE_PREFIX_COMMAND_FN_DATA(handler_, prefix_, user_data_) \
    DCC_DECORATE_MESSAGE_COMMAND_FN_DATA(handler_, (prefix_), (user_data_))

#define DCC_DECORATE_TEXT_COMMAND(handler_, prefix_, command_name_) \
    DCC_DECORATE_PREFIX_COMMAND(handler_, (prefix_), (command_name_))

#define DCC_DECORATE_TEXT_COMMAND_FN(handler_, prefix_) \
    DCC_DECORATE_PREFIX_COMMAND_FN(handler_, (prefix_))

#define DCC_DECORATE_TASK_MS(handler_, interval_ms_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_MS, \
        .interval = (interval_ms_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_MS_DATA(handler_, interval_ms_, user_data_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_MS, \
        .interval = (interval_ms_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_SECONDS(handler_, interval_seconds_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_SECONDS, \
        .interval = (interval_seconds_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_MINUTES(handler_, interval_minutes_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_MINUTES, \
        .interval = (interval_minutes_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_HOURS(handler_, interval_hours_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_HOURS, \
        .interval = (interval_hours_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_KST(handler_, hhmm_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_KST, \
        .hhmm = (hhmm_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_DAILY_KST(handler_, hhmm_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST, \
        .hhmm = (hhmm_), \
        .handler = (handler_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#define DCC_DECORATE_TASK_DAILY_KST_DATA(handler_, hhmm_, user_data_) \
    static void handler_(dcc_app_t *app, void *user_data); \
    static const dcc_app_extension_task_t DCC_DECORATED_ROUTE_NAME(handler_) = { \
        .size = sizeof(dcc_app_extension_task_t), \
        .type = DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST, \
        .hhmm = (hhmm_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    }; \
    static void handler_(dcc_app_t *app, void *user_data)

#endif
