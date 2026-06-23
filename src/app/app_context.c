#include "internal/app/dcc_app_internal.h"

#include <dcc/cache.h>
#include <dcc/rest/interactions/responses.h>
#include <dcc/rest/interactions/original_response.h>

#include <string.h>

dcc_app_t *dcc_ctx_app(const dcc_ctx_t *ctx) {
    return ctx != NULL ? ctx->app : NULL;
}

dcc_client_t *dcc_ctx_client(const dcc_ctx_t *ctx) {
    return ctx != NULL ? ctx->client : NULL;
}

const dcc_event_t *dcc_ctx_event(const dcc_ctx_t *ctx) {
    return ctx != NULL ? ctx->event : NULL;
}

const dcc_interaction_t *dcc_ctx_interaction(const dcc_ctx_t *ctx) {
    return ctx != NULL ? ctx->interaction : NULL;
}

dcc_interaction_flow_t *dcc_ctx_flow(dcc_ctx_t *ctx) {
    return ctx != NULL ? &ctx->flow : NULL;
}

void *dcc_ctx_user_data(const dcc_ctx_t *ctx) {
    return ctx != NULL ? ctx->user_data : NULL;
}

dcc_event_type_t dcc_ctx_event_type(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->event != NULL ? dcc_event_type(ctx->event) : DCC_EVENT_MAX;
}

dcc_snowflake_t dcc_ctx_interaction_id(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->id : 0U;
}

dcc_snowflake_t dcc_ctx_application_id(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->application_id : 0U;
}

dcc_snowflake_t dcc_ctx_guild_id(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->guild_id : 0U;
}

dcc_snowflake_t dcc_ctx_channel_id(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->channel_id : 0U;
}

static const dcc_channel_t *dcc_ctx_cached_channel(const dcc_ctx_t *ctx) {
    dcc_client_t *client = dcc_ctx_client(ctx);
    dcc_snowflake_t channel_id = dcc_ctx_channel_id(ctx);
    return client != NULL && channel_id != 0U ? dcc_cache_get_channel(client, channel_id) : NULL;
}

