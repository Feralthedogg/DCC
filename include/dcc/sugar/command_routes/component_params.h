#ifndef DCC_SUGAR_COMMAND_ROUTES_COMPONENT_PARAMS_H
#define DCC_SUGAR_COMMAND_ROUTES_COMPONENT_PARAMS_H

#include <dcc/sugar/command_routes/typed.h>

#define DCC_APP_BUTTON_NS_PARAMS_FN(app_, namespace_, handler_, args_type_, params_) \
    DCC_APP_TYPED_BUTTON((app_), DCC_ON_BUTTON_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_APP_BUTTON_NS_PARAMS_FN_DATA(app_, namespace_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_BUTTON( \
        (app_), DCC_ON_BUTTON_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_BUTTON_NS_PARAMS_FN_DATA_GUARDED(app_, namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_APP_TYPED_BUTTON( \
        (app_), \
        DCC_ON_BUTTON_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_BUTTON_NS_PREFIX_PARAMS_FN_DATA(app_, namespace_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_BUTTON( \
        (app_), DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_BUTTON_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
    app_, namespace_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_APP_TYPED_BUTTON( \
        (app_), \
        DCC_ON_BUTTON_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_APP_SELECT_NS_PARAMS_FN(app_, namespace_, handler_, args_type_, params_) \
    DCC_APP_TYPED_SELECT((app_), DCC_ON_SELECT_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_APP_SELECT_NS_PARAMS_FN_DATA(app_, namespace_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_SELECT( \
        (app_), DCC_ON_SELECT_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_SELECT_NS_PARAMS_FN_DATA_GUARDED(app_, namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_APP_TYPED_SELECT( \
        (app_), \
        DCC_ON_SELECT_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_SELECT_NS_PREFIX_PARAMS_FN_DATA(app_, namespace_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_SELECT( \
        (app_), DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_SELECT_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
    app_, namespace_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_APP_TYPED_SELECT( \
        (app_), \
        DCC_ON_SELECT_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )

#endif /* DCC_SUGAR_COMMAND_ROUTES_COMPONENT_PARAMS_H */
