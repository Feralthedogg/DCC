#ifndef DCC_SUGAR_COMMAND_ROUTES_SLASH_H
#define DCC_SUGAR_COMMAND_ROUTES_SLASH_H

#include <dcc/sugar/command_routes/typed.h>

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

#endif /* DCC_SUGAR_COMMAND_ROUTES_SLASH_H */
