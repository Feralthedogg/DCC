#ifndef DCC_SUGAR_APP_REGISTRATION_POLICY_COMMANDS_H
#define DCC_SUGAR_APP_REGISTRATION_POLICY_COMMANDS_H

#include <dcc/sugar/app_registration/policy_helpers.h>

#define DCC_APP_ON_SLASH_POLICY(app_, name_, description_, handler_, policy_) \
    dcc_sugar_app_on_slash_policy( \
        (app_), \
        (name_), \
        (description_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SLASH_DATA_POLICY(app_, name_, description_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_slash_policy( \
        (app_), \
        (name_), \
        (description_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SLASH_BUILDER_POLICY(app_, builder_, handler_, policy_) \
    dcc_sugar_app_on_slash_builder_policy( \
        (app_), \
        (builder_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SLASH_BUILDER_DATA_POLICY(app_, builder_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_slash_builder_policy( \
        (app_), \
        (builder_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SUBCOMMAND_POLICY(app_, command_name_, subcommand_path_, handler_, policy_) \
    dcc_sugar_app_on_subcommand_policy( \
        (app_), \
        (command_name_), \
        (subcommand_path_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_SUBCOMMAND_DATA_POLICY( \
    app_, \
    command_name_, \
    subcommand_path_, \
    handler_, \
    user_data_, \
    policy_ \
) \
    dcc_sugar_app_on_subcommand_policy( \
        (app_), \
        (command_name_), \
        (subcommand_path_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_AUTOCOMPLETE_POLICY(app_, name_, handler_, policy_) \
    dcc_sugar_app_on_autocomplete_policy( \
        (app_), \
        (name_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_AUTOCOMPLETE_DATA_POLICY(app_, name_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_autocomplete_policy( \
        (app_), \
        (name_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_USER_CONTEXT_MENU_POLICY(app_, name_, handler_, policy_) \
    dcc_sugar_app_on_user_context_menu_policy( \
        (app_), \
        (name_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_USER_CONTEXT_MENU_DATA_POLICY(app_, name_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_user_context_menu_policy( \
        (app_), \
        (name_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_POLICY(app_, name_, handler_, policy_) \
    dcc_sugar_app_on_message_context_menu_policy( \
        (app_), \
        (name_), \
        (handler_), \
        NULL, \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )
#define DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA_POLICY(app_, name_, handler_, user_data_, policy_) \
    dcc_sugar_app_on_message_context_menu_policy( \
        (app_), \
        (name_), \
        (handler_), \
        (user_data_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )

#endif /* DCC_SUGAR_APP_REGISTRATION_POLICY_COMMANDS_H */
