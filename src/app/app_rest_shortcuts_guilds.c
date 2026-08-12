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

#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>
#include <string.h>

static dcc_status_t app_raw_json(
    dcc_app_t *app,
    dcc_rest_method_t method,
    const char *path,
    const char *query,
    const char *json,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_endpoint_submit_legacy_raw(
        dcc_app_client(app), method, path, query,
        json != NULL ? "application/json" : NULL,
        json, json != NULL ? strlen(json) : 0U, cb, user_data
    );
}

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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_member,
        dcc_app_client(app), guild_id, user_id
    );
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_add_guild_member_role,
        dcc_app_client(app), guild_id, user_id, role_id
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_remove_guild_member_role,
        dcc_app_client(app), guild_id, user_id, role_id
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
    dcc_rest_guild_member_update_t body = DCC_REST_GUILD_MEMBER_UPDATE_INIT;
    body.present = DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_COMMUNICATION_DISABLED_UNTIL;
    body.communication_disabled_until = communication_disabled_until;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_guild_member,
        dcc_app_client(app), guild_id, user_id, &body
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
    dcc_rest_guild_member_update_t body = DCC_REST_GUILD_MEMBER_UPDATE_INIT;
    body.present = DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_COMMUNICATION_DISABLED_UNTIL;
    body.nulls = DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_COMMUNICATION_DISABLED_UNTIL;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_guild_member,
        dcc_app_client(app), guild_id, user_id, &body
    );
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
    dcc_rest_guild_member_update_t body = DCC_REST_GUILD_MEMBER_UPDATE_INIT;
    body.present = DCC_REST_GUILD_MEMBER_UPDATE_PRESENT_CHANNEL_ID;
    body.channel_id = channel_id;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_guild_member,
        dcc_app_client(app), guild_id, user_id, &body
    );
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_remove_guild_member,
        dcc_app_client(app), guild_id, user_id
    );
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
    char path[96];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/bans",
        (unsigned long long)guild_id
    );
    return status == DCC_OK ? app_raw_json(
        app, DCC_REST_GET, path, query, NULL, cb, user_data
    ) : status;
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
    dcc_rest_guild_bans_query_t query = DCC_REST_GUILD_BANS_QUERY_INIT;
    if (before != 0U) {
        query.present |= DCC_REST_GUILD_BANS_QUERY_PRESENT_BEFORE;
        query.before = before;
    }
    if (after != 0U) {
        query.present |= DCC_REST_GUILD_BANS_QUERY_PRESENT_AFTER;
        query.after = after;
    }
    if (limit != 0U) {
        if (limit > UINT16_MAX) return DCC_ERR_INVALID_ARG;
        query.present |= DCC_REST_GUILD_BANS_QUERY_PRESENT_LIMIT;
        query.limit = (uint16_t)limit;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_bans,
        dcc_app_client(app), guild_id, &query
    );
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_ban,
        dcc_app_client(app), guild_id, user_id
    );
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
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/bans/%llu",
        (unsigned long long)guild_id, (unsigned long long)user_id
    );
    return status == DCC_OK ? app_raw_json(
        app, DCC_REST_PUT, path, NULL, json_body, cb, user_data
    ) : status;
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
    dcc_rest_guild_ban_create_t body = DCC_REST_GUILD_BAN_CREATE_INIT;
    body.present = DCC_REST_GUILD_BAN_CREATE_PRESENT_DELETE_MESSAGE_SECONDS;
    body.delete_message_seconds = delete_message_seconds;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_create_guild_ban,
        dcc_app_client(app), guild_id, user_id, &body
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
    if (params->size < sizeof(*params)) return DCC_ERR_INVALID_ARG;
    return dcc_app_create_guild_ban_seconds(
        app, params->guild_id, params->user_id,
        params->delete_message_seconds, cb, user_data
    );
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_delete_guild_ban,
        dcc_app_client(app), guild_id, user_id
    );
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
    char path[96];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/audit-logs",
        (unsigned long long)guild_id
    );
    return status == DCC_OK ? app_raw_json(
        app, DCC_REST_GET, path, query, NULL, cb, user_data
    ) : status;
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
    dcc_rest_guild_audit_log_query_t query = DCC_REST_GUILD_AUDIT_LOG_QUERY_INIT;
    if (user_id != 0U) { query.present |= DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_USER_ID; query.user_id = user_id; }
    if (action_type != 0U) { query.present |= DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_ACTION_TYPE; query.action_type = action_type; }
    if (before != 0U) { query.present |= DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_BEFORE; query.before = before; }
    if (after != 0U) { query.present |= DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_AFTER; query.after = after; }
    if (limit != 0U) { if (limit > UINT16_MAX) return DCC_ERR_INVALID_ARG; query.present |= DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_LIMIT; query.limit = (uint16_t)limit; }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_audit_log,
        dcc_app_client(app), guild_id, &query
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
    char path[96];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/members/@me",
        (unsigned long long)guild_id
    );
    return status == DCC_OK ? app_raw_json(
        app, DCC_REST_PATCH, path, NULL, json_body, cb, user_data
    ) : status;
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
    if (params->size < sizeof(*params) || params->guild_id == 0U)
        return DCC_ERR_INVALID_ARG;
    dcc_rest_current_guild_member_update_t body = DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_INIT;
    if (params->nick != NULL) { body.present |= DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_PRESENT_NICK; body.nick = params->nick; }
    if (params->banner != NULL) { body.present |= DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_PRESENT_BANNER; body.banner = params->banner; }
    if (params->avatar != NULL) { body.present |= DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_PRESENT_AVATAR; body.avatar = params->avatar; }
    if (params->bio != NULL) { body.present |= DCC_REST_CURRENT_GUILD_MEMBER_UPDATE_PRESENT_BIO; body.bio = params->bio; }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_current_guild_member,
        dcc_app_client(app), params->guild_id, &body
    );
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
    char path[96];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/members/@me/nick",
        (unsigned long long)guild_id
    );
    return status == DCC_OK ? app_raw_json(
        app, DCC_REST_PATCH, path, NULL, json_body, cb, user_data
    ) : status;
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
    if (params->size < sizeof(*params) || params->guild_id == 0U)
        return DCC_ERR_INVALID_ARG;
    dcc_rest_current_guild_member_nickname_t body = DCC_REST_CURRENT_GUILD_MEMBER_NICKNAME_INIT;
    body.present = DCC_REST_CURRENT_GUILD_MEMBER_NICKNAME_PRESENT_NICK;
    if (params->nick != NULL) body.nick = params->nick;
    else body.nulls = DCC_REST_CURRENT_GUILD_MEMBER_NICKNAME_PRESENT_NICK;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_set_current_guild_member_nickname,
        dcc_app_client(app), params->guild_id, &body
    );
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
    dcc_rest_guild_prune_query_t query = DCC_REST_GUILD_PRUNE_QUERY_INIT;
    if (days != 0U) { query.present |= DCC_REST_GUILD_PRUNE_QUERY_PRESENT_DAYS; query.days = days; }
    if (include_role_count != 0U) { query.present |= DCC_REST_GUILD_PRUNE_QUERY_PRESENT_INCLUDE_ROLES; query.include_roles = include_roles; query.include_role_count = include_role_count; }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_prune_count,
        dcc_app_client(app), guild_id, &query
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
    dcc_rest_guild_prune_t body = DCC_REST_GUILD_PRUNE_INIT;
    if (days != 0U) { body.present |= DCC_REST_GUILD_PRUNE_PRESENT_DAYS; body.days = days; }
    body.present |= DCC_REST_GUILD_PRUNE_PRESENT_COMPUTE_PRUNE_COUNT;
    body.compute_prune_count = compute_prune_count;
    if (include_role_count != 0U) { body.present |= DCC_REST_GUILD_PRUNE_PRESENT_INCLUDE_ROLES; body.include_roles = include_roles; body.include_role_count = include_role_count; }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_begin_guild_prune,
        dcc_app_client(app), guild_id, &body
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
    if (params->size < sizeof(*params)) return DCC_ERR_INVALID_ARG;
    return dcc_app_begin_guild_prune_options(
        app, params->guild_id, params->days, params->include_roles,
        params->include_role_count, params->compute_prune_count, cb, user_data
    );
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_integrations,
        dcc_app_client(app), guild_id
    );
}

