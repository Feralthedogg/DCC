#include <dcc/dcc.h>
#include <dcc/sugar.h>

#include "internal/client/dcc_client_state_internal.h"
#include "internal/events/dcc_event_state_internal.h"
#include "support/app_smoke_interactions.h"
#include "support/app_smoke_response_state.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct app_smoke_state {
    unsigned middleware_count;
    unsigned route_middleware_count;
    unsigned policy_middleware_count;
    unsigned policy_check_count;
    unsigned route_check_count;
    unsigned route_check_reject_count;
    unsigned group_middleware_count;
    unsigned generic_event_count;
    unsigned ready_count;
    unsigned message_count;
    unsigned slash_count;
    unsigned subcommand_count;
    unsigned autocomplete_count;
    unsigned button_count;
    unsigned check_button_count;
    unsigned cooldown_button_count;
    unsigned dm_only_button_count;
    unsigned nsfw_button_count;
    unsigned nsfw_reject_button_count;
    unsigned channel_type_button_count;
    unsigned channel_type_reject_button_count;
    unsigned role_button_count;
    unsigned role_reject_button_count;
    unsigned view_count;
    unsigned group_button_count;
    unsigned group_select_count;
    unsigned group_modal_count;
    unsigned group_view_count;
    unsigned modal_count;
    unsigned typed_slash_count;
    unsigned typed_subcommand_count;
    unsigned typed_autocomplete_count;
    unsigned typed_subcommand_autocomplete_count;
    unsigned typed_modal_count;
    unsigned typed_button_count;
    unsigned typed_select_count;
    unsigned session_count;
    unsigned error_count;
    unsigned module_setup_count;
    unsigned module_button_count;
    unsigned module_cleanup_count;
    unsigned extension_slash_count;
    unsigned extension_button_count;
    unsigned extension_ready_count;
    unsigned extension_message_update_count;
    unsigned app_state_cleanup_count;
    unsigned bad;
} app_smoke_state_t;

typedef struct app_smoke_slash_args {
    const char *name;
    int64_t count;
    double ratio;
    uint8_t flag;
    dcc_snowflake_t user;
    const dcc_user_t *user_object;
    const dcc_member_t *member_object;
    const dcc_role_t *role_object;
    const dcc_channel_t *channel_object;
    const dcc_message_t *message_object;
    const dcc_attachment_t *attachment_object;
    int64_t missing_count;
} app_smoke_slash_args_t;

typedef struct app_smoke_modal_args {
    const char *birthday;
    uint8_t public_birth;
    const char **tags;
    size_t tag_count;
    const char *fallback;
} app_smoke_modal_args_t;

typedef struct app_smoke_typed_slash_args {
    const char *name;
    int64_t limit;
} app_smoke_typed_slash_args_t;

typedef struct app_smoke_typed_modal_args {
    const char *birthday;
    uint8_t public_birth;
} app_smoke_typed_modal_args_t;

typedef struct app_smoke_typed_button_args {
    const char *action;
    uint32_t component_type;
} app_smoke_typed_button_args_t;

typedef struct app_smoke_typed_select_args {
    const char *choice;
    const char **values;
    size_t value_count;
} app_smoke_typed_select_args_t;

typedef struct app_smoke_infer_guild_state {
    unsigned called;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t guild_id;
    dcc_status_t status;
} app_smoke_infer_guild_state_t;

static void app_smoke_infer_guild_cb(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t guild_id,
    dcc_status_t status,
    void *user_data
) {
    (void)app;
    app_smoke_infer_guild_state_t *state = (app_smoke_infer_guild_state_t *)user_data;
    if (state == NULL) {
        return;
    }
    state->called++;
    state->channel_id = channel_id;
    state->guild_id = guild_id;
    state->status = status;
}

static dcc_status_t middleware(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->middleware_count++;
    if (dcc_ctx_app(ctx) == NULL ||
        dcc_ctx_client(ctx) == NULL ||
        dcc_ctx_flow(ctx) == NULL ||
        dcc_flow_state(dcc_ctx_flow(ctx)) != DCC_INTERACTION_FLOW_READY ||
        DCC_CTX_RESPONSE_STATE(ctx) != DCC_INTERACTION_FLOW_READY ||
        DCC_CTX_RESPONDED(ctx) ||
        DCC_CTX_DEFERRED(ctx) ||
        DCC_CTX_REPLIED(ctx) ||
        DCC_CTX_CAN_FOLLOWUP(ctx) ||
        DCC_CTX_CAN_EDIT_ORIGINAL(ctx) ||
        strcmp(DCC_CTX_RESPONSE_STATE_NAME(ctx), "ready") != 0) {
        state->bad = 1U;
        return DCC_ERR_STATE;
    }
    return DCC_OK;
}

static dcc_status_t route_middleware(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->route_middleware_count++;
    const char *action = DCC_CTX_ACTION(ctx, "page:");
    if (DCC_CTX_EVENT_TYPE(ctx) != DCC_EVENT_BUTTON_CLICK ||
        !DCC_CTX_HAS_CUSTOM_ID(ctx, "page:next") ||
        action == NULL ||
        strcmp(action, "next") != 0) {
        state->bad = 1U;
        return DCC_ERR_STATE;
    }
    return DCC_OK;
}

static dcc_status_t policy_middleware(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->policy_middleware_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        dcc_ctx_custom_id(ctx) == NULL ||
        strcmp(dcc_ctx_custom_id(ctx), "extension:open") != 0) {
        state->bad = 1U;
        return DCC_ERR_STATE;
    }
    return DCC_OK;
}

static dcc_status_t policy_check(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->policy_check_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "extension:open") != 0) {
        state->bad = 1U;
        return DCC_ERR_STATE;
    }
    return DCC_OK;
}

static dcc_status_t group_middleware(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->group_middleware_count++;
    if (!DCC_CTX_CUSTOM_ID_STARTS_WITH(ctx, "settings:")) {
        state->bad = 1U;
        return DCC_ERR_STATE;
    }
    return DCC_OK;
}