dcc_status_t dcc_ctx_channel_type(const dcc_ctx_t *ctx, uint32_t *out_type) {
    if (out_type == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const dcc_channel_t *channel = dcc_ctx_cached_channel(ctx);
    if (channel == NULL) {
        *out_type = 0U;
        return DCC_ERR_NOT_FOUND;
    }
    *out_type = channel->type;
    return DCC_OK;
}

uint8_t dcc_ctx_channel_is_type(const dcc_ctx_t *ctx, uint32_t channel_type) {
    uint32_t actual = 0U;
    return dcc_ctx_channel_type(ctx, &actual) == DCC_OK && actual == channel_type ? 1U : 0U;
}

uint8_t dcc_ctx_channel_is_nsfw(const dcc_ctx_t *ctx) {
    const dcc_channel_t *channel = dcc_ctx_cached_channel(ctx);
    return channel != NULL && channel->nsfw != 0U ? 1U : 0U;
}

dcc_snowflake_t dcc_ctx_command_id(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->command_id : 0U;
}

dcc_snowflake_t dcc_ctx_message_id(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->message_id : 0U;
}

dcc_snowflake_t dcc_ctx_target_id(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->target_id : 0U;
}

dcc_snowflake_t dcc_ctx_user_id(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->user_id : 0U;
}

const dcc_guild_t *dcc_ctx_guild(const dcc_ctx_t *ctx) {
    dcc_client_t *client = dcc_ctx_client(ctx);
    dcc_snowflake_t guild_id = dcc_ctx_guild_id(ctx);
    return client != NULL && guild_id != 0U ? dcc_cache_get_guild(client, guild_id) : NULL;
}

const dcc_channel_t *dcc_ctx_channel(const dcc_ctx_t *ctx) {
    return dcc_ctx_cached_channel(ctx);
}

const dcc_user_t *dcc_ctx_resolved_user(const dcc_ctx_t *ctx, dcc_snowflake_t id) {
    if (ctx == NULL || id == 0U) {
        return NULL;
    }

    const dcc_user_t *user = dcc_interaction_resolved_user(ctx->interaction, id);
    if (user != NULL) {
        return user;
    }

    dcc_client_t *client = dcc_ctx_client(ctx);
    return client != NULL ? dcc_cache_get_user(client, id) : NULL;
}

const dcc_member_t *dcc_ctx_resolved_member(const dcc_ctx_t *ctx, dcc_snowflake_t user_id) {
    if (ctx == NULL || user_id == 0U) {
        return NULL;
    }

    const dcc_member_t *member = dcc_interaction_resolved_member(ctx->interaction, user_id);
    if (member != NULL) {
        return member;
    }

    dcc_client_t *client = dcc_ctx_client(ctx);
    dcc_snowflake_t guild_id = dcc_ctx_guild_id(ctx);
    return client != NULL && guild_id != 0U ? dcc_cache_get_member(client, guild_id, user_id) : NULL;
}

const dcc_role_t *dcc_ctx_resolved_role(const dcc_ctx_t *ctx, dcc_snowflake_t id) {
    if (ctx == NULL || id == 0U) {
        return NULL;
    }

    const dcc_role_t *role = dcc_interaction_resolved_role(ctx->interaction, id);
    if (role != NULL) {
        return role;
    }

    dcc_client_t *client = dcc_ctx_client(ctx);
    dcc_snowflake_t guild_id = dcc_ctx_guild_id(ctx);
    return client != NULL && guild_id != 0U ? dcc_cache_get_role(client, guild_id, id) : NULL;
}

const dcc_channel_t *dcc_ctx_resolved_channel(const dcc_ctx_t *ctx, dcc_snowflake_t id) {
    if (ctx == NULL || id == 0U) {
        return NULL;
    }

    const dcc_channel_t *channel = dcc_interaction_resolved_channel(ctx->interaction, id);
    if (channel != NULL) {
        return channel;
    }

    dcc_client_t *client = dcc_ctx_client(ctx);
    return client != NULL ? dcc_cache_get_channel(client, id) : NULL;
}

const dcc_message_t *dcc_ctx_resolved_message(const dcc_ctx_t *ctx, dcc_snowflake_t id) {
    if (ctx == NULL || id == 0U) {
        return NULL;
    }

    const dcc_message_t *message = dcc_interaction_resolved_message(ctx->interaction, id);
    if (message != NULL) {
        return message;
    }

    dcc_client_t *client = dcc_ctx_client(ctx);
    return client != NULL ? dcc_cache_get_message(client, id) : NULL;
}

const dcc_attachment_t *dcc_ctx_resolved_attachment(const dcc_ctx_t *ctx, dcc_snowflake_t id) {
    return ctx != NULL && id != 0U
        ? dcc_interaction_resolved_attachment(ctx->interaction, id)
        : NULL;
}

const dcc_user_t *dcc_ctx_user(const dcc_ctx_t *ctx) {
    return dcc_ctx_resolved_user(ctx, dcc_ctx_user_id(ctx));
}

const dcc_member_t *dcc_ctx_member(const dcc_ctx_t *ctx) {
    return dcc_ctx_resolved_member(ctx, dcc_ctx_user_id(ctx));
}

const dcc_user_t *dcc_ctx_target_user(const dcc_ctx_t *ctx) {
    return dcc_ctx_resolved_user(ctx, dcc_ctx_target_id(ctx));
}

const dcc_member_t *dcc_ctx_target_member(const dcc_ctx_t *ctx) {
    return dcc_ctx_resolved_member(ctx, dcc_ctx_target_id(ctx));
}

const dcc_role_t *dcc_ctx_target_role(const dcc_ctx_t *ctx) {
    return dcc_ctx_resolved_role(ctx, dcc_ctx_target_id(ctx));
}

const dcc_channel_t *dcc_ctx_target_channel(const dcc_ctx_t *ctx) {
    return dcc_ctx_resolved_channel(ctx, dcc_ctx_target_id(ctx));
}

const dcc_message_t *dcc_ctx_target_message(const dcc_ctx_t *ctx) {
    return dcc_ctx_resolved_message(ctx, dcc_ctx_target_id(ctx));
}

uint8_t dcc_ctx_has_member_permissions(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->has_member_permissions : 0U;
}

dcc_permission_t dcc_ctx_member_permissions(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->member_permissions : 0U;
}

uint8_t dcc_ctx_member_has_permissions(const dcc_ctx_t *ctx, dcc_permission_t permissions) {
    return dcc_ctx_has_member_permissions(ctx) &&
        dcc_permissions_can(dcc_ctx_member_permissions(ctx), permissions)
        ? 1U
        : 0U;
}

static uint8_t dcc_ctx_member_role_list_has(
    const dcc_snowflake_t *role_ids,
    size_t role_id_count,
    dcc_snowflake_t role_id
) {
    if (role_ids == NULL || role_id_count == 0U || role_id == 0U) {
        return 0U;
    }
    for (size_t i = 0U; i < role_id_count; ++i) {
        if (role_ids[i] == role_id) {
            return 1U;
        }
    }
    return 0U;
}

static uint8_t dcc_ctx_member_object_has_role(const dcc_member_t *member, dcc_snowflake_t role_id) {
    return member != NULL
        ? dcc_ctx_member_role_list_has(member->role_ids, member->role_ids_count, role_id)
        : 0U;
}

uint8_t dcc_ctx_member_has_role(const dcc_ctx_t *ctx, dcc_snowflake_t role_id) {
    if (ctx == NULL || ctx->interaction == NULL || role_id == 0U) {
        return 0U;
    }

    dcc_snowflake_t user_id = dcc_ctx_user_id(ctx);
    dcc_snowflake_t guild_id = dcc_ctx_guild_id(ctx);
    const dcc_interaction_t *interaction = ctx->interaction;
    for (size_t i = 0U; interaction->resolved.members != NULL && i < interaction->resolved.members_count; ++i) {
        const dcc_member_t *member = &interaction->resolved.members[i];
        if (member->user.id == user_id &&
            (member->guild_id == 0U || guild_id == 0U || member->guild_id == guild_id) &&
            dcc_ctx_member_object_has_role(member, role_id)) {
            return 1U;
        }
    }

    dcc_client_t *client = dcc_ctx_client(ctx);
    const dcc_member_t *cached_member =
        client != NULL ? dcc_cache_get_member(client, guild_id, user_id) : NULL;
    return dcc_ctx_member_object_has_role(cached_member, role_id);
}

uint8_t dcc_ctx_member_has_any_role(
    const dcc_ctx_t *ctx,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
) {
    if (role_ids == NULL || role_id_count == 0U) {
        return 0U;
    }
    for (size_t i = 0U; i < role_id_count; ++i) {
        if (dcc_ctx_member_has_role(ctx, role_ids[i])) {
            return 1U;
        }
    }
    return 0U;
}

uint8_t dcc_ctx_member_has_all_roles(
    const dcc_ctx_t *ctx,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
) {
    if (role_ids == NULL || role_id_count == 0U) {
        return 0U;
    }
    for (size_t i = 0U; i < role_id_count; ++i) {
        if (!dcc_ctx_member_has_role(ctx, role_ids[i])) {
            return 0U;
        }
    }
    return 1U;
}

const dcc_component_session_result_t *dcc_ctx_component_session_result(const dcc_ctx_t *ctx) {
    return ctx != NULL ? ctx->component_session : NULL;
}

dcc_component_session_verify_status_t dcc_ctx_component_session_status(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->component_session != NULL
        ? ctx->component_session->status
        : DCC_COMPONENT_SESSION_VERIFY_NOT_FOUND;
}

const char *dcc_ctx_component_session_action(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->component_session != NULL ? ctx->component_session->action : NULL;
}

const char *dcc_ctx_token(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->token : NULL;
}

const char *dcc_ctx_name(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->name : NULL;
}

const char *dcc_ctx_custom_id(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL ? ctx->interaction->custom_id : NULL;
}

static dcc_interaction_flow_state_t dcc_ctx_auto_deferred_flow_state(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->auto_defer != NULL && ctx->auto_defer->ephemeral
        ? DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL
        : DCC_INTERACTION_FLOW_DEFERRED;
}

dcc_interaction_flow_state_t dcc_ctx_response_state(const dcc_ctx_t *ctx) {
    if (ctx == NULL) {
        return DCC_INTERACTION_FLOW_FAILED;
    }

    dcc_app_response_state_t auto_state = dcc_app_auto_defer_response_state(ctx);
    switch (auto_state) {
        case DCC_APP_RESPONSE_READY:
            return dcc_flow_state(&ctx->flow);
        case DCC_APP_RESPONSE_DEFERRED:
            return dcc_ctx_auto_deferred_flow_state(ctx);
        case DCC_APP_RESPONSE_REPLIED:
            return ctx->flow.state != DCC_INTERACTION_FLOW_READY
                ? ctx->flow.state
                : DCC_INTERACTION_FLOW_REPLIED;
        case DCC_APP_RESPONSE_FAILED:
            return DCC_INTERACTION_FLOW_FAILED;
    }

    return DCC_INTERACTION_FLOW_FAILED;
}

const char *dcc_ctx_response_state_string(const dcc_ctx_t *ctx) {
    return dcc_flow_state_string(dcc_ctx_response_state(ctx));
}

uint8_t dcc_ctx_response_sent(const dcc_ctx_t *ctx) {
    dcc_interaction_flow_state_t state = dcc_ctx_response_state(ctx);
    return state != DCC_INTERACTION_FLOW_READY && state != DCC_INTERACTION_FLOW_FAILED ? 1U : 0U;
}

uint8_t dcc_ctx_deferred(const dcc_ctx_t *ctx) {
    dcc_interaction_flow_state_t state = dcc_ctx_response_state(ctx);
    return state == DCC_INTERACTION_FLOW_DEFERRED ||
        state == DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
        state == DCC_INTERACTION_FLOW_DEFERRED_UPDATE
        ? 1U
        : 0U;
}

uint8_t dcc_ctx_replied(const dcc_ctx_t *ctx) {
    dcc_interaction_flow_state_t state = dcc_ctx_response_state(ctx);
    return state == DCC_INTERACTION_FLOW_REPLIED ||
        state == DCC_INTERACTION_FLOW_ORIGINAL_EDITED ||
        state == DCC_INTERACTION_FLOW_FOLLOWED_UP ||
        state == DCC_INTERACTION_FLOW_MODAL
        ? 1U
        : 0U;
}

uint8_t dcc_ctx_followed_up(const dcc_ctx_t *ctx) {
    return dcc_ctx_response_state(ctx) == DCC_INTERACTION_FLOW_FOLLOWED_UP ? 1U : 0U;
}

uint8_t dcc_ctx_response_failed(const dcc_ctx_t *ctx) {
    return dcc_ctx_response_state(ctx) == DCC_INTERACTION_FLOW_FAILED ? 1U : 0U;
}

uint8_t dcc_ctx_can_followup(const dcc_ctx_t *ctx) {
    dcc_interaction_flow_state_t state = dcc_ctx_response_state(ctx);
    return state != DCC_INTERACTION_FLOW_READY && state != DCC_INTERACTION_FLOW_FAILED ? 1U : 0U;
}

uint8_t dcc_ctx_can_edit_original(const dcc_ctx_t *ctx) {
    dcc_interaction_flow_state_t state = dcc_ctx_response_state(ctx);
    return state == DCC_INTERACTION_FLOW_DEFERRED ||
        state == DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
        state == DCC_INTERACTION_FLOW_DEFERRED_UPDATE ||
        state == DCC_INTERACTION_FLOW_REPLIED ||
        state == DCC_INTERACTION_FLOW_ORIGINAL_EDITED
        ? 1U
        : 0U;
}

static dcc_status_t dcc_ctx_flow_reply_auto(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (ctx->auto_defer == NULL) {
        return dcc_flow_reply(&ctx->flow, message, cb, user_data);
    }

    dcc_app_response_state_t response_state = dcc_app_auto_defer_response_state(ctx);
    if (response_state == DCC_APP_RESPONSE_READY) {
        dcc_status_t status = dcc_app_auto_defer_claim_initial(ctx, DCC_APP_RESPONSE_REPLIED);
        if (status != DCC_OK) {
            return dcc_ctx_flow_reply_auto(ctx, message, cb, user_data);
        }
        ctx->flow.state = DCC_INTERACTION_FLOW_READY;
        status = dcc_flow_reply(&ctx->flow, message, cb, user_data);
        dcc_app_auto_defer_mark(ctx, DCC_APP_RESPONSE_REPLIED, status);
        return status;
    }
    if (response_state == DCC_APP_RESPONSE_DEFERRED) {
        ctx->flow.state = dcc_ctx_auto_deferred_flow_state(ctx);
        dcc_status_t status = dcc_flow_reply(&ctx->flow, message, cb, user_data);
        dcc_app_auto_defer_mark(ctx, DCC_APP_RESPONSE_REPLIED, status);
        return status;
    }
    if (response_state == DCC_APP_RESPONSE_REPLIED) {
        ctx->flow.state = DCC_INTERACTION_FLOW_REPLIED;
        return dcc_flow_reply(&ctx->flow, message, cb, user_data);
    }
    return DCC_ERR_STATE;
}

static dcc_status_t dcc_ctx_claim_initial_auto(
    dcc_ctx_t *ctx,
    dcc_app_response_state_t response_state
) {
    if (ctx == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (ctx->auto_defer == NULL) {
        return DCC_OK;
    }
    return dcc_app_auto_defer_claim_initial(ctx, response_state);
}

dcc_status_t dcc_ctx_reply(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_ctx_flow_reply_auto(ctx, message, cb, user_data);
}

dcc_status_t dcc_ctx_reply_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U,
    };
    return dcc_ctx_flow_reply_auto(ctx, &message, cb, user_data);
}