dcc_status_t dcc_app_modify_guild_integration(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    (void)app;
    (void)guild_id;
    (void)integration_id;
    (void)json_body;
    (void)cb;
    (void)user_data;
    return DCC_ERR_INVALID_ARG;
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_delete_guild_integration,
        dcc_app_client(app), guild_id, integration_id
    );
}

dcc_status_t dcc_app_sync_guild_integration(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    dcc_rest_cb cb,
    void *user_data
) {
    (void)app;
    (void)guild_id;
    (void)integration_id;
    (void)cb;
    (void)user_data;
    return DCC_ERR_INVALID_ARG;
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_widget,
        dcc_app_client(app), guild_id
    );
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
    char path[96];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/widget",
        (unsigned long long)guild_id
    );
    return status == DCC_OK ? app_raw_json(
        app, DCC_REST_PATCH, path, NULL, json_body, cb, user_data
    ) : status;
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
    if (params->size < sizeof(*params)) return DCC_ERR_INVALID_ARG;
    dcc_rest_guild_widget_update_t body = DCC_REST_GUILD_WIDGET_UPDATE_INIT;
    body.present = DCC_REST_GUILD_WIDGET_UPDATE_PRESENT_ENABLED |
        DCC_REST_GUILD_WIDGET_UPDATE_PRESENT_CHANNEL_ID;
    body.enabled = params->enabled;
    if (params->channel_id != 0U) body.channel_id = params->channel_id;
    else body.nulls = DCC_REST_GUILD_WIDGET_UPDATE_PRESENT_CHANNEL_ID;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_guild_widget,
        dcc_app_client(app), guild_id, &body
    );
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_vanity_url,
        dcc_app_client(app), guild_id
    );
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
    return DCC_ERR_INVALID_ARG;
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_welcome_screen,
        dcc_app_client(app), guild_id
    );
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
    char path[96];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/welcome-screen",
        (unsigned long long)guild_id
    );
    return status == DCC_OK ? app_raw_json(
        app, DCC_REST_PATCH, path, NULL, json_body, cb, user_data
    ) : status;
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
    if (params->size < sizeof(*params)) return DCC_ERR_INVALID_ARG;
    dcc_rest_guild_welcome_screen_update_t body = DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_INIT;
    body.present = DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_ENABLED;
    body.enabled = params->enabled;
    if (params->description != NULL) {
        body.present |= DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_DESCRIPTION;
        body.description = params->description;
    }
    dcc_rest_welcome_channel_t *channels = NULL;
    if (params->welcome_channel_count != 0U) {
        channels = calloc(params->welcome_channel_count, sizeof(*channels));
        if (channels == NULL) return DCC_ERR_NOMEM;
        for (size_t i = 0U; i < params->welcome_channel_count; ++i) {
            dcc_rest_welcome_channel_init(
                &channels[i], params->welcome_channels[i].channel_id,
                params->welcome_channels[i].description
            );
            if (params->welcome_channels[i].emoji_id != 0U) {
                channels[i].present |= DCC_REST_WELCOME_CHANNEL_PRESENT_EMOJI_ID;
                channels[i].emoji_id = params->welcome_channels[i].emoji_id;
            } else if (params->welcome_channels[i].emoji_name != NULL) {
                channels[i].present |= DCC_REST_WELCOME_CHANNEL_PRESENT_EMOJI_NAME;
                channels[i].emoji_name = params->welcome_channels[i].emoji_name;
            }
        }
        body.present |= DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_WELCOME_CHANNELS;
        body.welcome_channels = channels;
        body.welcome_channel_count = params->welcome_channel_count;
    }
    dcc_rest_call_options_t options;
    void *bridge = NULL;
    dcc_status_t status = dcc_endpoint_legacy_options(cb, user_data, &options, &bridge);
    if (status == DCC_OK) status = dcc_rest_modify_guild_welcome_screen(
        dcc_app_client(app), guild_id, &body, &options, NULL
    );
    if (status != DCC_OK) dcc_endpoint_legacy_bridge_release(bridge);
    free(channels);
    return status;
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_create_auto_moderation_rule,
        dcc_app_client(app), guild_id, params
    );
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
    if (params->rule_id == 0U) return DCC_ERR_INVALID_ARG;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_auto_moderation_rule,
        dcc_app_client(app), guild_id, params->rule_id, params
    );
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_create_guild_emoji,
        dcc_app_client(app), guild_id, params
    );
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
    return DCC_ERR_INVALID_ARG;
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
    dcc_guild_sticker_params_t params = DCC_GUILD_STICKER_PARAMS_INIT;
    params.present = DCC_GUILD_STICKER_PARAMS_PRESENT_FILE;
    params.file = *file;
    for (size_t index = 0U; index < field_count; ++index) {
        if (fields[index].name == NULL) return DCC_ERR_INVALID_ARG;
        if (strcmp(fields[index].name, "name") == 0) {
            params.present |= DCC_GUILD_STICKER_PARAMS_PRESENT_NAME;
            params.name = fields[index].value;
        } else if (strcmp(fields[index].name, "description") == 0) {
            params.present |= DCC_GUILD_STICKER_PARAMS_PRESENT_DESCRIPTION;
            params.description = fields[index].value;
        } else if (strcmp(fields[index].name, "tags") == 0) {
            params.present |= DCC_GUILD_STICKER_PARAMS_PRESENT_TAGS;
            params.tags = fields[index].value;
        } else {
            return DCC_ERR_INVALID_ARG;
        }
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_create_guild_sticker,
        dcc_app_client(app), guild_id, &params
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
    return DCC_ERR_INVALID_ARG;
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
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_guild_message_search,
        dcc_app_client(app), guild_id, params
    );
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
    dcc_rest_scheduled_event_query_t query = DCC_REST_SCHEDULED_EVENT_QUERY_INIT;
    query.present = DCC_REST_SCHEDULED_EVENT_QUERY_PRESENT_WITH_USER_COUNT;
    query.with_user_count = 1U;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_scheduled_events,
        dcc_app_client(app), guild_id, &query
    );
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
    dcc_rest_scheduled_event_query_t query = DCC_REST_SCHEDULED_EVENT_QUERY_INIT;
    query.present = DCC_REST_SCHEDULED_EVENT_QUERY_PRESENT_WITH_USER_COUNT;
    query.with_user_count = 1U;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_scheduled_event,
        dcc_app_client(app), guild_id, event_id, &query
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
    return DCC_ERR_INVALID_ARG;
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
    return DCC_ERR_INVALID_ARG;
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
    dcc_rest_scheduled_event_users_query_t query =
        DCC_REST_SCHEDULED_EVENT_USERS_QUERY_INIT;
    query.present = DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_LIMIT;
    query.limit = limit;
    if (before != 0U) {
        query.present |= DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_BEFORE;
        query.before = before;
    }
    if (after != 0U) {
        query.present |= DCC_REST_SCHEDULED_EVENT_USERS_QUERY_PRESENT_AFTER;
        query.after = after;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_scheduled_event_users,
        dcc_app_client(app), guild_id, event_id, &query
    );
}
