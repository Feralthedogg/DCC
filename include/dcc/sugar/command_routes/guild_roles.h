#ifndef DCC_SUGAR_COMMAND_ROUTES_GUILD_ROLES_H
#define DCC_SUGAR_COMMAND_ROUTES_GUILD_ROLES_H

#include <dcc/sugar/command_routes/channels.h>

#define DCC_APP_GET_GUILD_ROLES(app_, guild_id_) \
    dcc_app_get_guild_roles((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_ROLES_CB(app_, guild_id_, cb_, user_data_) \
    dcc_app_get_guild_roles((app_), (guild_id_), (cb_), (user_data_))
#define DCC_APP_CREATE_GUILD_ROLE(app_, guild_id_, json_body_) \
    dcc_app_create_guild_role((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_ROLE_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    dcc_app_create_guild_role((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_CREATE_GUILD_ROLE_PARAMS(app_, params_) \
    dcc_app_create_guild_role_params((app_), (params_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_ROLE_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_create_guild_role_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_EDIT_GUILD_ROLE(app_, guild_id_, role_id_, json_body_) \
    dcc_app_modify_guild_role((app_), (guild_id_), (role_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_ROLE_CB(app_, guild_id_, role_id_, json_body_, cb_, user_data_) \
    dcc_app_modify_guild_role((app_), (guild_id_), (role_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_EDIT_GUILD_ROLE_PARAMS(app_, params_) \
    dcc_app_modify_guild_role_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_ROLE_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_modify_guild_role_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_REORDER_GUILD_ROLES(app_, guild_id_, positions_, position_count_) \
    dcc_app_modify_guild_role_positions((app_), (guild_id_), (positions_), (position_count_), NULL, NULL)
#define DCC_APP_REORDER_GUILD_ROLES_CB(app_, guild_id_, positions_, position_count_, cb_, user_data_) \
    dcc_app_modify_guild_role_positions( \
        (app_), \
        (guild_id_), \
        (positions_), \
        (position_count_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_DELETE_GUILD_ROLE(app_, guild_id_, role_id_) \
    dcc_app_delete_guild_role((app_), (guild_id_), (role_id_), NULL, NULL)
#define DCC_APP_DELETE_GUILD_ROLE_CB(app_, guild_id_, role_id_, cb_, user_data_) \
    dcc_app_delete_guild_role((app_), (guild_id_), (role_id_), (cb_), (user_data_))

#endif /* DCC_SUGAR_COMMAND_ROUTES_GUILD_ROLES_H */