dcc_status_t dcc_ctx_reply_ephemeral_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .content = content,
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL,
        .has_content = 1U,
        .has_flags = 1U,
    };
    return dcc_ctx_flow_reply_auto(ctx, &message, cb, user_data);
}

dcc_status_t dcc_ctx_reply_embed(
    dcc_ctx_t *ctx,
    const char *title,
    const char *description,
    uint32_t color,
    uint8_t ephemeral,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || (title == NULL && description == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_embed_builder_t embed = {
        .title = title,
        .description = description,
        .color = color,
        .has_color = 1U,
    };
    dcc_message_builder_t message = {
        .embeds = &embed,
        .embeds_count = 1U,
    };
    if (ephemeral) {
        message.flags = DCC_MESSAGE_FLAG_EPHEMERAL;
        message.has_flags = 1U;
    }
    return dcc_ctx_flow_reply_auto(ctx, &message, cb, user_data);
}

dcc_status_t dcc_ctx_reply_error(
    dcc_ctx_t *ctx,
    const char *title,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_ctx_reply_embed(
        ctx,
        title != NULL ? title : "Error",
        description,
        0xED4245U,
        1U,
        cb,
        user_data
    );
}

dcc_status_t dcc_ctx_reply_success(
    dcc_ctx_t *ctx,
    const char *title,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_ctx_reply_embed(
        ctx,
        title != NULL ? title : "Done",
        description,
        0x57F287U,
        1U,
        cb,
        user_data
    );
}

dcc_status_t dcc_ctx_defer(dcc_ctx_t *ctx, dcc_rest_cb cb, void *user_data) {
    dcc_status_t status = dcc_ctx_claim_initial_auto(ctx, DCC_APP_RESPONSE_DEFERRED);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_flow_defer(&ctx->flow, cb, user_data);
    dcc_app_auto_defer_mark(ctx, DCC_APP_RESPONSE_DEFERRED, status);
    return status;
}

dcc_status_t dcc_ctx_defer_ephemeral(dcc_ctx_t *ctx, dcc_rest_cb cb, void *user_data) {
    dcc_status_t status = dcc_ctx_claim_initial_auto(ctx, DCC_APP_RESPONSE_DEFERRED);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_flow_defer_ephemeral(&ctx->flow, cb, user_data);
    dcc_app_auto_defer_mark(ctx, DCC_APP_RESPONSE_DEFERRED, status);
    return status;
}

dcc_status_t dcc_ctx_update_message(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_ctx_claim_initial_auto(ctx, DCC_APP_RESPONSE_REPLIED);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_interaction_update_message(ctx->client, ctx->interaction, message, cb, user_data);
    ctx->flow.state = status == DCC_OK ? DCC_INTERACTION_FLOW_REPLIED : DCC_INTERACTION_FLOW_FAILED;
    dcc_app_auto_defer_mark(ctx, DCC_APP_RESPONSE_REPLIED, status);
    return status;
}

dcc_status_t dcc_ctx_show_modal(
    dcc_ctx_t *ctx,
    const dcc_modal_builder_t *modal,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_status_t status = dcc_ctx_claim_initial_auto(ctx, DCC_APP_RESPONSE_REPLIED);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_flow_show_modal(&ctx->flow, modal, cb, user_data);
    dcc_app_auto_defer_mark(ctx, DCC_APP_RESPONSE_REPLIED, status);
    return status;
}

dcc_status_t dcc_ctx_show_modal_flow(
    dcc_ctx_t *ctx,
    const dcc_app_modal_flow_t *flow,
    dcc_rest_cb cb,
    void *user_data
) {
    if (flow == NULL ||
        flow->size < offsetof(dcc_app_modal_flow_t, route) ||
        sizeof(flow->route) > flow->size - offsetof(dcc_app_modal_flow_t, route) ||
        !flow->modal.has_custom_id ||
        !flow->modal.has_title ||
        flow->modal.custom_id == NULL ||
        flow->modal.custom_id[0] == '\0' ||
        flow->modal.title == NULL ||
        flow->modal.title[0] == '\0' ||
        flow->route.custom_id == NULL ||
        flow->route.prefix != 0U ||
        strcmp(flow->modal.custom_id, flow->route.custom_id) != 0) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_ctx_show_modal(ctx, &flow->modal, cb, user_data);
}

dcc_status_t dcc_ctx_reply_autocomplete(
    dcc_ctx_t *ctx,
    const dcc_autocomplete_builder_t *autocomplete,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || autocomplete == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_interaction_response_create_autocomplete_from_interaction(
        ctx->client,
        ctx->interaction,
        autocomplete,
        cb,
        user_data
    );
}

dcc_status_t dcc_ctx_reply_autocomplete_matching(
    dcc_ctx_t *ctx,
    const dcc_autocomplete_choice_t *choices,
    size_t choices_count,
    size_t max_choices,
    dcc_rest_cb cb,
    void *user_data
) {
    if (max_choices > DCC_AUTOCOMPLETE_MAX_CHOICES) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_autocomplete_choice_t filtered[DCC_AUTOCOMPLETE_MAX_CHOICES];
    size_t filtered_count = 0U;
    dcc_status_t status = dcc_ctx_autocomplete_filter_choices(
        ctx,
        choices,
        choices_count,
        filtered,
        max_choices,
        &filtered_count
    );
    if (status != DCC_OK) {
        return status;
    }

    dcc_autocomplete_builder_t autocomplete = {
        .choices = filtered,
        .choices_count = filtered_count,
    };
    return dcc_ctx_reply_autocomplete(ctx, &autocomplete, cb, user_data);
}

dcc_status_t dcc_ctx_edit_original(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_app_auto_defer_response_state(ctx) == DCC_APP_RESPONSE_DEFERRED) {
        ctx->flow.state = dcc_ctx_auto_deferred_flow_state(ctx);
    }
    dcc_status_t status = dcc_flow_edit_original(&ctx->flow, message, cb, user_data);
    dcc_app_auto_defer_mark(ctx, DCC_APP_RESPONSE_REPLIED, status);
    return status;
}