static void on_slash(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->slash_count++;
    const char *bound_name = NULL;
    int64_t bound_count = 0;
    double bound_ratio = 0.0;
    uint8_t bound_flag = 0U;
    dcc_snowflake_t bound_user = 0U;
    const dcc_user_t *bound_user_object = NULL;
    const dcc_member_t *bound_member_object = NULL;
    const dcc_role_t *bound_role_object = NULL;
    const dcc_channel_t *bound_channel_object = NULL;
    const dcc_message_t *bound_message_object = NULL;
    const dcc_attachment_t *bound_attachment_object = NULL;
    int64_t fallback_count = 0;
    dcc_status_t bind_status =
        DCC_CTX_BIND_OPTIONS(
            ctx,
            DCC_CTX_OPTION_REQUIRED_STRING("name", &bound_name),
            DCC_CTX_OPTION_REQUIRED_INT("count", &bound_count),
            DCC_CTX_OPTION_NUMBER("ratio", &bound_ratio, 0.0),
            DCC_CTX_OPTION_BOOL("flag", &bound_flag, 0U),
            DCC_CTX_OPTION_SNOWFLAKE("user", &bound_user, 0U),
            DCC_CTX_BIND_REQUIRED_USER("user", &bound_user_object),
            DCC_CTX_BIND_REQUIRED_MEMBER("user", &bound_member_object),
            DCC_CTX_BIND_REQUIRED_ROLE("role", &bound_role_object),
            DCC_CTX_BIND_REQUIRED_CHANNEL("channel", &bound_channel_object),
            DCC_CTX_BIND_REQUIRED_MESSAGE("message", &bound_message_object),
            DCC_CTX_BIND_REQUIRED_ATTACHMENT("attachment", &bound_attachment_object),
            DCC_CTX_OPTION_INT("missing_count", &fallback_count, 99)
        );
    const char *missing_required = NULL;
    dcc_status_t missing_status =
        DCC_CTX_BIND_OPTIONS(
            ctx,
            DCC_CTX_OPTION_REQUIRED_STRING("missing", &missing_required)
        );
    app_smoke_slash_args_t args;
    memset(&args, 0, sizeof(args));
    dcc_status_t field_bind_status =
        DCC_CTX_BIND_OPTION_FIELDS(
            ctx,
            &args,
            DCC_CTX_OPTION_FIELD_REQUIRED_STRING(app_smoke_slash_args_t, name, "name"),
            DCC_CTX_OPTION_FIELD_REQUIRED_INT(app_smoke_slash_args_t, count, "count"),
            DCC_CTX_OPTION_FIELD_NUMBER(app_smoke_slash_args_t, ratio, "ratio", 0.0),
            DCC_CTX_OPTION_FIELD_BOOL(app_smoke_slash_args_t, flag, "flag", 0U),
            DCC_CTX_OPTION_FIELD_SNOWFLAKE(app_smoke_slash_args_t, user, "user", 0U),
            DCC_CTX_OPTION_FIELD_REQUIRED_USER(app_smoke_slash_args_t, user_object, "user"),
            DCC_CTX_OPTION_FIELD_REQUIRED_MEMBER(app_smoke_slash_args_t, member_object, "user"),
            DCC_CTX_OPTION_FIELD_REQUIRED_ROLE(app_smoke_slash_args_t, role_object, "role"),
            DCC_CTX_OPTION_FIELD_REQUIRED_CHANNEL(app_smoke_slash_args_t, channel_object, "channel"),
            DCC_CTX_OPTION_FIELD_REQUIRED_MESSAGE(app_smoke_slash_args_t, message_object, "message"),
            DCC_CTX_OPTION_FIELD_REQUIRED_ATTACHMENT(
                app_smoke_slash_args_t,
                attachment_object,
                "attachment"
            ),
            DCC_CTX_OPTION_FIELD_INT(app_smoke_slash_args_t, missing_count, "missing_count", 99)
        );
    dcc_ctx_validation_error_t validation_error;
    dcc_status_t validation_status =
        DCC_CTX_VALIDATE_FIELDS(
            &args,
            &validation_error,
            DCC_VALIDATE_STRING_LENGTH(app_smoke_slash_args_t, name, "name", 1U, 16U),
            DCC_VALIDATE_INT_RANGE(app_smoke_slash_args_t, count, "count", 1, 10),
            DCC_VALIDATE_NUMBER_RANGE(app_smoke_slash_args_t, ratio, "ratio", 0.0, 1.0)
        );
    app_smoke_slash_args_t invalid_args = args;
    invalid_args.count = 99;
    dcc_ctx_validation_error_t invalid_error;
    dcc_status_t invalid_status =
        DCC_CTX_VALIDATE_FIELDS(
            &invalid_args,
            &invalid_error,
            DCC_VALIDATE_INT_RANGE(app_smoke_slash_args_t, count, "count", 1, 10)
        );
    const dcc_guild_t *ctx_guild = DCC_CTX_GUILD(ctx);
    const dcc_channel_t *ctx_channel = DCC_CTX_CHANNEL(ctx);
    const dcc_user_t *ctx_user = DCC_CTX_USER(ctx);
    const dcc_member_t *ctx_member = DCC_CTX_MEMBER(ctx);
    const dcc_user_t *option_user = DCC_CTX_OPT_USER(ctx, "user");
    const dcc_member_t *option_member = DCC_CTX_OPT_MEMBER(ctx, "user");
    const dcc_role_t *option_role = DCC_CTX_OPT_ROLE(ctx, "role");
    const dcc_channel_t *option_channel = DCC_CTX_OPT_CHANNEL(ctx, "channel");
    const dcc_message_t *option_message = DCC_CTX_OPT_MESSAGE(ctx, "message");
    const dcc_attachment_t *option_attachment = DCC_CTX_OPT_ATTACHMENT(ctx, "attachment");
    const dcc_user_t *target_user = DCC_CTX_TARGET_USER(ctx);
    const dcc_member_t *target_member = DCC_CTX_TARGET_MEMBER(ctx);
    if (DCC_CTX_EVENT_TYPE(ctx) != DCC_EVENT_SLASH_COMMAND ||
        strcmp(DCC_CTX_OPT_STRING(ctx, "name", ""), "feral") != 0 ||
        DCC_CTX_OPT_INT(ctx, "count", 0) != 7 ||
        DCC_CTX_OPT_NUMBER(ctx, "ratio", 0.0) < 0.24 ||
        DCC_CTX_OPT_NUMBER(ctx, "ratio", 0.0) > 0.26 ||
        DCC_CTX_OPT_BOOL(ctx, "flag", 0) != 1U ||
        DCC_CTX_OPT_SNOWFLAKE(ctx, "user", 0) != 42U ||
        DCC_CTX_OPT_PRESENT(ctx, "missing") != 0U ||
        bind_status != DCC_OK ||
        strcmp(bound_name, "feral") != 0 ||
        bound_count != 7 ||
        bound_ratio < 0.24 ||
        bound_ratio > 0.26 ||
        bound_flag != 1U ||
        bound_user != 42U ||
        bound_user_object != option_user ||
        bound_member_object != option_member ||
        bound_role_object != option_role ||
        bound_channel_object != option_channel ||
        bound_message_object != option_message ||
        bound_attachment_object != option_attachment ||
        fallback_count != 99 ||
        missing_status != DCC_ERR_NOT_FOUND ||
        field_bind_status != DCC_OK ||
        validation_status != DCC_OK ||
        validation_error.status != DCC_OK ||
        invalid_status != DCC_ERR_INVALID_ARG ||
        strcmp(invalid_error.field, "count") != 0 ||
        strcmp(args.name, "feral") != 0 ||
        args.count != 7 ||
        args.ratio < 0.24 ||
        args.ratio > 0.26 ||
        args.flag != 1U ||
        args.user != 42U ||
        args.user_object != option_user ||
        args.member_object != option_member ||
        args.role_object != option_role ||
        args.channel_object != option_channel ||
        args.message_object != option_message ||
        args.attachment_object != option_attachment ||
        args.missing_count != 99 ||
        DCC_CTX_CLIENT(ctx) == NULL ||
        DCC_CTX_APP(ctx) == NULL ||
        DCC_CTX_INTERACTION(ctx) == NULL ||
        DCC_CTX_INTERACTION_ID(ctx) != 100U ||
        DCC_CTX_APPLICATION_ID(ctx) != 200U ||
        DCC_CTX_GUILD_ID(ctx) != 300U ||
        DCC_CTX_CHANNEL_ID(ctx) != 400U ||
        DCC_CTX_COMMAND_ID(ctx) != 500U ||
        DCC_CTX_AUTHOR_ID(ctx) != 803U ||
        DCC_CTX_USER_ID(ctx) != 803U ||
        ctx_guild == NULL ||
        ctx_guild->id != 300U ||
        ctx_channel == NULL ||
        ctx_channel->id != 400U ||
        ctx_user == NULL ||
        ctx_user->id != 803U ||
        DCC_CTX_AUTHOR(ctx) != ctx_user ||
        ctx_member == NULL ||
        ctx_member->user.id != 803U ||
        option_user == NULL ||
        option_user->id != 42U ||
        strcmp(option_user->username, "option-user") != 0 ||
        option_member == NULL ||
        option_member->user.id != 42U ||
        strcmp(option_member->nick, "Option Member") != 0 ||
        option_role == NULL ||
        option_role->id != 910U ||
        strcmp(option_role->name, "Mod") != 0 ||
        option_channel == NULL ||
        option_channel->id != 401U ||
        strcmp(option_channel->name, "general") != 0 ||
        option_message == NULL ||
        option_message->id != 700U ||
        strcmp(option_message->content, "resolved message") != 0 ||
        option_attachment == NULL ||
        option_attachment->id != 88U ||
        strcmp(option_attachment->filename, "resolved.txt") != 0 ||
        strcmp(DCC_CTX_USER_MENTION(ctx), "<@803>") != 0 ||
        strcmp(DCC_CTX_AUTHOR_MENTION(ctx), "<@803>") != 0 ||
        strcmp(DCC_CTX_MEMBER_MENTION(ctx), "<@!803>") != 0 ||
        strcmp(DCC_CTX_CHANNEL_MENTION(ctx), "<#400>") != 0 ||
        strcmp(DCC_CTX_TARGET_MENTION(ctx), "<@42>") != 0 ||
        strcmp(DCC_CTX_TARGET_USER_MENTION(ctx), "<@42>") != 0 ||
        strcmp(DCC_CTX_TARGET_MEMBER_MENTION(ctx), "<@!42>") != 0 ||
        strcmp(DCC_CTX_TARGET_ROLE_MENTION(ctx), "<@&42>") != 0 ||
        strcmp(DCC_CTX_TARGET_CHANNEL_MENTION(ctx), "<#42>") != 0 ||
        strcmp(DCC_OPTION_MENTION(ctx, "user"), "<@42>") != 0 ||
        strcmp(DCC_OPTION_USER_MENTION(ctx, "user"), "<@42>") != 0 ||
        strcmp(DCC_OPTION_MEMBER_MENTION(ctx, "user"), "<@!42>") != 0 ||
        strcmp(DCC_OPTION_ROLE_MENTION(ctx, "role"), "<@&910>") != 0 ||
        strcmp(DCC_OPTION_CHANNEL_MENTION(ctx, "channel"), "<#401>") != 0 ||
        strcmp(DCC_PARAM_USER_MENTION(ctx, "user"), "<@42>") != 0 ||
        strcmp(DCC_PARAM_ROLE_MENTION(ctx, "role"), "<@&910>") != 0 ||
        DCC_CTX_OPTION_USER(ctx, "user") != option_user ||
        DCC_CTX_OPTION_MEMBER(ctx, "user") != option_member ||
        DCC_CTX_OPTION_ROLE(ctx, "role") != option_role ||
        DCC_CTX_OPTION_CHANNEL(ctx, "channel") != option_channel ||
        DCC_CTX_OPTION_MESSAGE(ctx, "message") != option_message ||
        DCC_CTX_OPTION_ATTACHMENT(ctx, "attachment") != option_attachment ||
        DCC_CTX_RESOLVED_USER(ctx, 42U) != option_user ||
        DCC_CTX_RESOLVED_MEMBER(ctx, 42U) != option_member ||
        DCC_CTX_RESOLVED_ROLE(ctx, 910U) != option_role ||
        DCC_CTX_RESOLVED_CHANNEL(ctx, 401U) != option_channel ||
        DCC_CTX_RESOLVED_MESSAGE(ctx, 700U) != option_message ||
        DCC_CTX_RESOLVED_ATTACHMENT(ctx, 88U) != option_attachment ||
        target_user != option_user ||
        target_member != option_member ||
        !DCC_CTX_IS_GUILD(ctx) ||
        DCC_CTX_IS_DM(ctx) ||
        !DCC_CTX_IS_AUTHOR(ctx, 803U) ||
        DCC_CTX_IS_AUTHOR(ctx, 0U) ||
        DCC_CTX_HAS_MEMBER_PERMISSIONS(ctx) != 1U ||
        DCC_CTX_MEMBER_PERMISSIONS(ctx) != DCC_PERMISSION_MANAGE_MESSAGES ||
        DCC_CTX_MEMBER_HAS_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_MESSAGES) != 1U ||
        DCC_CTX_HAS_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_MESSAGES) != 1U ||
        strcmp(DCC_CTX_TOKEN_OR(ctx, ""), "slash-token") != 0 ||
        strcmp(DCC_CTX_NAME_OR(ctx, ""), "hello") != 0 ||
        strcmp(DCC_CTX_COMMAND_NAME(ctx), "hello") != 0 ||
        DCC_CTX_USER_DATA(ctx) != user_data) {
        state->bad = 1U;
    }
}

static void on_subcommand(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->subcommand_count++;
    char path[64];
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_SLASH_COMMAND ||
        strcmp(dcc_ctx_name(ctx), "admin") != 0 ||
        strcmp(dcc_ctx_subcommand_group(ctx), "moderation") != 0 ||
        strcmp(dcc_ctx_subcommand(ctx), "ban") != 0 ||
        dcc_ctx_subcommand_path(ctx, path, sizeof(path)) != DCC_OK ||
        strcmp(path, "moderation/ban") != 0 ||
        strcmp(dcc_ctx_option_string(ctx, "reason", ""), "spam") != 0) {
        state->bad = 1U;
    }
}

static void on_generic_event(dcc_app_t *app, const dcc_event_t *event, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->generic_event_count++;
    if (app == NULL || event == NULL || dcc_event_type(event) != DCC_EVENT_RESUMED) {
        state->bad = 1U;
    }
}

static void on_ready_event(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->ready_count++;
    if (app == NULL || ready == NULL || ready->shard_id != 1U ||
        ready->shard_count != 2U || strcmp(ready->session_id, "ready-session") != 0) {
        state->bad = 1U;
    }
}

static void on_message_create_event(
    dcc_app_t *app,
    const dcc_message_t *message,
    const dcc_event_t *event,
    void *user_data
) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->message_count++;
    if (app == NULL || message == NULL || event == NULL ||
        dcc_event_type(event) != DCC_EVENT_MESSAGE_CREATE ||
        message->id != 700U ||
        strcmp(message->content, "hello app") != 0) {
        state->bad = 1U;
    }
}

static void on_extension_ready_event(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->extension_ready_count++;
    if (app == NULL || ready == NULL || ready->shard_id != 1U) {
        state->bad = 1U;
    }
}

