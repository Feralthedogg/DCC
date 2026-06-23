#ifndef DCC_SUGAR_COMMAND_ROUTES_MODALS_H
#define DCC_SUGAR_COMMAND_ROUTES_MODALS_H

#include <dcc/sugar/command_routes/typed.h>

#define DCC_APP_MODAL_PARAMS(app_, custom_id_, args_type_, handler_, params_) \
    DCC_APP_TYPED_MODAL((app_), DCC_ON_MODAL_PARAMS((custom_id_), args_type_, (handler_), params_))
#define DCC_APP_MODAL_PARAMS_DATA(app_, custom_id_, args_type_, handler_, user_data_, params_) \
    DCC_APP_TYPED_MODAL( \
        (app_), DCC_ON_MODAL_PARAMS_DATA((custom_id_), args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_APP_MODAL_NS_PARAMS(app_, namespace_, action_, args_type_, handler_, params_) \
    DCC_APP_TYPED_MODAL((app_), DCC_ON_MODAL_NS_PARAMS(namespace_, action_, args_type_, (handler_), params_))
#define DCC_APP_MODAL_NS_PARAMS_DATA(app_, namespace_, action_, args_type_, handler_, user_data_, params_) \
    DCC_APP_TYPED_MODAL( \
        (app_), DCC_ON_MODAL_NS_PARAMS_DATA(namespace_, action_, args_type_, (handler_), (user_data_), params_) \
    )
#define DCC_APP_MODAL_NS_PARAMS_FN(app_, namespace_, handler_, args_type_, params_) \
    DCC_APP_TYPED_MODAL((app_), DCC_ON_MODAL_NS_PARAMS_FN(namespace_, handler_, args_type_, params_))
#define DCC_APP_MODAL_NS_PARAMS_FN_DATA(app_, namespace_, handler_, args_type_, user_data_, params_) \
    DCC_APP_TYPED_MODAL( \
        (app_), DCC_ON_MODAL_NS_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_MODAL_NS_PARAMS_FN_DATA_GUARDED(app_, namespace_, handler_, args_type_, user_data_, params_, ...) \
    DCC_APP_TYPED_MODAL( \
        (app_), \
        DCC_ON_MODAL_NS_PARAMS_FN_DATA_GUARDED(namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_MODAL_NS_PREFIX_PARAMS_FN_DATA( \
    app_, namespace_, handler_, args_type_, user_data_, params_ \
) \
    DCC_APP_TYPED_MODAL( \
        (app_), DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA(namespace_, handler_, args_type_, (user_data_), params_) \
    )
#define DCC_APP_MODAL_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
    app_, namespace_, handler_, args_type_, user_data_, params_, ... \
) \
    DCC_APP_TYPED_MODAL( \
        (app_), \
        DCC_ON_MODAL_NS_PREFIX_PARAMS_FN_DATA_GUARDED( \
            namespace_, handler_, args_type_, (user_data_), params_, __VA_ARGS__ \
        ) \
    )
#define DCC_APP_MODAL_FLOW_NS_PARAMS_FN_DATA(app_, namespace_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_APP_MODAL_FLOW( \
        (app_), DCC_MODAL_FLOW_NS_PARAMS_FN_DATA(namespace_, (title_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )
#define DCC_APP_MODAL_V2_FLOW_NS_PARAMS_FN_DATA(app_, namespace_, title_, args_type_, handler_, user_data_, params_, ...) \
    DCC_APP_MODAL_FLOW( \
        (app_), \
        DCC_MODAL_V2_FLOW_NS_PARAMS_FN_DATA(namespace_, (title_), args_type_, (handler_), (user_data_), params_, __VA_ARGS__) \
    )

#endif /* DCC_SUGAR_COMMAND_ROUTES_MODALS_H */
