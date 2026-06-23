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
    return dcc_rest_get_channel(dcc_app_client(app), channel_id, cb, user_data);
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

    status = dcc_rest_get_channel(
        client,
        channel_id,
        dcc_app_infer_guild_id_from_channel_rest_cb,
        state
    );
    if (status != DCC_OK) {
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
    return dcc_rest_get_guild_channels(dcc_app_client(app), guild_id, cb, user_data);
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
    return dcc_rest_create_guild_channel(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_create_guild_channel_params(
    dcc_app_t *app,
    const dcc_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_channel_params(dcc_app_client(app), params, cb, user_data);
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
    return dcc_rest_modify_channel(dcc_app_client(app), channel_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_channel_params(
    dcc_app_t *app,
    const dcc_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_channel_params(dcc_app_client(app), params, cb, user_data);
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
    return dcc_rest_delete_channel(dcc_app_client(app), channel_id, cb, user_data);
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
    return dcc_rest_trigger_channel_typing(dcc_app_client(app), channel_id, cb, user_data);
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

    char body[64];
    int written = snprintf(
        body,
        sizeof(body),
        "{\"webhook_channel_id\":\"%llu\"}",
        (unsigned long long)webhook_channel_id
    );
    if (written < 0 || (size_t)written >= sizeof(body)) {
        return DCC_ERR_NOMEM;
    }
    return dcc_rest_follow_news_channel(dcc_app_client(app), channel_id, body, cb, user_data);
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
    dcc_channel_voice_status_params_t params = {
        .size = sizeof(params),
        .channel_id = channel_id,
        .status = status,
    };
    return dcc_rest_set_channel_voice_status_params(dcc_app_client(app), &params, cb, user_data);
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
    return dcc_rest_modify_current_user_voice_state_params(dcc_app_client(app), params, cb, user_data);
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
    return dcc_rest_modify_user_voice_state_params(dcc_app_client(app), params, cb, user_data);
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
    return dcc_rest_get_guild_voice_regions(dcc_app_client(app), guild_id, cb, user_data);
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
    return dcc_rest_modify_guild_channel_positions(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_channel_positions_params(
    dcc_app_t *app,
    const dcc_channel_positions_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_channel_positions_params(dcc_app_client(app), params, cb, user_data);
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
    return dcc_rest_get_channel_invites(dcc_app_client(app), channel_id, cb, user_data);
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
    return dcc_rest_create_channel_invite(dcc_app_client(app), channel_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_create_channel_invite_params(
    dcc_app_t *app,
    const dcc_invite_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_channel_invite_params(dcc_app_client(app), params, cb, user_data);
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
    return dcc_rest_modify_channel_permission(
        dcc_app_client(app),
        channel_id,
        overwrite_id,
        json_body,
        cb,
        user_data
    );
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
    return dcc_rest_modify_channel_permission_params(dcc_app_client(app), channel_id, overwrite, cb, user_data);
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
    return dcc_rest_delete_channel_permission(dcc_app_client(app), channel_id, overwrite_id, cb, user_data);
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
    return dcc_rest_get_guild_roles(dcc_app_client(app), guild_id, cb, user_data);
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
    return dcc_rest_create_guild_role(dcc_app_client(app), guild_id, json_body, cb, user_data);
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
    return dcc_rest_create_guild_role_params(dcc_app_client(app), params, cb, user_data);
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
    return dcc_rest_modify_guild_role(dcc_app_client(app), guild_id, role_id, json_body, cb, user_data);
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
    return dcc_rest_modify_guild_role_params(dcc_app_client(app), params, cb, user_data);
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
    return dcc_rest_modify_guild_role_positions_params(
        dcc_app_client(app),
        guild_id,
        positions,
        position_count,
        cb,
        user_data
    );
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
    return dcc_rest_delete_guild_role(dcc_app_client(app), guild_id, role_id, cb, user_data);
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
    return dcc_rest_edit_message_builder(
        dcc_app_client(app),
        channel_id,
        message_id,
        message,
        cb,
        user_data
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
        .content = content,
        .has_content = 1U,
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
    return dcc_rest_delete_message(dcc_app_client(app), channel_id, message_id, cb, user_data);
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
    return dcc_rest_crosspost_message(dcc_app_client(app), channel_id, message_id, cb, user_data);
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
    return dcc_rest_pin_message(dcc_app_client(app), channel_id, message_id, cb, user_data);
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
    return dcc_rest_unpin_message(dcc_app_client(app), channel_id, message_id, cb, user_data);
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
    return dcc_rest_add_message_reaction(dcc_app_client(app), channel_id, message_id, reaction, cb, user_data);
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
    return dcc_rest_delete_own_message_reaction(
        dcc_app_client(app),
        channel_id,
        message_id,
        reaction,
        cb,
        user_data
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
    return dcc_rest_delete_user_message_reaction(
        dcc_app_client(app),
        channel_id,
        message_id,
        reaction,
        user_id,
        cb,
        user_data
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
    return dcc_rest_delete_all_message_reactions(dcc_app_client(app), channel_id, message_id, cb, user_data);
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
    return dcc_rest_delete_all_message_reactions_for_emoji(
        dcc_app_client(app),
        channel_id,
        message_id,
        reaction,
        cb,
        user_data
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

    dcc_thread_params_t local = *params;
    local.size = sizeof(local);
    local.channel_id = channel_id;
    local.message_id = message_id;
    return dcc_rest_create_thread_from_message_params(dcc_app_client(app), &local, cb, user_data);
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

    dcc_thread_params_t params = {
        .size = sizeof(params),
        .channel_id = channel_id,
        .message_id = message_id,
        .name = name,
    };
    return dcc_app_create_thread_from_message(app, channel_id, message_id, &params, cb, user_data);
}

dcc_status_t dcc_app_modify_thread(
    dcc_app_t *app,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL || params->thread_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_thread_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_archive_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_thread_params_t params = {
        .size = sizeof(params),
        .thread_id = thread_id,
        .archived = 1U,
    };
    return dcc_app_modify_thread(app, &params, cb, user_data);
}

dcc_status_t dcc_app_lock_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_thread_params_t params = {
        .size = sizeof(params),
        .thread_id = thread_id,
        .archived = 1U,
        .locked = 1U,
    };
    return dcc_app_modify_thread(app, &params, cb, user_data);
}

dcc_status_t dcc_app_unlock_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_thread_params_t params = {
        .size = sizeof(params),
        .thread_id = thread_id,
    };
    return dcc_app_modify_thread(app, &params, cb, user_data);
}