static void on_extension_message_update_event(
    dcc_app_t *app,
    const dcc_message_t *message,
    const dcc_event_t *event,
    void *user_data
) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->extension_message_update_count++;
    if (app == NULL || message == NULL || event == NULL ||
        dcc_event_type(event) != DCC_EVENT_MESSAGE_UPDATE ||
        message->id != 701U ||
        strcmp(message->content, "hello extension") != 0) {
        state->bad = 1U;
    }
}

static void on_autocomplete(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->autocomplete_count++;
    const dcc_interaction_option_t *focused = dcc_interaction_focused_option(dcc_ctx_interaction(ctx));
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_AUTOCOMPLETE ||
        focused == NULL ||
        focused->name == NULL ||
        strcmp(focused->name, "name") != 0 ||
        strcmp(dcc_ctx_option_string(ctx, "name", ""), "fe") != 0 ||
        strcmp(dcc_ctx_name(ctx), "hello") != 0) {
        state->bad = 1U;
    }
}

static void on_extension_slash(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->extension_slash_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_SLASH_COMMAND ||
        strcmp(dcc_ctx_name(ctx), "exthello") != 0 ||
        dcc_ctx_user_data(ctx) != state) {
        state->bad = 1U;
    }
}

static void on_extension_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->extension_button_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "extension:open") != 0 ||
        dcc_ctx_user_data(ctx) != state) {
        state->bad = 1U;
    }
}

static void on_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->button_count++;
    const char *action = DCC_CTX_CUSTOM_ID_SUFFIX(ctx, "page:");
    if (DCC_CTX_EVENT_TYPE(ctx) != DCC_EVENT_BUTTON_CLICK ||
        DCC_CTX_INTERACTION(ctx) == NULL ||
        DCC_CTX_MESSAGE_ID(ctx) != 600U ||
        !DCC_CTX_CUSTOM_ID_STARTS_WITH(ctx, "page:") ||
        strcmp(DCC_CTX_CUSTOM_ID_OR(ctx, ""), "page:next") != 0 ||
        action == NULL ||
        strcmp(action, "next") != 0) {
        state->bad = 1U;
    }
}

static void on_cooldown_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->cooldown_button_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "cooldown:test") != 0 ||
        dcc_ctx_user_id(ctx) != 803U) {
        state->bad = 1U;
    }
}

static dcc_status_t allow_check(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->route_check_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "check:ok") != 0) {
        state->bad = 1U;
        return DCC_ERR_STATE;
    }
    return DCC_OK;
}

static dcc_status_t reject_check(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->route_check_reject_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "check:deny") != 0) {
        state->bad = 1U;
    }
    return DCC_ERR_STATE;
}

static void on_check_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->check_button_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "check:ok") != 0) {
        state->bad = 1U;
    }
}

static void on_rejected_check_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->bad = 1U;
    (void)ctx;
}

static void on_dm_only_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->dm_only_button_count++;
    state->bad = 1U;
    (void)ctx;
}

static void on_nsfw_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->nsfw_button_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "nsfw:ok") != 0 ||
        !dcc_ctx_channel_is_nsfw(ctx) ||
        !DCC_CTX_CHANNEL_IS_NSFW(ctx)) {
        state->bad = 1U;
    }
}

static void on_nsfw_rejected_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->nsfw_reject_button_count++;
    state->bad = 1U;
    (void)ctx;
}

static void on_channel_type_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    uint32_t channel_type = UINT32_MAX;
    state->channel_type_button_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "channel:text") != 0 ||
        dcc_ctx_channel_type(ctx, &channel_type) != DCC_OK ||
        channel_type != DCC_CHANNEL_TEXT ||
        !DCC_CTX_CHANNEL_IS_TYPE(ctx, DCC_CHANNEL_TEXT)) {
        state->bad = 1U;
    }
}

static void on_channel_type_rejected_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->channel_type_reject_button_count++;
    state->bad = 1U;
    (void)ctx;
}

static void on_role_button(dcc_ctx_t *ctx, void *user_data) {
    static const dcc_snowflake_t roles[] = { 910U, 911U };
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->role_button_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "role:ok") != 0 ||
        !dcc_ctx_member_has_role(ctx, 910U) ||
        !DCC_CTX_MEMBER_HAS_ROLE(ctx, 911U) ||
        !DCC_CTX_MEMBER_HAS_ALL_ROLES(ctx, roles, DCC_ARRAY_LEN(roles))) {
        state->bad = 1U;
    }
}

static void on_role_rejected_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->role_reject_button_count++;
    state->bad = 1U;
    (void)ctx;
}

static void on_module_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->module_button_count++;
    if (dcc_ctx_state(ctx) != state ||
        DCC_CTX_STATE(app_smoke_state_t, ctx) != state ||
        dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "module:ping") != 0) {
        state->bad = 1U;
    }
}

static void on_view_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->view_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "view:confirm") != 0) {
        state->bad = 1U;
    }
}

static void on_group_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->group_button_count++;
    if (dcc_ctx_user_data(ctx) != state ||
        dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "settings:open") != 0) {
        state->bad = 1U;
    }
}

static void on_group_select(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->group_select_count++;
    if (dcc_ctx_user_data(ctx) != state ||
        dcc_ctx_event_type(ctx) != DCC_EVENT_SELECT_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "settings:mode") != 0) {
        state->bad = 1U;
    }
}

static void on_group_modal(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->group_modal_count++;
    if (dcc_ctx_user_data(ctx) != state ||
        dcc_ctx_event_type(ctx) != DCC_EVENT_FORM_SUBMIT ||
        strcmp(dcc_ctx_custom_id(ctx), "settings:edit") != 0) {
        state->bad = 1U;
    }
}

static void on_group_view_button(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->group_view_count++;
    if (dcc_ctx_user_data(ctx) != state ||
        dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        strcmp(dcc_ctx_custom_id(ctx), "settings:view") != 0) {
        state->bad = 1U;
    }
}

static void on_modal(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->modal_count++;
    const char *birthday = NULL;
    uint8_t public_birth = 0U;
    const char **tags = NULL;
    size_t tag_count = 0U;
    const char *fallback = NULL;
    dcc_status_t bind_status =
        DCC_CTX_BIND_FORM(
            ctx,
            DCC_CTX_FORM_REQUIRED_STRING("birthday", &birthday),
            DCC_CTX_FORM_REQUIRED_BOOL("public", &public_birth),
            DCC_CTX_FORM_REQUIRED_VALUES("tags", &tags, &tag_count),
            DCC_CTX_FORM_STRING("missing", &fallback, "fallback")
        );
    const char *missing_required = NULL;
    dcc_status_t missing_status =
        DCC_CTX_BIND_FORM(
            ctx,
            DCC_CTX_FORM_REQUIRED_STRING("missing_required", &missing_required)
        );
    app_smoke_modal_args_t args;
    memset(&args, 0, sizeof(args));
    dcc_status_t field_bind_status =
        DCC_CTX_BIND_FORM_FIELDS(
            ctx,
            &args,
            DCC_CTX_FORM_FIELD_REQUIRED_STRING(app_smoke_modal_args_t, birthday, "birthday"),
            DCC_CTX_FORM_FIELD_REQUIRED_BOOL(app_smoke_modal_args_t, public_birth, "public"),
            DCC_CTX_FORM_FIELD_REQUIRED_VALUES(app_smoke_modal_args_t, tags, tag_count, "tags"),
            DCC_CTX_FORM_FIELD_STRING(app_smoke_modal_args_t, fallback, "missing", "fallback")
        );
    dcc_ctx_validation_error_t validation_error;
    dcc_status_t validation_status =
        DCC_CTX_VALIDATE_FIELDS(
            &args,
            &validation_error,
            DCC_VALIDATE_DATE_YYYYMMDD(app_smoke_modal_args_t, birthday, "birthday")
        );
    app_smoke_modal_args_t invalid_args = args;
    invalid_args.birthday = "2007";
    dcc_ctx_validation_error_t invalid_error;
    dcc_status_t invalid_status =
        DCC_CTX_VALIDATE_FIELDS(
            &invalid_args,
            &invalid_error,
            DCC_VALIDATE_DATE_YYYYMMDD(app_smoke_modal_args_t, birthday, "birthday")
        );
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_FORM_SUBMIT ||
        strcmp(dcc_ctx_form_string(ctx, "birthday", ""), "20070101") != 0 ||
        dcc_ctx_form_bool(ctx, "public", 0) != 1U ||
        bind_status != DCC_OK ||
        strcmp(birthday, "20070101") != 0 ||
        public_birth != 1U ||
        tags == NULL ||
        tag_count != 2U ||
        strcmp(tags[0], "alpha") != 0 ||
        strcmp(tags[1], "beta") != 0 ||
        strcmp(fallback, "fallback") != 0 ||
        missing_status != DCC_ERR_NOT_FOUND ||
        field_bind_status != DCC_OK ||
        validation_status != DCC_OK ||
        validation_error.status != DCC_OK ||
        invalid_status != DCC_ERR_INVALID_ARG ||
        strcmp(invalid_error.field, "birthday") != 0 ||
        strcmp(args.birthday, "20070101") != 0 ||
        args.public_birth != 1U ||
        args.tags == NULL ||
        args.tag_count != 2U ||
        strcmp(args.tags[0], "alpha") != 0 ||
        strcmp(args.tags[1], "beta") != 0 ||
        strcmp(args.fallback, "fallback") != 0) {
        state->bad = 1U;
    }
}

static void on_typed_slash(dcc_ctx_t *ctx, void *args, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    const app_smoke_typed_slash_args_t *typed_args =
        (const app_smoke_typed_slash_args_t *)args;
    state->typed_slash_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_SLASH_COMMAND ||
        typed_args == NULL ||
        typed_args->name == NULL ||
        strcmp(typed_args->name, "typed-feral") != 0 ||
        typed_args->limit != 12 ||
        dcc_ctx_user_data(ctx) != state) {
        state->bad = 1U;
    }
}

static void on_typed_subcommand(dcc_ctx_t *ctx, void *args, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    const app_smoke_typed_slash_args_t *typed_args =
        (const app_smoke_typed_slash_args_t *)args;
    char path[64];
    state->typed_subcommand_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_SLASH_COMMAND ||
        dcc_ctx_subcommand_path(ctx, path, sizeof(path)) != DCC_OK ||
        strcmp(path, "moderation/ban") != 0 ||
        typed_args == NULL ||
        typed_args->name == NULL ||
        strcmp(typed_args->name, "typed-sub-feral") != 0 ||
        typed_args->limit != 15 ||
        dcc_ctx_user_data(ctx) != state) {
        state->bad = 1U;
    }
}

