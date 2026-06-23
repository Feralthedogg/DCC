#ifndef DCC_SUGAR_COMMAND_ROUTES_AUTOCOMPLETE_H
#define DCC_SUGAR_COMMAND_ROUTES_AUTOCOMPLETE_H

#include <dcc/sugar/command_routes/typed.h>

#define DCC_APP_AUTOCOMPLETE_PARAMS(app_, command_name_, args_type_, handler_, params_) \
    DCC_APP_TYPED_AUTOCOMPLETE( \
        (app_), DCC_ON_AUTOCOMPLETE_PARAMS((command_name_), args_type_, (handler_), params_) \
    )
#define DCC_APP_AUTOCOMPLETE_PARAMS_DATA(app_, command_name_, args_type_, handler_, user_data_, params_) \
    DCC_APP_TYPED_AUTOCOMPLETE( \
        (app_), DCC_ON_AUTOCOMPLETE_PARAMS_DATA((command_name_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_APP_AUTOCOMPLETE_PARAMS_FN(app_, handler_, args_type_, params_) \
    DCC_APP_TYPED_AUTOCOMPLETE((app_), DCC_ON_AUTOCOMPLETE_PARAMS_FN(handler_, args_type_, params_))
#define DCC_APP_AUTOCOMPLETE_PARAMS_FN_DATA(app_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_AUTOCOMPLETE( \
        (app_), DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA(handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_AUTOCOMPLETE_PARAMS_FN_DATA_GUARDED(app_, handler_, args_type_, user_data_, params_, ...) \
    DCC_APP_TYPED_AUTOCOMPLETE( \
        (app_), DCC_ON_AUTOCOMPLETE_PARAMS_FN_DATA_GUARDED(handler_, args_type_, (user_data_), params_, __VA_ARGS__) \
    )

#endif /* DCC_SUGAR_COMMAND_ROUTES_AUTOCOMPLETE_H */