dcc_status_t dcc_ctx_delete_original(dcc_ctx_t *ctx, dcc_rest_cb cb, void *user_data) {
    return ctx != NULL
        ? dcc_rest_interaction_original_response_delete_from_interaction(
            ctx->client,
            ctx->interaction,
            cb,
            user_data
        )
        : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_ctx_followup(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_response_state_t response_state = dcc_app_auto_defer_response_state(ctx);
    if (response_state == DCC_APP_RESPONSE_DEFERRED) {
        ctx->flow.state = dcc_ctx_auto_deferred_flow_state(ctx);
    } else if (response_state == DCC_APP_RESPONSE_REPLIED) {
        ctx->flow.state = DCC_INTERACTION_FLOW_REPLIED;
    }
    return dcc_flow_followup(&ctx->flow, message, cb, user_data);
}

dcc_status_t dcc_ctx_followup_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U,
    };
    return dcc_ctx_followup(ctx, &message, cb, user_data);
}

dcc_status_t dcc_ctx_followup_ephemeral_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .content = content,
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL,
        .has_content = 1U,
        .has_flags = 1U,
    };
    return dcc_ctx_followup(ctx, &message, cb, user_data);
}

static const dcc_interaction_option_t *dcc_ctx_find_option(
    const dcc_interaction_option_t *options,
    size_t count,
    const char *name
) {
    if (name == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < count; ++i) {
        if (options[i].name != NULL && strcmp(options[i].name, name) == 0) {
            return &options[i];
        }
        const dcc_interaction_option_t *child =
            dcc_ctx_find_option(options[i].options, options[i].options_count, name);
        if (child != NULL) {
            return child;
        }
    }
    return NULL;
}

