/* Included by <dcc/sugar.h>. */
#define DCC_APP_INFER_GUILD_FROM_CHANNEL(app_, channel_id_, cb_, user_data_) \
    dcc_app_infer_guild_id_from_channel((app_), (channel_id_), (cb_), (user_data_))
#define DCC_INFER_GUILD_FROM_CHANNEL(app_, channel_id_, cb_, user_data_) \
    DCC_APP_INFER_GUILD_FROM_CHANNEL((app_), (channel_id_), (cb_), (user_data_))

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
#define DCC_APP_GET_GUILD_AUDIT_LOG(app_, guild_id_) \
    dcc_app_get_guild_audit_log((app_), (guild_id_), NULL, NULL, NULL)
#define DCC_APP_GET_GUILD_AUDIT_LOG_QUERY(app_, guild_id_, query_) \
    dcc_app_get_guild_audit_log((app_), (guild_id_), (query_), NULL, NULL)
#define DCC_APP_GET_GUILD_AUDIT_LOG_PAGE(app_, guild_id_, user_id_, action_type_, before_, after_, limit_) \
    dcc_app_get_guild_audit_log_page( \
        (app_), \
        (guild_id_), \
        (user_id_), \
        (action_type_), \
        (before_), \
        (after_), \
        (limit_), \
        NULL, \
        NULL \
    )
