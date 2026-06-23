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

static int dcc_app_string_invalid(const char *value) {
    return value == NULL || value[0] == '\0';
}

dcc_status_t dcc_app_get_current_application(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_current_application(dcc_app_client(app), cb, user_data);
}

dcc_status_t dcc_app_get_gateway_bot(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_gateway_bot(dcc_app_client(app), cb, user_data);
}

dcc_status_t dcc_app_get_skus(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || application_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_skus(dcc_app_client(app), application_id, cb, user_data);
}

dcc_status_t dcc_app_get_entitlements(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || application_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_entitlements(dcc_app_client(app), application_id, query, cb, user_data);
}

dcc_status_t dcc_app_get_entitlements_page(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_snowflake_t user_id,
    const dcc_snowflake_t *sku_ids,
    size_t sku_id_count,
    dcc_snowflake_t before_id,
    dcc_snowflake_t after_id,
    uint8_t limit,
    dcc_snowflake_t guild_id,
    uint8_t exclude_ended,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || application_id == 0U || (sku_ids == NULL && sku_id_count > 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_entitlements_page(
        dcc_app_client(app),
        application_id,
        user_id,
        sku_ids,
        sku_id_count,
        before_id,
        after_id,
        limit,
        guild_id,
        exclude_ended,
        cb,
        user_data
    );
}

dcc_status_t dcc_app_create_test_entitlement(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || application_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_test_entitlement(dcc_app_client(app), application_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_create_test_entitlement_params(
    dcc_app_t *app,
    const dcc_test_entitlement_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_test_entitlement_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_delete_test_entitlement(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || application_id == 0U || entitlement_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_test_entitlement(dcc_app_client(app), application_id, entitlement_id, cb, user_data);
}

dcc_status_t dcc_app_consume_entitlement(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || application_id == 0U || entitlement_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_consume_entitlement(dcc_app_client(app), application_id, entitlement_id, cb, user_data);
}

dcc_status_t dcc_app_get_current_user(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_current_user(dcc_app_client(app), cb, user_data);
}

dcc_status_t dcc_app_modify_current_user(
    dcc_app_t *app,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_current_user(dcc_app_client(app), json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_current_user_params(
    dcc_app_t *app,
    const dcc_current_user_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_current_user_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_get_user(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || user_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_user(dcc_app_client(app), user_id, cb, user_data);
}

dcc_status_t dcc_app_get_current_user_connections(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_current_user_connections(dcc_app_client(app), cb, user_data);
}

dcc_status_t dcc_app_get_current_user_guilds(
    dcc_app_t *app,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_current_user_guilds(dcc_app_client(app), query, cb, user_data);
}

dcc_status_t dcc_app_get_current_user_dms(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_current_user_dms(dcc_app_client(app), cb, user_data);
}

dcc_status_t dcc_app_create_dm_channel(
    dcc_app_t *app,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_dm_channel(dcc_app_client(app), json_body, cb, user_data);
}

dcc_status_t dcc_app_create_dm_channel_params(
    dcc_app_t *app,
    const dcc_dm_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_dm_channel_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_add_group_dm_recipient(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || user_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_add_group_dm_recipient(dcc_app_client(app), channel_id, user_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_add_group_dm_recipient_params(
    dcc_app_t *app,
    const dcc_group_dm_recipient_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_add_group_dm_recipient_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_remove_group_dm_recipient(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || user_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_remove_group_dm_recipient(dcc_app_client(app), channel_id, user_id, cb, user_data);
}

dcc_status_t dcc_app_create_direct_message(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || user_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_direct_message(dcc_app_client(app), user_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_create_direct_message_builder(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || user_id == 0U || message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_direct_message_builder(dcc_app_client(app), user_id, message, cb, user_data);
}

dcc_status_t dcc_app_create_direct_message_text(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
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
    return dcc_app_create_direct_message_builder(app, user_id, &message, cb, user_data);
}

dcc_status_t dcc_app_leave_guild(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_leave_guild(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_get_guild_invites(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_invites(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_get_invite(
    dcc_app_t *app,
    const char *invite_code,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || dcc_app_string_invalid(invite_code)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_invite(dcc_app_client(app), invite_code, query, cb, user_data);
}

dcc_status_t dcc_app_get_invite_full(
    dcc_app_t *app,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || dcc_app_string_invalid(invite_code)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_invite_full(dcc_app_client(app), invite_code, cb, user_data);
}

dcc_status_t dcc_app_delete_invite(
    dcc_app_t *app,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || dcc_app_string_invalid(invite_code)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_invite(dcc_app_client(app), invite_code, cb, user_data);
}

dcc_status_t dcc_app_create_stage_instance(
    dcc_app_t *app,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_stage_instance(dcc_app_client(app), json_body, cb, user_data);
}

dcc_status_t dcc_app_create_stage_instance_params(
    dcc_app_t *app,
    const dcc_stage_instance_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_stage_instance_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_get_stage_instance(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_stage_instance(dcc_app_client(app), channel_id, cb, user_data);
}

dcc_status_t dcc_app_modify_stage_instance(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_stage_instance(dcc_app_client(app), channel_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_stage_instance_params(
    dcc_app_t *app,
    const dcc_stage_instance_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_stage_instance_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_delete_stage_instance(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || channel_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_stage_instance(dcc_app_client(app), channel_id, cb, user_data);
}

dcc_status_t dcc_app_get_template(
    dcc_app_t *app,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || dcc_app_string_invalid(code)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_template(dcc_app_client(app), code, cb, user_data);
}

dcc_status_t dcc_app_create_guild_from_template(
    dcc_app_t *app,
    const char *code,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || dcc_app_string_invalid(code) || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_from_template(dcc_app_client(app), code, json_body, cb, user_data);
}

dcc_status_t dcc_app_create_guild_from_template_params(
    dcc_app_t *app,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_from_template_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_get_guild_templates(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_get_guild_templates(dcc_app_client(app), guild_id, cb, user_data);
}

dcc_status_t dcc_app_create_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_template(dcc_app_client(app), guild_id, json_body, cb, user_data);
}

dcc_status_t dcc_app_create_guild_template_params(
    dcc_app_t *app,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_template_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *code,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || dcc_app_string_invalid(code) || json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_template(dcc_app_client(app), guild_id, code, json_body, cb, user_data);
}

dcc_status_t dcc_app_modify_guild_template_params(
    dcc_app_t *app,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_modify_guild_template_params(dcc_app_client(app), params, cb, user_data);
}

dcc_status_t dcc_app_sync_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || dcc_app_string_invalid(code)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_sync_guild_template(dcc_app_client(app), guild_id, code, cb, user_data);
}

dcc_status_t dcc_app_delete_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
) {
    if (app == NULL || guild_id == 0U || dcc_app_string_invalid(code)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_delete_guild_template(dcc_app_client(app), guild_id, code, cb, user_data);
}

dcc_status_t dcc_ctx_add_member_role(
    dcc_ctx_t *ctx,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || dcc_ctx_app(ctx) == NULL || dcc_ctx_guild_id(ctx) == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_add_member_role(
        dcc_ctx_app(ctx),
        dcc_ctx_guild_id(ctx),
        user_id,
        role_id,
        cb,
        user_data
    );
}

dcc_status_t dcc_ctx_remove_member_role(
    dcc_ctx_t *ctx,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || dcc_ctx_app(ctx) == NULL || dcc_ctx_guild_id(ctx) == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_remove_member_role(
        dcc_ctx_app(ctx),
        dcc_ctx_guild_id(ctx),
        user_id,
        role_id,
        cb,
        user_data
    );
}

dcc_status_t dcc_ctx_create_thread_from_message(
    dcc_ctx_t *ctx,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL ||
        dcc_ctx_app(ctx) == NULL ||
        dcc_ctx_channel_id(ctx) == 0U ||
        dcc_ctx_message_id(ctx) == 0U ||
        params == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_create_thread_from_message(
        dcc_ctx_app(ctx),
        dcc_ctx_channel_id(ctx),
        dcc_ctx_message_id(ctx),
        params,
        cb,
        user_data
    );
}

dcc_status_t dcc_ctx_create_thread_from_message_name(
    dcc_ctx_t *ctx,
    const char *name,
    dcc_rest_cb cb,
    void *user_data
) {
    if (name == NULL || name[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_thread_params_t params = {
        .size = sizeof(params),
        .name = name,
    };
    return dcc_ctx_create_thread_from_message(ctx, &params, cb, user_data);
}

dcc_status_t dcc_ctx_archive_current_thread(
    dcc_ctx_t *ctx,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || dcc_ctx_app(ctx) == NULL || dcc_ctx_channel_id(ctx) == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_archive_thread(dcc_ctx_app(ctx), dcc_ctx_channel_id(ctx), cb, user_data);
}

dcc_status_t dcc_ctx_lock_current_thread(
    dcc_ctx_t *ctx,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || dcc_ctx_app(ctx) == NULL || dcc_ctx_channel_id(ctx) == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_lock_thread(dcc_ctx_app(ctx), dcc_ctx_channel_id(ctx), cb, user_data);
}

dcc_status_t dcc_ctx_unlock_current_thread(
    dcc_ctx_t *ctx,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || dcc_ctx_app(ctx) == NULL || dcc_ctx_channel_id(ctx) == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_unlock_thread(dcc_ctx_app(ctx), dcc_ctx_channel_id(ctx), cb, user_data);
}

dcc_status_t dcc_ctx_add_author_role(
    dcc_ctx_t *ctx,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || dcc_ctx_user_id(ctx) == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_ctx_add_member_role(ctx, dcc_ctx_user_id(ctx), role_id, cb, user_data);
}

dcc_status_t dcc_ctx_remove_author_role(
    dcc_ctx_t *ctx,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || dcc_ctx_user_id(ctx) == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_ctx_remove_member_role(ctx, dcc_ctx_user_id(ctx), role_id, cb, user_data);
}
