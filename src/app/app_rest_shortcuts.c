#include "internal/app/dcc_app_internal.h"

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
#include <dcc/rest/resources/entitlements.h>
#include <dcc/rest/resources/gateway.h>
#include <dcc/rest/resources/group_dms.h>
#include <dcc/rest/resources/invites.h>
#include <dcc/rest/resources/stage_instances.h>
#include <dcc/rest/resources/templates.h>

#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include <dcc/rest/resources/users.h>
#include <dcc/rest/resources/voice_states.h>
#include <dcc/rest/roles.h>
#include <dcc/rest/response_helpers.h>
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

#include "internal/client/dcc_client_guild_inference_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dcc_app_infer_guild_state {
    dcc_app_t *app;
    dcc_snowflake_t channel_id;
    dcc_app_infer_guild_cb cb;
    void *user_data;
} dcc_app_infer_guild_state_t;

static void dcc_app_infer_guild_id_from_channel_rest_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    dcc_app_infer_guild_state_t *state = (dcc_app_infer_guild_state_t *)user_data;
    if (state == NULL) {
        return;
    }

    dcc_snowflake_t guild_id = 0U;
    dcc_status_t status = response != NULL
        ? dcc_rest_response_guild_id(response, &guild_id)
        : DCC_ERR_INVALID_ARG;
    if (status == DCC_OK && guild_id != 0U) {
        dcc_client_channel_guild_inference_store(client, state->channel_id, guild_id);
    }
    state->cb(state->app, state->channel_id, guild_id, status, state->user_data);
    free(state);
}

dcc_status_t dcc_app_get_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_channel, dcc_app_client(app), channel_id
    );
}

dcc_status_t dcc_app_infer_guild_id_from_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_app_infer_guild_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || cb == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_client_t *client = dcc_app_client(app);
    dcc_snowflake_t guild_id = 0U;
    dcc_status_t status = dcc_client_infer_guild_id_from_channel(client, channel_id, &guild_id);
    if (status == DCC_OK) {
        cb(app, channel_id, guild_id, DCC_OK, user_data);
        return DCC_OK;
    }

    dcc_app_infer_guild_state_t *state =
        (dcc_app_infer_guild_state_t *)malloc(sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->app = app;
    state->channel_id = channel_id;
    state->cb = cb;
    state->user_data = user_data;

    dcc_rest_call_options_t options;
    void *bridge = NULL;
    status = dcc_endpoint_legacy_options(
        dcc_app_infer_guild_id_from_channel_rest_cb, state, &options, &bridge
    );
    if (status == DCC_OK) {
        status = dcc_rest_get_channel(client, channel_id, &options, NULL);
    }
    if (status != DCC_OK) {
        dcc_endpoint_legacy_bridge_release(bridge);
        free(state);
    }
    return status;
}

dcc_status_t dcc_app_get_guild_channels(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_channels,
        dcc_app_client(app), guild_id
    );
}

dcc_status_t dcc_app_create_guild_channel(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    char path[80];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/channels",
        (unsigned long long)guild_id
    );
    return status == DCC_OK ? dcc_endpoint_submit_legacy_raw(
        dcc_app_client(app), DCC_REST_POST, path, NULL,
        "application/json", json_body, strlen(json_body), cb, user_data
    ) : status;
}

dcc_status_t dcc_app_create_guild_channel_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_create_guild_channel,
        dcc_app_client(app), guild_id, params
    );
}

dcc_status_t dcc_app_modify_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    char path[64];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu", (unsigned long long)channel_id
    );
    return status == DCC_OK ? dcc_endpoint_submit_legacy_raw(
        dcc_app_client(app), DCC_REST_PATCH, path, NULL,
        "application/json", json_body, strlen(json_body), cb, user_data
    ) : status;
}

dcc_status_t dcc_app_modify_channel_params(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_channel,
        dcc_app_client(app), channel_id, params
    );
}

dcc_status_t dcc_app_delete_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_delete_channel, dcc_app_client(app), channel_id
    );
}