#define DCC_APP_GET_GUILD_INTEGRATIONS(app_, guild_id_) \
    dcc_app_get_guild_integrations((app_), (guild_id_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_INTEGRATION(app_, guild_id_, integration_id_, json_body_) \
    dcc_app_modify_guild_integration((app_), (guild_id_), (integration_id_), (json_body_), NULL, NULL)
#define DCC_APP_DELETE_GUILD_INTEGRATION(app_, guild_id_, integration_id_) \
    dcc_app_delete_guild_integration((app_), (guild_id_), (integration_id_), NULL, NULL)
#define DCC_APP_SYNC_GUILD_INTEGRATION(app_, guild_id_, integration_id_) \
    dcc_app_sync_guild_integration((app_), (guild_id_), (integration_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_WIDGET(app_, guild_id_) \
    dcc_app_get_guild_widget((app_), (guild_id_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_WIDGET(app_, guild_id_, json_body_) \
    dcc_app_modify_guild_widget((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_WIDGET_PARAMS(app_, guild_id_, params_) \
    dcc_app_modify_guild_widget_params((app_), (guild_id_), (params_), NULL, NULL)
#define DCC_APP_GET_GUILD_VANITY_URL(app_, guild_id_) \
    dcc_app_get_guild_vanity_url((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_ONBOARDING(app_, guild_id_) \
    dcc_app_get_guild_onboarding((app_), (guild_id_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_ONBOARDING(app_, guild_id_, json_body_) \
    dcc_app_modify_guild_onboarding((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_ONBOARDING_PARAMS(app_, params_) \
    dcc_app_modify_guild_onboarding_params((app_), (params_), NULL, NULL)
#define DCC_APP_GET_GUILD_WELCOME_SCREEN(app_, guild_id_) \
    dcc_app_get_guild_welcome_screen((app_), (guild_id_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_WELCOME_SCREEN(app_, guild_id_, json_body_) \
    dcc_app_modify_guild_welcome_screen((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_WELCOME_SCREEN_PARAMS(app_, guild_id_, params_) \
    dcc_app_modify_guild_welcome_screen_params((app_), (guild_id_), (params_), NULL, NULL)
#define DCC_APP_GET_AUTO_MOD_RULES(app_, guild_id_) \
    dcc_app_get_auto_moderation_rules((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_AUTO_MOD_RULE(app_, guild_id_, rule_id_) \
    dcc_app_get_auto_moderation_rule((app_), (guild_id_), (rule_id_), NULL, NULL)
#define DCC_APP_CREATE_AUTO_MOD_RULE(app_, guild_id_, json_body_) \
    dcc_app_create_auto_moderation_rule((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_AUTO_MOD_RULE_PARAMS(app_, guild_id_, params_) \
    dcc_app_create_auto_moderation_rule_params((app_), (guild_id_), (params_), NULL, NULL)
#define DCC_APP_EDIT_AUTO_MOD_RULE(app_, guild_id_, rule_id_, json_body_) \
    dcc_app_modify_auto_moderation_rule((app_), (guild_id_), (rule_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_AUTO_MOD_RULE_PARAMS(app_, guild_id_, params_) \
    dcc_app_modify_auto_moderation_rule_params((app_), (guild_id_), (params_), NULL, NULL)
#define DCC_APP_DELETE_AUTO_MOD_RULE(app_, guild_id_, rule_id_) \
    dcc_app_delete_auto_moderation_rule((app_), (guild_id_), (rule_id_), NULL, NULL)
#define DCC_GUILD_AUDIT_LOG_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_AUDIT_LOG((app_), (guild_id_))
#define DCC_GUILD_AUDIT_LOG_FETCH_QUERY(app_, guild_id_, query_) \
    DCC_APP_GET_GUILD_AUDIT_LOG_QUERY((app_), (guild_id_), (query_))
#define DCC_GUILD_AUDIT_LOG_FETCH_PAGE(app_, guild_id_, user_id_, action_type_, before_, after_, limit_) \
    DCC_APP_GET_GUILD_AUDIT_LOG_PAGE( \
        (app_), \
        (guild_id_), \
        (user_id_), \
        (action_type_), \
        (before_), \
        (after_), \
        (limit_) \
    )
#define DCC_GUILD_INTEGRATIONS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_INTEGRATIONS((app_), (guild_id_))
#define DCC_GUILD_INTEGRATION_EDIT(app_, guild_id_, integration_id_, json_body_) \
    DCC_APP_EDIT_GUILD_INTEGRATION((app_), (guild_id_), (integration_id_), (json_body_))
#define DCC_GUILD_INTEGRATION_DELETE(app_, guild_id_, integration_id_) \
    DCC_APP_DELETE_GUILD_INTEGRATION((app_), (guild_id_), (integration_id_))
#define DCC_GUILD_INTEGRATION_SYNC(app_, guild_id_, integration_id_) \
    DCC_APP_SYNC_GUILD_INTEGRATION((app_), (guild_id_), (integration_id_))
#define DCC_GUILD_WIDGET_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_WIDGET((app_), (guild_id_))
#define DCC_GUILD_WIDGET_EDIT(app_, guild_id_, json_body_) \
    DCC_APP_EDIT_GUILD_WIDGET((app_), (guild_id_), (json_body_))
#define DCC_GUILD_WIDGET_EDIT_PARAMS(app_, guild_id_, params_) \
    DCC_APP_EDIT_GUILD_WIDGET_PARAMS((app_), (guild_id_), (params_))
#define DCC_GUILD_VANITY_URL_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_VANITY_URL((app_), (guild_id_))
#define DCC_GUILD_ONBOARDING_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_ONBOARDING((app_), (guild_id_))
#define DCC_GUILD_ONBOARDING_EDIT(app_, guild_id_, json_body_) \
    DCC_APP_EDIT_GUILD_ONBOARDING((app_), (guild_id_), (json_body_))
#define DCC_GUILD_ONBOARDING_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_GUILD_ONBOARDING_PARAMS((app_), (params_))
#define DCC_GUILD_WELCOME_SCREEN_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_WELCOME_SCREEN((app_), (guild_id_))
#define DCC_GUILD_WELCOME_SCREEN_EDIT(app_, guild_id_, json_body_) \
    DCC_APP_EDIT_GUILD_WELCOME_SCREEN((app_), (guild_id_), (json_body_))
#define DCC_GUILD_WELCOME_SCREEN_EDIT_PARAMS(app_, guild_id_, params_) \
    DCC_APP_EDIT_GUILD_WELCOME_SCREEN_PARAMS((app_), (guild_id_), (params_))
#define DCC_AUTO_MOD_RULES_FETCH(app_, guild_id_) \
    DCC_APP_GET_AUTO_MOD_RULES((app_), (guild_id_))
#define DCC_AUTO_MODERATION_RULES_FETCH(app_, guild_id_) \
    DCC_APP_GET_AUTO_MOD_RULES((app_), (guild_id_))
#define DCC_AUTO_MOD_RULE_FETCH(app_, guild_id_, rule_id_) \
    DCC_APP_GET_AUTO_MOD_RULE((app_), (guild_id_), (rule_id_))
#define DCC_AUTO_MODERATION_RULE_FETCH(app_, guild_id_, rule_id_) \
    DCC_APP_GET_AUTO_MOD_RULE((app_), (guild_id_), (rule_id_))
#define DCC_AUTO_MOD_RULE_CREATE(app_, guild_id_, json_body_) \
    DCC_APP_CREATE_AUTO_MOD_RULE((app_), (guild_id_), (json_body_))
#define DCC_AUTO_MOD_RULE_CREATE_PARAMS(app_, guild_id_, params_) \
    DCC_APP_CREATE_AUTO_MOD_RULE_PARAMS((app_), (guild_id_), (params_))
#define DCC_AUTO_MOD_RULE_EDIT(app_, guild_id_, rule_id_, json_body_) \
    DCC_APP_EDIT_AUTO_MOD_RULE((app_), (guild_id_), (rule_id_), (json_body_))
#define DCC_AUTO_MOD_RULE_EDIT_PARAMS(app_, guild_id_, params_) \
    DCC_APP_EDIT_AUTO_MOD_RULE_PARAMS((app_), (guild_id_), (params_))
#define DCC_AUTO_MOD_RULE_DELETE(app_, guild_id_, rule_id_) \
    DCC_APP_DELETE_AUTO_MOD_RULE((app_), (guild_id_), (rule_id_))
#define DCC_APP_GET_GUILD_EMOJIS(app_, guild_id_) \
    dcc_app_get_guild_emojis((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_EMOJI(app_, guild_id_, emoji_id_) \
    dcc_app_get_guild_emoji((app_), (guild_id_), (emoji_id_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_EMOJI(app_, guild_id_, json_body_) \
    dcc_app_create_guild_emoji((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_EMOJI_PARAMS(app_, guild_id_, params_) \
    dcc_app_create_guild_emoji_params((app_), (guild_id_), (params_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_EMOJI(app_, guild_id_, emoji_id_, json_body_) \
    dcc_app_modify_guild_emoji((app_), (guild_id_), (emoji_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_EMOJI_PARAMS(app_, guild_id_, params_) \
    dcc_app_modify_guild_emoji_params((app_), (guild_id_), (params_), NULL, NULL)
#define DCC_APP_DELETE_GUILD_EMOJI(app_, guild_id_, emoji_id_) \
    dcc_app_delete_guild_emoji((app_), (guild_id_), (emoji_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_STICKERS(app_, guild_id_) \
    dcc_app_get_guild_stickers((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_STICKER(app_, guild_id_, sticker_id_) \
    dcc_app_get_guild_sticker((app_), (guild_id_), (sticker_id_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_STICKER_MULTIPART(app_, guild_id_, fields_, field_count_, file_) \
    dcc_app_create_guild_sticker_multipart( \
        (app_), \
        (guild_id_), \
        (fields_), \
        (field_count_), \
        (file_), \
        NULL, \
        NULL \
    )
#define DCC_APP_CREATE_GUILD_STICKER_PARAMS(app_, params_) \
    dcc_app_create_guild_sticker_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_STICKER(app_, guild_id_, sticker_id_, json_body_) \
    dcc_app_modify_guild_sticker((app_), (guild_id_), (sticker_id_), (json_body_), NULL, NULL)
#define DCC_APP_DELETE_GUILD_STICKER(app_, guild_id_, sticker_id_) \
    dcc_app_delete_guild_sticker((app_), (guild_id_), (sticker_id_), NULL, NULL)
#define DCC_APP_GET_STICKER(app_, sticker_id_) \
    dcc_app_get_sticker((app_), (sticker_id_), NULL, NULL)
#define DCC_APP_GET_STICKER_PACKS(app_) \
    dcc_app_get_sticker_packs((app_), NULL, NULL)
#define DCC_APP_GUILD_MESSAGE_SEARCH(app_, guild_id_, query_) \
    dcc_app_guild_message_search((app_), (guild_id_), (query_), NULL, NULL)
#define DCC_APP_GUILD_MESSAGE_SEARCH_PARAMS(app_, guild_id_, params_) \
    dcc_app_guild_message_search_params((app_), (guild_id_), (params_), NULL, NULL)
#define DCC_GUILD_EMOJIS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_EMOJIS((app_), (guild_id_))
#define DCC_GUILD_EMOJI_FETCH(app_, guild_id_, emoji_id_) \
    DCC_APP_GET_GUILD_EMOJI((app_), (guild_id_), (emoji_id_))
#define DCC_GUILD_EMOJI_CREATE(app_, guild_id_, json_body_) \
    DCC_APP_CREATE_GUILD_EMOJI((app_), (guild_id_), (json_body_))
#define DCC_GUILD_EMOJI_CREATE_PARAMS(app_, guild_id_, params_) \
    DCC_APP_CREATE_GUILD_EMOJI_PARAMS((app_), (guild_id_), (params_))
#define DCC_GUILD_EMOJI_EDIT(app_, guild_id_, emoji_id_, json_body_) \
    DCC_APP_EDIT_GUILD_EMOJI((app_), (guild_id_), (emoji_id_), (json_body_))
#define DCC_GUILD_EMOJI_EDIT_PARAMS(app_, guild_id_, params_) \
    DCC_APP_EDIT_GUILD_EMOJI_PARAMS((app_), (guild_id_), (params_))
#define DCC_GUILD_EMOJI_DELETE(app_, guild_id_, emoji_id_) \
    DCC_APP_DELETE_GUILD_EMOJI((app_), (guild_id_), (emoji_id_))
#define DCC_GUILD_STICKERS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_STICKERS((app_), (guild_id_))
#define DCC_GUILD_STICKER_FETCH(app_, guild_id_, sticker_id_) \
    DCC_APP_GET_GUILD_STICKER((app_), (guild_id_), (sticker_id_))
#define DCC_GUILD_STICKER_CREATE_MULTIPART(app_, guild_id_, fields_, field_count_, file_) \
    DCC_APP_CREATE_GUILD_STICKER_MULTIPART((app_), (guild_id_), (fields_), (field_count_), (file_))
#define DCC_GUILD_STICKER_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_STICKER_PARAMS((app_), (params_))
#define DCC_GUILD_STICKER_EDIT(app_, guild_id_, sticker_id_, json_body_) \
    DCC_APP_EDIT_GUILD_STICKER((app_), (guild_id_), (sticker_id_), (json_body_))
#define DCC_GUILD_STICKER_DELETE(app_, guild_id_, sticker_id_) \
    DCC_APP_DELETE_GUILD_STICKER((app_), (guild_id_), (sticker_id_))
#define DCC_STICKER_FETCH(app_, sticker_id_) \
    DCC_APP_GET_STICKER((app_), (sticker_id_))
#define DCC_STICKER_PACKS_FETCH(app_) \
    DCC_APP_GET_STICKER_PACKS((app_))
#define DCC_GUILD_MESSAGES_SEARCH(app_, guild_id_, query_) \
    DCC_APP_GUILD_MESSAGE_SEARCH((app_), (guild_id_), (query_))
#define DCC_GUILD_MESSAGES_SEARCH_PARAMS(app_, guild_id_, params_) \
    DCC_APP_GUILD_MESSAGE_SEARCH_PARAMS((app_), (guild_id_), (params_))
#define DCC_APP_GET_GUILD_SCHEDULED_EVENTS(app_, guild_id_) \
    dcc_app_get_guild_scheduled_events((app_), (guild_id_), NULL, NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENTS_QUERY(app_, guild_id_, query_) \
    dcc_app_get_guild_scheduled_events((app_), (guild_id_), (query_), NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENTS_WITH_USER_COUNT(app_, guild_id_) \
    dcc_app_get_guild_scheduled_events_with_user_count((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT(app_, guild_id_, event_id_) \
    dcc_app_get_guild_scheduled_event((app_), (guild_id_), (event_id_), NULL, NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT_QUERY(app_, guild_id_, event_id_, query_) \
    dcc_app_get_guild_scheduled_event((app_), (guild_id_), (event_id_), (query_), NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT_WITH_USER_COUNT(app_, guild_id_, event_id_) \
    dcc_app_get_guild_scheduled_event_with_user_count((app_), (guild_id_), (event_id_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_SCHEDULED_EVENT(app_, guild_id_, json_body_) \
    dcc_app_create_guild_scheduled_event((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_SCHEDULED_EVENT_PARAMS(app_, params_) \
    dcc_app_create_guild_scheduled_event_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_SCHEDULED_EVENT(app_, guild_id_, event_id_, json_body_) \
    dcc_app_modify_guild_scheduled_event((app_), (guild_id_), (event_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_SCHEDULED_EVENT_PARAMS(app_, params_) \
    dcc_app_modify_guild_scheduled_event_params((app_), (params_), NULL, NULL)
#define DCC_APP_DELETE_GUILD_SCHEDULED_EVENT(app_, guild_id_, event_id_) \
    dcc_app_delete_guild_scheduled_event((app_), (guild_id_), (event_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS(app_, guild_id_, event_id_) \
    dcc_app_get_guild_scheduled_event_users((app_), (guild_id_), (event_id_), NULL, NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS_QUERY(app_, guild_id_, event_id_, query_) \
    dcc_app_get_guild_scheduled_event_users((app_), (guild_id_), (event_id_), (query_), NULL, NULL)
#define DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS_PAGE(app_, guild_id_, event_id_, limit_, before_, after_) \
    dcc_app_get_guild_scheduled_event_users_page( \
        (app_), \
        (guild_id_), \
        (event_id_), \
        (limit_), \
        (before_), \
        (after_), \
        NULL, \
        NULL \
    )
#define DCC_GUILD_SCHEDULED_EVENTS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENTS((app_), (guild_id_))
#define DCC_GUILD_SCHEDULED_EVENTS_FETCH_WITH_USER_COUNT(app_, guild_id_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENTS_WITH_USER_COUNT((app_), (guild_id_))
#define DCC_SCHEDULED_EVENT_FETCH(app_, guild_id_, event_id_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENT((app_), (guild_id_), (event_id_))
#define DCC_SCHEDULED_EVENT_FETCH_WITH_USER_COUNT(app_, guild_id_, event_id_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENT_WITH_USER_COUNT((app_), (guild_id_), (event_id_))
#define DCC_SCHEDULED_EVENT_CREATE(app_, guild_id_, json_body_) \
    DCC_APP_CREATE_GUILD_SCHEDULED_EVENT((app_), (guild_id_), (json_body_))
#define DCC_SCHEDULED_EVENT_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_SCHEDULED_EVENT_PARAMS((app_), (params_))
#define DCC_SCHEDULED_EVENT_EDIT(app_, guild_id_, event_id_, json_body_) \
    DCC_APP_EDIT_GUILD_SCHEDULED_EVENT((app_), (guild_id_), (event_id_), (json_body_))
#define DCC_SCHEDULED_EVENT_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_GUILD_SCHEDULED_EVENT_PARAMS((app_), (params_))
#define DCC_SCHEDULED_EVENT_DELETE(app_, guild_id_, event_id_) \
    DCC_APP_DELETE_GUILD_SCHEDULED_EVENT((app_), (guild_id_), (event_id_))
#define DCC_SCHEDULED_EVENT_USERS_FETCH(app_, guild_id_, event_id_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS((app_), (guild_id_), (event_id_))
#define DCC_SCHEDULED_EVENT_USERS_FETCH_PAGE(app_, guild_id_, event_id_, limit_, before_, after_) \
    DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS_PAGE((app_), (guild_id_), (event_id_), (limit_), (before_), (after_))
#define DCC_APP_GET_CHANNEL_WEBHOOKS(app_, channel_id_) \
    dcc_app_get_channel_webhooks((app_), (channel_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_WEBHOOKS(app_, guild_id_) \
    dcc_app_get_guild_webhooks((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_WEBHOOK(app_, webhook_id_) \
    dcc_app_get_webhook((app_), (webhook_id_), NULL, NULL)
#define DCC_APP_GET_WEBHOOK_TOKEN(app_, webhook_id_, webhook_token_) \
    dcc_app_get_webhook_with_token((app_), (webhook_id_), (webhook_token_), NULL, NULL)
#define DCC_APP_CREATE_WEBHOOK(app_, channel_id_, json_body_) \
    dcc_app_create_webhook((app_), (channel_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_WEBHOOK_PARAMS(app_, params_) \
    dcc_app_create_webhook_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_WEBHOOK(app_, webhook_id_, json_body_) \
    dcc_app_modify_webhook((app_), (webhook_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_WEBHOOK_PARAMS(app_, params_) \
    dcc_app_modify_webhook_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_WEBHOOK_TOKEN(app_, webhook_id_, webhook_token_, json_body_) \
    dcc_app_modify_webhook_with_token((app_), (webhook_id_), (webhook_token_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_WEBHOOK_TOKEN_PARAMS(app_, params_) \
    dcc_app_modify_webhook_with_token_params((app_), (params_), NULL, NULL)
#define DCC_APP_DELETE_WEBHOOK(app_, webhook_id_) \
    dcc_app_delete_webhook((app_), (webhook_id_), NULL, NULL)
#define DCC_APP_DELETE_WEBHOOK_TOKEN(app_, webhook_id_, webhook_token_) \
    dcc_app_delete_webhook_with_token((app_), (webhook_id_), (webhook_token_), NULL, NULL)
#define DCC_APP_EXECUTE_WEBHOOK(app_, webhook_id_, webhook_token_, query_, json_body_) \
    dcc_app_execute_webhook((app_), (webhook_id_), (webhook_token_), (query_), (json_body_), NULL, NULL)
#define DCC_APP_EXECUTE_WEBHOOK_OPTIONS( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    wait_, \
    thread_id_, \
    with_components_, \
    json_body_ \
) \
    dcc_app_execute_webhook_options( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (wait_), \
        (thread_id_), \
        (with_components_), \
        (json_body_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EXECUTE_WEBHOOK_MESSAGE( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    wait_, \
    thread_id_, \
    with_components_, \
    message_ \
) \
    dcc_app_execute_webhook_message( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (wait_), \
        (thread_id_), \
        (with_components_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        NULL, \
        NULL \
    )
#define DCC_APP_EXECUTE_WEBHOOK_TEXT(app_, webhook_id_, webhook_token_, wait_, thread_id_, content_) \
    dcc_app_execute_webhook_text( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (wait_), \
        (thread_id_), \
        (content_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EXECUTE_WEBHOOK_MULTIPART( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    query_, \
    payload_json_, \
    files_, \
    file_count_ \
) \
    dcc_app_execute_webhook_multipart( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (query_), \
        (payload_json_), \
        (files_), \
        (file_count_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EXECUTE_WEBHOOK_MULTIPART_OPTIONS( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    wait_, \
    thread_id_, \
    with_components_, \
    payload_json_, \
    files_, \
    file_count_ \
) \
    dcc_app_execute_webhook_multipart_options( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (wait_), \
        (thread_id_), \
        (with_components_), \
        (payload_json_), \
        (files_), \
        (file_count_), \
        NULL, \
        NULL \
    )
#define DCC_APP_GET_WEBHOOK_MESSAGE(app_, webhook_id_, webhook_token_, message_id_) \
    dcc_app_get_webhook_message((app_), (webhook_id_), (webhook_token_), (message_id_), NULL, NULL, NULL)
#define DCC_APP_GET_WEBHOOK_MESSAGE_QUERY(app_, webhook_id_, webhook_token_, message_id_, query_) \
    dcc_app_get_webhook_message((app_), (webhook_id_), (webhook_token_), (message_id_), (query_), NULL, NULL)
#define DCC_APP_GET_WEBHOOK_MESSAGE_THREAD(app_, webhook_id_, webhook_token_, message_id_, thread_id_) \
    dcc_app_get_webhook_message_thread( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE(app_, webhook_id_, webhook_token_, message_id_, json_body_) \
    dcc_app_modify_webhook_message( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        NULL, \
        (json_body_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_QUERY(app_, webhook_id_, webhook_token_, message_id_, query_, json_body_) \
    dcc_app_modify_webhook_message( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (query_), \
        (json_body_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_THREAD( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    message_id_, \
    thread_id_, \
    json_body_ \
) \
    dcc_app_modify_webhook_message_thread( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        (json_body_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_BUILDER(app_, webhook_id_, webhook_token_, message_id_, message_) \
    dcc_app_modify_webhook_message_builder( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        NULL, \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_BUILDER_THREAD( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    message_id_, \
    thread_id_, \
    message_ \
) \
    dcc_app_modify_webhook_message_builder_thread( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_MULTIPART( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    message_id_, \
    query_, \
    payload_json_, \
    files_, \
    file_count_ \
) \
    dcc_app_modify_webhook_message_multipart( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (query_), \
        (payload_json_), \
        (files_), \
        (file_count_), \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_WEBHOOK_MESSAGE_MULTIPART_THREAD( \
    app_, \
    webhook_id_, \
    webhook_token_, \
    message_id_, \
    thread_id_, \
    payload_json_, \
    files_, \
    file_count_ \
) \
    dcc_app_modify_webhook_message_multipart_thread( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        (payload_json_), \
        (files_), \
        (file_count_), \
        NULL, \
        NULL \
    )
#define DCC_APP_DELETE_WEBHOOK_MESSAGE(app_, webhook_id_, webhook_token_, message_id_) \
    dcc_app_delete_webhook_message((app_), (webhook_id_), (webhook_token_), (message_id_), NULL, NULL, NULL)
#define DCC_APP_DELETE_WEBHOOK_MESSAGE_QUERY(app_, webhook_id_, webhook_token_, message_id_, query_) \
    dcc_app_delete_webhook_message((app_), (webhook_id_), (webhook_token_), (message_id_), (query_), NULL, NULL)
#define DCC_APP_DELETE_WEBHOOK_MESSAGE_THREAD(app_, webhook_id_, webhook_token_, message_id_, thread_id_) \
    dcc_app_delete_webhook_message_thread( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        NULL, \
        NULL \
    )
#define DCC_CHANNEL_WEBHOOKS_FETCH(app_, channel_id_) \
    DCC_APP_GET_CHANNEL_WEBHOOKS((app_), (channel_id_))
#define DCC_GUILD_WEBHOOKS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_WEBHOOKS((app_), (guild_id_))
#define DCC_WEBHOOK_FETCH(app_, webhook_id_) \
    DCC_APP_GET_WEBHOOK((app_), (webhook_id_))
#define DCC_WEBHOOK_FETCH_TOKEN(app_, webhook_id_, webhook_token_) \
    DCC_APP_GET_WEBHOOK_TOKEN((app_), (webhook_id_), (webhook_token_))
#define DCC_WEBHOOK_CREATE(app_, channel_id_, json_body_) \
    DCC_APP_CREATE_WEBHOOK((app_), (channel_id_), (json_body_))
#define DCC_WEBHOOK_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_WEBHOOK_PARAMS((app_), (params_))
#define DCC_WEBHOOK_EDIT(app_, webhook_id_, json_body_) \
    DCC_APP_EDIT_WEBHOOK((app_), (webhook_id_), (json_body_))
#define DCC_WEBHOOK_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_WEBHOOK_PARAMS((app_), (params_))
#define DCC_WEBHOOK_EDIT_TOKEN(app_, webhook_id_, webhook_token_, json_body_) \
    DCC_APP_EDIT_WEBHOOK_TOKEN((app_), (webhook_id_), (webhook_token_), (json_body_))
#define DCC_WEBHOOK_EDIT_TOKEN_PARAMS(app_, params_) \
    DCC_APP_EDIT_WEBHOOK_TOKEN_PARAMS((app_), (params_))
#define DCC_WEBHOOK_DELETE(app_, webhook_id_) \
    DCC_APP_DELETE_WEBHOOK((app_), (webhook_id_))
#define DCC_WEBHOOK_DELETE_TOKEN(app_, webhook_id_, webhook_token_) \
    DCC_APP_DELETE_WEBHOOK_TOKEN((app_), (webhook_id_), (webhook_token_))
#define DCC_WEBHOOK_SEND_JSON(app_, webhook_id_, webhook_token_, json_body_) \
    DCC_APP_EXECUTE_WEBHOOK((app_), (webhook_id_), (webhook_token_), NULL, (json_body_))
#define DCC_WEBHOOK_SEND_JSON_QUERY(app_, webhook_id_, webhook_token_, query_, json_body_) \
    DCC_APP_EXECUTE_WEBHOOK((app_), (webhook_id_), (webhook_token_), (query_), (json_body_))
#define DCC_WEBHOOK_SEND_OPTIONS(app_, webhook_id_, webhook_token_, wait_, thread_id_, with_components_, json_body_) \
    DCC_APP_EXECUTE_WEBHOOK_OPTIONS( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (wait_), \
        (thread_id_), \
        (with_components_), \
        (json_body_) \
    )
#define DCC_WEBHOOK_SEND(app_, webhook_id_, webhook_token_, message_) \
    DCC_APP_EXECUTE_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), 0U, 0U, 0U, (message_))
#define DCC_WEBHOOK_SEND_WAIT(app_, webhook_id_, webhook_token_, message_) \
    DCC_APP_EXECUTE_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), 1U, 0U, 0U, (message_))
#define DCC_WEBHOOK_SEND_THREAD(app_, webhook_id_, webhook_token_, thread_id_, message_) \
    DCC_APP_EXECUTE_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), 0U, (thread_id_), 0U, (message_))
#define DCC_WEBHOOK_SEND_TEXT(app_, webhook_id_, webhook_token_, content_) \
    DCC_APP_EXECUTE_WEBHOOK_TEXT((app_), (webhook_id_), (webhook_token_), 0U, 0U, (content_))
#define DCC_WEBHOOK_SEND_TEXT_WAIT(app_, webhook_id_, webhook_token_, content_) \
    DCC_APP_EXECUTE_WEBHOOK_TEXT((app_), (webhook_id_), (webhook_token_), 1U, 0U, (content_))
#define DCC_WEBHOOK_SEND_UI(app_, webhook_id_, webhook_token_, ...) \
    DCC_APP_EXECUTE_WEBHOOK_MESSAGE( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        0U, \
        0U, \
        1U, \
        DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__) \
    )
#define DCC_WEBHOOK_SEND_UI_WAIT(app_, webhook_id_, webhook_token_, ...) \
    DCC_APP_EXECUTE_WEBHOOK_MESSAGE( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        1U, \
        0U, \
        1U, \
        DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__) \
    )
#define DCC_WEBHOOK_MESSAGE_FETCH(app_, webhook_id_, webhook_token_, message_id_) \
    DCC_APP_GET_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), (message_id_))
#define DCC_WEBHOOK_MESSAGE_FETCH_THREAD(app_, webhook_id_, webhook_token_, message_id_, thread_id_) \
    DCC_APP_GET_WEBHOOK_MESSAGE_THREAD((app_), (webhook_id_), (webhook_token_), (message_id_), (thread_id_))
#define DCC_WEBHOOK_MESSAGE_EDIT_JSON(app_, webhook_id_, webhook_token_, message_id_, json_body_) \
    DCC_APP_EDIT_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), (message_id_), (json_body_))
#define DCC_WEBHOOK_MESSAGE_EDIT(app_, webhook_id_, webhook_token_, message_id_, message_) \
    DCC_APP_EDIT_WEBHOOK_MESSAGE_BUILDER((app_), (webhook_id_), (webhook_token_), (message_id_), (message_))
#define DCC_WEBHOOK_MESSAGE_EDIT_THREAD(app_, webhook_id_, webhook_token_, message_id_, thread_id_, message_) \
    DCC_APP_EDIT_WEBHOOK_MESSAGE_BUILDER_THREAD( \
        (app_), \
        (webhook_id_), \
        (webhook_token_), \
        (message_id_), \
        (thread_id_), \
        (message_) \
    )
#define DCC_WEBHOOK_MESSAGE_DELETE(app_, webhook_id_, webhook_token_, message_id_) \
    DCC_APP_DELETE_WEBHOOK_MESSAGE((app_), (webhook_id_), (webhook_token_), (message_id_))
#define DCC_WEBHOOK_MESSAGE_DELETE_THREAD(app_, webhook_id_, webhook_token_, message_id_, thread_id_) \
    DCC_APP_DELETE_WEBHOOK_MESSAGE_THREAD((app_), (webhook_id_), (webhook_token_), (message_id_), (thread_id_))
#define DCC_APP_GET_CURRENT_APPLICATION(app_) \
    dcc_app_get_current_application((app_), NULL, NULL)
#define DCC_APP_GET_GATEWAY_BOT(app_) \
    dcc_app_get_gateway_bot((app_), NULL, NULL)
#define DCC_APP_GET_SKUS(app_, application_id_) \
    dcc_app_get_skus((app_), (application_id_), NULL, NULL)
#define DCC_APP_GET_ENTITLEMENTS(app_, application_id_) \
    dcc_app_get_entitlements((app_), (application_id_), NULL, NULL, NULL)
#define DCC_APP_GET_ENTITLEMENTS_QUERY(app_, application_id_, query_) \
    dcc_app_get_entitlements((app_), (application_id_), (query_), NULL, NULL)
#define DCC_APP_GET_ENTITLEMENTS_PAGE( \
    app_, \
    application_id_, \
    user_id_, \
    sku_ids_, \
    sku_id_count_, \
    before_id_, \
    after_id_, \
    limit_, \
    guild_id_, \
    exclude_ended_ \
) \
    dcc_app_get_entitlements_page( \
        (app_), \
        (application_id_), \
        (user_id_), \
        (sku_ids_), \
        (sku_id_count_), \
        (before_id_), \
        (after_id_), \
        (limit_), \
        (guild_id_), \
        (exclude_ended_), \
        NULL, \
        NULL \
    )
#define DCC_APP_CREATE_TEST_ENTITLEMENT(app_, application_id_, json_body_) \
    dcc_app_create_test_entitlement((app_), (application_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_TEST_ENTITLEMENT_PARAMS(app_, params_) \
    dcc_app_create_test_entitlement_params((app_), (params_), NULL, NULL)
#define DCC_APP_DELETE_TEST_ENTITLEMENT(app_, application_id_, entitlement_id_) \
    dcc_app_delete_test_entitlement((app_), (application_id_), (entitlement_id_), NULL, NULL)
#define DCC_APP_CONSUME_ENTITLEMENT(app_, application_id_, entitlement_id_) \
    dcc_app_consume_entitlement((app_), (application_id_), (entitlement_id_), NULL, NULL)
#define DCC_APP_GET_CURRENT_USER(app_) \
    dcc_app_get_current_user((app_), NULL, NULL)
#define DCC_APP_EDIT_CURRENT_USER(app_, json_body_) \
    dcc_app_modify_current_user((app_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_CURRENT_USER_PARAMS(app_, params_) \
    dcc_app_modify_current_user_params((app_), (params_), NULL, NULL)
#define DCC_APP_GET_USER(app_, user_id_) \
    dcc_app_get_user((app_), (user_id_), NULL, NULL)
#define DCC_APP_GET_CURRENT_USER_CONNECTIONS(app_) \
    dcc_app_get_current_user_connections((app_), NULL, NULL)
#define DCC_APP_GET_CURRENT_USER_GUILDS(app_) \
    dcc_app_get_current_user_guilds((app_), NULL, NULL, NULL)
#define DCC_APP_GET_CURRENT_USER_GUILDS_QUERY(app_, query_) \
    dcc_app_get_current_user_guilds((app_), (query_), NULL, NULL)
#define DCC_APP_GET_CURRENT_USER_DMS(app_) \
    dcc_app_get_current_user_dms((app_), NULL, NULL)
#define DCC_APP_CREATE_DM_CHANNEL(app_, json_body_) \
    dcc_app_create_dm_channel((app_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_DM_CHANNEL_PARAMS(app_, params_) \
    dcc_app_create_dm_channel_params((app_), (params_), NULL, NULL)
#define DCC_APP_ADD_GROUP_DM_RECIPIENT(app_, channel_id_, user_id_, json_body_) \
    dcc_app_add_group_dm_recipient((app_), (channel_id_), (user_id_), (json_body_), NULL, NULL)
#define DCC_APP_ADD_GROUP_DM_RECIPIENT_PARAMS(app_, params_) \
    dcc_app_add_group_dm_recipient_params((app_), (params_), NULL, NULL)
#define DCC_APP_REMOVE_GROUP_DM_RECIPIENT(app_, channel_id_, user_id_) \
    dcc_app_remove_group_dm_recipient((app_), (channel_id_), (user_id_), NULL, NULL)
#define DCC_APP_CREATE_DIRECT_MESSAGE(app_, user_id_, json_body_) \
    dcc_app_create_direct_message((app_), (user_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_DIRECT_MESSAGE_BUILDER(app_, user_id_, message_) \
    dcc_app_create_direct_message_builder( \
        (app_), \
        (user_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        NULL, \
        NULL \
    )
#define DCC_APP_CREATE_DIRECT_MESSAGE_TEXT(app_, user_id_, content_) \
    dcc_app_create_direct_message_text((app_), (user_id_), (content_), NULL, NULL)
#define DCC_APP_LEAVE_GUILD(app_, guild_id_) \
    dcc_app_leave_guild((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_INVITES(app_, guild_id_) \
    dcc_app_get_guild_invites((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_INVITE(app_, invite_code_) \
    dcc_app_get_invite((app_), (invite_code_), NULL, NULL, NULL)
#define DCC_APP_GET_INVITE_QUERY(app_, invite_code_, query_) \
    dcc_app_get_invite((app_), (invite_code_), (query_), NULL, NULL)
#define DCC_APP_GET_INVITE_FULL(app_, invite_code_) \
    dcc_app_get_invite_full((app_), (invite_code_), NULL, NULL)
#define DCC_APP_DELETE_INVITE(app_, invite_code_) \
    dcc_app_delete_invite((app_), (invite_code_), NULL, NULL)
#define DCC_APP_CREATE_STAGE(app_, json_body_) \
    dcc_app_create_stage_instance((app_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_STAGE_PARAMS(app_, params_) \
    dcc_app_create_stage_instance_params((app_), (params_), NULL, NULL)
#define DCC_APP_GET_STAGE(app_, channel_id_) \
    dcc_app_get_stage_instance((app_), (channel_id_), NULL, NULL)
#define DCC_APP_EDIT_STAGE(app_, channel_id_, json_body_) \
    dcc_app_modify_stage_instance((app_), (channel_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_STAGE_PARAMS(app_, params_) \
    dcc_app_modify_stage_instance_params((app_), (params_), NULL, NULL)
#define DCC_APP_DELETE_STAGE(app_, channel_id_) \
    dcc_app_delete_stage_instance((app_), (channel_id_), NULL, NULL)
#define DCC_APP_GET_TEMPLATE(app_, code_) \
    dcc_app_get_template((app_), (code_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_FROM_TEMPLATE(app_, code_, json_body_) \
    dcc_app_create_guild_from_template((app_), (code_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_FROM_TEMPLATE_PARAMS(app_, params_) \
    dcc_app_create_guild_from_template_params((app_), (params_), NULL, NULL)
#define DCC_APP_GET_GUILD_TEMPLATES(app_, guild_id_) \
    dcc_app_get_guild_templates((app_), (guild_id_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_TEMPLATE(app_, guild_id_, json_body_) \
    dcc_app_create_guild_template((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_TEMPLATE_PARAMS(app_, params_) \
    dcc_app_create_guild_template_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_TEMPLATE(app_, guild_id_, code_, json_body_) \
    dcc_app_modify_guild_template((app_), (guild_id_), (code_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_TEMPLATE_PARAMS(app_, params_) \
    dcc_app_modify_guild_template_params((app_), (params_), NULL, NULL)
#define DCC_APP_SYNC_GUILD_TEMPLATE(app_, guild_id_, code_) \
    dcc_app_sync_guild_template((app_), (guild_id_), (code_), NULL, NULL)
#define DCC_APP_DELETE_GUILD_TEMPLATE(app_, guild_id_, code_) \
    dcc_app_delete_guild_template((app_), (guild_id_), (code_), NULL, NULL)
#define DCC_APPLICATION_FETCH(app_) \
    DCC_APP_GET_CURRENT_APPLICATION((app_))
#define DCC_GATEWAY_BOT_FETCH(app_) \
    DCC_APP_GET_GATEWAY_BOT((app_))
#define DCC_SKUS_FETCH(app_, application_id_) \
    DCC_APP_GET_SKUS((app_), (application_id_))
#define DCC_ENTITLEMENTS_FETCH(app_, application_id_) \
    DCC_APP_GET_ENTITLEMENTS((app_), (application_id_))
#define DCC_ENTITLEMENTS_FETCH_QUERY(app_, application_id_, query_) \
    DCC_APP_GET_ENTITLEMENTS_QUERY((app_), (application_id_), (query_))
#define DCC_ENTITLEMENTS_FETCH_PAGE( \
    app_, \
    application_id_, \
    user_id_, \
    sku_ids_, \
    sku_id_count_, \
    before_id_, \
    after_id_, \
    limit_, \
    guild_id_, \
    exclude_ended_ \
) \
    DCC_APP_GET_ENTITLEMENTS_PAGE( \
        (app_), \
        (application_id_), \
        (user_id_), \
        (sku_ids_), \
        (sku_id_count_), \
        (before_id_), \
        (after_id_), \
        (limit_), \
        (guild_id_), \
        (exclude_ended_) \
    )
#define DCC_TEST_ENTITLEMENT_CREATE(app_, application_id_, json_body_) \
    DCC_APP_CREATE_TEST_ENTITLEMENT((app_), (application_id_), (json_body_))
#define DCC_TEST_ENTITLEMENT_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_TEST_ENTITLEMENT_PARAMS((app_), (params_))
#define DCC_TEST_ENTITLEMENT_DELETE(app_, application_id_, entitlement_id_) \
    DCC_APP_DELETE_TEST_ENTITLEMENT((app_), (application_id_), (entitlement_id_))
#define DCC_ENTITLEMENT_CONSUME(app_, application_id_, entitlement_id_) \
    DCC_APP_CONSUME_ENTITLEMENT((app_), (application_id_), (entitlement_id_))
#define DCC_ME_FETCH(app_) \
    DCC_APP_GET_CURRENT_USER((app_))
#define DCC_ME_EDIT_USER(app_, json_body_) \
    DCC_APP_EDIT_CURRENT_USER((app_), (json_body_))
