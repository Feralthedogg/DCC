#ifndef DCC_SUGAR_GUILD_SHORTCUTS_MODERATION_H
#define DCC_SUGAR_GUILD_SHORTCUTS_MODERATION_H

#include <dcc/sugar/guild_shortcuts/members.h>

#define DCC_APP_GET_GUILD_BANS(app_, guild_id_) \
    dcc_app_get_guild_bans((app_), (guild_id_), NULL, NULL, NULL)
#define DCC_APP_GET_GUILD_BANS_CB(app_, guild_id_, cb_, user_data_) \
    dcc_app_get_guild_bans((app_), (guild_id_), NULL, (cb_), (user_data_))
#define DCC_APP_GET_GUILD_BANS_QUERY(app_, guild_id_, query_) \
    dcc_app_get_guild_bans((app_), (guild_id_), (query_), NULL, NULL)
#define DCC_APP_GET_GUILD_BANS_QUERY_CB(app_, guild_id_, query_, cb_, user_data_) \
    dcc_app_get_guild_bans((app_), (guild_id_), (query_), (cb_), (user_data_))
#define DCC_APP_GET_GUILD_BANS_PAGE(app_, guild_id_, before_, after_, limit_) \
    dcc_app_get_guild_bans_page((app_), (guild_id_), (before_), (after_), (limit_), NULL, NULL)
#define DCC_APP_GET_GUILD_BANS_PAGE_CB(app_, guild_id_, before_, after_, limit_, cb_, user_data_) \
    dcc_app_get_guild_bans_page((app_), (guild_id_), (before_), (after_), (limit_), (cb_), (user_data_))
