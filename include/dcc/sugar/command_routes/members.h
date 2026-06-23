#ifndef DCC_SUGAR_COMMAND_ROUTES_MEMBERS_H
#define DCC_SUGAR_COMMAND_ROUTES_MEMBERS_H

#include <dcc/sugar/command_routes/guild_roles.h>

#define DCC_APP_ADD_MEMBER_ROLE(app_, guild_id_, user_id_, role_id_) \
    dcc_app_add_member_role((app_), (guild_id_), (user_id_), (role_id_), NULL, NULL)
#define DCC_APP_ADD_MEMBER_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    dcc_app_add_member_role((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_APP_REMOVE_MEMBER_ROLE(app_, guild_id_, user_id_, role_id_) \
    dcc_app_remove_member_role((app_), (guild_id_), (user_id_), (role_id_), NULL, NULL)
#define DCC_APP_REMOVE_MEMBER_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    dcc_app_remove_member_role((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_MEMBER_ADD_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_APP_ADD_MEMBER_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_MEMBER_ADD_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_APP_ADD_MEMBER_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_MEMBER_REMOVE_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_APP_REMOVE_MEMBER_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_MEMBER_REMOVE_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_APP_REMOVE_MEMBER_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_ADD_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_MEMBER_ADD_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_GUILD_MEMBER_ADD_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_MEMBER_ADD_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_REMOVE_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_MEMBER_REMOVE_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_GUILD_MEMBER_REMOVE_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_MEMBER_REMOVE_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_MEMBER_FETCH(app_, guild_id_, user_id_) \
    dcc_app_get_member((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_MEMBER_FETCH_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_get_member((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_MEMBER_TIMEOUT(app_, guild_id_, user_id_, until_) \
    dcc_app_set_member_timeout((app_), (guild_id_), (user_id_), (until_), NULL, NULL)
#define DCC_MEMBER_TIMEOUT_CB(app_, guild_id_, user_id_, until_, cb_, user_data_) \
    dcc_app_set_member_timeout((app_), (guild_id_), (user_id_), (until_), (cb_), (user_data_))
#define DCC_MEMBER_CLEAR_TIMEOUT(app_, guild_id_, user_id_) \
    dcc_app_clear_member_timeout((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_MEMBER_CLEAR_TIMEOUT_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_clear_member_timeout((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_MEMBER_MOVE(app_, guild_id_, user_id_, channel_id_) \
    dcc_app_move_member((app_), (guild_id_), (user_id_), (channel_id_), NULL, NULL)
#define DCC_MEMBER_MOVE_CB(app_, guild_id_, user_id_, channel_id_, cb_, user_data_) \
    dcc_app_move_member((app_), (guild_id_), (user_id_), (channel_id_), (cb_), (user_data_))
#define DCC_MEMBER_KICK(app_, guild_id_, user_id_) \
    dcc_app_remove_member((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_MEMBER_KICK_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_remove_member((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_FETCH(app_, guild_id_, user_id_) \
    DCC_MEMBER_FETCH((app_), (guild_id_), (user_id_))
#define DCC_GUILD_MEMBER_FETCH_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    DCC_MEMBER_FETCH_CB((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_TIMEOUT(app_, guild_id_, user_id_, until_) \
    DCC_MEMBER_TIMEOUT((app_), (guild_id_), (user_id_), (until_))
#define DCC_GUILD_MEMBER_TIMEOUT_CB(app_, guild_id_, user_id_, until_, cb_, user_data_) \
    DCC_MEMBER_TIMEOUT_CB((app_), (guild_id_), (user_id_), (until_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_CLEAR_TIMEOUT(app_, guild_id_, user_id_) \
    DCC_MEMBER_CLEAR_TIMEOUT((app_), (guild_id_), (user_id_))
#define DCC_GUILD_MEMBER_CLEAR_TIMEOUT_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    DCC_MEMBER_CLEAR_TIMEOUT_CB((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_MOVE(app_, guild_id_, user_id_, channel_id_) \
    DCC_MEMBER_MOVE((app_), (guild_id_), (user_id_), (channel_id_))
#define DCC_GUILD_MEMBER_MOVE_CB(app_, guild_id_, user_id_, channel_id_, cb_, user_data_) \
    DCC_MEMBER_MOVE_CB((app_), (guild_id_), (user_id_), (channel_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_KICK(app_, guild_id_, user_id_) \
    DCC_MEMBER_KICK((app_), (guild_id_), (user_id_))
#define DCC_GUILD_MEMBER_KICK_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    DCC_MEMBER_KICK_CB((app_), (guild_id_), (user_id_), (cb_), (user_data_))

#endif /* DCC_SUGAR_COMMAND_ROUTES_MEMBERS_H */