static void on_typed_autocomplete(dcc_ctx_t *ctx, void *args, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    const app_smoke_typed_slash_args_t *typed_args =
        (const app_smoke_typed_slash_args_t *)args;
    state->typed_autocomplete_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_AUTOCOMPLETE ||
        dcc_ctx_focused_option_name(ctx) == NULL ||
        strcmp(dcc_ctx_focused_option_name(ctx), "name") != 0 ||
        typed_args == NULL ||
        typed_args->name == NULL ||
        strcmp(typed_args->name, "typed-auto-fe") != 0 ||
        typed_args->limit != 10 ||
        dcc_ctx_user_data(ctx) != state) {
        state->bad = 1U;
    }
}

static void on_typed_subcommand_autocomplete(dcc_ctx_t *ctx, void *args, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    const app_smoke_typed_slash_args_t *typed_args =
        (const app_smoke_typed_slash_args_t *)args;
    char path[64];
    state->typed_subcommand_autocomplete_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_AUTOCOMPLETE ||
        dcc_ctx_subcommand_path(ctx, path, sizeof(path)) != DCC_OK ||
        strcmp(path, "moderation/ban") != 0 ||
        dcc_ctx_focused_option_name(ctx) == NULL ||
        strcmp(dcc_ctx_focused_option_name(ctx), "name") != 0 ||
        typed_args == NULL ||
        typed_args->name == NULL ||
        strcmp(typed_args->name, "typed-sub-auto-fe") != 0 ||
        typed_args->limit != 18 ||
        dcc_ctx_user_data(ctx) != state) {
        state->bad = 1U;
    }
}

static void on_typed_modal(dcc_ctx_t *ctx, void *args, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    const app_smoke_typed_modal_args_t *typed_args =
        (const app_smoke_typed_modal_args_t *)args;
    state->typed_modal_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_FORM_SUBMIT ||
        typed_args == NULL ||
        typed_args->birthday == NULL ||
        strcmp(typed_args->birthday, "20080102") != 0 ||
        typed_args->public_birth != 0U ||
        dcc_ctx_user_data(ctx) != state) {
        state->bad = 1U;
    }
}

static void on_typed_button(dcc_ctx_t *ctx, void *args, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    const app_smoke_typed_button_args_t *typed_args =
        (const app_smoke_typed_button_args_t *)args;
    state->typed_button_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        typed_args == NULL ||
        typed_args->action == NULL ||
        strcmp(typed_args->action, "confirm") != 0 ||
        typed_args->component_type != 2U ||
        dcc_ctx_user_data(ctx) != state) {
        state->bad = 1U;
    }
}

static void on_typed_select(dcc_ctx_t *ctx, void *args, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    const app_smoke_typed_select_args_t *typed_args =
        (const app_smoke_typed_select_args_t *)args;
    state->typed_select_count++;
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_SELECT_CLICK ||
        typed_args == NULL ||
        typed_args->choice == NULL ||
        strcmp(typed_args->choice, "alpha") != 0 ||
        typed_args->values == NULL ||
        typed_args->value_count != 2U ||
        strcmp(typed_args->values[0], "alpha") != 0 ||
        strcmp(typed_args->values[1], "beta") != 0 ||
        dcc_ctx_user_data(ctx) != state) {
        state->bad = 1U;
    }
}

static void on_error(dcc_ctx_t *ctx, dcc_status_t status, const char *message, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->error_count++;
    if (ctx == NULL || status != DCC_ERR_STATE || message == NULL || message[0] == '\0') {
        state->bad = 1U;
    }
}

static void on_task(dcc_app_t *app, void *user_data) {
    (void)app;
    (void)user_data;
}

static void app_smoke_state_cleanup(void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->app_state_cleanup_count++;
}

static void app_smoke_module_cleanup(void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->module_cleanup_count++;
}

static dcc_status_t app_smoke_module_setup(dcc_app_t *app, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->module_setup_count++;
    if (dcc_app_state(app) != state || DCC_APP_STATE(app_smoke_state_t, app) != state) {
        state->bad = 1U;
        return DCC_ERR_STATE;
    }
    return dcc_app_button(app, "module:ping", on_module_button, state);
}

static void on_session(dcc_ctx_t *ctx, void *user_data) {
    app_smoke_state_t *state = (app_smoke_state_t *)user_data;
    state->session_count++;
    const dcc_component_session_result_t *result = dcc_ctx_component_session_result(ctx);
    if (dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        result == NULL ||
        dcc_ctx_component_session_status(ctx) != DCC_COMPONENT_SESSION_VERIFY_OK ||
        dcc_ctx_component_session_action(ctx) == NULL ||
        strcmp(dcc_ctx_component_session_action(ctx), "next") != 0 ||
        strcmp(result->session_id, "appsess") != 0 ||
        dcc_ctx_custom_id(ctx) == NULL ||
        dcc_ctx_custom_id(ctx)[0] == '\0') {
        state->bad = 1U;
    }
}

static dcc_status_t dispatch_interaction(
    dcc_app_t *app,
    dcc_event_type_t type,
    const dcc_interaction_t *interaction
) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = type;
    event.data.interaction = *interaction;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static dcc_status_t dispatch_event(dcc_app_t *app, const dcc_event_t *event) {
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), event);
}

static int app_smoke_write_file(const char *path, const char *content) {
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        return 0;
    }
    int ok = fputs(content, file) >= 0;
    return fclose(file) == 0 && ok;
}

static int app_smoke_check_dotenv(void) {
    const char first_path[] = "dcc_app_smoke_first.env";
    const char second_path[] = "dcc_app_smoke_second.env";
    const char app_path[] = "dcc_app_smoke_app.env";

    if (dcc_app_load_env_file(NULL, 1U) != DCC_ERR_INVALID_ARG ||
        dcc_app_load_env_file("dcc_app_smoke_missing.env", 1U) != DCC_ERR_NOT_FOUND) {
        return 0;
    }
    if (!app_smoke_write_file(
            first_path,
            "  DCC_APP_SMOKE_ENV = \"old value\" \n"
            "export DCC_APP_SMOKE_QUOTED='quoted value'\n"
            "# ignored\n"
        )) {
        return 0;
    }
    if (!app_smoke_write_file(second_path, "DCC_APP_SMOKE_ENV=new value\n")) {
        (void)remove(first_path);
        return 0;
    }
    if (!app_smoke_write_file(
            app_path,
            "DCC_APP_SMOKE_TOKEN=token\n"
            "DCC_APPLICATION_ID=123456789\n"
            "DCC_INTENTS=all\n"
            "DCC_INFER_GUILD_FROM_CHANNEL=true\n"
            "DCC_COMMAND_GUILD_ID=987654321\n"
            "DCC_COMMAND_SYNC=plan\n"
            "DCC_COMMAND_SYNC_DELETE_STALE=true\n"
            "DCC_AUTO_DEFER_MS=1750\n"
            "DCC_AUTO_DEFER_EPHEMERAL=true\n"
            "DCC_STORE_FILE=dcc_app_smoke_store.kv\n"
            "DCC_APP_SMOKE_U32=42\n"
            "DCC_APP_SMOKE_U32_BAD=999\n"
            "DCC_APP_SMOKE_INTENTS=guilds,message_content+voice\n"
            "DCC_APP_SMOKE_INTENTS_HEX=0x201\n"
            "DCC_APP_SMOKE_INTENTS_BAD=wat\n"
        )) {
        (void)remove(first_path);
        (void)remove(second_path);
        return 0;
    }

    int ok = dcc_app_load_env_file(first_path, 1U) == DCC_OK &&
        getenv("DCC_APP_SMOKE_ENV") != NULL &&
        strcmp(getenv("DCC_APP_SMOKE_ENV"), "old value") == 0 &&
        getenv("DCC_APP_SMOKE_QUOTED") != NULL &&
        strcmp(getenv("DCC_APP_SMOKE_QUOTED"), "quoted value") == 0 &&
        dcc_app_load_env_file(second_path, 0U) == DCC_OK &&
        strcmp(getenv("DCC_APP_SMOKE_ENV"), "old value") == 0 &&
        dcc_app_load_env_file(second_path, 1U) == DCC_OK &&
        strcmp(getenv("DCC_APP_SMOKE_ENV"), "new value") == 0;

    dcc_app_options_t env_options;
    const char *resolved_token = NULL;
    const char *resolved_sugar_token = NULL;
    uint32_t direct_u32 = 0U;
    uint32_t direct_u32_or = 0U;
    uint32_t range_u32 = 0U;
    uint32_t missing_range_u32 = 0U;
    dcc_intents_t direct_intents = 0U;
    dcc_intents_t hex_intents = 0U;
    dcc_intents_t missing_intents = 0U;
    dcc_intents_t bound_intents = 0U;
    dcc_app_env_binding_t intents_binding =
        DCC_CONFIG_INTENTS_DEFAULT("DCC_APP_SMOKE_INTENTS", DCC_INTENTS_DEFAULT, &bound_intents);
    dcc_intents_t expected_intents =
        DCC_INTENTS_MESSAGES | DCC_INTENT_MESSAGE_CONTENT | DCC_INTENT_GUILD_VOICE_STATES;
    ok = ok &&
        dcc_app_load_env_file(app_path, 1U) == DCC_OK &&
        dcc_app_env_get_token("DCC_APP_SMOKE_TOKEN", &resolved_token) == DCC_OK &&
        resolved_token != NULL &&
        strcmp(resolved_token, "token") == 0 &&
        DCC_ENV_TOKEN_NAMED("DCC_APP_SMOKE_TOKEN", &resolved_sugar_token) == DCC_OK &&
        resolved_sugar_token != NULL &&
        strcmp(resolved_sugar_token, "token") == 0 &&
        dcc_app_env_get_u32("DCC_APP_SMOKE_U32", &direct_u32) == DCC_OK &&
        direct_u32 == 42U &&
        DCC_ENV_U32_OR("DCC_APP_SMOKE_MISSING_U32", 55U, &direct_u32_or) == DCC_OK &&
        direct_u32_or == 55U &&
        DCC_ENV_U32_RANGE("DCC_APP_SMOKE_U32", 1U, 100U, &range_u32) == DCC_OK &&
        range_u32 == 42U &&
        DCC_ENV_U32_RANGE_OR("DCC_APP_SMOKE_MISSING_U32_RANGE", 77U, 1U, 100U, &missing_range_u32) == DCC_OK &&
        missing_range_u32 == 77U &&
        DCC_ENV_INTENTS("DCC_APP_SMOKE_INTENTS", &direct_intents) == DCC_OK &&
        direct_intents == expected_intents &&
        DCC_ENV_INTENTS("DCC_APP_SMOKE_INTENTS_HEX", &hex_intents) == DCC_OK &&
        hex_intents == (DCC_INTENT_GUILDS | DCC_INTENT_GUILD_MESSAGES) &&
        DCC_ENV_INTENTS_OR("DCC_APP_SMOKE_MISSING_INTENTS", DCC_INTENT_GUILDS, &missing_intents) == DCC_OK &&
        missing_intents == DCC_INTENT_GUILDS &&
        DCC_ENV_INTENTS("DCC_APP_SMOKE_INTENTS_BAD", &direct_intents) == DCC_ERR_INVALID_ARG &&
        dcc_app_env_bind(&intents_binding, 1U) == DCC_OK &&
        bound_intents == expected_intents &&
        dcc_app_env_get_u32_range("DCC_APP_SMOKE_U32_BAD", 1U, 100U, &range_u32) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_options_from_env(&env_options, "DCC_APP_SMOKE_TOKEN") == DCC_OK &&
        env_options.client.token != NULL &&
        strcmp(env_options.client.token, "token") == 0 &&
        env_options.client.intents == DCC_INTENTS_ALL &&
        env_options.client.infer_guild_id_from_channel == 1U &&
        env_options.auto_defer_after_ms == 1750U &&
        env_options.auto_defer_ephemeral == 1U &&
        env_options.store_file_path != NULL &&
        strcmp(env_options.store_file_path, "dcc_app_smoke_store.kv") == 0 &&
        env_options.command_registry.guild_id == 987654321U &&
        env_options.command_sync_on_ready == 1U &&
        env_options.command_sync.application_id == 123456789U &&
        env_options.command_sync.apply == 0U &&
        env_options.command_sync.command_registry.guild_id == 987654321U &&
        env_options.command_sync.command_registry.delete_stale == 1U &&
        env_options.command_sync.command_registry.dry_run == 1U;

    (void)remove(first_path);
    (void)remove(second_path);
    (void)remove(app_path);
    return ok;
}

