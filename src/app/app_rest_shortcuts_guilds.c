#include <dcc/app.h>

#include <dcc/rest/emojis_stickers/guild_emojis.h>
#include <dcc/rest/emojis_stickers/guild_stickers.h>
#include <dcc/rest/emojis_stickers/message_search.h>
#include <dcc/rest/emojis_stickers/stickers.h>
#include <dcc/rest/channels/actions.h>
#include <dcc/rest/channels/core.h>
#include <dcc/rest/channels/invites.h>
#include <dcc/rest/channels/permissions.h>
#include <dcc/rest/channels/positions.h>
#include <dcc/rest/guilds/audit_log.h>
#include <dcc/rest/guilds/auto_moderation.h>
#include <dcc/rest/guilds/bans.h>
#include <dcc/rest/guilds/current_member.h>
#include <dcc/rest/guilds/integrations.h>
#include <dcc/rest/guilds/onboarding.h>
#include <dcc/rest/guilds/prune.h>
#include <dcc/rest/guilds/scheduled_events.h>
#include <dcc/rest/guilds/welcome_screen.h>
#include <dcc/rest/guilds/widgets.h>
#include <dcc/rest/guild_members/lifecycle.h>
#include <dcc/rest/guild_members/query.h>
#include <dcc/rest/guild_members/roles.h>
#include <dcc/rest/invites.h>
#include <dcc/rest/messages/edit.h>
#include <dcc/rest/messages/pins.h>
#include <dcc/rest/messages/reactions.h>
#include <dcc/rest/resources/direct_messages.h>
#include <dcc/rest/resources/entitlements.h>
#include <dcc/rest/resources/gateway.h>
#include <dcc/rest/resources/group_dms.h>
#include <dcc/rest/resources/invites.h>
#include <dcc/rest/resources/stage_instances.h>
#include <dcc/rest/resources/templates.h>
#include <dcc/rest/resources/users.h>
#include <dcc/rest/resources/voice_states.h>
#include <dcc/rest/roles.h>
#include <dcc/rest/threads/create.h>
#include <dcc/rest/threads/lifecycle.h>
#include <dcc/rest/webhooks/execute.h>
#include <dcc/rest/webhooks/management/create.h>
#include <dcc/rest/webhooks/management/delete.h>
#include <dcc/rest/webhooks/management/fetch.h>
#include <dcc/rest/webhooks/management/modify.h>
#include <dcc/rest/webhooks/messages/delete.h>
#include <dcc/rest/webhooks/messages/edit.h>
#include <dcc/rest/webhooks/messages/fetch.h>

dcc_status_t dcc_app_get_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_member(dcc_app_client(app), guild_id, user_id, cb, user_data);
}

