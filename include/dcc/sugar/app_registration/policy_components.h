#ifndef DCC_SUGAR_APP_REGISTRATION_POLICY_COMPONENTS_H
#define DCC_SUGAR_APP_REGISTRATION_POLICY_COMPONENTS_H

#include <dcc/sugar/app_registration/policy_helpers.h>

#define DCC_APP_ON_BUTTON_POLICY(app_, custom_id_, handler_, policy_) \
    dcc_sugar_app_on_button_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_BUTTON_DATA_POLICY(app_, custom_id_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_button_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_BUTTON_PREFIX_POLICY(app_, custom_id_prefix_, handler_, policy_) \
    dcc_sugar_app_on_button_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_BUTTON_PREFIX_DATA_POLICY( \
    app_, \
    custom_id_prefix_, \
    handler_, \
    user_data_, \
    policy_ \
) \
    dcc_sugar_app_on_button_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SELECT_POLICY(app_, custom_id_, handler_, policy_) \
    dcc_sugar_app_on_select_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SELECT_DATA_POLICY(app_, custom_id_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_select_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SELECT_PREFIX_POLICY(app_, custom_id_prefix_, handler_, policy_) \
    dcc_sugar_app_on_select_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SELECT_PREFIX_DATA_POLICY( \
    app_, \
    custom_id_prefix_, \
    handler_, \
    user_data_, \
    policy_ \
) \
    dcc_sugar_app_on_select_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MODAL_POLICY(app_, custom_id_, handler_, policy_) \
    dcc_sugar_app_on_modal_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MODAL_DATA_POLICY(app_, custom_id_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_modal_policy( \
        (app_), \
        (custom_id_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MODAL_PREFIX_POLICY(app_, custom_id_prefix_, handler_, policy_) \
    dcc_sugar_app_on_modal_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MODAL_PREFIX_DATA_POLICY( \
    app_, \
    custom_id_prefix_, \
    handler_, \
    user_data_, \
    policy_ \
) \
    dcc_sugar_app_on_modal_prefix_policy( \
        (app_), \
        (custom_id_prefix_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )

#endif /* DCC_SUGAR_APP_REGISTRATION_POLICY_COMPONENTS_H */
