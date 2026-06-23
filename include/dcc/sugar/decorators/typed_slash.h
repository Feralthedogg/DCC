#ifndef DCC_SUGAR_DECORATORS_TYPED_SLASH_H
#define DCC_SUGAR_DECORATORS_TYPED_SLASH_H

#include <dcc/sugar/decorators/base.h>

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


#endif /* DCC_SUGAR_DECORATORS_TYPED_SLASH_H */