dcc_status_t dcc_app_trigger_channel_typing(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_trigger_channel_typing,
        dcc_app_client(app), channel_id
    );
}

dcc_status_t dcc_app_follow_news_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t webhook_channel_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || webhook_channel_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_follow_news_channel_t follow = DCC_REST_FOLLOW_NEWS_CHANNEL_INIT;
    follow.webhook_channel_id = webhook_channel_id;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_follow_news_channel,
        dcc_app_client(app), channel_id, &follow
    );
}

dcc_status_t dcc_app_set_channel_voice_status(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *status,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || status == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_channel_voice_status_params_t params =
        DCC_CHANNEL_VOICE_STATUS_PARAMS_INIT;
    params.present = DCC_CHANNEL_VOICE_STATUS_PRESENT_STATUS;
    params.status = status;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_set_channel_voice_status,
        dcc_app_client(app), channel_id, &params
    );
}

dcc_status_t dcc_app_get_current_user_voice_state(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_current_user_voice_state(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_modify_current_user_voice_state(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_current_user_voice_state(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_current_user_voice_state_params(
    dcc_app_t *app,
    const dcc_voice_state_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_current_user_voice_state_params_t typed = DCC_CURRENT_USER_VOICE_STATE_PARAMS_INIT;
    if (params->channel_id != 0U) { typed.present |= DCC_VOICE_STATE_PRESENT_CHANNEL_ID; typed.channel_id = params->channel_id; }
    typed.present |= DCC_VOICE_STATE_PRESENT_SUPPRESS;
    typed.suppress = params->suppress;
    if (params->request_to_speak_timestamp != NULL) { typed.present |= DCC_VOICE_STATE_PRESENT_REQUEST_TO_SPEAK_TIMESTAMP; typed.request_to_speak_timestamp = params->request_to_speak_timestamp; }
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data,
        dcc_rest_modify_current_user_voice_state, dcc_app_client(app),
        params->guild_id, &typed);
}

dcc_status_t dcc_app_get_user_voice_state(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || user_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_user_voice_state(dcc_app_client(app), guild_id, user_id, cb, user_data);
}

dcc_status_t dcc_app_modify_user_voice_state(
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
    return dcc_rest_modify_user_voice_state(dcc_app_client(app), guild_id, user_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_user_voice_state_params(
    dcc_app_t *app,
    const dcc_voice_state_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_user_voice_state_params_t typed = DCC_USER_VOICE_STATE_PARAMS_INIT;
    if (params->channel_id != 0U) { typed.present |= DCC_VOICE_STATE_PRESENT_CHANNEL_ID; typed.channel_id = params->channel_id; }
    typed.present |= DCC_VOICE_STATE_PRESENT_SUPPRESS;
    typed.suppress = params->suppress;
    DCC_ENDPOINT_LEGACY_RETURN(cb, user_data, dcc_rest_modify_user_voice_state,
        dcc_app_client(app), params->guild_id, params->user_id, &typed);
}

dcc_status_t dcc_app_get_voice_regions(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_voice_regions(dcc_app_client(app), cb, user_data);
}

dcc_status_t dcc_app_get_guild_voice_regions(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_voice_regions,
        dcc_app_client(app), guild_id
    );
}

dcc_status_t dcc_app_modify_guild_channel_positions(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    char path[80];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/channels",
        (unsigned long long)guild_id
    );
    return status == DCC_OK ? dcc_endpoint_submit_legacy_raw(
        dcc_app_client(app), DCC_REST_PATCH, path, NULL,
        "application/json", json_body, strlen(json_body), cb, user_data
    ) : status;
}

dcc_status_t dcc_app_modify_guild_channel_positions_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_channel_positions_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_guild_channel_positions,
        dcc_app_client(app), guild_id, params
    );
}

dcc_status_t dcc_app_get_channel_invites(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_channel_invites,
        dcc_app_client(app), channel_id
    );
}