const dcc_interaction_option_t *dcc_ctx_option(const dcc_ctx_t *ctx, const char *name) {
    return ctx != NULL && ctx->interaction != NULL
        ? dcc_ctx_find_option(ctx->interaction->options, ctx->interaction->options_count, name)
        : NULL;
}

const dcc_interaction_option_t *dcc_ctx_focused_option(const dcc_ctx_t *ctx) {
    return ctx != NULL ? dcc_interaction_focused_option(ctx->interaction) : NULL;
}

const char *dcc_ctx_focused_option_name(const dcc_ctx_t *ctx) {
    const dcc_interaction_option_t *option = dcc_ctx_focused_option(ctx);
    return option != NULL ? option->name : NULL;
}

const char *dcc_ctx_focused_option_string(const dcc_ctx_t *ctx, const char *fallback) {
    const dcc_interaction_option_t *option = dcc_ctx_focused_option(ctx);
    return option != NULL &&
           option->value_type == DCC_INTERACTION_OPTION_VALUE_STRING &&
           option->string_value != NULL
        ? option->string_value
        : fallback;
}

uint8_t dcc_ctx_option_present(const dcc_ctx_t *ctx, const char *name) {
    return dcc_ctx_option(ctx, name) != NULL;
}

const char *dcc_ctx_option_string(const dcc_ctx_t *ctx, const char *name, const char *fallback) {
    const dcc_interaction_option_t *option = dcc_ctx_option(ctx, name);
    return option != NULL && option->value_type == DCC_INTERACTION_OPTION_VALUE_STRING && option->string_value != NULL
        ? option->string_value
        : fallback;
}

