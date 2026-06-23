#ifndef DCC_SUGAR_APP_REGISTRATION_GUARDED_COMPONENTS_H
#define DCC_SUGAR_APP_REGISTRATION_GUARDED_COMPONENTS_H

#include <dcc/sugar/app_registration/guarded_commands.h>

#define DCC_APP_ON_BUTTON_GUARDED(app_, custom_id_, handler_, ...) \
    DCC_APP_ON_BUTTON_POLICY((app_), (custom_id_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_DATA_GUARDED(app_, custom_id_, handler_, user_data_, ...) \
    DCC_APP_ON_BUTTON_DATA_POLICY((app_), (custom_id_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_PREFIX_GUARDED(app_, custom_id_prefix_, handler_, ...) \
    DCC_APP_ON_BUTTON_PREFIX_POLICY((app_), (custom_id_prefix_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_PREFIX_DATA_GUARDED(app_, custom_id_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_BUTTON_PREFIX_DATA_POLICY( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_BUTTON_NS_GUARDED(app_, namespace_, action_, handler_, ...) \
    DCC_APP_ON_BUTTON_NS_POLICY((app_), namespace_, action_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_NS_DATA_GUARDED(app_, namespace_, action_, handler_, user_data_, ...) \
    DCC_APP_ON_BUTTON_NS_DATA_POLICY((app_), namespace_, action_, (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_NS_PREFIX_GUARDED(app_, namespace_, action_prefix_, handler_, ...) \
    DCC_APP_ON_BUTTON_NS_PREFIX_POLICY((app_), namespace_, action_prefix_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_BUTTON_NS_PREFIX_DATA_GUARDED(app_, namespace_, action_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_BUTTON_NS_PREFIX_DATA_POLICY( \
        (app_), \
        namespace_, \
        action_prefix_, \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_SELECT_GUARDED(app_, custom_id_, handler_, ...) \
    DCC_APP_ON_SELECT_POLICY((app_), (custom_id_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_DATA_GUARDED(app_, custom_id_, handler_, user_data_, ...) \
    DCC_APP_ON_SELECT_DATA_POLICY((app_), (custom_id_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_PREFIX_GUARDED(app_, custom_id_prefix_, handler_, ...) \
    DCC_APP_ON_SELECT_PREFIX_POLICY((app_), (custom_id_prefix_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_PREFIX_DATA_GUARDED(app_, custom_id_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_SELECT_PREFIX_DATA_POLICY( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_SELECT_NS_GUARDED(app_, namespace_, action_, handler_, ...) \
    DCC_APP_ON_SELECT_NS_POLICY((app_), namespace_, action_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_NS_DATA_GUARDED(app_, namespace_, action_, handler_, user_data_, ...) \
    DCC_APP_ON_SELECT_NS_DATA_POLICY((app_), namespace_, action_, (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_NS_PREFIX_GUARDED(app_, namespace_, action_prefix_, handler_, ...) \
    DCC_APP_ON_SELECT_NS_PREFIX_POLICY((app_), namespace_, action_prefix_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_SELECT_NS_PREFIX_DATA_GUARDED(app_, namespace_, action_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_SELECT_NS_PREFIX_DATA_POLICY( \
        (app_), \
        namespace_, \
        action_prefix_, \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_MODAL_GUARDED(app_, custom_id_, handler_, ...) \
    DCC_APP_ON_MODAL_POLICY((app_), (custom_id_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_DATA_GUARDED(app_, custom_id_, handler_, user_data_, ...) \
    DCC_APP_ON_MODAL_DATA_POLICY((app_), (custom_id_), (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_PREFIX_GUARDED(app_, custom_id_prefix_, handler_, ...) \
    DCC_APP_ON_MODAL_PREFIX_POLICY((app_), (custom_id_prefix_), (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_PREFIX_DATA_GUARDED(app_, custom_id_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_MODAL_PREFIX_DATA_POLICY( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#define DCC_APP_ON_MODAL_NS_GUARDED(app_, namespace_, action_, handler_, ...) \
    DCC_APP_ON_MODAL_NS_POLICY((app_), namespace_, action_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_NS_DATA_GUARDED(app_, namespace_, action_, handler_, user_data_, ...) \
    DCC_APP_ON_MODAL_NS_DATA_POLICY((app_), namespace_, action_, (handler_), (user_data_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_NS_PREFIX_GUARDED(app_, namespace_, action_prefix_, handler_, ...) \
    DCC_APP_ON_MODAL_NS_PREFIX_POLICY((app_), namespace_, action_prefix_, (handler_), DCC_ROUTE_POLICY(__VA_ARGS__))

#define DCC_APP_ON_MODAL_NS_PREFIX_DATA_GUARDED(app_, namespace_, action_prefix_, handler_, user_data_, ...) \
    DCC_APP_ON_MODAL_NS_PREFIX_DATA_POLICY( \
        (app_), \
        namespace_, \
        action_prefix_, \
        (handler_), \
        (user_data_), \
        DCC_ROUTE_POLICY(__VA_ARGS__) \
    )

#endif /* DCC_SUGAR_APP_REGISTRATION_GUARDED_COMPONENTS_H */