#define DCC_APP_GET_GUILD_BAN(app_, guild_id_, user_id_) \
    dcc_app_get_guild_ban((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_BAN_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_get_guild_ban((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_APP_BAN_MEMBER(app_, guild_id_, user_id_) \
    dcc_app_create_guild_ban_seconds((app_), (guild_id_), (user_id_), 0U, NULL, NULL)
#define DCC_APP_BAN_MEMBER_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_create_guild_ban_seconds((app_), (guild_id_), (user_id_), 0U, (cb_), (user_data_))
#define DCC_APP_BAN_MEMBER_SECONDS(app_, guild_id_, user_id_, delete_message_seconds_) \
    dcc_app_create_guild_ban_seconds((app_), (guild_id_), (user_id_), (delete_message_seconds_), NULL, NULL)
#define DCC_APP_BAN_MEMBER_SECONDS_CB(app_, guild_id_, user_id_, delete_message_seconds_, cb_, user_data_) \
    dcc_app_create_guild_ban_seconds( \
        (app_), \
        (guild_id_), \
        (user_id_), \
        (delete_message_seconds_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_BAN_MEMBER_JSON(app_, guild_id_, user_id_, json_body_) \
    dcc_app_create_guild_ban((app_), (guild_id_), (user_id_), (json_body_), NULL, NULL)
#define DCC_APP_BAN_MEMBER_JSON_CB(app_, guild_id_, user_id_, json_body_, cb_, user_data_) \
    dcc_app_create_guild_ban((app_), (guild_id_), (user_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_BAN_MEMBER_PARAMS(app_, params_) \
    dcc_app_create_guild_ban_params((app_), (params_), NULL, NULL)
#define DCC_APP_BAN_MEMBER_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_create_guild_ban_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_UNBAN_MEMBER(app_, guild_id_, user_id_) \
    dcc_app_delete_guild_ban((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_APP_UNBAN_MEMBER_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_delete_guild_ban((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_APP_EDIT_CURRENT_MEMBER(app_, guild_id_, json_body_) \
    dcc_app_modify_current_guild_member((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_CURRENT_MEMBER_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    dcc_app_modify_current_guild_member((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_EDIT_CURRENT_MEMBER_PARAMS(app_, params_) \
    dcc_app_modify_current_guild_member_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_CURRENT_MEMBER_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_modify_current_guild_member_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_SET_MY_NICK_JSON(app_, guild_id_, json_body_) \
    dcc_app_set_current_guild_member_nickname((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_SET_MY_NICK_JSON_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    dcc_app_set_current_guild_member_nickname((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_SET_MY_NICK_PARAMS(app_, params_) \
    dcc_app_set_current_guild_member_nickname_params((app_), (params_), NULL, NULL)
#define DCC_APP_SET_MY_NICK_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_set_current_guild_member_nickname_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_SET_MY_NICK(app_, guild_id_, nick_) \
    dcc_app_set_current_guild_member_nickname_params( \
        (app_), \
        &((dcc_guild_member_nickname_params_t[]){ { \
            .size = sizeof(dcc_guild_member_nickname_params_t), \
            .guild_id = (guild_id_), \
            .nick = (nick_) \
        } })[0], \
        NULL, \
        NULL \
    )
#define DCC_CURRENT_MEMBER_EDIT(app_, guild_id_, json_body_) \
    DCC_APP_EDIT_CURRENT_MEMBER((app_), (guild_id_), (json_body_))
#define DCC_CURRENT_MEMBER_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_CURRENT_MEMBER_PARAMS((app_), (params_))
#define DCC_CURRENT_MEMBER_SET_NICK(app_, guild_id_, nick_) \
    DCC_APP_SET_MY_NICK((app_), (guild_id_), (nick_))
#define DCC_ME_EDIT(app_, guild_id_, json_body_) \
    DCC_APP_EDIT_CURRENT_MEMBER((app_), (guild_id_), (json_body_))
#define DCC_ME_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_CURRENT_MEMBER_PARAMS((app_), (params_))
#define DCC_ME_SET_NICK(app_, guild_id_, nick_) \
    DCC_APP_SET_MY_NICK((app_), (guild_id_), (nick_))
#define DCC_APP_GET_GUILD_PRUNE_COUNT(app_, guild_id_, days_) \
    dcc_app_get_guild_prune_count_options((app_), (guild_id_), (days_), NULL, 0U, NULL, NULL)
#define DCC_APP_GET_GUILD_PRUNE_COUNT_CB(app_, guild_id_, days_, cb_, user_data_) \
    dcc_app_get_guild_prune_count_options((app_), (guild_id_), (days_), NULL, 0U, (cb_), (user_data_))
#define DCC_APP_GET_GUILD_PRUNE_COUNT_OPTIONS(app_, guild_id_, days_, include_roles_, include_role_count_) \
    dcc_app_get_guild_prune_count_options( \
        (app_), \
        (guild_id_), \
        (days_), \
        (include_roles_), \
        (include_role_count_), \
        NULL, \
        NULL \
    )
#define DCC_APP_BEGIN_GUILD_PRUNE(app_, guild_id_, days_, compute_prune_count_) \
    dcc_app_begin_guild_prune_options( \
        (app_), \
        (guild_id_), \
        (days_), \
        NULL, \
        0U, \
        (compute_prune_count_), \
        NULL, \
        NULL \
    )
#define DCC_APP_BEGIN_GUILD_PRUNE_OPTIONS(app_, guild_id_, days_, include_roles_, include_role_count_, compute_) \
    dcc_app_begin_guild_prune_options( \
        (app_), \
        (guild_id_), \
        (days_), \
        (include_roles_), \
        (include_role_count_), \
        (compute_), \
        NULL, \
        NULL \
    )
#define DCC_APP_BEGIN_GUILD_PRUNE_PARAMS(app_, params_) \
    dcc_app_begin_guild_prune_params((app_), (params_), NULL, NULL)
#define DCC_APP_BEGIN_GUILD_PRUNE_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_begin_guild_prune_params((app_), (params_), (cb_), (user_data_))
#define DCC_GUILD_BANS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_BANS((app_), (guild_id_))
#define DCC_GUILD_BANS_FETCH_PAGE(app_, guild_id_, before_, after_, limit_) \
    DCC_APP_GET_GUILD_BANS_PAGE((app_), (guild_id_), (before_), (after_), (limit_))
#define DCC_GUILD_BAN_FETCH(app_, guild_id_, user_id_) \
    DCC_APP_GET_GUILD_BAN((app_), (guild_id_), (user_id_))
#define DCC_GUILD_BAN(app_, guild_id_, user_id_) \
    DCC_APP_BAN_MEMBER((app_), (guild_id_), (user_id_))
#define DCC_GUILD_BAN_SECONDS(app_, guild_id_, user_id_, delete_message_seconds_) \
    DCC_APP_BAN_MEMBER_SECONDS((app_), (guild_id_), (user_id_), (delete_message_seconds_))
#define DCC_GUILD_BAN_PARAMS(app_, params_) \
    DCC_APP_BAN_MEMBER_PARAMS((app_), (params_))
#define DCC_GUILD_UNBAN(app_, guild_id_, user_id_) \
    DCC_APP_UNBAN_MEMBER((app_), (guild_id_), (user_id_))
#define DCC_MEMBER_BAN(app_, guild_id_, user_id_) \
    DCC_APP_BAN_MEMBER((app_), (guild_id_), (user_id_))
#define DCC_MEMBER_BAN_SECONDS(app_, guild_id_, user_id_, delete_message_seconds_) \
    DCC_APP_BAN_MEMBER_SECONDS((app_), (guild_id_), (user_id_), (delete_message_seconds_))
#define DCC_MEMBER_UNBAN(app_, guild_id_, user_id_) \
    DCC_APP_UNBAN_MEMBER((app_), (guild_id_), (user_id_))
#define DCC_GUILD_PRUNE_COUNT(app_, guild_id_, days_) \
    DCC_APP_GET_GUILD_PRUNE_COUNT((app_), (guild_id_), (days_))
#define DCC_GUILD_PRUNE_COUNT_OPTIONS(app_, guild_id_, days_, include_roles_, include_role_count_) \
    DCC_APP_GET_GUILD_PRUNE_COUNT_OPTIONS((app_), (guild_id_), (days_), (include_roles_), (include_role_count_))
#define DCC_GUILD_PRUNE_BEGIN(app_, guild_id_, days_, compute_prune_count_) \
    DCC_APP_BEGIN_GUILD_PRUNE((app_), (guild_id_), (days_), (compute_prune_count_))
#define DCC_GUILD_PRUNE_BEGIN_OPTIONS(app_, guild_id_, days_, include_roles_, include_role_count_, compute_) \
    DCC_APP_BEGIN_GUILD_PRUNE_OPTIONS( \
        (app_), \
        (guild_id_), \
        (days_), \
        (include_roles_), \
        (include_role_count_), \
        (compute_) \
    )
#define DCC_GUILD_PRUNE_BEGIN_PARAMS(app_, params_) \
    DCC_APP_BEGIN_GUILD_PRUNE_PARAMS((app_), (params_))

#endif /* DCC_SUGAR_GUILD_SHORTCUTS_MODERATION_H */