dcc_status_t dcc_app_add_member_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U || role_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_add_guild_member_role(
        dcc_app_client(app),
        guild_id,
        user_id,
        role_id,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_remove_member_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U || role_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_remove_guild_member_role(
        dcc_app_client(app),
        guild_id,
        user_id,
        role_id,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_set_member_timeout(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *communication_disabled_until,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U || communication_disabled_until == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_set_guild_member_timeout(
        dcc_app_client(app),
        guild_id,
        user_id,
        communication_disabled_until,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_clear_member_timeout(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_clear_guild_member_timeout(dcc_app_client(app), guild_id, user_id, cb, user_data);
}

dcc_status_t dcc_app_move_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U || channel_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_move_guild_member(dcc_app_client(app), guild_id, user_id, channel_id, cb, user_data);
}

dcc_status_t dcc_app_remove_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_remove_guild_member(dcc_app_client(app), guild_id, user_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_bans(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_bans(dcc_app_client(app), guild_id, query, cb, user_data);
}

dcc_status_t dcc_app_get_guild_bans_page(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    uint64_t limit,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_bans_page(dcc_app_client(app), guild_id, before, after, limit, cb, user_data);
}

dcc_status_t dcc_app_get_guild_ban(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_ban(dcc_app_client(app), guild_id, user_id, cb, user_data);
}

dcc_status_t dcc_app_create_guild_ban(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_ban(dcc_app_client(app), guild_id, user_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_create_guild_ban_seconds(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    uint32_t delete_message_seconds,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_ban_seconds(
        dcc_app_client(app),
        guild_id,
        user_id,
        delete_message_seconds,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_create_guild_ban_params(
    dcc_app_t *app,
    const dcc_guild_ban_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_ban_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_delete_guild_ban(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_guild_ban(dcc_app_client(app), guild_id, user_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_audit_log(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_audit_log(dcc_app_client(app), guild_id, query, cb, user_data);
}

dcc_status_t dcc_app_get_guild_audit_log_page(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    uint32_t action_type,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    uint32_t limit,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_audit_log_page(
        dcc_app_client(app),
        guild_id,
        user_id,
        action_type,
        before,
        after,
        limit,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_modify_current_guild_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_current_guild_member(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_current_guild_member_params(
    dcc_app_t *app,
    const dcc_current_guild_member_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_current_guild_member_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_set_current_guild_member_nickname(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_set_current_guild_member_nickname(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_set_current_guild_member_nickname_params(
    dcc_app_t *app,
    const dcc_guild_member_nickname_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_set_current_guild_member_nickname_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_get_guild_prune_count_options(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    uint32_t days,
    const dcc_snowflake_t *include_roles,
    size_t include_role_count,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_prune_count_options(
        dcc_app_client(app),
        guild_id,
        days,
        include_roles,
        include_role_count,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_begin_guild_prune_options(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    uint32_t days,
    const dcc_snowflake_t *include_roles,
    size_t include_role_count,
    uint8_t compute_prune_count,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_begin_guild_prune_options(
        dcc_app_client(app),
        guild_id,
        days,
        include_roles,
        include_role_count,
        compute_prune_count,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_begin_guild_prune_params(
    dcc_app_t *app,
    const dcc_guild_prune_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_begin_guild_prune_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_get_guild_integrations(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_integrations(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_integration(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || integration_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_integration(
        dcc_app_client(app),
        guild_id,
        integration_id,
        json_body,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_delete_guild_integration(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || integration_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_guild_integration(dcc_app_client(app), guild_id, integration_id, cb, user_data);
}

dcc_status_t dcc_app_sync_guild_integration(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || integration_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_sync_guild_integration(dcc_app_client(app), guild_id, integration_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_widget(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_widget(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_widget(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_widget(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_widget_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_guild_widget_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_widget_params(dcc_app_client(app), guild_id, params, cb, user_data);
}

dcc_status_t dcc_app_get_guild_vanity_url(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_vanity_url(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_onboarding(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_onboarding(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_onboarding(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_onboarding(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_onboarding_params(
    dcc_app_t *app,
    const dcc_onboarding_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_onboarding_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_get_guild_welcome_screen(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_welcome_screen(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_welcome_screen(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_welcome_screen(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_welcome_screen_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_welcome_screen_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_welcome_screen_params(dcc_app_client(app), guild_id, params, cb, user_data);
}

dcc_status_t dcc_app_get_auto_moderation_rules(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_auto_moderation_rules(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_get_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || rule_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_auto_moderation_rule(dcc_app_client(app), guild_id, rule_id, cb, user_data);
}

dcc_status_t dcc_app_create_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_auto_moderation_rule(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_create_auto_moderation_rule_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_auto_moderation_rule_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_auto_moderation_rule_params(dcc_app_client(app), guild_id, params, cb, user_data);
}

dcc_status_t dcc_app_modify_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || rule_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_auto_moderation_rule(
        dcc_app_client(app),
        guild_id,
        rule_id,
        json_body,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_modify_auto_moderation_rule_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_auto_moderation_rule_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_auto_moderation_rule_params(dcc_app_client(app), guild_id, params, cb, user_data);
}

dcc_status_t dcc_app_delete_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || rule_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_auto_moderation_rule(dcc_app_client(app), guild_id, rule_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_emojis(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_emojis(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || emoji_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_emoji(dcc_app_client(app), guild_id, emoji_id, cb, user_data);
}

dcc_status_t dcc_app_create_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_emoji(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_create_guild_emoji_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_emoji_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_emoji_params(dcc_app_client(app), guild_id, params, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || emoji_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_emoji(dcc_app_client(app), guild_id, emoji_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_emoji_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_emoji_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_emoji_params(dcc_app_client(app), guild_id, params, cb, user_data);
}

dcc_status_t dcc_app_delete_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || emoji_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_guild_emoji(dcc_app_client(app), guild_id, emoji_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_stickers(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_stickers(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_sticker(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || sticker_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_sticker(dcc_app_client(app), guild_id, sticker_id, cb, user_data);
}

dcc_status_t dcc_app_create_guild_sticker_multipart(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *file,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || file == NULL || (fields == NULL && field_count > 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_sticker_multipart(
        dcc_app_client(app),
        guild_id,
        fields,
        field_count,
        file,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_create_guild_sticker_params(
    dcc_app_t *app,
    const dcc_guild_sticker_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_sticker_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_sticker(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || sticker_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_sticker(dcc_app_client(app), guild_id, sticker_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_delete_guild_sticker(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || sticker_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_guild_sticker(dcc_app_client(app), guild_id, sticker_id, cb, user_data);
}

dcc_status_t dcc_app_get_sticker(
    dcc_app_t *app,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || sticker_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_sticker(dcc_app_client(app), sticker_id, cb, user_data);
}

dcc_status_t dcc_app_get_sticker_packs(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_sticker_packs(dcc_app_client(app), cb, user_data);
}

dcc_status_t dcc_app_guild_message_search(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_guild_message_search(dcc_app_client(app), guild_id, query, cb, user_data);
}

dcc_status_t dcc_app_guild_message_search_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_message_search_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_guild_message_search_params(dcc_app_client(app), guild_id, params, cb, user_data);
}

dcc_status_t dcc_app_get_guild_scheduled_events(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_scheduled_events(dcc_app_client(app), guild_id, query, cb, user_data);
}

dcc_status_t dcc_app_get_guild_scheduled_events_with_user_count(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_scheduled_events_with_user_count(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || event_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_scheduled_event(dcc_app_client(app), guild_id, event_id, query, cb, user_data);
}

dcc_status_t dcc_app_get_guild_scheduled_event_with_user_count(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || event_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_scheduled_event_with_user_count(
        dcc_app_client(app),
        guild_id,
        event_id,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_create_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_scheduled_event(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_create_guild_scheduled_event_params(
    dcc_app_t *app,
    const dcc_scheduled_event_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_scheduled_event_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || event_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_scheduled_event(dcc_app_client(app), guild_id, event_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_scheduled_event_params(
    dcc_app_t *app,
    const dcc_scheduled_event_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_scheduled_event_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_delete_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || event_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_guild_scheduled_event(dcc_app_client(app), guild_id, event_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_scheduled_event_users(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || event_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_scheduled_event_users(
        dcc_app_client(app),
        guild_id,
        event_id,
        query,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_get_guild_scheduled_event_users_page(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    uint8_t limit,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || event_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_scheduled_event_users_page(
        dcc_app_client(app),
        guild_id,
        event_id,
        limit,
        before,
        after,
        cb,
        user_data
    );
}