int64_t dcc_ctx_option_int(const dcc_ctx_t *ctx, const char *name, int64_t fallback) {
    const dcc_interaction_option_t *option = dcc_ctx_option(ctx, name);
    return option != NULL && option->value_type == DCC_INTERACTION_OPTION_VALUE_INTEGER
        ? option->integer_value
        : fallback;
}

double dcc_ctx_option_number(const dcc_ctx_t *ctx, const char *name, double fallback) {
    const dcc_interaction_option_t *option = dcc_ctx_option(ctx, name);
    return option != NULL && option->value_type == DCC_INTERACTION_OPTION_VALUE_NUMBER
        ? option->number_value
        : fallback;
}

uint8_t dcc_ctx_option_bool(const dcc_ctx_t *ctx, const char *name, uint8_t fallback) {
    const dcc_interaction_option_t *option = dcc_ctx_option(ctx, name);
    return option != NULL && option->value_type == DCC_INTERACTION_OPTION_VALUE_BOOLEAN
        ? option->boolean_value
        : fallback;
}

dcc_snowflake_t dcc_ctx_option_snowflake(
    const dcc_ctx_t *ctx,
    const char *name,
    dcc_snowflake_t fallback
) {
    const dcc_interaction_option_t *option = dcc_ctx_option(ctx, name);
    return option != NULL && option->value_type == DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE
        ? option->snowflake_value
        : fallback;
}