int main(void) {
    if (dcc_app_run_with_signals(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_run_defined_with_signals(NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_run_from_env_defined_with_signals(NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_run_dotenv_defined_with_signals(NULL, NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "signal runner invalid-argument contract failed\n");
        return 1;
    }
    if (!app_smoke_check_response_state()) {
        return 1;
    }

    if (!app_smoke_check_dotenv()) {
        fprintf(stderr, "dotenv smoke failed\n");
        return 1;
    }

    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;
    options.client.enable_cache = 1U;
    const char *runtime_store_path = "dcc_app_smoke_runtime_store.kv";
    (void)remove(runtime_store_path);
    (void)remove("dcc_app_smoke_runtime_store.kv.tmp");

    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create(&options, &app);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_app_create failed: %s\n", dcc_status_string(status));
        return 1;
    }
    if (dcc_app_store(NULL) != NULL ||
        dcc_app_store(app) != NULL ||
        dcc_app_store_open_file(NULL, runtime_store_path) != DCC_ERR_INVALID_ARG ||
        DCC_APP_STORE_FILE(app, runtime_store_path) != DCC_OK ||
        DCC_APP_STORE(app) == NULL ||
        DCC_STORE_SET_U64(DCC_APP_STORE(app), "app.store.count", 42U) != DCC_OK) {
        fprintf(stderr, "app store setup failed\n");
        dcc_app_destroy(app);
        (void)remove(runtime_store_path);
        (void)remove("dcc_app_smoke_runtime_store.kv.tmp");
        return 1;
    }
    uint64_t store_count = 0U;
    if (DCC_STORE_GET_U64(DCC_APP_STORE(app), "app.store.count", &store_count) != DCC_OK ||
        store_count != 42U) {
        fprintf(stderr, "app store round-trip failed\n");
        dcc_app_destroy(app);
        (void)remove(runtime_store_path);
        (void)remove("dcc_app_smoke_runtime_store.kv.tmp");
        return 1;
    }
    DCC_APP_STORE_CLOSE(app);
    if (DCC_APP_STORE(app) != NULL ||
        DCC_APP_STORE_FILE(app, runtime_store_path) != DCC_OK ||
        DCC_APP_STORE(app) == NULL) {
        fprintf(stderr, "app store reopen failed\n");
        dcc_app_destroy(app);
        (void)remove(runtime_store_path);
        (void)remove("dcc_app_smoke_runtime_store.kv.tmp");
        return 1;
    }
    dcc_guild_t guild = {
        .id = 300U,
        .name = "App Smoke Guild"
    };
    dcc_channel_t command_channel = {
        .id = 400U,
        .guild_id = 300U,
        .type = DCC_CHANNEL_TEXT,
        .name = "commands"
    };
    dcc_channel_t nsfw_channel = {
        .id = 401U,
        .guild_id = 300U,
        .type = DCC_CHANNEL_TEXT,
        .nsfw = 1U
    };
    dcc_channel_t sfw_channel = {
        .id = 402U,
        .guild_id = 300U,
        .type = DCC_CHANNEL_TEXT,
        .nsfw = 0U
    };
    dcc_channel_t voice_channel = {
        .id = 403U,
        .guild_id = 300U,
        .type = DCC_CHANNEL_VOICE
    };
    dcc_snowflake_t member_role_ids[] = { 910U, 911U };
    dcc_member_t member = {
        .guild_id = 300U,
        .user = { .id = 803U },
        .role_ids = member_role_ids,
        .role_ids_count = DCC_ARRAY_LEN(member_role_ids)
    };
    dcc_user_t user = {
        .id = 803U,
        .username = "author"
    };
    status = dcc_cache_put_guild(dcc_app_client(app), &guild);
    if (status == DCC_OK) {
        status = dcc_cache_put_channel(dcc_app_client(app), &command_channel);
    }
    if (status == DCC_OK) {
        status = dcc_cache_put_channel(dcc_app_client(app), &nsfw_channel);
    }
    if (status == DCC_OK) {
        status = dcc_cache_put_channel(dcc_app_client(app), &sfw_channel);
    }
    if (status == DCC_OK) {
        status = dcc_cache_put_channel(dcc_app_client(app), &voice_channel);
    }
    if (status == DCC_OK) {
        status = dcc_cache_put_member(dcc_app_client(app), &member);
    }
    if (status == DCC_OK) {
        status = dcc_cache_put_user(dcc_app_client(app), &user);
    }
    if (status != DCC_OK) {
        fprintf(stderr, "cache channel setup failed: %s\n", dcc_status_string(status));
        dcc_app_destroy(app);
        return 1;
    }
    app_smoke_infer_guild_state_t infer_state;
    memset(&infer_state, 0, sizeof(infer_state));
    status = DCC_INFER_GUILD_FROM_CHANNEL(app, command_channel.id, app_smoke_infer_guild_cb, &infer_state);
    if (status != DCC_OK ||
        infer_state.called != 1U ||
        infer_state.status != DCC_OK ||
        infer_state.channel_id != command_channel.id ||
        infer_state.guild_id != command_channel.guild_id) {
        fprintf(stderr, "app guild inference cache hit failed: %s\n", dcc_status_string(status));
        dcc_app_destroy(app);
        return 1;
    }

    app_smoke_state_t state;
    memset(&state, 0, sizeof(state));
    status = dcc_app_set_state(app, &state, app_smoke_state_cleanup);
    if (status != DCC_OK || dcc_app_state(app) != &state) {
        fprintf(stderr, "app state setup failed: %s\n", dcc_status_string(status));
        dcc_app_destroy(app);
        return 1;
    }
    dcc_app_module_t module =
        DCC_APP_MODULE_WITH_CLEANUP("smoke.module", app_smoke_module_setup, &state, app_smoke_module_cleanup);
    if (dcc_app_module_register(app, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_module_register(NULL, &module) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "app module invalid-arg check failed\n");
        dcc_app_destroy(app);
        return 1;
    }
    status = dcc_app_module_register(app, &module);
    if (status != DCC_OK ||
        dcc_app_module_register(app, &module) != DCC_ERR_STATE ||
        dcc_app_module_count(app) != 1U) {
        fprintf(stderr, "app module setup failed: %s\n", dcc_status_string(status));
        dcc_app_destroy(app);
        return 1;
    }

    const char secret[] = "app-session-secret";
    dcc_component_session_options_t session_options;
    dcc_component_session_options_init(&session_options);
    dcc_component_session_t session;
    memset(&session, 0, sizeof(session));
    status = dcc_component_session_options_set_secret(
        &session_options,
        secret,
        sizeof(secret) - 1U
    );
    if (status == DCC_OK) {
        status = dcc_component_session_options_set_ids(&session_options, "appsess", "nonce");
    }
    if (status == DCC_OK) {
        status = dcc_component_session_options_set_ttl(&session_options, 1000U, 10000U);
    }
    if (status == DCC_OK) {
        status = dcc_component_session_options_lock_user(&session_options, 803U);
    }
    if (status == DCC_OK) {
        status = dcc_component_session_options_lock_channel(&session_options, 400U);
    }
    if (status == DCC_OK) {
        status = dcc_component_session_options_lock_guild(&session_options, 300U);
    }
    if (status == DCC_OK) {
        status = dcc_component_session_create(&session_options, &session);
    }
    if (status != DCC_OK) {
        fprintf(stderr, "component session setup failed: %s\n", dcc_status_string(status));
        dcc_app_destroy(app);
        return 1;
    }

    status = dcc_app_auto_defer_ephemeral(app, 1500U);
    if (status == DCC_OK) {
        status = dcc_app_disable_auto_defer(app);
    }
    if (status == DCC_OK) {
        status = dcc_app_auto_defer(app, 1500U);
    }
    dcc_app_command_sync_options_t ready_sync;
    dcc_app_command_sync_options_init(&ready_sync);
    if (status == DCC_OK &&
        (dcc_app_sync_commands_on_ready(app, &ready_sync) != DCC_ERR_INVALID_ARG ||
         dcc_app_sync_commands_on_ready(NULL, &ready_sync) != DCC_ERR_INVALID_ARG)) {
        status = DCC_ERR_STATE;
    }
    ready_sync.application_id = 123U;
    if (status == DCC_OK) {
        status = dcc_app_sync_commands_on_ready(app, &ready_sync);
    }
    if (status == DCC_OK) {
        status = dcc_app_disable_command_sync_on_ready(app);
    }
    ready_sync.application_id = 0U;
    ready_sync.infer_application_id = 1U;
    if (status == DCC_OK) {
        status = dcc_app_sync_commands_on_ready(app, &ready_sync);
    }
    if (status == DCC_OK) {
        status = dcc_app_disable_command_sync_on_ready(app);
    }
    if (status == DCC_OK) {
        status = dcc_app_every_minutes(app, 5U, on_task, &state);
    }
    if (status == DCC_OK) {
        status = dcc_app_every_hours(app, 1U, on_task, &state);
    }
    if (status == DCC_OK) {
        status = dcc_app_every_kst(app, "00:00", on_task, &state);
    }
    if (status == DCC_OK) {
        status = dcc_app_on(app, DCC_EVENT_RESUMED, on_generic_event, &state);
    }
    if (status == DCC_OK) {
        status = dcc_app_on_ready(app, on_ready_event, &state);
    }
    if (status == DCC_OK) {
        status = dcc_app_on_message_create(app, on_message_create_event, &state);
    }
    if (status == DCC_OK) {
        status = dcc_app_on_error(app, on_error, &state);
    }
    dcc_app_extension_slash_command_t extension_commands[] = {
        DCC_EXTENSION_SLASH("exthello", "Extension hello", on_extension_slash, &state)
    };
    dcc_app_extension_component_route_t extension_components[] = {
        DCC_EXTENSION_BUTTON_POLICY(
            "extension:open",
            on_extension_button,
            &state,
            DCC_ROUTE_POLICY(
                DCC_ROUTE_MIDDLEWARE(DCC_EXTENSION_MIDDLEWARE(policy_middleware, &state)),
                DCC_ROUTE_GUILD_ONLY(),
                DCC_ROUTE_OWNER(803U),
                DCC_ROUTE_REQUIRED_PERMISSIONS(DCC_PERMISSION_MANAGE_MESSAGES),
                DCC_ROUTE_CHECK(policy_check, &state)
            )
        )
    };
    dcc_app_extension_event_t extension_events[] = {
        DCC_EXTENSION_READY(on_extension_ready_event, &state),
        DCC_EXTENSION_MESSAGE_UPDATE(on_extension_message_update_event, &state)
    };
    dcc_app_extension_task_t extension_tasks[] = {
        DCC_EXTENSION_TASK_SECONDS(30U, on_task, &state)
    };
    dcc_app_extension_t extension = DCC_APP_EXTENSION("smoke.extension");
    extension.slash_commands = extension_commands;
    extension.slash_command_count = DCC_ARRAY_LEN(extension_commands);
    extension.component_routes = extension_components;
    extension.component_route_count = DCC_ARRAY_LEN(extension_components);
    extension.events = extension_events;
    extension.event_count = DCC_ARRAY_LEN(extension_events);
    extension.tasks = extension_tasks;
    extension.task_count = DCC_ARRAY_LEN(extension_tasks);
    dcc_application_command_builder_t typed_command =
        DCC_SLASH_COMMAND_WITH_OPTIONS(
            "typed",
            "Typed slash command",
            DCC_COMMAND_STRING_OPTION("name", "Name", 1U),
            DCC_COMMAND_INTEGER_OPTION_RANGE("limit", "Limit", 0U, 1, 25)
        );
    dcc_application_command_builder_t typed_autocomplete_command =
        DCC_SLASH_COMMAND_WITH_OPTIONS(
            "typed_search",
            "Typed autocomplete command",
            DCC_COMMAND_STRING_OPTION_AUTOCOMPLETE("name", "Name", 1U),
            DCC_COMMAND_INTEGER_OPTION_RANGE("limit", "Limit", 0U, 1, 25)
        );
    dcc_command_route_t typed_slash =
        DCC_TYPED_SLASH(
            &typed_command,
            app_smoke_typed_slash_args_t,
            on_typed_slash,
            &state,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(app_smoke_typed_slash_args_t, name, "name"),
                DCC_CTX_OPTION_FIELD_INT(app_smoke_typed_slash_args_t, limit, "limit", 10)
            ),
            DCC_CTX_FIELD_VALIDATORS(
                DCC_VALIDATE_STRING_LENGTH(app_smoke_typed_slash_args_t, name, "name", 1U, 32U),
                DCC_VALIDATE_INT_RANGE(app_smoke_typed_slash_args_t, limit, "limit", 1, 25)
            )
        );
    dcc_autocomplete_route_t typed_autocomplete =
        DCC_TYPED_AUTOCOMPLETE_BUILDER(
            &typed_autocomplete_command,
            app_smoke_typed_slash_args_t,
            on_typed_autocomplete,
            &state,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(app_smoke_typed_slash_args_t, name, "name"),
                DCC_CTX_OPTION_FIELD_INT(app_smoke_typed_slash_args_t, limit, "limit", 10)
            ),
            DCC_CTX_FIELD_VALIDATORS(
                DCC_VALIDATE_STRING_LENGTH(app_smoke_typed_slash_args_t, name, "name", 1U, 32U),
                DCC_VALIDATE_INT_RANGE(app_smoke_typed_slash_args_t, limit, "limit", 1, 25)
            )
        );
    dcc_application_command_builder_t typed_subcommand_command =
        DCC_SLASH_COMMAND_WITH_OPTIONS(
            "typed_admin",
            "Typed admin command",
            DCC_COMMAND_SUBCOMMAND_GROUP(
                "moderation",
                "Moderation",
                DCC_COMMAND_SUBCOMMAND(
                    "ban",
                    "Ban a member",
                    DCC_COMMAND_STRING_OPTION_AUTOCOMPLETE("name", "Name", 1U),
                    DCC_COMMAND_INTEGER_OPTION_RANGE("limit", "Limit", 0U, 1, 25)
                )
            )
        );
    dcc_subcommand_route_t typed_subcommand =
        DCC_TYPED_SUBCOMMAND_BUILDER(
            &typed_subcommand_command,
            "moderation/ban",
            app_smoke_typed_slash_args_t,
            on_typed_subcommand,
            &state,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(app_smoke_typed_slash_args_t, name, "name"),
                DCC_CTX_OPTION_FIELD_INT(app_smoke_typed_slash_args_t, limit, "limit", 10)
            ),
            DCC_CTX_FIELD_VALIDATORS(
                DCC_VALIDATE_STRING_LENGTH(app_smoke_typed_slash_args_t, name, "name", 1U, 32U),
                DCC_VALIDATE_INT_RANGE(app_smoke_typed_slash_args_t, limit, "limit", 1, 25)
            )
        );
    dcc_autocomplete_route_t typed_subcommand_autocomplete =
        DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE_BUILDER(
            &typed_subcommand_command,
            "moderation/ban",
            app_smoke_typed_slash_args_t,
            on_typed_subcommand_autocomplete,
            &state,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(app_smoke_typed_slash_args_t, name, "name"),
                DCC_CTX_OPTION_FIELD_INT(app_smoke_typed_slash_args_t, limit, "limit", 10)
            ),
            DCC_CTX_FIELD_VALIDATORS(
                DCC_VALIDATE_STRING_LENGTH(app_smoke_typed_slash_args_t, name, "name", 1U, 32U),
                DCC_VALIDATE_INT_RANGE(app_smoke_typed_slash_args_t, limit, "limit", 1, 25)
            )
        );
    dcc_modal_route_t typed_modal =
        DCC_TYPED_MODAL(
            "typed.birthday",
            app_smoke_typed_modal_args_t,
            on_typed_modal,
            &state,
            DCC_CTX_FORM_FIELD_BINDINGS(
                DCC_CTX_FORM_FIELD_REQUIRED_STRING(app_smoke_typed_modal_args_t, birthday, "birthday"),
                DCC_CTX_FORM_FIELD_BOOL(app_smoke_typed_modal_args_t, public_birth, "public", 1U)
            ),
            DCC_CTX_FIELD_VALIDATORS(
                DCC_VALIDATE_DATE_YYYYMMDD(app_smoke_typed_modal_args_t, birthday, "birthday")
            )
        );
    dcc_button_route_t typed_button =
        DCC_TYPED_BUTTON_PREFIX(
            "typed.button:",
            app_smoke_typed_button_args_t,
            on_typed_button,
            &state,
            DCC_CTX_COMPONENT_FIELD_BINDINGS(
                DCC_CTX_COMPONENT_FIELD_REQUIRED_CUSTOM_ID_SUFFIX(
                    app_smoke_typed_button_args_t,
                    action,
                    "typed.button:"
                ),
                DCC_CTX_COMPONENT_FIELD_TYPE(app_smoke_typed_button_args_t, component_type)
            ),
            DCC_CTX_FIELD_VALIDATORS(
                DCC_VALIDATE_STRING_REQUIRED(app_smoke_typed_button_args_t, action, "action")
            )
        );
    dcc_select_route_t typed_select =
        DCC_TYPED_SELECT(
            "typed.select",
            app_smoke_typed_select_args_t,
            on_typed_select,
            &state,
            DCC_CTX_COMPONENT_FIELD_BINDINGS(
                DCC_CTX_COMPONENT_FIELD_REQUIRED_FIRST_VALUE(app_smoke_typed_select_args_t, choice),
                DCC_CTX_COMPONENT_FIELD_REQUIRED_VALUES(
                    app_smoke_typed_select_args_t,
                    values,
                    value_count
                )
            ),
            DCC_CTX_FIELD_VALIDATORS(
                DCC_VALIDATE_STRING_REQUIRED(app_smoke_typed_select_args_t, choice, "choice")
            )
        );
    dcc_application_command_builder_t defined_command =
        DCC_SLASH_COMMAND_WITH_OPTIONS(
            "defined",
            "Defined slash command",
            DCC_COMMAND_STRING_OPTION("name", "Name", 1U)
        );
    dcc_command_route_t defined_slash =
        DCC_TYPED_SLASH(
            &defined_command,
            app_smoke_typed_slash_args_t,
            on_typed_slash,
            &state,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(app_smoke_typed_slash_args_t, name, "name")
            ),
            DCC_CTX_FIELD_VALIDATORS(
                DCC_VALIDATE_STRING_REQUIRED(app_smoke_typed_slash_args_t, name, "name")
            )
        );
    dcc_subcommand_route_t defined_subcommand =
        DCC_TYPED_SUBCOMMAND(
            "defined",
            "admin/reload",
            app_smoke_typed_slash_args_t,
            on_typed_subcommand,
            &state,
            DCC_CTX_NO_OPTION_FIELD_BINDINGS(),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_autocomplete_route_t defined_autocomplete =
        DCC_TYPED_AUTOCOMPLETE(
            "defined",
            app_smoke_typed_slash_args_t,
            on_typed_autocomplete,
            &state,
            DCC_CTX_NO_OPTION_FIELD_BINDINGS(),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_modal_route_t defined_modal =
        DCC_TYPED_MODAL(
            "defined.modal",
            app_smoke_typed_modal_args_t,
            on_typed_modal,
            &state,
            DCC_CTX_FORM_FIELD_BINDINGS(
                DCC_CTX_FORM_FIELD_REQUIRED_STRING(app_smoke_typed_modal_args_t, birthday, "birthday")
            ),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_button_route_t defined_button =
        DCC_TYPED_BUTTON_PREFIX(
            "defined.button:",
            app_smoke_typed_button_args_t,
            on_typed_button,
            &state,
            DCC_CTX_COMPONENT_FIELD_BINDINGS(
                DCC_CTX_COMPONENT_FIELD_REQUIRED_CUSTOM_ID_SUFFIX(
                    app_smoke_typed_button_args_t,
                    action,
                    "defined.button:"
                )
            ),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_select_route_t defined_select =
        DCC_TYPED_SELECT(
            "defined.select",
            app_smoke_typed_select_args_t,
            on_typed_select,
            &state,
            DCC_CTX_COMPONENT_FIELD_BINDINGS(
                DCC_CTX_COMPONENT_FIELD_REQUIRED_FIRST_VALUE(app_smoke_typed_select_args_t, choice)
            ),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_app_definition_t definition =
        DCC_APP(
            "smoke.definition",
            DCC_APP_TYPED_SLASHES(defined_slash),
            DCC_APP_SUBCOMMAND_ROUTES(defined_subcommand),
            DCC_APP_AUTOCOMPLETE_ROUTES(defined_autocomplete),
            DCC_APP_MODAL_ROUTES(defined_modal),
            DCC_APP_BUTTON_ROUTES(defined_button),
            DCC_APP_SELECT_ROUTES(defined_select),
            DCC_APP_EVENTS(DCC_ON_EVENT_DATA(DCC_EVENT_RESUMED, on_generic_event, &state)),
            DCC_APP_AUTO_DEFER_EPHEMERAL_MS(1250U)
        );
    if (status == DCC_OK &&
        (dcc_app_extension_register(NULL, &extension) != DCC_ERR_INVALID_ARG ||
         dcc_app_extension_register(app, NULL) != DCC_ERR_INVALID_ARG ||
         dcc_app_extension_register(app, &(dcc_app_extension_t){0}) != DCC_ERR_INVALID_ARG)) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_app_extension_register(app, &extension);
    }
    if (status == DCC_OK &&
        (dcc_app_apply(NULL, &definition) != DCC_ERR_INVALID_ARG ||
         dcc_app_apply(app, NULL) != DCC_ERR_INVALID_ARG ||
         dcc_app_apply(app, &(dcc_app_definition_t){0}) != DCC_ERR_INVALID_ARG)) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_app_apply(app, &definition);
    }
    if (status == DCC_OK) {
        status = dcc_app_slash(app, "hello", "Say hello", on_slash, &state);
    }
    if (status == DCC_OK &&
        (dcc_app_slash_typed(NULL, &typed_slash) != DCC_ERR_INVALID_ARG ||
         dcc_app_slash_typed(app, NULL) != DCC_ERR_INVALID_ARG ||
         dcc_app_subcommand_typed(NULL, &typed_subcommand) != DCC_ERR_INVALID_ARG ||
         dcc_app_subcommand_typed(app, NULL) != DCC_ERR_INVALID_ARG ||
         dcc_app_autocomplete_typed(NULL, &typed_autocomplete) != DCC_ERR_INVALID_ARG ||
         dcc_app_autocomplete_typed(app, NULL) != DCC_ERR_INVALID_ARG ||
         dcc_app_modal_typed(NULL, &typed_modal) != DCC_ERR_INVALID_ARG ||
         dcc_app_modal_typed(app, NULL) != DCC_ERR_INVALID_ARG ||
         dcc_app_button_typed(NULL, &typed_button) != DCC_ERR_INVALID_ARG ||
         dcc_app_button_typed(app, NULL) != DCC_ERR_INVALID_ARG ||
         dcc_app_select_typed(NULL, &typed_select) != DCC_ERR_INVALID_ARG ||
         dcc_app_select_typed(app, NULL) != DCC_ERR_INVALID_ARG)) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = DCC_APP_REGISTER_COMMAND(app, typed_slash);
    }
    if (status == DCC_OK) {
        status = DCC_APP_REGISTER_SUBCOMMAND(app, typed_subcommand);
    }
    if (status == DCC_OK) {
        status = DCC_APP_REGISTER_AUTOCOMPLETE(app, typed_autocomplete);
    }
    if (status == DCC_OK) {
        status = DCC_APP_REGISTER_AUTOCOMPLETE(app, typed_subcommand_autocomplete);
    }
    if (status == DCC_OK) {
        status = dcc_app_subcommand(app, "admin", "moderation/ban", on_subcommand, &state);
    }
    if (status == DCC_OK) {
        status = dcc_app_use_guild_only(app);
    }
    if (status == DCC_OK) {
        status = dcc_app_use_owner(app, 803U);
    }
    if (status == DCC_OK) {
        status = dcc_app_use_required_permissions(app, DCC_PERMISSION_MANAGE_MESSAGES);
    }
    if (status == DCC_OK) {
        status = dcc_app_use(app, middleware, &state);
    }
    dcc_app_route_id_t page_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_button_prefix_route(app, "page:", on_button, &state, &page_route);
    }
    if (status == DCC_OK) {
        status = DCC_APP_REGISTER_BUTTON(app, typed_button);
    }
    if (status == DCC_OK) {
        status = DCC_APP_REGISTER_SELECT(app, typed_select);
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use(app, page_route, route_middleware, &state);
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_required_permissions(app, page_route, DCC_PERMISSION_MANAGE_MESSAGES);
    }
    dcc_app_route_id_t cooldown_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_button_route(app, "cooldown:test", on_cooldown_button, &state, &cooldown_route);
    }
    dcc_app_cooldown_options_t cooldown = DCC_COOLDOWN_USER(1U, 60000U);
    if (status == DCC_OK &&
        (dcc_app_use_cooldown(NULL, &cooldown) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_cooldown(NULL, cooldown_route, &cooldown) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_cooldown(app, DCC_APP_ROUTE_INVALID, &cooldown) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_cooldown(app, cooldown_route, NULL) != DCC_ERR_INVALID_ARG)) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_cooldown(app, cooldown_route, &cooldown);
    }
    dcc_app_route_id_t check_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_button_route(app, "check:ok", on_check_button, &state, &check_route);
    }
    dcc_app_route_id_t rejected_check_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_button_route(
            app,
            "check:deny",
            on_rejected_check_button,
            &state,
            &rejected_check_route
        );
    }
    dcc_app_check_t route_check = DCC_CHECK(allow_check, &state);
    dcc_app_check_t rejected_check =
        DCC_CHECK_MESSAGE(reject_check, &state, "Unavailable", "This check rejected the interaction.");
    if (status == DCC_OK &&
        (dcc_app_use_check(NULL, &route_check) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_check(NULL, check_route, &route_check) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_check(app, DCC_APP_ROUTE_INVALID, &route_check) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_check(app, check_route, NULL) != DCC_ERR_INVALID_ARG)) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_check(app, check_route, &route_check);
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_check(app, rejected_check_route, &rejected_check);
    }
    dcc_app_route_id_t dm_only_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_button_route(app, "dm:only", on_dm_only_button, &state, &dm_only_route);
    }
    if (status == DCC_OK &&
        (dcc_app_use_dm_only(NULL) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_dm_only(NULL, dm_only_route) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_dm_only(app, DCC_APP_ROUTE_INVALID) != DCC_ERR_INVALID_ARG)) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_dm_only(app, dm_only_route);
    }
    dcc_app_route_id_t nsfw_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_button_route(app, "nsfw:ok", on_nsfw_button, &state, &nsfw_route);
    }
    dcc_app_route_id_t nsfw_rejected_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_button_route(
            app,
            "nsfw:deny",
            on_nsfw_rejected_button,
            &state,
            &nsfw_rejected_route
        );
    }
    if (status == DCC_OK &&
        (dcc_app_use_nsfw_only(NULL) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_nsfw_only(NULL, nsfw_route) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_nsfw_only(app, DCC_APP_ROUTE_INVALID) != DCC_ERR_INVALID_ARG)) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_nsfw_only(app, nsfw_route);
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_nsfw_only(app, nsfw_rejected_route);
    }
    dcc_app_route_id_t channel_type_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_button_route(
            app,
            "channel:text",
            on_channel_type_button,
            &state,
            &channel_type_route
        );
    }
    dcc_app_route_id_t channel_type_rejected_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_button_route(
            app,
            "channel:voice",
            on_channel_type_rejected_button,
            &state,
            &channel_type_rejected_route
        );
    }
    uint32_t text_channel_types[] = { DCC_CHANNEL_TEXT };
    if (status == DCC_OK &&
        (dcc_app_use_channel_types(NULL, text_channel_types, DCC_ARRAY_LEN(text_channel_types)) !=
             DCC_ERR_INVALID_ARG ||
         dcc_app_use_channel_types(app, NULL, DCC_ARRAY_LEN(text_channel_types)) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_channel_types(NULL, channel_type_route, text_channel_types, DCC_ARRAY_LEN(text_channel_types)) !=
             DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_channel_types(app, DCC_APP_ROUTE_INVALID, text_channel_types, DCC_ARRAY_LEN(text_channel_types)) !=
             DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_channel_types(app, channel_type_route, NULL, DCC_ARRAY_LEN(text_channel_types)) !=
             DCC_ERR_INVALID_ARG)) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_channel_types(app, channel_type_route, text_channel_types, DCC_ARRAY_LEN(text_channel_types));
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_channel_types(app, channel_type_rejected_route, text_channel_types, DCC_ARRAY_LEN(text_channel_types));
    }
    dcc_app_route_id_t role_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_button_route(app, "role:ok", on_role_button, &state, &role_route);
    }
    dcc_app_route_id_t role_rejected_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_button_route(app, "role:deny", on_role_rejected_button, &state, &role_rejected_route);
    }
    dcc_snowflake_t required_roles[] = { 910U, 911U };
    dcc_snowflake_t rejected_roles[] = { 990U, 991U };
    if (status == DCC_OK &&
        (dcc_app_use_required_roles(NULL, required_roles, DCC_ARRAY_LEN(required_roles)) != DCC_ERR_INVALID_ARG ||
         dcc_app_use_required_roles(app, NULL, DCC_ARRAY_LEN(required_roles)) != DCC_ERR_INVALID_ARG ||
         dcc_app_use_any_role(NULL, required_roles, DCC_ARRAY_LEN(required_roles)) != DCC_ERR_INVALID_ARG ||
         dcc_app_use_any_role(app, NULL, DCC_ARRAY_LEN(required_roles)) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_required_roles(NULL, role_route, required_roles, DCC_ARRAY_LEN(required_roles)) !=
             DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_required_roles(app, DCC_APP_ROUTE_INVALID, required_roles, DCC_ARRAY_LEN(required_roles)) !=
             DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_required_roles(app, role_route, NULL, DCC_ARRAY_LEN(required_roles)) !=
             DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_any_role(NULL, role_route, required_roles, DCC_ARRAY_LEN(required_roles)) !=
             DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_any_role(app, DCC_APP_ROUTE_INVALID, required_roles, DCC_ARRAY_LEN(required_roles)) !=
             DCC_ERR_INVALID_ARG ||
         dcc_app_route_use_any_role(app, role_route, NULL, DCC_ARRAY_LEN(required_roles)) != DCC_ERR_INVALID_ARG)) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_required_roles(app, role_route, required_roles, DCC_ARRAY_LEN(required_roles));
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_any_role(app, role_rejected_route, rejected_roles, DCC_ARRAY_LEN(rejected_roles));
    }
    dcc_app_view_t view =
        DCC_APP_VIEW(DCC_VIEW_BUTTON("view:confirm", on_view_button, &state));
    if (status == DCC_OK) {
        status = dcc_app_view_register(app, &view);
    }
    dcc_app_route_group_t group;
    if (status == DCC_OK &&
        (dcc_app_route_group_init(NULL, app, "bad:", &state) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_group_init(&group, NULL, "bad:", &state) != DCC_ERR_INVALID_ARG ||
         dcc_app_route_group_init(&group, app, "", &state) != DCC_ERR_INVALID_ARG)) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_app_route_group_init(&group, app, "settings:", &state);
    }
    dcc_app_route_id_t group_button_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_route_group_button_route(&group, "open", on_group_button, &group_button_route);
    }
    if (status == DCC_OK && group_button_route == DCC_APP_ROUTE_INVALID) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_app_route_group_select(&group, "mode", on_group_select);
    }
    if (status == DCC_OK) {
        status = dcc_app_route_group_modal(&group, "edit", on_group_modal);
    }
    dcc_app_view_t group_view =
        DCC_APP_VIEW(DCC_VIEW_BUTTON("view", on_group_view_button, NULL));
    if (status == DCC_OK) {
        status = dcc_app_route_group_view_register(&group, &group_view);
    }
    dcc_app_route_group_t empty_group;
    if (status == DCC_OK) {
        status = dcc_app_route_group_init(&empty_group, app, "empty:", &state);
    }
    if (status == DCC_OK &&
        dcc_app_route_group_use(&empty_group, group_middleware, &state) != DCC_ERR_NOT_FOUND) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_app_route_group_use(&group, group_middleware, &state);
    }
    if (status == DCC_OK) {
        status = dcc_app_route_group_use_required_permissions(
            &group,
            DCC_PERMISSION_MANAGE_MESSAGES
        );
    }
    if (status == DCC_OK) {
        status = dcc_app_modal(app, "birthday.modal", on_modal, &state);
    }
    if (status == DCC_OK) {
        status = DCC_APP_REGISTER_MODAL(app, typed_modal);
    }
    dcc_app_route_id_t autocomplete_route = DCC_APP_ROUTE_INVALID;
    if (status == DCC_OK) {
        status = dcc_app_autocomplete_route(app, "hello", on_autocomplete, &state, &autocomplete_route);
    }
    if (status == DCC_OK) {
        status = dcc_app_route_use_required_permissions(
            app,
            autocomplete_route,
            DCC_PERMISSION_MANAGE_MESSAGES
        );
    }
    dcc_component_session_listener_options_t listen;
    dcc_component_session_listener_options_init(&listen);
    listen.now_ms = 2000U;
    if (status == DCC_OK) {
        status = dcc_app_component_session(app, &session, &listen, on_session, &state);
    }
    if (status != DCC_OK) {
        fprintf(stderr, "route registration failed: %s\n", dcc_status_string(status));
        dcc_component_session_deinit(&session);
        dcc_app_destroy(app);
        return 1;
    }

    app_smoke_interactions_t fx;
    if (!app_smoke_interactions_init(&fx, &session)) {
        fprintf(stderr, "app smoke interaction fixture setup failed\n");
        dcc_component_session_deinit(&session);
        dcc_app_destroy(app);
        return 1;
    }

    if (dispatch_event(app, &fx.resumed_event) != DCC_OK ||
        dispatch_event(app, &fx.ready_event) != DCC_OK ||
        dispatch_event(app, &fx.message_event) != DCC_OK ||
        dispatch_event(app, &fx.message_update_event) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_SLASH_COMMAND, &fx.slash) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_SLASH_COMMAND, &fx.typed_slash) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_SLASH_COMMAND, &fx.typed_subcommand) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_SLASH_COMMAND, &fx.extension_slash) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_SLASH_COMMAND, &fx.admin) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_AUTOCOMPLETE, &fx.autocomplete) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_AUTOCOMPLETE, &fx.typed_autocomplete) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_AUTOCOMPLETE, &fx.typed_subcommand_autocomplete) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.denied_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.cooldown_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.cooldown_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.check_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.rejected_check_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.dm_only_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.nsfw_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.nsfw_rejected_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.channel_type_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.channel_type_rejected_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.role_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.role_rejected_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.typed_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_SELECT_CLICK, &fx.typed_select) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.extension_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.view_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.module_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.group_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_SELECT_CLICK, &fx.group_select) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.group_view_button) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_FORM_SUBMIT, &fx.modal) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_FORM_SUBMIT, &fx.typed_modal) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_FORM_SUBMIT, &fx.group_modal) != DCC_OK ||
        dispatch_interaction(app, DCC_EVENT_BUTTON_CLICK, &fx.session_button) != DCC_OK) {
        fprintf(stderr, "dispatch failed\n");
        dcc_component_session_deinit(&session);
        dcc_app_destroy(app);
        return 1;
    }

    int ok = state.bad == 0U &&
        state.middleware_count == 32U &&
        state.route_middleware_count == 1U &&
        state.policy_middleware_count == 1U &&
        state.policy_check_count == 1U &&
        state.route_check_count == 1U &&
        state.route_check_reject_count == 1U &&
        state.group_middleware_count == 4U &&
        state.generic_event_count == 2U &&
        state.ready_count == 1U &&
        state.message_count == 1U &&
        state.slash_count == 1U &&
        state.typed_slash_count == 1U &&
        state.typed_subcommand_count == 1U &&
        state.typed_autocomplete_count == 1U &&
        state.typed_subcommand_autocomplete_count == 1U &&
        state.extension_slash_count == 1U &&
        state.extension_button_count == 1U &&
        state.extension_ready_count == 1U &&
        state.extension_message_update_count == 1U &&
        state.subcommand_count == 1U &&
        state.autocomplete_count == 1U &&
        state.button_count == 1U &&
        state.check_button_count == 1U &&
        state.cooldown_button_count == 1U &&
        state.dm_only_button_count == 0U &&
        state.nsfw_button_count == 1U &&
        state.nsfw_reject_button_count == 0U &&
        state.channel_type_button_count == 1U &&
        state.channel_type_reject_button_count == 0U &&
        state.role_button_count == 1U &&
        state.role_reject_button_count == 0U &&
        state.typed_button_count == 1U &&
        state.typed_select_count == 1U &&
        state.view_count == 1U &&
        state.group_button_count == 1U &&
        state.group_select_count == 1U &&
        state.group_modal_count == 1U &&
        state.group_view_count == 1U &&
        state.modal_count == 1U &&
        state.typed_modal_count == 1U &&
        state.session_count == 1U &&
        state.error_count == 7U &&
        state.module_setup_count == 1U &&
        state.module_button_count == 1U &&
        state.module_cleanup_count == 0U &&
        state.app_state_cleanup_count == 0U &&
        dcc_command_registry_count(dcc_app_command_registry(app)) == 6U;

    dcc_command_registry_options_t registry_options;
    dcc_command_registry_options_init(&registry_options);
    (void)dcc_command_registry_options_set_dry_run(&registry_options, 1U);
    dcc_command_registry_plan_t plan;
    memset(&plan, 0, sizeof(plan));
    dcc_status_t sync_status = dcc_app_sync_commands_from_json(
        app,
        123U,
        &registry_options,
        "[]",
        2U,
        &plan,
        NULL,
        NULL
    );
    ok = ok && sync_status == DCC_OK && plan.create_count == 6U && plan.diff_count == 6U;
    dcc_command_registry_plan_deinit(&plan);

    if (!ok) {
        fprintf(
            stderr,
            "app smoke failed: bad=%u middleware=%u route_middleware=%u policy_middleware=%u policy_check=%u route_check=%u route_check_reject=%u group_middleware=%u generic=%u ready=%u message=%u slash=%u typed_slash=%u typed_subcommand=%u typed_autocomplete=%u typed_subcommand_autocomplete=%u subcommand=%u autocomplete=%u button=%u check_button=%u cooldown_button=%u dm_only_button=%u nsfw_button=%u nsfw_reject_button=%u channel_type_button=%u channel_type_reject_button=%u role_button=%u role_reject_button=%u typed_button=%u typed_select=%u view=%u module=%u modal=%u session=%u error=%u module_setup=%u commands=%zu\n",
            state.bad,
            state.middleware_count,
            state.route_middleware_count,
            state.policy_middleware_count,
            state.policy_check_count,
            state.route_check_count,
            state.route_check_reject_count,
            state.group_middleware_count,
            state.generic_event_count,
            state.ready_count,
            state.message_count,
            state.slash_count,
            state.typed_slash_count,
            state.typed_subcommand_count,
            state.typed_autocomplete_count,
            state.typed_subcommand_autocomplete_count,
            state.subcommand_count,
            state.autocomplete_count,
            state.button_count,
            state.check_button_count,
            state.cooldown_button_count,
            state.dm_only_button_count,
            state.nsfw_button_count,
            state.nsfw_reject_button_count,
            state.channel_type_button_count,
            state.channel_type_reject_button_count,
            state.role_button_count,
            state.role_reject_button_count,
            state.typed_button_count,
            state.typed_select_count,
            state.view_count,
            state.module_button_count,
            state.modal_count,
            state.session_count,
            state.error_count,
            state.module_setup_count,
            dcc_command_registry_count(dcc_app_command_registry(app))
        );
        dcc_component_session_deinit(&session);
        dcc_app_destroy(app);
        (void)remove(runtime_store_path);
        (void)remove("dcc_app_smoke_runtime_store.kv.tmp");
        return 1;
    }

    dcc_component_session_deinit(&session);
    dcc_app_destroy(app);
    (void)remove(runtime_store_path);
    (void)remove("dcc_app_smoke_runtime_store.kv.tmp");
    if (state.module_cleanup_count != 1U || state.app_state_cleanup_count != 1U) {
        fprintf(
            stderr,
            "app cleanup failed: module_cleanup=%u app_state_cleanup=%u\n",
            state.module_cleanup_count,
            state.app_state_cleanup_count
        );
        return 1;
    }
    return 0;
}