dcc_status_t dcc_app_create_channel_invite(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    char path[80];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/invites",
        (unsigned long long)channel_id
    );
    return status == DCC_OK ? dcc_endpoint_submit_legacy_raw(
        dcc_app_client(app), DCC_REST_POST, path, NULL,
        "application/json", json_body, strlen(json_body), cb, user_data
    ) : status;
}

dcc_status_t dcc_app_create_channel_invite_params(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_invite_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_create_channel_invite,
        dcc_app_client(app), channel_id, params
    );
}

dcc_status_t dcc_app_modify_channel_permission(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || overwrite_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/permissions/%llu",
        (unsigned long long)channel_id, (unsigned long long)overwrite_id
    );
    return status == DCC_OK ? dcc_endpoint_submit_legacy_raw(
        dcc_app_client(app), DCC_REST_PUT, path, NULL,
        "application/json", json_body, strlen(json_body), cb, user_data
    ) : status;
}

dcc_status_t dcc_app_modify_channel_permission_params(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_channel_permission_overwrite_t *overwrite,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || overwrite == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_channel_permission,
        dcc_app_client(app), channel_id, overwrite->id, overwrite
    );
}

dcc_status_t dcc_app_delete_channel_permission(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || overwrite_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_delete_channel_permission,
        dcc_app_client(app), channel_id, overwrite_id
    );
}

dcc_status_t dcc_app_get_guild_roles(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_get_guild_roles,
        dcc_app_client(app), guild_id
    );
}

dcc_status_t dcc_app_create_guild_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    char path[80];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/roles",
        (unsigned long long)guild_id
    );
    return status == DCC_OK ? dcc_endpoint_submit_legacy_raw(
        dcc_app_client(app), DCC_REST_POST, path, NULL, "application/json",
        json_body, strlen(json_body), cb, user_data
    ) : status;
}

dcc_status_t dcc_app_create_guild_role_params(
    dcc_app_t *app,
    const dcc_role_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (params->size < sizeof(*params) || params->guild_id == 0U)
        return DCC_ERR_INVALID_ARG;
    dcc_rest_guild_role_create_t body = DCC_REST_GUILD_ROLE_CREATE_INIT;
    if (params->name != NULL) { body.present |= DCC_REST_GUILD_ROLE_CREATE_PRESENT_NAME; body.name = params->name; }
    body.present |= DCC_REST_GUILD_ROLE_CREATE_PRESENT_PERMISSIONS |
        DCC_REST_GUILD_ROLE_CREATE_PRESENT_COLOR |
        DCC_REST_GUILD_ROLE_CREATE_PRESENT_HOIST |
        DCC_REST_GUILD_ROLE_CREATE_PRESENT_MENTIONABLE;
    body.permissions = params->permissions;
    body.color = params->color;
    body.hoist = params->hoist;
    body.mentionable = params->mentionable;
    if (params->icon != NULL) { body.present |= DCC_REST_GUILD_ROLE_CREATE_PRESENT_ICON; body.icon = params->icon; }
    if (params->unicode_emoji != NULL) { body.present |= DCC_REST_GUILD_ROLE_CREATE_PRESENT_UNICODE_EMOJI; body.unicode_emoji = params->unicode_emoji; }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_create_guild_role,
        dcc_app_client(app), params->guild_id, &body
    );
}

dcc_status_t dcc_app_modify_guild_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || role_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    char path[96];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/roles/%llu",
        (unsigned long long)guild_id, (unsigned long long)role_id
    );
    return status == DCC_OK ? dcc_endpoint_submit_legacy_raw(
        dcc_app_client(app), DCC_REST_PATCH, path, NULL, "application/json",
        json_body, strlen(json_body), cb, user_data
    ) : status;
}

