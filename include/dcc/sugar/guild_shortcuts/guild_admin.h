#ifndef DCC_SUGAR_GUILD_SHORTCUTS_GUILD_ADMIN_H
#define DCC_SUGAR_GUILD_SHORTCUTS_GUILD_ADMIN_H

#include <dcc/sugar/guild_shortcuts/base.h>

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

#endif /* DCC_SUGAR_GUILD_SHORTCUTS_GUILD_ADMIN_H */