static dcc_snowflake_t dcc_ctx_option_resolved_id(const dcc_ctx_t *ctx, const char *name) {
    const dcc_interaction_option_t *option = dcc_ctx_option(ctx, name);
    return option != NULL && option->value_type == DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE
        ? option->snowflake_value
        : 0U;
}

const dcc_user_t *dcc_ctx_option_user(const dcc_ctx_t *ctx, const char *name) {
    return dcc_ctx_resolved_user(ctx, dcc_ctx_option_resolved_id(ctx, name));
}

const dcc_member_t *dcc_ctx_option_member(const dcc_ctx_t *ctx, const char *name) {
    return dcc_ctx_resolved_member(ctx, dcc_ctx_option_resolved_id(ctx, name));
}

const dcc_role_t *dcc_ctx_option_role(const dcc_ctx_t *ctx, const char *name) {
    return dcc_ctx_resolved_role(ctx, dcc_ctx_option_resolved_id(ctx, name));
}

const dcc_channel_t *dcc_ctx_option_channel(const dcc_ctx_t *ctx, const char *name) {
    return dcc_ctx_resolved_channel(ctx, dcc_ctx_option_resolved_id(ctx, name));
}

const dcc_message_t *dcc_ctx_option_message(const dcc_ctx_t *ctx, const char *name) {
    return dcc_ctx_resolved_message(ctx, dcc_ctx_option_resolved_id(ctx, name));
}

const dcc_attachment_t *dcc_ctx_option_attachment(const dcc_ctx_t *ctx, const char *name) {
    return dcc_ctx_resolved_attachment(ctx, dcc_ctx_option_resolved_id(ctx, name));
}

static uint8_t dcc_ctx_autocomplete_ascii_lower(uint8_t ch) {
    return ch >= 'A' && ch <= 'Z' ? (uint8_t)(ch + ('a' - 'A')) : ch;
}

static uint8_t dcc_ctx_autocomplete_starts_with_ignore_case(
    const char *value,
    const char *prefix
) {
    if (value == NULL || prefix == NULL) {
        return 0U;
    }
    while (*prefix != '\0') {
        if (*value == '\0') {
            return 0U;
        }
        if (dcc_ctx_autocomplete_ascii_lower((uint8_t)*value) !=
            dcc_ctx_autocomplete_ascii_lower((uint8_t)*prefix)) {
            return 0U;
        }
        ++value;
        ++prefix;
    }
    return 1U;
}

static uint8_t dcc_ctx_autocomplete_choice_matches_query(
    const dcc_autocomplete_choice_t *choice,
    const char *query
) {
    if (choice == NULL) {
        return 0U;
    }
    if (query == NULL || query[0] == '\0') {
        return 1U;
    }
    if (dcc_ctx_autocomplete_starts_with_ignore_case(choice->name, query)) {
        return 1U;
    }
    return choice->value_type == DCC_AUTOCOMPLETE_CHOICE_STRING
        ? dcc_ctx_autocomplete_starts_with_ignore_case(choice->value_string, query)
        : 0U;
}