dcc_status_t dcc_app_modify_guild_role_params(
    dcc_app_t *app,
    const dcc_role_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (params->size < sizeof(*params) || params->guild_id == 0U || params->role_id == 0U)
        return DCC_ERR_INVALID_ARG;
    dcc_rest_guild_role_update_t body = DCC_REST_GUILD_ROLE_UPDATE_INIT;
    if (params->name != NULL) { body.present |= DCC_REST_GUILD_ROLE_UPDATE_PRESENT_NAME; body.name = params->name; }
    body.present |= DCC_REST_GUILD_ROLE_UPDATE_PRESENT_PERMISSIONS |
        DCC_REST_GUILD_ROLE_UPDATE_PRESENT_COLOR |
        DCC_REST_GUILD_ROLE_UPDATE_PRESENT_HOIST |
        DCC_REST_GUILD_ROLE_UPDATE_PRESENT_MENTIONABLE;
    body.permissions = params->permissions;
    body.color = params->color;
    body.hoist = params->hoist;
    body.mentionable = params->mentionable;
    if (params->icon != NULL) { body.present |= DCC_REST_GUILD_ROLE_UPDATE_PRESENT_ICON; body.icon = params->icon; }
    if (params->unicode_emoji != NULL) { body.present |= DCC_REST_GUILD_ROLE_UPDATE_PRESENT_UNICODE_EMOJI; body.unicode_emoji = params->unicode_emoji; }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_guild_role,
        dcc_app_client(app), params->guild_id, params->role_id, &body
    );
}

dcc_status_t dcc_app_modify_guild_role_positions(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_role_position_t *positions,
    size_t position_count,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || positions == NULL || position_count == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_rest_guild_role_position_t *typed = calloc(position_count, sizeof(*typed));
    if (typed == NULL) return DCC_ERR_NOMEM;
    for (size_t i = 0U; i < position_count; ++i) {
        dcc_rest_guild_role_position_init(&typed[i], positions[i].role_id);
        typed[i].present = DCC_REST_GUILD_ROLE_POSITION_PRESENT_POSITION;
        typed[i].position = positions[i].position;
    }
    dcc_rest_call_options_t options;
    void *bridge = NULL;
    dcc_status_t status = dcc_endpoint_legacy_options(cb, user_data, &options, &bridge);
    if (status == DCC_OK) status = dcc_rest_modify_guild_role_positions(
        dcc_app_client(app), guild_id, typed, position_count, &options, NULL
    );
    if (status != DCC_OK) dcc_endpoint_legacy_bridge_release(bridge);
    free(typed);
    return status;
}

dcc_status_t dcc_app_delete_guild_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || role_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_delete_guild_role,
        dcc_app_client(app), guild_id, role_id
    );
}

dcc_status_t dcc_app_edit_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message_id == 0U || message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, message);
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_edit_message,
        dcc_app_client(app), channel_id, message_id, &payload
    );
}

dcc_status_t dcc_app_edit_message_text(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
) {
    if (content == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .size = sizeof(message),
        .version = DCC_MESSAGE_BUILDER_VERSION,
        .present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT,
        .content = content,
    };
    return dcc_app_edit_message(app, channel_id, message_id, &message, cb, user_data);
}

dcc_status_t dcc_app_delete_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_delete_message,
        dcc_app_client(app), channel_id, message_id
    );
}

dcc_status_t dcc_app_crosspost_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_crosspost_message,
        dcc_app_client(app), channel_id, message_id
    );
}

dcc_status_t dcc_app_pin_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_pin_message,
        dcc_app_client(app), channel_id, message_id
    );
}

dcc_status_t dcc_app_unpin_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_unpin_message,
        dcc_app_client(app), channel_id, message_id
    );
}

dcc_status_t dcc_app_add_message_reaction(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message_id == 0U || reaction == NULL || reaction[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_add_message_reaction,
        dcc_app_client(app), channel_id, message_id, reaction
    );
}

dcc_status_t dcc_app_delete_own_message_reaction(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message_id == 0U || reaction == NULL || reaction[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_delete_own_message_reaction,
        dcc_app_client(app), channel_id, message_id, reaction
    );
}

dcc_status_t dcc_app_delete_user_message_reaction(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL ||
        channel_id == 0U ||
        message_id == 0U ||
        reaction == NULL ||
        reaction[0] == '\0' ||
        user_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_delete_user_message_reaction,
        dcc_app_client(app), channel_id, message_id, reaction, user_id
    );
}

