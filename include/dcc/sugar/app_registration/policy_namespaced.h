#ifndef DCC_SUGAR_APP_REGISTRATION_POLICY_NAMESPACED_H
#define DCC_SUGAR_APP_REGISTRATION_POLICY_NAMESPACED_H

#include <dcc/sugar/app_registration/policy_commands.h>
#include <dcc/sugar/app_registration/policy_components.h>

#define DCC_APP_ON_BUTTON_NS_POLICY(app_, namespace_, action_, handler_, policy_) \
    DCC_APP_ON_BUTTON_POLICY((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_APP_ON_BUTTON_NS_DATA_POLICY(app_, namespace_, action_, handler_, user_data_, policy_) \
    DCC_APP_ON_BUTTON_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ID(namespace_, action_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )
#define DCC_APP_ON_BUTTON_NS_PREFIX_POLICY(app_, namespace_, action_prefix_, handler_, policy_) \
    DCC_APP_ON_BUTTON_PREFIX_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (policy_) \
    )
#define DCC_APP_ON_BUTTON_NS_PREFIX_DATA_POLICY(app_, namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_APP_ON_BUTTON_PREFIX_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_APP_ON_SELECT_NS_POLICY(app_, namespace_, action_, handler_, policy_) \
    DCC_APP_ON_SELECT_POLICY((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_APP_ON_SELECT_NS_DATA_POLICY(app_, namespace_, action_, handler_, user_data_, policy_) \
    DCC_APP_ON_SELECT_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ID(namespace_, action_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )
#define DCC_APP_ON_SELECT_NS_PREFIX_POLICY(app_, namespace_, action_prefix_, handler_, policy_) \
    DCC_APP_ON_SELECT_PREFIX_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (policy_) \
    )
#define DCC_APP_ON_SELECT_NS_PREFIX_DATA_POLICY(app_, namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_APP_ON_SELECT_PREFIX_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_APP_ON_MODAL_NS_POLICY(app_, namespace_, action_, handler_, policy_) \
    DCC_APP_ON_MODAL_POLICY((app_), DCC_COMPONENT_ID(namespace_, action_), (handler_), (policy_))
#define DCC_APP_ON_MODAL_NS_DATA_POLICY(app_, namespace_, action_, handler_, user_data_, policy_) \
    DCC_APP_ON_MODAL_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ID(namespace_, action_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )
#define DCC_APP_ON_MODAL_NS_PREFIX_POLICY(app_, namespace_, action_prefix_, handler_, policy_) \
    DCC_APP_ON_MODAL_PREFIX_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (policy_) \
    )
#define DCC_APP_ON_MODAL_NS_PREFIX_DATA_POLICY(app_, namespace_, action_prefix_, handler_, user_data_, policy_) \
    DCC_APP_ON_MODAL_PREFIX_DATA_POLICY( \
        (app_), \
        DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_), \
        (handler_), \
        (user_data_), \
        (policy_) \
    )

#define DCC_APP_ON_COMMAND_POLICY(app_, name_, description_, handler_, policy_) \
    DCC_APP_ON_SLASH_POLICY((app_), (name_), (description_), (handler_), (policy_))

#define DCC_APP_ON_COMMAND_DATA_POLICY(app_, name_, description_, handler_, user_data_, policy_) \
    DCC_APP_ON_SLASH_DATA_POLICY((app_), (name_), (description_), (handler_), (user_data_), (policy_))

#define DCC_APP_ON_COMMAND_BUILDER_POLICY(app_, builder_, handler_, policy_) \
    DCC_APP_ON_SLASH_BUILDER_POLICY((app_), (builder_), (handler_), (policy_))

#define DCC_APP_ON_COMMAND_BUILDER_DATA_POLICY(app_, builder_, handler_, user_data_, policy_) \
    DCC_APP_ON_SLASH_BUILDER_DATA_POLICY((app_), (builder_), (handler_), (user_data_), (policy_))

#endif /* DCC_SUGAR_APP_REGISTRATION_POLICY_NAMESPACED_H */