dcc_status_t dcc_ctx_autocomplete_filter_choices(
    const dcc_ctx_t *ctx,
    const dcc_autocomplete_choice_t *choices,
    size_t choices_count,
    dcc_autocomplete_choice_t *out_choices,
    size_t out_capacity,
    size_t *out_count
) {
    if (out_count == NULL ||
        (choices_count != 0U && choices == NULL) ||
        (out_capacity != 0U && out_choices == NULL) ||
        out_capacity > DCC_AUTOCOMPLETE_MAX_CHOICES) {
        return DCC_ERR_INVALID_ARG;
    }

    *out_count = 0U;
    const char *query = dcc_ctx_focused_option_string(ctx, "");
    for (size_t i = 0U; i < choices_count && *out_count < out_capacity; ++i) {
        if (dcc_ctx_autocomplete_choice_matches_query(&choices[i], query)) {
            out_choices[*out_count] = choices[i];
            (*out_count)++;
        }
    }
    return DCC_OK;
}

const dcc_interaction_form_field_t *dcc_ctx_form_field(const dcc_ctx_t *ctx, const char *custom_id) {
    if (ctx == NULL || ctx->interaction == NULL || custom_id == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < ctx->interaction->form_fields_count; ++i) {
        const dcc_interaction_form_field_t *field = &ctx->interaction->form_fields[i];
        if (field->custom_id != NULL && strcmp(field->custom_id, custom_id) == 0) {
            return field;
        }
    }
    return NULL;
}

const char *dcc_ctx_form_string(const dcc_ctx_t *ctx, const char *custom_id, const char *fallback) {
    const dcc_interaction_form_field_t *field = dcc_ctx_form_field(ctx, custom_id);
    return field != NULL && field->value_type == DCC_INTERACTION_FORM_VALUE_STRING && field->value != NULL
        ? field->value
        : fallback;
}

uint8_t dcc_ctx_form_bool(const dcc_ctx_t *ctx, const char *custom_id, uint8_t fallback) {
    const dcc_interaction_form_field_t *field = dcc_ctx_form_field(ctx, custom_id);
    return field != NULL && field->value_type == DCC_INTERACTION_FORM_VALUE_BOOLEAN
        ? field->boolean_value
        : fallback;
}

size_t dcc_ctx_form_values_count(const dcc_ctx_t *ctx, const char *custom_id) {
    const dcc_interaction_form_field_t *field = dcc_ctx_form_field(ctx, custom_id);
    return field != NULL &&
           field->value_type == DCC_INTERACTION_FORM_VALUE_VALUES &&
           field->values != NULL
        ? field->values_count
        : 0U;
}

const char *dcc_ctx_form_value(const dcc_ctx_t *ctx, const char *custom_id, size_t index) {
    const dcc_interaction_form_field_t *field = dcc_ctx_form_field(ctx, custom_id);
    if (field == NULL ||
        field->value_type != DCC_INTERACTION_FORM_VALUE_VALUES ||
        field->values == NULL ||
        index >= field->values_count) {
        return NULL;
    }
    return field->values[index];
}

const char *dcc_ctx_form_first_value(
    const dcc_ctx_t *ctx,
    const char *custom_id,
    const char *fallback
) {
    const char *value = dcc_ctx_form_value(ctx, custom_id, 0U);
    return value != NULL ? value : fallback;
}

const char **dcc_ctx_form_values(
    const dcc_ctx_t *ctx,
    const char *custom_id,
    size_t *out_count
) {
    const dcc_interaction_form_field_t *field = dcc_ctx_form_field(ctx, custom_id);
    if (field == NULL ||
        field->value_type != DCC_INTERACTION_FORM_VALUE_VALUES ||
        field->values == NULL) {
        if (out_count != NULL) {
            *out_count = 0U;
        }
        return NULL;
    }
    if (out_count != NULL) {
        *out_count = field->values_count;
    }
    return field->values;
}

size_t dcc_ctx_component_value_count(const dcc_ctx_t *ctx) {
    return ctx != NULL && ctx->interaction != NULL && ctx->interaction->values != NULL
        ? ctx->interaction->values_count
        : 0U;
}

const char *dcc_ctx_component_value(const dcc_ctx_t *ctx, size_t index) {
    if (ctx == NULL ||
        ctx->interaction == NULL ||
        ctx->interaction->values == NULL ||
        index >= ctx->interaction->values_count) {
        return NULL;
    }
    return ctx->interaction->values[index];
}

const char *dcc_ctx_component_first_value(const dcc_ctx_t *ctx, const char *fallback) {
    const char *value = dcc_ctx_component_value(ctx, 0U);
    return value != NULL ? value : fallback;
}

const char **dcc_ctx_component_values(const dcc_ctx_t *ctx, size_t *out_count) {
    if (ctx == NULL || ctx->interaction == NULL || ctx->interaction->values == NULL) {
        if (out_count != NULL) {
            *out_count = 0U;
        }
        return NULL;
    }
    if (out_count != NULL) {
        *out_count = ctx->interaction->values_count;
    }
    return ctx->interaction->values;
}