dcc_status_t dcc_app_delete_all_message_reactions(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_delete_all_message_reactions,
        dcc_app_client(app), channel_id, message_id
    );
}

dcc_status_t dcc_app_delete_all_message_reactions_for_emoji(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message_id == 0U || reaction == NULL || reaction[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_delete_all_message_reactions_for_emoji,
        dcc_app_client(app), channel_id, message_id, reaction
    );
}

dcc_status_t dcc_app_create_thread_from_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || message_id == 0U || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_create_thread_from_message,
        dcc_app_client(app), channel_id, message_id, params
    );
}

dcc_status_t dcc_app_create_thread_from_message_name(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *name,
    dcc_rest_cb cb,
    void *user_data
) {
    if (name == NULL || name[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_thread_params_t params = DCC_THREAD_PARAMS_INIT;
    params.present = DCC_THREAD_PARAMS_PRESENT_NAME;
    params.name = name;
    return dcc_app_create_thread_from_message(app, channel_id, message_id, &params, cb, user_data);
}

dcc_status_t dcc_app_modify_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    static const uint64_t known =
        DCC_THREAD_PARAMS_PRESENT_NAME |
        DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION |
        DCC_THREAD_PARAMS_PRESENT_TYPE |
        DCC_THREAD_PARAMS_PRESENT_INVITABLE |
        DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER |
        DCC_THREAD_PARAMS_PRESENT_MESSAGE |
        DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS;
    dcc_endpoint_record_view_t view;
    if (app == NULL || thread_id == 0U ||
        dcc_endpoint_record_read(
            params,
            offsetof(dcc_thread_params_t, version),
            offsetof(dcc_thread_params_t, present),
            DCC_ENDPOINT_FIELD_END(dcc_thread_params_t, present),
            DCC_THREAD_PARAMS_VERSION,
            known,
            &view
        ) != DCC_OK ||
        dcc_endpoint_field_partially_covered(
            view.size, offsetof(dcc_thread_params_t, name), sizeof(params->name)
        ) ||
        dcc_endpoint_field_partially_covered(
            view.size, offsetof(dcc_thread_params_t, auto_archive_duration),
            sizeof(params->auto_archive_duration)
        ) ||
        dcc_endpoint_field_partially_covered(
            view.size, offsetof(dcc_thread_params_t, type), sizeof(params->type)
        ) ||
        dcc_endpoint_field_partially_covered(
            view.size, offsetof(dcc_thread_params_t, invitable),
            sizeof(params->invitable)
        ) ||
        dcc_endpoint_field_partially_covered(
            view.size, offsetof(dcc_thread_params_t, rate_limit_per_user),
            sizeof(params->rate_limit_per_user)
        ) ||
        dcc_endpoint_field_partially_covered(
            view.size, offsetof(dcc_thread_params_t, message),
            sizeof(params->message)
        ) ||
        dcc_endpoint_field_partially_covered(
            view.size, offsetof(dcc_thread_params_t, applied_tags),
            sizeof(params->applied_tags)
        ) ||
        dcc_endpoint_field_partially_covered(
            view.size, offsetof(dcc_thread_params_t, applied_tag_count),
            sizeof(params->applied_tag_count)
        ) ||
        !dcc_endpoint_present_field_covered(
            &view, DCC_THREAD_PARAMS_PRESENT_NAME,
            offsetof(dcc_thread_params_t, name), sizeof(params->name)
        ) ||
        !dcc_endpoint_present_field_covered(
            &view, DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION,
            offsetof(dcc_thread_params_t, auto_archive_duration),
            sizeof(params->auto_archive_duration)
        ) ||
        !dcc_endpoint_present_field_covered(
            &view, DCC_THREAD_PARAMS_PRESENT_INVITABLE,
            offsetof(dcc_thread_params_t, invitable), sizeof(params->invitable)
        ) ||
        !dcc_endpoint_present_field_covered(
            &view, DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER,
            offsetof(dcc_thread_params_t, rate_limit_per_user),
            sizeof(params->rate_limit_per_user)
        ) ||
        !dcc_endpoint_present_field_covered(
            &view, DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS,
            offsetof(dcc_thread_params_t, applied_tags),
            sizeof(params->applied_tags)
        ) ||
        ((view.present & DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS) != 0U &&
         !dcc_endpoint_field_covered(
             view.size, offsetof(dcc_thread_params_t, applied_tag_count),
             sizeof(params->applied_tag_count)
         )) ||
        (view.present & (DCC_THREAD_PARAMS_PRESENT_TYPE |
            DCC_THREAD_PARAMS_PRESENT_MESSAGE)) != 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_channel_params_t channel = DCC_CHANNEL_PARAMS_INIT;
    channel.kind = DCC_CHANNEL_PARAMS_THREAD;
    if ((view.present & DCC_THREAD_PARAMS_PRESENT_NAME) != 0U) {
        channel.payload.thread.present |= DCC_CHANNEL_THREAD_PRESENT_NAME;
        channel.payload.thread.name = params->name;
    }
    if ((view.present & DCC_THREAD_PARAMS_PRESENT_AUTO_ARCHIVE_DURATION) != 0U) {
        channel.payload.thread.present |=
            DCC_CHANNEL_THREAD_PRESENT_AUTO_ARCHIVE_DURATION;
        channel.payload.thread.auto_archive_duration = params->auto_archive_duration;
    }
    if ((view.present & DCC_THREAD_PARAMS_PRESENT_INVITABLE) != 0U) {
        channel.payload.thread.present |= DCC_CHANNEL_THREAD_PRESENT_INVITABLE;
        channel.payload.thread.invitable = params->invitable;
    }
    if ((view.present & DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER) != 0U) {
        channel.payload.thread.present |=
            DCC_CHANNEL_THREAD_PRESENT_RATE_LIMIT_PER_USER;
        channel.payload.thread.rate_limit_per_user = params->rate_limit_per_user;
    }
    if ((view.present & DCC_THREAD_PARAMS_PRESENT_APPLIED_TAGS) != 0U) {
        channel.payload.thread.present |= DCC_CHANNEL_THREAD_PRESENT_APPLIED_TAGS;
        channel.payload.thread.applied_tags = params->applied_tags;
        channel.payload.thread.applied_tag_count = params->applied_tag_count;
    }
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_channel,
        dcc_app_client(app), thread_id, &channel
    );
}

dcc_status_t dcc_app_archive_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || thread_id == 0U) return DCC_ERR_INVALID_ARG;
    dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
    params.kind = DCC_CHANNEL_PARAMS_THREAD;
    params.payload.thread.present = DCC_CHANNEL_THREAD_PRESENT_ARCHIVED;
    params.payload.thread.archived = 1U;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_channel,
        dcc_app_client(app), thread_id, &params
    );
}

dcc_status_t dcc_app_lock_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || thread_id == 0U) return DCC_ERR_INVALID_ARG;
    dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
    params.kind = DCC_CHANNEL_PARAMS_THREAD;
    params.payload.thread.present = DCC_CHANNEL_THREAD_PRESENT_ARCHIVED |
        DCC_CHANNEL_THREAD_PRESENT_LOCKED;
    params.payload.thread.archived = 1U;
    params.payload.thread.locked = 1U;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_channel,
        dcc_app_client(app), thread_id, &params
    );
}

dcc_status_t dcc_app_unlock_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || thread_id == 0U) return DCC_ERR_INVALID_ARG;
    dcc_channel_params_t params = DCC_CHANNEL_PARAMS_INIT;
    params.kind = DCC_CHANNEL_PARAMS_THREAD;
    params.payload.thread.present = DCC_CHANNEL_THREAD_PRESENT_LOCKED;
    params.payload.thread.locked = 0U;
    DCC_ENDPOINT_LEGACY_RETURN(
        cb, user_data, dcc_rest_modify_channel,
        dcc_app_client(app), thread_id, &params
    );
}
