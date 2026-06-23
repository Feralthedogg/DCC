#ifndef DCC_SUGAR_CONTEXT_H
#define DCC_SUGAR_CONTEXT_H

#include <dcc/app.h>
#include <dcc/sugar/cdn.h>
#include <dcc/sugar/format.h>
#include <dcc/sugar/links.h>
#include <dcc/sugar/reply.h>
#include <string.h>

static inline uint8_t dcc_sugar_ctx_has_text(const char *value) {
    return value != NULL && value[0] != '\0' ? 1U : 0U;
}

static inline const char *dcc_sugar_ctx_text_or(const char *value, const char *fallback) {
    return dcc_sugar_ctx_has_text(value) ? value : fallback;
}

static inline uint8_t dcc_sugar_ctx_text_eq(const char *left, const char *right) {
    return left != NULL && right != NULL && strcmp(left, right) == 0 ? 1U : 0U;
}

static inline uint8_t dcc_sugar_ctx_text_starts_with(const char *value, const char *prefix) {
    size_t prefix_len;
    if (value == NULL || prefix == NULL) {
        return 0U;
    }
    prefix_len = strlen(prefix);
    return strncmp(value, prefix, prefix_len) == 0 ? 1U : 0U;
}

static inline const char *dcc_sugar_ctx_text_suffix_after(const char *value, const char *prefix) {
    size_t prefix_len;
    if (!dcc_sugar_ctx_text_starts_with(value, prefix)) {
        return NULL;
    }
    prefix_len = strlen(prefix);
    return value + prefix_len;
}

static inline uint8_t dcc_sugar_ctx_option_has_type(
    const dcc_ctx_t *ctx,
    const char *name,
    dcc_interaction_option_value_type_t value_type
) {
    const dcc_interaction_option_t *option = dcc_ctx_option(ctx, name);
    return option != NULL && option->value_type == value_type ? 1U : 0U;
}

static inline uint8_t dcc_sugar_ctx_form_has_type(
    const dcc_ctx_t *ctx,
    const char *custom_id,
    dcc_interaction_form_value_type_t value_type
) {
    const dcc_interaction_form_field_t *field = dcc_ctx_form_field(ctx, custom_id);
    return field != NULL && field->value_type == value_type ? 1U : 0U;
}

static inline const char *dcc_sugar_user_username_or(
    const dcc_user_t *user,
    const char *fallback
) {
    return user != NULL ? dcc_sugar_ctx_text_or(user->username, fallback) : fallback;
}

static inline const char *dcc_sugar_user_global_name_or(
    const dcc_user_t *user,
    const char *fallback
) {
    return user != NULL ? dcc_sugar_ctx_text_or(user->global_name, fallback) : fallback;
}

static inline const char *dcc_sugar_user_display_name_or(
    const dcc_user_t *user,
    const char *fallback
) {
    if (user == NULL) {
        return fallback;
    }
    if (dcc_sugar_ctx_has_text(user->global_name)) {
        return user->global_name;
    }
    return dcc_sugar_ctx_text_or(user->username, fallback);
}

static inline const char *dcc_sugar_member_nick_or(
    const dcc_member_t *member,
    const char *fallback
) {
    return member != NULL ? dcc_sugar_ctx_text_or(member->nick, fallback) : fallback;
}

static inline const char *dcc_sugar_member_display_name_or(
    const dcc_member_t *member,
    const char *fallback
) {
    if (member == NULL) {
        return fallback;
    }
    if (dcc_sugar_ctx_has_text(member->nick)) {
        return member->nick;
    }
    return dcc_sugar_user_display_name_or(&member->user, fallback);
}

static inline const char *dcc_sugar_guild_name_or(
    const dcc_guild_t *guild,
    const char *fallback
) {
    return guild != NULL ? dcc_sugar_ctx_text_or(guild->name, fallback) : fallback;
}

static inline const char *dcc_sugar_channel_name_or(
    const dcc_channel_t *channel,
    const char *fallback
) {
    return channel != NULL ? dcc_sugar_ctx_text_or(channel->name, fallback) : fallback;
}

static inline const char *dcc_sugar_role_name_or(
    const dcc_role_t *role,
    const char *fallback
) {
    return role != NULL ? dcc_sugar_ctx_text_or(role->name, fallback) : fallback;
}

static inline const char *dcc_sugar_guild_icon_url(
    char *buffer,
    size_t buffer_size,
    const dcc_guild_t *guild,
    const char *extension,
    uint32_t image_size
) {
    if (guild == NULL) {
        (void)dcc_sugar_cdn_done(buffer, buffer_size, -1);
        return "";
    }
    return dcc_sugar_cdn_guild_icon(
        buffer,
        buffer_size,
        guild->id,
        guild->icon,
        extension,
        image_size
    );
}

static inline const char *dcc_sugar_user_avatar_url(
    char *buffer,
    size_t buffer_size,
    const dcc_user_t *user,
    const char *extension,
    uint32_t image_size
) {
    if (user == NULL) {
        (void)dcc_sugar_cdn_done(buffer, buffer_size, -1);
        return "";
    }
    return dcc_sugar_cdn_user_avatar(
        buffer,
        buffer_size,
        user->id,
        user->avatar,
        extension,
        image_size
    );
}

static inline const char *dcc_sugar_member_avatar_url(
    char *buffer,
    size_t buffer_size,
    const dcc_member_t *member,
    const dcc_user_t *fallback_user,
    const char *extension,
    uint32_t image_size
) {
    if (member != NULL && dcc_sugar_ctx_has_text(member->avatar) &&
        member->guild_id != 0U && member->user.id != 0U) {
        return dcc_sugar_cdn_member_avatar(
            buffer,
            buffer_size,
            member->guild_id,
            member->user.id,
            member->avatar,
            extension,
            image_size
        );
    }
    if (member != NULL && member->user.id != 0U) {
        return dcc_sugar_user_avatar_url(
            buffer,
            buffer_size,
            &member->user,
            extension,
            image_size
        );
    }
    return dcc_sugar_user_avatar_url(
        buffer,
        buffer_size,
        fallback_user,
        extension,
        image_size
    );
}

static inline uint8_t dcc_sugar_ctx_is_guild(const dcc_ctx_t *ctx) {
    return dcc_ctx_interaction(ctx) != NULL && dcc_ctx_guild_id(ctx) != 0U ? 1U : 0U;
}

static inline uint8_t dcc_sugar_ctx_is_dm(const dcc_ctx_t *ctx) {
    return dcc_ctx_interaction(ctx) != NULL && dcc_ctx_guild_id(ctx) == 0U ? 1U : 0U;
}

static inline uint8_t dcc_sugar_ctx_is_author(const dcc_ctx_t *ctx, dcc_snowflake_t user_id) {
    return user_id != 0U && dcc_ctx_user_id(ctx) == user_id ? 1U : 0U;
}

static inline const char *dcc_sugar_ctx_message_link(
    char *buffer,
    size_t buffer_size,
    const dcc_ctx_t *ctx
) {
    if (ctx == NULL || dcc_ctx_channel_id(ctx) == 0U || dcc_ctx_message_id(ctx) == 0U) {
        if (buffer != NULL && buffer_size != 0U) {
            buffer[0] = '\0';
        }
        return "";
    }
    if (dcc_ctx_guild_id(ctx) == 0U) {
        return dcc_sugar_message_link(
            buffer,
            buffer_size,
            ((dcc_message_link_t){
                .channel_id = dcc_ctx_channel_id(ctx),
                .message_id = dcc_ctx_message_id(ctx),
                .is_dm = 1U
            })
        );
    }
    return dcc_sugar_message_link(
        buffer,
        buffer_size,
        ((dcc_message_link_t){
            .guild_id = dcc_ctx_guild_id(ctx),
            .channel_id = dcc_ctx_channel_id(ctx),
            .message_id = dcc_ctx_message_id(ctx)
        })
    );
}

static inline const char *dcc_sugar_ctx_custom_id_or(const dcc_ctx_t *ctx, const char *fallback) {
    return dcc_sugar_ctx_text_or(dcc_ctx_custom_id(ctx), fallback);
}

static inline const char *dcc_sugar_ctx_name_or(const dcc_ctx_t *ctx, const char *fallback) {
    return dcc_sugar_ctx_text_or(dcc_ctx_name(ctx), fallback);
}

static inline const char *dcc_sugar_ctx_token_or(const dcc_ctx_t *ctx, const char *fallback) {
    return dcc_sugar_ctx_text_or(dcc_ctx_token(ctx), fallback);
}

static inline uint8_t dcc_sugar_ctx_has_custom_id(const dcc_ctx_t *ctx, const char *custom_id) {
    return dcc_sugar_ctx_text_eq(dcc_ctx_custom_id(ctx), custom_id);
}

static inline uint8_t dcc_sugar_ctx_custom_id_starts_with(
    const dcc_ctx_t *ctx,
    const char *prefix
) {
    return dcc_sugar_ctx_text_starts_with(dcc_ctx_custom_id(ctx), prefix);
}

static inline const char *dcc_sugar_ctx_custom_id_suffix(
    const dcc_ctx_t *ctx,
    const char *prefix
) {
    return dcc_sugar_ctx_text_suffix_after(dcc_ctx_custom_id(ctx), prefix);
}

static inline const char *dcc_sugar_ctx_component_action(
    const dcc_ctx_t *ctx,
    const char *namespace
) {
    const char *custom_id = dcc_ctx_custom_id(ctx);
    size_t namespace_len;
    if (custom_id == NULL || namespace == NULL || namespace[0] == '\0') {
        return NULL;
    }
    namespace_len = strlen(namespace);
    if (strncmp(custom_id, namespace, namespace_len) != 0 || custom_id[namespace_len] != '.') {
        return NULL;
    }
    return custom_id + namespace_len + 1U;
}

static inline const char *dcc_sugar_ctx_component_action_or(
    const dcc_ctx_t *ctx,
    const char *namespace,
    const char *fallback
) {
    const char *action = dcc_sugar_ctx_component_action(ctx, namespace);
    return action != NULL ? action : fallback;
}

static inline uint8_t dcc_sugar_ctx_component_in_namespace(
    const dcc_ctx_t *ctx,
    const char *namespace
) {
    return dcc_sugar_ctx_component_action(ctx, namespace) != NULL ? 1U : 0U;
}

static inline uint8_t dcc_sugar_ctx_component_has_action(
    const dcc_ctx_t *ctx,
    const char *namespace,
    const char *action
) {
    return dcc_sugar_ctx_text_eq(dcc_sugar_ctx_component_action(ctx, namespace), action);
}

#define DCC_CTX_APP(ctx_) dcc_ctx_app((ctx_))
#define DCC_CTX_CLIENT(ctx_) dcc_ctx_client((ctx_))
#define DCC_CTX_EVENT(ctx_) dcc_ctx_event((ctx_))
#define DCC_CTX_INTERACTION(ctx_) dcc_ctx_interaction((ctx_))
#define DCC_CTX_FLOW(ctx_) dcc_ctx_flow((ctx_))
#define DCC_CTX_EVENT_TYPE(ctx_) dcc_ctx_event_type((ctx_))
#define DCC_CTX_INTERACTION_ID(ctx_) dcc_ctx_interaction_id((ctx_))
#define DCC_CTX_APPLICATION_ID(ctx_) dcc_ctx_application_id((ctx_))
#define DCC_CTX_GUILD_ID(ctx_) dcc_ctx_guild_id((ctx_))
#define DCC_CTX_CHANNEL_ID(ctx_) dcc_ctx_channel_id((ctx_))
#define DCC_CTX_CHANNEL_TYPE(ctx_, out_type_) dcc_ctx_channel_type((ctx_), (out_type_))
#define DCC_CTX_CHANNEL_IS_TYPE(ctx_, channel_type_) \
    dcc_ctx_channel_is_type((ctx_), (channel_type_))
#define DCC_CTX_CHANNEL_IS_NSFW(ctx_) dcc_ctx_channel_is_nsfw((ctx_))
#define DCC_CTX_COMMAND_ID(ctx_) dcc_ctx_command_id((ctx_))
#define DCC_CTX_MESSAGE_ID(ctx_) dcc_ctx_message_id((ctx_))
#define DCC_CTX_TARGET_ID(ctx_) dcc_ctx_target_id((ctx_))
#define DCC_CTX_USER_ID(ctx_) dcc_ctx_user_id((ctx_))
#define DCC_CTX_AUTHOR_ID(ctx_) dcc_ctx_user_id((ctx_))
#define DCC_CTX_GUILD(ctx_) dcc_ctx_guild((ctx_))
#define DCC_CTX_CHANNEL(ctx_) dcc_ctx_channel((ctx_))
#define DCC_CTX_USER(ctx_) dcc_ctx_user((ctx_))
#define DCC_CTX_AUTHOR(ctx_) dcc_ctx_user((ctx_))
#define DCC_CTX_MEMBER(ctx_) dcc_ctx_member((ctx_))
#define DCC_CTX_RESOLVED_USER(ctx_, id_) dcc_ctx_resolved_user((ctx_), (id_))
#define DCC_CTX_RESOLVED_MEMBER(ctx_, user_id_) dcc_ctx_resolved_member((ctx_), (user_id_))
#define DCC_CTX_RESOLVED_ROLE(ctx_, id_) dcc_ctx_resolved_role((ctx_), (id_))
#define DCC_CTX_RESOLVED_CHANNEL(ctx_, id_) dcc_ctx_resolved_channel((ctx_), (id_))
#define DCC_CTX_RESOLVED_MESSAGE(ctx_, id_) dcc_ctx_resolved_message((ctx_), (id_))
#define DCC_CTX_RESOLVED_ATTACHMENT(ctx_, id_) dcc_ctx_resolved_attachment((ctx_), (id_))
#define DCC_CTX_TARGET_USER(ctx_) dcc_ctx_target_user((ctx_))
#define DCC_CTX_TARGET_MEMBER(ctx_) dcc_ctx_target_member((ctx_))
#define DCC_CTX_TARGET_ROLE(ctx_) dcc_ctx_target_role((ctx_))
#define DCC_CTX_TARGET_CHANNEL(ctx_) dcc_ctx_target_channel((ctx_))
#define DCC_CTX_TARGET_MESSAGE(ctx_) dcc_ctx_target_message((ctx_))
#define DCC_CTX_USER_DATA(ctx_) dcc_ctx_user_data((ctx_))
#define DCC_CTX_TOKEN(ctx_) dcc_ctx_token((ctx_))
#define DCC_CTX_NAME(ctx_) dcc_ctx_name((ctx_))
#define DCC_CTX_COMMAND_NAME(ctx_) dcc_ctx_name((ctx_))
#define DCC_CTX_CUSTOM_ID(ctx_) dcc_ctx_custom_id((ctx_))
#define DCC_CTX_SUBCOMMAND(ctx_) dcc_ctx_subcommand((ctx_))
#define DCC_CTX_SUBCOMMAND_GROUP(ctx_) dcc_ctx_subcommand_group((ctx_))
#define DCC_CTX_RESPONSE_STATE(ctx_) dcc_ctx_response_state((ctx_))
#define DCC_CTX_RESPONSE_STATE_NAME(ctx_) dcc_ctx_response_state_string((ctx_))
#define DCC_CTX_RESPONDED(ctx_) dcc_ctx_response_sent((ctx_))
#define DCC_CTX_RESPONSE_SENT(ctx_) dcc_ctx_response_sent((ctx_))
#define DCC_CTX_DEFERRED(ctx_) dcc_ctx_deferred((ctx_))
#define DCC_CTX_REPLIED(ctx_) dcc_ctx_replied((ctx_))
#define DCC_CTX_FOLLOWED_UP(ctx_) dcc_ctx_followed_up((ctx_))
#define DCC_CTX_RESPONSE_FAILED(ctx_) dcc_ctx_response_failed((ctx_))
#define DCC_CTX_CAN_FOLLOWUP(ctx_) dcc_ctx_can_followup((ctx_))
#define DCC_CTX_CAN_EDIT_ORIGINAL(ctx_) dcc_ctx_can_edit_original((ctx_))
#define DCC_CTX_HAS_MEMBER_PERMISSIONS(ctx_) dcc_ctx_has_member_permissions((ctx_))
#define DCC_CTX_MEMBER_PERMISSIONS(ctx_) dcc_ctx_member_permissions((ctx_))
#define DCC_CTX_MEMBER_HAS_PERMISSIONS(ctx_, permissions_) \
    dcc_ctx_member_has_permissions((ctx_), (permissions_))
#define DCC_CTX_MEMBER_HAS_ROLE(ctx_, role_id_) dcc_ctx_member_has_role((ctx_), (role_id_))
#define DCC_CTX_MEMBER_HAS_ANY_ROLE(ctx_, role_ids_, role_id_count_) \
    dcc_ctx_member_has_any_role((ctx_), (role_ids_), (role_id_count_))
#define DCC_CTX_MEMBER_HAS_ALL_ROLES(ctx_, role_ids_, role_id_count_) \
    dcc_ctx_member_has_all_roles((ctx_), (role_ids_), (role_id_count_))
#define DCC_CTX_MEMBER_HAS_ANY_ROLE_IDS(ctx_, ...) \
    dcc_ctx_member_has_any_role( \
        (ctx_), \
        (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t) \
    )
#define DCC_CTX_MEMBER_HAS_ALL_ROLE_IDS(ctx_, ...) \
    dcc_ctx_member_has_all_roles( \
        (ctx_), \
        (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t) \
    )
#define DCC_CTX_HAS_ROLE(ctx_, role_id_) DCC_CTX_MEMBER_HAS_ROLE((ctx_), (role_id_))
#define DCC_CTX_HAS_ANY_ROLE_IDS(ctx_, ...) DCC_CTX_MEMBER_HAS_ANY_ROLE_IDS((ctx_), __VA_ARGS__)
#define DCC_CTX_HAS_ALL_ROLE_IDS(ctx_, ...) DCC_CTX_MEMBER_HAS_ALL_ROLE_IDS((ctx_), __VA_ARGS__)
#define DCC_CTX_HAS_PERMISSIONS(ctx_, permissions_) \
    dcc_ctx_member_has_permissions((ctx_), (permissions_))

#define DCC_USER(ctx_) DCC_CTX_USER((ctx_))
#define DCC_AUTHOR(ctx_) DCC_CTX_AUTHOR((ctx_))
#define DCC_MEMBER(ctx_) DCC_CTX_MEMBER((ctx_))
#define DCC_GUILD(ctx_) DCC_CTX_GUILD((ctx_))
#define DCC_CHANNEL(ctx_) DCC_CTX_CHANNEL((ctx_))
#define DCC_USER_ID(ctx_) DCC_CTX_USER_ID((ctx_))
#define DCC_AUTHOR_ID(ctx_) DCC_CTX_AUTHOR_ID((ctx_))
#define DCC_GUILD_ID(ctx_) DCC_CTX_GUILD_ID((ctx_))
#define DCC_CHANNEL_ID(ctx_) DCC_CTX_CHANNEL_ID((ctx_))
#define DCC_MESSAGE_ID(ctx_) DCC_CTX_MESSAGE_ID((ctx_))
#define DCC_TARGET_ID(ctx_) DCC_CTX_TARGET_ID((ctx_))
#define DCC_COMMAND_ID(ctx_) DCC_CTX_COMMAND_ID((ctx_))
#define DCC_COMMAND_NAME(ctx_) DCC_CTX_COMMAND_NAME((ctx_))
#define DCC_CUSTOM_ID(ctx_) DCC_CTX_CUSTOM_ID((ctx_))
#define DCC_CUSTOM_ID_OR(ctx_, fallback_) DCC_CTX_CUSTOM_ID_OR((ctx_), (fallback_))
#define DCC_ACTION(ctx_, prefix_) DCC_CTX_ACTION((ctx_), (prefix_))
#define DCC_USER_DATA(ctx_) DCC_CTX_USER_DATA((ctx_))

#define DCC_CTX_IS_GUILD(ctx_) dcc_sugar_ctx_is_guild((ctx_))
#define DCC_CTX_IS_DM(ctx_) dcc_sugar_ctx_is_dm((ctx_))
#define DCC_CTX_IS_AUTHOR(ctx_, user_id_) dcc_sugar_ctx_is_author((ctx_), (user_id_))
#define DCC_CTX_NAME_OR(ctx_, fallback_) dcc_sugar_ctx_name_or((ctx_), (fallback_))
#define DCC_CTX_TOKEN_OR(ctx_, fallback_) dcc_sugar_ctx_token_or((ctx_), (fallback_))
#define DCC_CTX_CUSTOM_ID_OR(ctx_, fallback_) dcc_sugar_ctx_custom_id_or((ctx_), (fallback_))
#define DCC_CTX_HAS_CUSTOM_ID(ctx_, custom_id_) dcc_sugar_ctx_has_custom_id((ctx_), (custom_id_))
#define DCC_CTX_CUSTOM_ID_STARTS_WITH(ctx_, prefix_) \
    dcc_sugar_ctx_custom_id_starts_with((ctx_), (prefix_))
#define DCC_CTX_CUSTOM_ID_SUFFIX(ctx_, prefix_) dcc_sugar_ctx_custom_id_suffix((ctx_), (prefix_))
#define DCC_CTX_ACTION(ctx_, prefix_) dcc_sugar_ctx_custom_id_suffix((ctx_), (prefix_))
#define DCC_CTX_COMPONENT_ACTION_OR(ctx_, namespace_, fallback_) \
    dcc_sugar_ctx_component_action_or((ctx_), (namespace_), (fallback_))
#define DCC_CTX_COMPONENT_ACTION(ctx_, namespace_) \
    dcc_sugar_ctx_component_action((ctx_), (namespace_))
#define DCC_CTX_COMPONENT_IN_NS(ctx_, namespace_) \
    dcc_sugar_ctx_component_in_namespace((ctx_), (namespace_))
#define DCC_CTX_COMPONENT_IN_NAMESPACE(ctx_, namespace_) \
    DCC_CTX_COMPONENT_IN_NS((ctx_), (namespace_))
#define DCC_CTX_COMPONENT_HAS_ACTION(ctx_, namespace_, action_) \
    dcc_sugar_ctx_component_has_action((ctx_), (namespace_), (action_))
#define DCC_CTX_COMPONENT_IS(ctx_, namespace_, action_) \
    DCC_CTX_COMPONENT_HAS_ACTION((ctx_), (namespace_), (action_))
#define DCC_CTX_ACTION_IN_NS(ctx_, namespace_) \
    DCC_CTX_COMPONENT_ACTION((ctx_), (namespace_))
#define DCC_CTX_ACTION_IN_NAMESPACE(ctx_, namespace_) \
    DCC_CTX_COMPONENT_ACTION((ctx_), (namespace_))

#define DCC_USER_USERNAME_OR(user_, fallback_) \
    dcc_sugar_user_username_or((user_), (fallback_))
#define DCC_USER_USERNAME(user_) DCC_USER_USERNAME_OR((user_), "")
#define DCC_USER_GLOBAL_NAME_OR(user_, fallback_) \
    dcc_sugar_user_global_name_or((user_), (fallback_))
#define DCC_USER_GLOBAL_NAME(user_) DCC_USER_GLOBAL_NAME_OR((user_), "")
#define DCC_USER_DISPLAY_NAME_OR(user_, fallback_) \
    dcc_sugar_user_display_name_or((user_), (fallback_))
#define DCC_USER_DISPLAY_NAME(user_) DCC_USER_DISPLAY_NAME_OR((user_), "")
#define DCC_MEMBER_NICK_OR(member_, fallback_) \
    dcc_sugar_member_nick_or((member_), (fallback_))
#define DCC_MEMBER_NICK(member_) DCC_MEMBER_NICK_OR((member_), "")
#define DCC_MEMBER_DISPLAY_NAME_OR(member_, fallback_) \
    dcc_sugar_member_display_name_or((member_), (fallback_))
#define DCC_MEMBER_DISPLAY_NAME(member_) DCC_MEMBER_DISPLAY_NAME_OR((member_), "")
#define DCC_GUILD_NAME_OR(guild_, fallback_) dcc_sugar_guild_name_or((guild_), (fallback_))
#define DCC_GUILD_NAME(guild_) DCC_GUILD_NAME_OR((guild_), "")
#define DCC_CHANNEL_NAME_OR(channel_, fallback_) \
    dcc_sugar_channel_name_or((channel_), (fallback_))
#define DCC_CHANNEL_NAME(channel_) DCC_CHANNEL_NAME_OR((channel_), "")
#define DCC_ROLE_NAME_OR(role_, fallback_) dcc_sugar_role_name_or((role_), (fallback_))
#define DCC_ROLE_NAME(role_) DCC_ROLE_NAME_OR((role_), "")

#define DCC_USER_AVATAR_URL_EXT(user_, extension_, image_size_) \
    dcc_sugar_user_avatar_url( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (user_), \
        (extension_), \
        (image_size_) \
    )
#define DCC_USER_AVATAR_URL(user_) \
    DCC_USER_AVATAR_URL_EXT((user_), "png", DCC_CDN_DEFAULT_SIZE)
#define DCC_MEMBER_AVATAR_URL_EXT(member_, fallback_user_, extension_, image_size_) \
    dcc_sugar_member_avatar_url( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (member_), \
        (fallback_user_), \
        (extension_), \
        (image_size_) \
    )
#define DCC_MEMBER_AVATAR_URL(member_, fallback_user_) \
    DCC_MEMBER_AVATAR_URL_EXT((member_), (fallback_user_), "png", DCC_CDN_DEFAULT_SIZE)
#define DCC_GUILD_ICON_URL_EXT(guild_, extension_, image_size_) \
    dcc_sugar_guild_icon_url( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (guild_), \
        (extension_), \
        (image_size_) \
    )
#define DCC_GUILD_ICON_URL(guild_) \
    DCC_GUILD_ICON_URL_EXT((guild_), "png", DCC_CDN_DEFAULT_SIZE)

#define DCC_CTX_GUILD_NAME_OR(ctx_, fallback_) \
    DCC_GUILD_NAME_OR(DCC_CTX_GUILD((ctx_)), (fallback_))
#define DCC_CTX_GUILD_NAME(ctx_) DCC_CTX_GUILD_NAME_OR((ctx_), "")
#define DCC_CTX_CHANNEL_NAME_OR(ctx_, fallback_) \
    DCC_CHANNEL_NAME_OR(DCC_CTX_CHANNEL((ctx_)), (fallback_))
#define DCC_CTX_CHANNEL_NAME(ctx_) DCC_CTX_CHANNEL_NAME_OR((ctx_), "")
#define DCC_CTX_AUTHOR_USERNAME_OR(ctx_, fallback_) \
    DCC_USER_USERNAME_OR(DCC_CTX_AUTHOR((ctx_)), (fallback_))
#define DCC_CTX_AUTHOR_USERNAME(ctx_) DCC_CTX_AUTHOR_USERNAME_OR((ctx_), "")
#define DCC_CTX_AUTHOR_GLOBAL_NAME_OR(ctx_, fallback_) \
    DCC_USER_GLOBAL_NAME_OR(DCC_CTX_AUTHOR((ctx_)), (fallback_))
#define DCC_CTX_AUTHOR_GLOBAL_NAME(ctx_) DCC_CTX_AUTHOR_GLOBAL_NAME_OR((ctx_), "")
#define DCC_CTX_MEMBER_NICK_OR(ctx_, fallback_) \
    DCC_MEMBER_NICK_OR(DCC_CTX_MEMBER((ctx_)), (fallback_))
#define DCC_CTX_MEMBER_NICK(ctx_) DCC_CTX_MEMBER_NICK_OR((ctx_), "")
#define DCC_CTX_AUTHOR_DISPLAY_NAME_OR(ctx_, fallback_) \
    dcc_sugar_member_display_name_or( \
        DCC_CTX_MEMBER((ctx_)), \
        DCC_USER_DISPLAY_NAME_OR(DCC_CTX_AUTHOR((ctx_)), (fallback_)) \
    )
#define DCC_CTX_AUTHOR_DISPLAY_NAME(ctx_) DCC_CTX_AUTHOR_DISPLAY_NAME_OR((ctx_), "")
#define DCC_CTX_TARGET_USERNAME_OR(ctx_, fallback_) \
    DCC_USER_USERNAME_OR(DCC_CTX_TARGET_USER((ctx_)), (fallback_))
#define DCC_CTX_TARGET_USERNAME(ctx_) DCC_CTX_TARGET_USERNAME_OR((ctx_), "")
#define DCC_CTX_TARGET_DISPLAY_NAME_OR(ctx_, fallback_) \
    dcc_sugar_member_display_name_or( \
        DCC_CTX_TARGET_MEMBER((ctx_)), \
        DCC_USER_DISPLAY_NAME_OR(DCC_CTX_TARGET_USER((ctx_)), (fallback_)) \
    )
#define DCC_CTX_TARGET_DISPLAY_NAME(ctx_) DCC_CTX_TARGET_DISPLAY_NAME_OR((ctx_), "")
#define DCC_CTX_TARGET_ROLE_NAME_OR(ctx_, fallback_) \
    DCC_ROLE_NAME_OR(DCC_CTX_TARGET_ROLE((ctx_)), (fallback_))
#define DCC_CTX_TARGET_ROLE_NAME(ctx_) DCC_CTX_TARGET_ROLE_NAME_OR((ctx_), "")
#define DCC_CTX_TARGET_CHANNEL_NAME_OR(ctx_, fallback_) \
    DCC_CHANNEL_NAME_OR(DCC_CTX_TARGET_CHANNEL((ctx_)), (fallback_))
#define DCC_CTX_TARGET_CHANNEL_NAME(ctx_) DCC_CTX_TARGET_CHANNEL_NAME_OR((ctx_), "")

#define DCC_CTX_GUILD_ICON_URL_EXT(ctx_, extension_, image_size_) \
    DCC_GUILD_ICON_URL_EXT(DCC_CTX_GUILD((ctx_)), (extension_), (image_size_))
#define DCC_CTX_GUILD_ICON_URL(ctx_) DCC_CTX_GUILD_ICON_URL_EXT((ctx_), "png", DCC_CDN_DEFAULT_SIZE)
#define DCC_CTX_AUTHOR_AVATAR_URL_EXT(ctx_, extension_, image_size_) \
    DCC_MEMBER_AVATAR_URL_EXT( \
        DCC_CTX_MEMBER((ctx_)), \
        DCC_CTX_AUTHOR((ctx_)), \
        (extension_), \
        (image_size_) \
    )
#define DCC_CTX_AUTHOR_AVATAR_URL(ctx_) \
    DCC_CTX_AUTHOR_AVATAR_URL_EXT((ctx_), "png", DCC_CDN_DEFAULT_SIZE)
#define DCC_CTX_TARGET_AVATAR_URL_EXT(ctx_, extension_, image_size_) \
    DCC_MEMBER_AVATAR_URL_EXT( \
        DCC_CTX_TARGET_MEMBER((ctx_)), \
        DCC_CTX_TARGET_USER((ctx_)), \
        (extension_), \
        (image_size_) \
    )
#define DCC_CTX_TARGET_AVATAR_URL(ctx_) \
    DCC_CTX_TARGET_AVATAR_URL_EXT((ctx_), "png", DCC_CDN_DEFAULT_SIZE)

#define DCC_CTX_USER_MENTION(ctx_) DCC_MENTION_USER(DCC_CTX_USER_ID((ctx_)))
#define DCC_CTX_AUTHOR_MENTION(ctx_) DCC_CTX_USER_MENTION((ctx_))
#define DCC_CTX_MEMBER_MENTION(ctx_) DCC_MENTION_MEMBER(DCC_CTX_USER_ID((ctx_)))
#define DCC_CTX_CHANNEL_MENTION(ctx_) DCC_MENTION_CHANNEL(DCC_CTX_CHANNEL_ID((ctx_)))
#define DCC_CTX_TARGET_MENTION(ctx_) DCC_MENTION_USER(DCC_CTX_TARGET_ID((ctx_)))
#define DCC_CTX_TARGET_USER_MENTION(ctx_) DCC_MENTION_USER(DCC_CTX_TARGET_ID((ctx_)))
#define DCC_CTX_TARGET_MEMBER_MENTION(ctx_) DCC_MENTION_MEMBER(DCC_CTX_TARGET_ID((ctx_)))
#define DCC_CTX_TARGET_ROLE_MENTION(ctx_) DCC_MENTION_ROLE(DCC_CTX_TARGET_ID((ctx_)))
#define DCC_CTX_TARGET_CHANNEL_MENTION(ctx_) DCC_MENTION_CHANNEL(DCC_CTX_TARGET_ID((ctx_)))
#define DCC_CTX_COMMAND_MENTION(ctx_) \
    DCC_MENTION_COMMAND(DCC_CTX_COMMAND_NAME((ctx_)), DCC_CTX_COMMAND_ID((ctx_)))
#define DCC_CTX_MESSAGE_LINK(ctx_) \
    dcc_sugar_ctx_message_link((char[DCC_MESSAGE_LINK_MAX]){ 0 }, DCC_MESSAGE_LINK_MAX, (ctx_))

#define DCC_CTX_FOCUSED_OPTION(ctx_) dcc_ctx_focused_option((ctx_))
#define DCC_CTX_FOCUSED_NAME(ctx_) dcc_ctx_focused_option_name((ctx_))
#define DCC_CTX_FOCUSED_STRING(ctx_, fallback_) dcc_ctx_focused_option_string((ctx_), (fallback_))

#define DCC_CTX_ARG(ctx_, name_) dcc_ctx_option((ctx_), (name_))
#define DCC_CTX_ARG_PRESENT(ctx_, name_) dcc_ctx_option_present((ctx_), (name_))
#define DCC_CTX_ARG_STRING_OR(ctx_, name_, fallback_) \
    dcc_ctx_option_string((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_STRING(ctx_, name_) DCC_CTX_ARG_STRING_OR((ctx_), (name_), NULL)
#define DCC_CTX_ARG_TEXT_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_STRING_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_TEXT(ctx_, name_) DCC_CTX_ARG_STRING((ctx_), (name_))
#define DCC_CTX_ARG_INT_OR(ctx_, name_, fallback_) \
    dcc_ctx_option_int((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_INT(ctx_, name_) DCC_CTX_ARG_INT_OR((ctx_), (name_), 0)
#define DCC_CTX_ARG_INTEGER_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_INT_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_INTEGER(ctx_, name_) DCC_CTX_ARG_INT((ctx_), (name_))
#define DCC_CTX_ARG_NUMBER_OR(ctx_, name_, fallback_) \
    dcc_ctx_option_number((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_NUMBER(ctx_, name_) DCC_CTX_ARG_NUMBER_OR((ctx_), (name_), 0.0)
#define DCC_CTX_ARG_BOOL_OR(ctx_, name_, fallback_) \
    dcc_ctx_option_bool((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_BOOL(ctx_, name_) DCC_CTX_ARG_BOOL_OR((ctx_), (name_), 0U)
#define DCC_CTX_ARG_BOOLEAN_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_BOOL_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_BOOLEAN(ctx_, name_) DCC_CTX_ARG_BOOL((ctx_), (name_))
#define DCC_CTX_ARG_SNOWFLAKE_OR(ctx_, name_, fallback_) \
    dcc_ctx_option_snowflake((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_SNOWFLAKE(ctx_, name_) DCC_CTX_ARG_SNOWFLAKE_OR((ctx_), (name_), 0U)
#define DCC_CTX_ARG_USER(ctx_, name_) dcc_ctx_option_user((ctx_), (name_))
#define DCC_CTX_ARG_MEMBER(ctx_, name_) dcc_ctx_option_member((ctx_), (name_))
#define DCC_CTX_ARG_ROLE(ctx_, name_) dcc_ctx_option_role((ctx_), (name_))
#define DCC_CTX_ARG_CHANNEL(ctx_, name_) dcc_ctx_option_channel((ctx_), (name_))
#define DCC_CTX_ARG_MESSAGE(ctx_, name_) dcc_ctx_option_message((ctx_), (name_))
#define DCC_CTX_ARG_ATTACHMENT(ctx_, name_) dcc_ctx_option_attachment((ctx_), (name_))
#define DCC_CTX_ARG_USERNAME_OR(ctx_, name_, fallback_) \
    DCC_USER_USERNAME_OR(DCC_CTX_ARG_USER((ctx_), (name_)), (fallback_))
#define DCC_CTX_ARG_USERNAME(ctx_, name_) DCC_CTX_ARG_USERNAME_OR((ctx_), (name_), "")
#define DCC_CTX_ARG_USER_DISPLAY_NAME_OR(ctx_, name_, fallback_) \
    dcc_sugar_member_display_name_or( \
        DCC_CTX_ARG_MEMBER((ctx_), (name_)), \
        DCC_USER_DISPLAY_NAME_OR(DCC_CTX_ARG_USER((ctx_), (name_)), (fallback_)) \
    )
#define DCC_CTX_ARG_USER_DISPLAY_NAME(ctx_, name_) \
    DCC_CTX_ARG_USER_DISPLAY_NAME_OR((ctx_), (name_), "")
#define DCC_CTX_ARG_MEMBER_DISPLAY_NAME_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_USER_DISPLAY_NAME_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_MEMBER_DISPLAY_NAME(ctx_, name_) \
    DCC_CTX_ARG_USER_DISPLAY_NAME((ctx_), (name_))
#define DCC_CTX_ARG_ROLE_NAME_OR(ctx_, name_, fallback_) \
    DCC_ROLE_NAME_OR(DCC_CTX_ARG_ROLE((ctx_), (name_)), (fallback_))
#define DCC_CTX_ARG_ROLE_NAME(ctx_, name_) DCC_CTX_ARG_ROLE_NAME_OR((ctx_), (name_), "")
#define DCC_CTX_ARG_CHANNEL_NAME_OR(ctx_, name_, fallback_) \
    DCC_CHANNEL_NAME_OR(DCC_CTX_ARG_CHANNEL((ctx_), (name_)), (fallback_))
#define DCC_CTX_ARG_CHANNEL_NAME(ctx_, name_) \
    DCC_CTX_ARG_CHANNEL_NAME_OR((ctx_), (name_), "")
#define DCC_CTX_ARG_USER_AVATAR_URL_EXT(ctx_, name_, extension_, image_size_) \
    DCC_MEMBER_AVATAR_URL_EXT( \
        DCC_CTX_ARG_MEMBER((ctx_), (name_)), \
        DCC_CTX_ARG_USER((ctx_), (name_)), \
        (extension_), \
        (image_size_) \
    )
#define DCC_CTX_ARG_USER_AVATAR_URL(ctx_, name_) \
    DCC_CTX_ARG_USER_AVATAR_URL_EXT((ctx_), (name_), "png", DCC_CDN_DEFAULT_SIZE)
#define DCC_CTX_ARG_MEMBER_AVATAR_URL_EXT(ctx_, name_, extension_, image_size_) \
    DCC_CTX_ARG_USER_AVATAR_URL_EXT((ctx_), (name_), (extension_), (image_size_))
#define DCC_CTX_ARG_MEMBER_AVATAR_URL(ctx_, name_) \
    DCC_CTX_ARG_USER_AVATAR_URL((ctx_), (name_))
#define DCC_CTX_ARG_MENTION(ctx_, name_) \
    DCC_MENTION_USER(DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_)))
#define DCC_CTX_ARG_USER_MENTION(ctx_, name_) \
    DCC_MENTION_USER(DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_)))
#define DCC_CTX_ARG_MEMBER_MENTION(ctx_, name_) \
    DCC_MENTION_MEMBER(DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_)))
#define DCC_CTX_ARG_ROLE_MENTION(ctx_, name_) \
    DCC_MENTION_ROLE(DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_)))
#define DCC_CTX_ARG_CHANNEL_MENTION(ctx_, name_) \
    DCC_MENTION_CHANNEL(DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_)))

#define DCC_CTX_OPT_PRESENT(ctx_, name_) dcc_ctx_option_present((ctx_), (name_))
#define DCC_CTX_OPT_STRING(ctx_, name_, fallback_) \
    dcc_ctx_option_string((ctx_), (name_), (fallback_))
#define DCC_CTX_OPT_INT(ctx_, name_, fallback_) dcc_ctx_option_int((ctx_), (name_), (fallback_))
#define DCC_CTX_OPT_NUMBER(ctx_, name_, fallback_) \
    dcc_ctx_option_number((ctx_), (name_), (fallback_))
#define DCC_CTX_OPT_BOOL(ctx_, name_, fallback_) dcc_ctx_option_bool((ctx_), (name_), (fallback_))
#define DCC_CTX_OPT_SNOWFLAKE(ctx_, name_, fallback_) \
    dcc_ctx_option_snowflake((ctx_), (name_), (fallback_))
#define DCC_CTX_OPT_USER(ctx_, name_) dcc_ctx_option_user((ctx_), (name_))
#define DCC_CTX_OPT_MEMBER(ctx_, name_) dcc_ctx_option_member((ctx_), (name_))
#define DCC_CTX_OPT_ROLE(ctx_, name_) dcc_ctx_option_role((ctx_), (name_))
#define DCC_CTX_OPT_CHANNEL(ctx_, name_) dcc_ctx_option_channel((ctx_), (name_))
#define DCC_CTX_OPT_MESSAGE(ctx_, name_) dcc_ctx_option_message((ctx_), (name_))
#define DCC_CTX_OPT_ATTACHMENT(ctx_, name_) dcc_ctx_option_attachment((ctx_), (name_))
#define DCC_CTX_OPTION_USER(ctx_, name_) dcc_ctx_option_user((ctx_), (name_))
#define DCC_CTX_OPTION_MEMBER(ctx_, name_) dcc_ctx_option_member((ctx_), (name_))
#define DCC_CTX_OPTION_ROLE(ctx_, name_) dcc_ctx_option_role((ctx_), (name_))
#define DCC_CTX_OPTION_CHANNEL(ctx_, name_) dcc_ctx_option_channel((ctx_), (name_))
#define DCC_CTX_OPTION_MESSAGE(ctx_, name_) dcc_ctx_option_message((ctx_), (name_))
#define DCC_CTX_OPTION_ATTACHMENT(ctx_, name_) dcc_ctx_option_attachment((ctx_), (name_))

#define DCC_PARAM_PRESENT(ctx_, name_) DCC_CTX_OPT_PRESENT((ctx_), (name_))
#define DCC_PARAM_STRING_OR(ctx_, name_, fallback_) \
    DCC_CTX_OPT_STRING((ctx_), (name_), (fallback_))
#define DCC_PARAM_STRING(ctx_, name_) DCC_PARAM_STRING_OR((ctx_), (name_), NULL)
#define DCC_PARAM_INT_OR(ctx_, name_, fallback_) DCC_CTX_OPT_INT((ctx_), (name_), (fallback_))
#define DCC_PARAM_INT(ctx_, name_) DCC_PARAM_INT_OR((ctx_), (name_), 0)
#define DCC_PARAM_INTEGER_OR(ctx_, name_, fallback_) \
    DCC_PARAM_INT_OR((ctx_), (name_), (fallback_))
#define DCC_PARAM_INTEGER(ctx_, name_) DCC_PARAM_INT((ctx_), (name_))
#define DCC_PARAM_NUMBER_OR(ctx_, name_, fallback_) \
    DCC_CTX_OPT_NUMBER((ctx_), (name_), (fallback_))
#define DCC_PARAM_NUMBER(ctx_, name_) DCC_PARAM_NUMBER_OR((ctx_), (name_), 0.0)
#define DCC_PARAM_BOOL_OR(ctx_, name_, fallback_) DCC_CTX_OPT_BOOL((ctx_), (name_), (fallback_))
#define DCC_PARAM_BOOL(ctx_, name_) DCC_PARAM_BOOL_OR((ctx_), (name_), 0U)
#define DCC_PARAM_BOOLEAN_OR(ctx_, name_, fallback_) \
    DCC_PARAM_BOOL_OR((ctx_), (name_), (fallback_))
#define DCC_PARAM_BOOLEAN(ctx_, name_) DCC_PARAM_BOOL((ctx_), (name_))
#define DCC_PARAM_SNOWFLAKE_OR(ctx_, name_, fallback_) \
    DCC_CTX_OPT_SNOWFLAKE((ctx_), (name_), (fallback_))
#define DCC_PARAM_SNOWFLAKE(ctx_, name_) DCC_PARAM_SNOWFLAKE_OR((ctx_), (name_), 0U)
#define DCC_PARAM_USER(ctx_, name_) DCC_CTX_OPT_USER((ctx_), (name_))
#define DCC_PARAM_MEMBER(ctx_, name_) DCC_CTX_OPT_MEMBER((ctx_), (name_))
#define DCC_PARAM_ROLE(ctx_, name_) DCC_CTX_OPT_ROLE((ctx_), (name_))
#define DCC_PARAM_CHANNEL(ctx_, name_) DCC_CTX_OPT_CHANNEL((ctx_), (name_))
#define DCC_PARAM_MESSAGE(ctx_, name_) DCC_CTX_OPT_MESSAGE((ctx_), (name_))
#define DCC_PARAM_ATTACHMENT(ctx_, name_) DCC_CTX_OPT_ATTACHMENT((ctx_), (name_))
#define DCC_PARAM_FOCUSED(ctx_) DCC_CTX_FOCUSED_OPTION((ctx_))
#define DCC_PARAM_FOCUSED_NAME(ctx_) DCC_CTX_FOCUSED_NAME((ctx_))
#define DCC_PARAM_FOCUSED_STRING_OR(ctx_, fallback_) \
    DCC_CTX_FOCUSED_STRING((ctx_), (fallback_))
#define DCC_PARAM_FOCUSED_STRING(ctx_) DCC_PARAM_FOCUSED_STRING_OR((ctx_), NULL)

#define DCC_OPTION_PRESENT(ctx_, name_) DCC_PARAM_PRESENT((ctx_), (name_))
#define DCC_OPTION_STRING_OR(ctx_, name_, fallback_) \
    DCC_PARAM_STRING_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_STRING(ctx_, name_) DCC_PARAM_STRING((ctx_), (name_))
#define DCC_OPTION_INT_OR(ctx_, name_, fallback_) \
    DCC_PARAM_INT_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_INT(ctx_, name_) DCC_PARAM_INT((ctx_), (name_))
#define DCC_OPTION_INTEGER_OR(ctx_, name_, fallback_) \
    DCC_PARAM_INTEGER_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_INTEGER(ctx_, name_) DCC_PARAM_INTEGER((ctx_), (name_))
#define DCC_OPTION_NUMBER_OR(ctx_, name_, fallback_) \
    DCC_PARAM_NUMBER_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_NUMBER(ctx_, name_) DCC_PARAM_NUMBER((ctx_), (name_))
#define DCC_OPTION_BOOL_OR(ctx_, name_, fallback_) \
    DCC_PARAM_BOOL_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_BOOL(ctx_, name_) DCC_PARAM_BOOL((ctx_), (name_))
#define DCC_OPTION_BOOLEAN_OR(ctx_, name_, fallback_) \
    DCC_PARAM_BOOLEAN_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_BOOLEAN(ctx_, name_) DCC_PARAM_BOOLEAN((ctx_), (name_))
#define DCC_OPTION_SNOWFLAKE_OR(ctx_, name_, fallback_) \
    DCC_PARAM_SNOWFLAKE_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_SNOWFLAKE(ctx_, name_) DCC_PARAM_SNOWFLAKE((ctx_), (name_))
#define DCC_OPTION_USER(ctx_, name_) DCC_PARAM_USER((ctx_), (name_))
#define DCC_OPTION_MEMBER(ctx_, name_) DCC_PARAM_MEMBER((ctx_), (name_))
#define DCC_OPTION_ROLE(ctx_, name_) DCC_PARAM_ROLE((ctx_), (name_))
#define DCC_OPTION_CHANNEL(ctx_, name_) DCC_PARAM_CHANNEL((ctx_), (name_))
#define DCC_OPTION_MESSAGE(ctx_, name_) DCC_PARAM_MESSAGE((ctx_), (name_))
#define DCC_OPTION_ATTACHMENT(ctx_, name_) DCC_PARAM_ATTACHMENT((ctx_), (name_))
#define DCC_OPTION_FOCUSED(ctx_) DCC_PARAM_FOCUSED((ctx_))
#define DCC_OPTION_FOCUSED_NAME(ctx_) DCC_PARAM_FOCUSED_NAME((ctx_))
#define DCC_OPTION_FOCUSED_STRING_OR(ctx_, fallback_) \
    DCC_PARAM_FOCUSED_STRING_OR((ctx_), (fallback_))
#define DCC_OPTION_FOCUSED_STRING(ctx_) DCC_PARAM_FOCUSED_STRING((ctx_))

#define DCC_CTX_OPTION(ctx_, name_) DCC_CTX_ARG((ctx_), (name_))
#define DCC_CTX_OPTION_PRESENT(ctx_, name_) DCC_CTX_ARG_PRESENT((ctx_), (name_))
#define DCC_CTX_OPTION_TEXT_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_TEXT_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_OPTION_TEXT(ctx_, name_) DCC_CTX_ARG_TEXT((ctx_), (name_))
#define DCC_CTX_OPTION_INTEGER_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_INTEGER_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_OPTION_INTEGER(ctx_, name_) DCC_CTX_ARG_INTEGER((ctx_), (name_))
#define DCC_CTX_OPTION_BOOLEAN_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_BOOLEAN_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_OPTION_BOOLEAN(ctx_, name_) DCC_CTX_ARG_BOOLEAN((ctx_), (name_))
#define DCC_CTX_OPTION_MENTION(ctx_, name_) DCC_CTX_ARG_MENTION((ctx_), (name_))
#define DCC_CTX_OPTION_USER_MENTION(ctx_, name_) DCC_CTX_ARG_USER_MENTION((ctx_), (name_))
#define DCC_CTX_OPTION_MEMBER_MENTION(ctx_, name_) \
    DCC_CTX_ARG_MEMBER_MENTION((ctx_), (name_))
#define DCC_CTX_OPTION_ROLE_MENTION(ctx_, name_) DCC_CTX_ARG_ROLE_MENTION((ctx_), (name_))
#define DCC_CTX_OPTION_CHANNEL_MENTION(ctx_, name_) \
    DCC_CTX_ARG_CHANNEL_MENTION((ctx_), (name_))

#define DCC_OPTION_MENTION(ctx_, name_) \
    DCC_MENTION_USER(DCC_OPTION_SNOWFLAKE((ctx_), (name_)))
#define DCC_OPTION_USER_MENTION(ctx_, name_) \
    DCC_MENTION_USER(DCC_OPTION_SNOWFLAKE((ctx_), (name_)))
#define DCC_OPTION_MEMBER_MENTION(ctx_, name_) \
    DCC_MENTION_MEMBER(DCC_OPTION_SNOWFLAKE((ctx_), (name_)))
#define DCC_OPTION_ROLE_MENTION(ctx_, name_) \
    DCC_MENTION_ROLE(DCC_OPTION_SNOWFLAKE((ctx_), (name_)))
#define DCC_OPTION_CHANNEL_MENTION(ctx_, name_) \
    DCC_MENTION_CHANNEL(DCC_OPTION_SNOWFLAKE((ctx_), (name_)))
#define DCC_PARAM_MENTION(ctx_, name_) DCC_OPTION_MENTION((ctx_), (name_))
#define DCC_PARAM_USER_MENTION(ctx_, name_) DCC_OPTION_USER_MENTION((ctx_), (name_))
#define DCC_PARAM_MEMBER_MENTION(ctx_, name_) DCC_OPTION_MEMBER_MENTION((ctx_), (name_))
#define DCC_PARAM_ROLE_MENTION(ctx_, name_) DCC_OPTION_ROLE_MENTION((ctx_), (name_))
#define DCC_PARAM_CHANNEL_MENTION(ctx_, name_) DCC_OPTION_CHANNEL_MENTION((ctx_), (name_))

#define DCC_CTX_FORM_PRESENT(ctx_, custom_id_) \
    (dcc_ctx_form_field((ctx_), (custom_id_)) != NULL ? 1U : 0U)
#define DCC_CTX_FORM_STRING_OR(ctx_, custom_id_, fallback_) \
    dcc_ctx_form_string((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_TEXT(ctx_, custom_id_, fallback_) \
    dcc_ctx_form_string((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_TEXT_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_TEXT((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_TEXT_VALUE(ctx_, custom_id_) \
    DCC_CTX_FORM_TEXT((ctx_), (custom_id_), NULL)
#define DCC_CTX_FORM_BOOL_OR(ctx_, custom_id_, fallback_) \
    dcc_ctx_form_bool((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_CHECKED(ctx_, custom_id_, fallback_) \
    dcc_ctx_form_bool((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_CHECKED_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_CHECKED((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_IS_CHECKED(ctx_, custom_id_) \
    DCC_CTX_FORM_CHECKED((ctx_), (custom_id_), 0U)
#define DCC_CTX_FORM_VALUE_COUNT(ctx_, custom_id_) \
    dcc_ctx_form_values_count((ctx_), (custom_id_))
#define DCC_CTX_FORM_VALUE(ctx_, custom_id_, index_) \
    dcc_ctx_form_value((ctx_), (custom_id_), (index_))
#define DCC_CTX_FORM_FIRST_VALUE(ctx_, custom_id_, fallback_) \
    dcc_ctx_form_first_value((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_FIRST_VALUE_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_FIRST_VALUE((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_PRESENT(ctx_, custom_id_) DCC_CTX_FORM_PRESENT((ctx_), (custom_id_))
#define DCC_CTX_FIELD_TEXT_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_TEXT((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_TEXT(ctx_, custom_id_) DCC_CTX_FIELD_TEXT_OR((ctx_), (custom_id_), NULL)
#define DCC_CTX_FIELD_STRING_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_STRING_OR((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_STRING(ctx_, custom_id_) \
    DCC_CTX_FIELD_STRING_OR((ctx_), (custom_id_), NULL)
#define DCC_CTX_FIELD_CHECKED_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_CHECKED((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_CHECKED(ctx_, custom_id_) \
    DCC_CTX_FIELD_CHECKED_OR((ctx_), (custom_id_), 0U)
#define DCC_CTX_FIELD_BOOL_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_BOOL_OR((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_BOOL(ctx_, custom_id_) DCC_CTX_FIELD_BOOL_OR((ctx_), (custom_id_), 0U)
#define DCC_CTX_FIELD_VALUE_COUNT(ctx_, custom_id_) \
    DCC_CTX_FORM_VALUE_COUNT((ctx_), (custom_id_))
#define DCC_CTX_FIELD_VALUE_AT(ctx_, custom_id_, index_) \
    DCC_CTX_FORM_VALUE((ctx_), (custom_id_), (index_))
#define DCC_CTX_FIELD_FIRST_VALUE_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_FIRST_VALUE((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_FIRST_VALUE(ctx_, custom_id_) \
    DCC_CTX_FIELD_FIRST_VALUE_OR((ctx_), (custom_id_), NULL)
#define DCC_CTX_FIELD_VALUES(ctx_, custom_id_, out_count_) \
    dcc_ctx_form_values((ctx_), (custom_id_), (out_count_))
#define DCC_FIELD_TEXT_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_TEXT((ctx_), (custom_id_), (fallback_))
#define DCC_FIELD_TEXT(ctx_, custom_id_) DCC_FIELD_TEXT_OR((ctx_), (custom_id_), NULL)
#define DCC_FIELD_CHECKED_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_CHECKED((ctx_), (custom_id_), (fallback_))
#define DCC_FIELD_CHECKED(ctx_, custom_id_) DCC_FIELD_CHECKED_OR((ctx_), (custom_id_), 0U)
#define DCC_FIELD_VALUE_COUNT(ctx_, custom_id_) DCC_CTX_FORM_VALUE_COUNT((ctx_), (custom_id_))
#define DCC_FIELD_VALUE_AT(ctx_, custom_id_, index_) \
    DCC_CTX_FORM_VALUE((ctx_), (custom_id_), (index_))
#define DCC_FIELD_FIRST_VALUE_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_FIRST_VALUE((ctx_), (custom_id_), (fallback_))
#define DCC_FIELD_FIRST_VALUE(ctx_, custom_id_) \
    DCC_FIELD_FIRST_VALUE_OR((ctx_), (custom_id_), NULL)
#define DCC_FIELD_VALUES_RAW(ctx_, custom_id_, out_count_) \
    dcc_ctx_form_values((ctx_), (custom_id_), (out_count_))

#define DCC_CTX_SELECTED_COUNT(ctx_) dcc_ctx_component_value_count((ctx_))
#define DCC_CTX_SELECTED_VALUE(ctx_, index_) dcc_ctx_component_value((ctx_), (index_))
#define DCC_CTX_SELECTED_VALUE_AT(ctx_, index_) DCC_CTX_SELECTED_VALUE((ctx_), (index_))
#define DCC_CTX_SELECTED_FIRST_OR(ctx_, fallback_) \
    dcc_ctx_component_first_value((ctx_), (fallback_))
#define DCC_CTX_SELECTED_FIRST(ctx_) DCC_CTX_SELECTED_FIRST_OR((ctx_), NULL)
#define DCC_CTX_SELECTED_VALUES(ctx_, out_count_) dcc_ctx_component_values((ctx_), (out_count_))
#define DCC_CTX_SELECT_COUNT(ctx_) DCC_CTX_SELECTED_COUNT((ctx_))
#define DCC_CTX_SELECT_VALUE(ctx_, index_) DCC_CTX_SELECTED_VALUE((ctx_), (index_))
#define DCC_CTX_SELECT_VALUE_AT(ctx_, index_) DCC_CTX_SELECTED_VALUE((ctx_), (index_))
#define DCC_CTX_SELECT_FIRST_OR(ctx_, fallback_) \
    DCC_CTX_SELECTED_FIRST_OR((ctx_), (fallback_))
#define DCC_CTX_SELECT_FIRST(ctx_) DCC_CTX_SELECTED_FIRST((ctx_))
#define DCC_CTX_SELECT_VALUES(ctx_, out_count_) DCC_CTX_SELECTED_VALUES((ctx_), (out_count_))
#define DCC_SELECTED_COUNT(ctx_) DCC_CTX_SELECTED_COUNT((ctx_))
#define DCC_SELECTED_VALUE(ctx_, index_) DCC_CTX_SELECTED_VALUE((ctx_), (index_))
#define DCC_SELECTED_FIRST_OR(ctx_, fallback_) DCC_CTX_SELECTED_FIRST_OR((ctx_), (fallback_))
#define DCC_SELECTED_FIRST(ctx_) DCC_SELECTED_FIRST_OR((ctx_), NULL)
#define DCC_SELECTED_VALUES(ctx_, out_count_) DCC_CTX_SELECTED_VALUES((ctx_), (out_count_))
#define DCC_SELECT_COUNT(ctx_) DCC_CTX_SELECTED_COUNT((ctx_))
#define DCC_SELECT_VALUE_AT(ctx_, index_) DCC_CTX_SELECTED_VALUE((ctx_), (index_))
#define DCC_SELECT_FIRST_OR(ctx_, fallback_) DCC_CTX_SELECTED_FIRST_OR((ctx_), (fallback_))
#define DCC_SELECT_FIRST(ctx_) DCC_SELECT_FIRST_OR((ctx_), NULL)
#define DCC_SELECT_VALUES_RAW(ctx_, out_count_) DCC_CTX_SELECTED_VALUES((ctx_), (out_count_))

#ifndef DCC_CTX_REQUIRED_OPTION_MESSAGE
#define DCC_CTX_REQUIRED_OPTION_MESSAGE "Missing option."
#endif
#ifndef DCC_CTX_REQUIRED_GUILD_MESSAGE
#define DCC_CTX_REQUIRED_GUILD_MESSAGE "This command can only be used in a server."
#endif
#ifndef DCC_CTX_REQUIRED_DM_MESSAGE
#define DCC_CTX_REQUIRED_DM_MESSAGE "This command can only be used in DMs."
#endif
#ifndef DCC_CTX_REQUIRED_AUTHOR_MESSAGE
#define DCC_CTX_REQUIRED_AUTHOR_MESSAGE "Only the original user can use this."
#endif
#ifndef DCC_CTX_REQUIRED_PERMISSION_MESSAGE
#define DCC_CTX_REQUIRED_PERMISSION_MESSAGE "Missing required permissions."
#endif
#ifndef DCC_CTX_REQUIRED_ROLE_MESSAGE
#define DCC_CTX_REQUIRED_ROLE_MESSAGE "Missing required role."
#endif
#ifndef DCC_CTX_REQUIRED_TEXT_MESSAGE
#define DCC_CTX_REQUIRED_TEXT_MESSAGE "Missing text."
#endif
#ifndef DCC_CTX_REQUIRED_FIELD_MESSAGE
#define DCC_CTX_REQUIRED_FIELD_MESSAGE "Missing field."
#endif
#ifndef DCC_CTX_REQUIRED_SELECT_MESSAGE
#define DCC_CTX_REQUIRED_SELECT_MESSAGE "Choose a value."
#endif
#ifndef DCC_CTX_REQUIRED_COMPONENT_MESSAGE
#define DCC_CTX_REQUIRED_COMPONENT_MESSAGE "Unknown component."
#endif
#ifndef DCC_CTX_REQUIRED_ACTION_MESSAGE
#define DCC_CTX_REQUIRED_ACTION_MESSAGE "Unknown component action."
#endif

#define DCC_CTX_REQUIRE_SELECTED_FIRST(var_, ctx_, message_) \
    const char *var_ = DCC_CTX_SELECTED_FIRST((ctx_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_SELECT_FIRST(var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECTED_FIRST(var_, (ctx_), (message_))
#define DCC_CTX_REQUIRE_SELECTED_VALUE(var_, ctx_, index_, message_) \
    const char *var_ = DCC_CTX_SELECTED_VALUE((ctx_), (index_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_SELECT_VALUE(var_, ctx_, index_, message_) \
    DCC_CTX_REQUIRE_SELECTED_VALUE(var_, (ctx_), (index_), (message_))
#define DCC_CTX_REQUIRE_SELECTED_VALUES(values_var_, count_var_, ctx_, message_) \
    size_t count_var_ = 0U; \
    const char **values_var_ = DCC_CTX_SELECTED_VALUES((ctx_), &count_var_); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), values_var_ != NULL && count_var_ != 0U, (message_))
#define DCC_CTX_REQUIRE_SELECT_VALUES(values_var_, count_var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECTED_VALUES(values_var_, count_var_, (ctx_), (message_))
#define DCC_REQUIRE_SELECTED_FIRST(var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECTED_FIRST(var_, (ctx_), (message_))
#define DCC_REQUIRE_SELECT_FIRST(var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECT_FIRST(var_, (ctx_), (message_))
#define DCC_REQUIRE_SELECTED_VALUE(var_, ctx_, index_, message_) \
    DCC_CTX_REQUIRE_SELECTED_VALUE(var_, (ctx_), (index_), (message_))
#define DCC_REQUIRE_SELECT_VALUE(var_, ctx_, index_, message_) \
    DCC_CTX_REQUIRE_SELECT_VALUE(var_, (ctx_), (index_), (message_))
#define DCC_REQUIRE_SELECTED_VALUES(values_var_, count_var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECTED_VALUES(values_var_, count_var_, (ctx_), (message_))
#define DCC_REQUIRE_SELECT_VALUES(values_var_, count_var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECT_VALUES(values_var_, count_var_, (ctx_), (message_))
#define DCC_CTX_REQUIRED_SELECTED_FIRST(var_, ctx_) \
    DCC_CTX_REQUIRE_SELECTED_FIRST(var_, (ctx_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECT_FIRST(var_, ctx_) \
    DCC_CTX_REQUIRE_SELECT_FIRST(var_, (ctx_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECTED_VALUE(var_, ctx_, index_) \
    DCC_CTX_REQUIRE_SELECTED_VALUE(var_, (ctx_), (index_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECT_VALUE(var_, ctx_, index_) \
    DCC_CTX_REQUIRE_SELECT_VALUE(var_, (ctx_), (index_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECTED_VALUES(values_var_, count_var_, ctx_) \
    DCC_CTX_REQUIRE_SELECTED_VALUES(values_var_, count_var_, (ctx_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECT_VALUES(values_var_, count_var_, ctx_) \
    DCC_CTX_REQUIRE_SELECT_VALUES(values_var_, count_var_, (ctx_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_REQUIRED_SELECTED_FIRST(var_, ctx_) \
    DCC_CTX_REQUIRED_SELECTED_FIRST(var_, (ctx_))
#define DCC_REQUIRED_SELECT_FIRST(var_, ctx_) \
    DCC_CTX_REQUIRED_SELECT_FIRST(var_, (ctx_))
#define DCC_REQUIRED_SELECTED_VALUE(var_, ctx_, index_) \
    DCC_CTX_REQUIRED_SELECTED_VALUE(var_, (ctx_), (index_))
#define DCC_REQUIRED_SELECT_VALUE(var_, ctx_, index_) \
    DCC_CTX_REQUIRED_SELECT_VALUE(var_, (ctx_), (index_))
#define DCC_REQUIRED_SELECTED_VALUES(values_var_, count_var_, ctx_) \
    DCC_CTX_REQUIRED_SELECTED_VALUES(values_var_, count_var_, (ctx_))
#define DCC_REQUIRED_SELECT_VALUES(values_var_, count_var_, ctx_) \
    DCC_CTX_REQUIRED_SELECT_VALUES(values_var_, count_var_, (ctx_))

#define DCC_CTX_REQUIRE(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_REPLY_EPHEMERAL((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BAD_INPUT(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_BAD_INPUT((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BAD_INPUT_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_BAD_INPUT_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_DENY(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_DENY((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_DENY_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_DENY_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_NOT_FOUND(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_NOT_FOUND((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_NOT_FOUND_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_NOT_FOUND_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BUSY(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_BUSY((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BUSY_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_BUSY_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_FAIL(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_FAIL((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_FAIL_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_FAIL_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_INTERNAL(ctx_, condition_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_INTERNAL_ERROR((ctx_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_GUILD(ctx_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_IS_GUILD((ctx_)), (message_))
#define DCC_CTX_REQUIRE_DM(ctx_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_IS_DM((ctx_)), (message_))
#define DCC_CTX_REQUIRE_AUTHOR(ctx_, user_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_IS_AUTHOR((ctx_), (user_id_)), (message_))
#define DCC_CTX_REQUIRE_PERMISSIONS(ctx_, permissions_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_PERMISSIONS((ctx_), (permissions_)), (message_))
#define DCC_CTX_REQUIRE_ROLE(ctx_, role_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_ROLE((ctx_), (role_id_)), (message_))
#define DCC_CTX_REQUIRE_ANY_ROLE(ctx_, role_ids_, role_id_count_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_ANY_ROLE((ctx_), (role_ids_), (role_id_count_)), (message_))
#define DCC_CTX_REQUIRE_ALL_ROLES(ctx_, role_ids_, role_id_count_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_ALL_ROLES((ctx_), (role_ids_), (role_id_count_)), (message_))
#define DCC_CTX_REQUIRE_ANY_ROLE_IDS(ctx_, message_, ...) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_ANY_ROLE_IDS((ctx_), __VA_ARGS__), (message_))
#define DCC_CTX_REQUIRE_ALL_ROLE_IDS(ctx_, message_, ...) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_ALL_ROLE_IDS((ctx_), __VA_ARGS__), (message_))
#define DCC_CTX_REQUIRE_TEXT(ctx_, value_, message_) \
    DCC_CTX_REQUIRE((ctx_), dcc_sugar_ctx_has_text((value_)), (message_))
#define DCC_CTX_REQUIRE_OPTION(ctx_, name_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_OPT_PRESENT((ctx_), (name_)), (message_))
#define DCC_CTX_REQUIRE_ARG(ctx_, name_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_ARG_PRESENT((ctx_), (name_)), (message_))
#define DCC_CTX_REQUIRE_ARG_TEXT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE((ctx_), dcc_sugar_ctx_has_text(DCC_CTX_ARG_TEXT((ctx_), (name_))), (message_))
#define DCC_CTX_REQUIRE_ARG_STRING(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_TEXT((ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_FIELD(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), dcc_sugar_ctx_has_text(DCC_FIELD_TEXT((ctx_), (custom_id_))), (message_))
#define DCC_CTX_REQUIRE_FORM(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_FORM_PRESENT((ctx_), (custom_id_)), (message_))
#define DCC_CTX_REQUIRE_FORM_TEXT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), dcc_sugar_ctx_has_text(DCC_CTX_FORM_TEXT_VALUE((ctx_), (custom_id_))), (message_))
#define DCC_CTX_REQUIRE_FORM_STRING(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_TEXT((ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_SELECTED(ctx_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_SELECTED_COUNT((ctx_)) != 0U, (message_))
#define DCC_CTX_REQUIRE_SELECT(ctx_, message_) DCC_CTX_REQUIRE_SELECTED((ctx_), (message_))
#define DCC_CTX_REQUIRE_CUSTOM_ID(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_HAS_CUSTOM_ID((ctx_), (custom_id_)), (message_))
#define DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX(ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_CUSTOM_ID_STARTS_WITH((ctx_), (prefix_)), (message_))
#define DCC_CTX_REQUIRED_GUILD(ctx_) DCC_CTX_REQUIRE_GUILD((ctx_), DCC_CTX_REQUIRED_GUILD_MESSAGE)
#define DCC_CTX_REQUIRED_DM(ctx_) DCC_CTX_REQUIRE_DM((ctx_), DCC_CTX_REQUIRED_DM_MESSAGE)
#define DCC_CTX_REQUIRED_AUTHOR(ctx_, user_id_) \
    DCC_CTX_REQUIRE_AUTHOR((ctx_), (user_id_), DCC_CTX_REQUIRED_AUTHOR_MESSAGE)
#define DCC_CTX_REQUIRED_PERMISSIONS(ctx_, permissions_) \
    DCC_CTX_REQUIRE_PERMISSIONS((ctx_), (permissions_), DCC_CTX_REQUIRED_PERMISSION_MESSAGE)
#define DCC_CTX_REQUIRED_ROLE(ctx_, role_id_) \
    DCC_CTX_REQUIRE_ROLE((ctx_), (role_id_), DCC_CTX_REQUIRED_ROLE_MESSAGE)
#define DCC_CTX_REQUIRED_ANY_ROLE(ctx_, role_ids_, role_id_count_) \
    DCC_CTX_REQUIRE_ANY_ROLE((ctx_), (role_ids_), (role_id_count_), DCC_CTX_REQUIRED_ROLE_MESSAGE)
#define DCC_CTX_REQUIRED_ALL_ROLES(ctx_, role_ids_, role_id_count_) \
    DCC_CTX_REQUIRE_ALL_ROLES((ctx_), (role_ids_), (role_id_count_), DCC_CTX_REQUIRED_ROLE_MESSAGE)
#define DCC_CTX_REQUIRED_ANY_ROLE_IDS(ctx_, ...) \
    DCC_CTX_REQUIRE_ANY_ROLE_IDS((ctx_), DCC_CTX_REQUIRED_ROLE_MESSAGE, __VA_ARGS__)
#define DCC_CTX_REQUIRED_ALL_ROLE_IDS(ctx_, ...) \
    DCC_CTX_REQUIRE_ALL_ROLE_IDS((ctx_), DCC_CTX_REQUIRED_ROLE_MESSAGE, __VA_ARGS__)
#define DCC_CTX_REQUIRED_TEXT(ctx_, value_) \
    DCC_CTX_REQUIRE_TEXT((ctx_), (value_), DCC_CTX_REQUIRED_TEXT_MESSAGE)
#define DCC_CTX_REQUIRED_OPTION(ctx_, name_) \
    DCC_CTX_REQUIRE_OPTION((ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_ARG(ctx_, name_) \
    DCC_CTX_REQUIRE_ARG((ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_ARG_TEXT(ctx_, name_) \
    DCC_CTX_REQUIRE_ARG_TEXT((ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_ARG_STRING(ctx_, name_) \
    DCC_CTX_REQUIRE_ARG_STRING((ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_FIELD(ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FIELD((ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_FORM(ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FORM((ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_FORM_TEXT(ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FORM_TEXT((ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_FORM_STRING(ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FORM_STRING((ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_SELECTED(ctx_) \
    DCC_CTX_REQUIRE_SELECTED((ctx_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECT(ctx_) DCC_CTX_REQUIRED_SELECTED((ctx_))
#define DCC_CTX_REQUIRED_CUSTOM_ID(ctx_, custom_id_) \
    DCC_CTX_REQUIRE_CUSTOM_ID((ctx_), (custom_id_), DCC_CTX_REQUIRED_COMPONENT_MESSAGE)
#define DCC_CTX_REQUIRED_CUSTOM_ID_PREFIX(ctx_, prefix_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX((ctx_), (prefix_), DCC_CTX_REQUIRED_COMPONENT_MESSAGE)
#define DCC_CTX_REQUIRE_CUSTOM_ID_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_HAS_CUSTOM_ID((ctx_), (custom_id_)), (message_))
#define DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX_BAD_INPUT(ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_CUSTOM_ID_STARTS_WITH((ctx_), (prefix_)), (message_))
#define DCC_CTX_REQUIRE_CUSTOM_ID_VALUE(var_, ctx_, message_) \
    const char *var_ = DCC_CTX_CUSTOM_ID((ctx_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_CUSTOM_ID_SUFFIX(var_, ctx_, prefix_, message_) \
    const char *var_ = DCC_CTX_CUSTOM_ID_SUFFIX((ctx_), (prefix_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_ACTION(var_, ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_SUFFIX(var_, (ctx_), (prefix_), (message_))
#define DCC_CTX_REQUIRE_COMPONENT_ACTION(var_, ctx_, namespace_, message_) \
    const char *var_ = DCC_CTX_COMPONENT_ACTION((ctx_), (namespace_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_ACTION_IN_NS(var_, ctx_, namespace_, message_) \
    DCC_CTX_REQUIRE_COMPONENT_ACTION(var_, (ctx_), (namespace_), (message_))
#define DCC_CTX_REQUIRE_ACTION_IN_NAMESPACE(var_, ctx_, namespace_, message_) \
    DCC_CTX_REQUIRE_COMPONENT_ACTION(var_, (ctx_), (namespace_), (message_))
#define DCC_CTX_REQUIRE_COMPONENT_IS(ctx_, namespace_, action_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_COMPONENT_IS((ctx_), (namespace_), (action_)), (message_))
#define DCC_CTX_REQUIRED_CUSTOM_ID_VALUE(var_, ctx_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_VALUE(var_, (ctx_), DCC_CTX_REQUIRED_COMPONENT_MESSAGE)
#define DCC_CTX_REQUIRED_CUSTOM_ID_SUFFIX(var_, ctx_, prefix_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_SUFFIX(var_, (ctx_), (prefix_), DCC_CTX_REQUIRED_ACTION_MESSAGE)
#define DCC_CTX_REQUIRED_ACTION(var_, ctx_, prefix_) \
    DCC_CTX_REQUIRE_ACTION(var_, (ctx_), (prefix_), DCC_CTX_REQUIRED_ACTION_MESSAGE)
#define DCC_CTX_REQUIRED_COMPONENT_ACTION(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRE_COMPONENT_ACTION(var_, (ctx_), (namespace_), DCC_CTX_REQUIRED_ACTION_MESSAGE)
#define DCC_CTX_REQUIRED_ACTION_IN_NS(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRED_COMPONENT_ACTION(var_, (ctx_), (namespace_))
#define DCC_CTX_REQUIRED_ACTION_IN_NAMESPACE(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRED_COMPONENT_ACTION(var_, (ctx_), (namespace_))
#define DCC_CTX_REQUIRED_COMPONENT_IS(ctx_, namespace_, action_) \
    DCC_CTX_REQUIRE_COMPONENT_IS((ctx_), (namespace_), (action_), DCC_CTX_REQUIRED_ACTION_MESSAGE)
#define DCC_CTX_REQUIRE_ARG_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_ARG_PRESENT((ctx_), (name_)), (message_))
#define DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(DCC_CTX_ARG_TEXT((ctx_), (name_))), (message_))
#define DCC_CTX_REQUIRE_ARG_STRING_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_OPTION_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_OPTION_TEXT_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_OPTION_STRING_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_STRING_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_STRING_OPTION(var_, ctx_, name_, message_) \
    const char *var_ = DCC_CTX_OPTION_TEXT((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_TEXT_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_STRING_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_INT_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_option_has_type((ctx_), (name_), DCC_INTERACTION_OPTION_VALUE_INTEGER), \
        (message_) \
    ); \
    int64_t var_ = DCC_CTX_OPTION_INTEGER((ctx_), (name_))
#define DCC_CTX_REQUIRE_INTEGER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_INT_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_NUMBER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_option_has_type((ctx_), (name_), DCC_INTERACTION_OPTION_VALUE_NUMBER), \
        (message_) \
    ); \
    double var_ = DCC_CTX_ARG_NUMBER((ctx_), (name_))
#define DCC_CTX_REQUIRE_BOOL_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_option_has_type((ctx_), (name_), DCC_INTERACTION_OPTION_VALUE_BOOLEAN), \
        (message_) \
    ); \
    uint8_t var_ = DCC_CTX_OPTION_BOOLEAN((ctx_), (name_))
#define DCC_CTX_REQUIRE_BOOLEAN_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BOOL_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_SNOWFLAKE_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_option_has_type((ctx_), (name_), DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE), \
        (message_) \
    ); \
    dcc_snowflake_t var_ = DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_))
#define DCC_CTX_REQUIRE_USER_OPTION(var_, ctx_, name_, message_) \
    const dcc_user_t *var_ = DCC_CTX_OPTION_USER((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRE_MEMBER_OPTION(var_, ctx_, name_, message_) \
    const dcc_member_t *var_ = DCC_CTX_OPTION_MEMBER((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRE_ROLE_OPTION(var_, ctx_, name_, message_) \
    const dcc_role_t *var_ = DCC_CTX_OPTION_ROLE((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRE_CHANNEL_OPTION(var_, ctx_, name_, message_) \
    const dcc_channel_t *var_ = DCC_CTX_OPTION_CHANNEL((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRE_MESSAGE_OPTION(var_, ctx_, name_, message_) \
    const dcc_message_t *var_ = DCC_CTX_OPTION_MESSAGE((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRE_ATTACHMENT_OPTION(var_, ctx_, name_, message_) \
    const dcc_attachment_t *var_ = DCC_CTX_OPTION_ATTACHMENT((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRED_STRING_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_STRING_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_TEXT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_TEXT_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_INT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_INT_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_INTEGER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_INTEGER_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_NUMBER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_NUMBER_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_BOOL_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_BOOL_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_BOOLEAN_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_BOOLEAN_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_SNOWFLAKE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_SNOWFLAKE_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_USER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_USER_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_MEMBER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_MEMBER_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_ROLE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_ROLE_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_CHANNEL_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_CHANNEL_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_MESSAGE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_MESSAGE_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_ATTACHMENT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_ATTACHMENT_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRE_FORM_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_FORM_PRESENT((ctx_), (custom_id_)), (message_))
#define DCC_CTX_REQUIRE_FORM_TEXT_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_has_text(DCC_CTX_FORM_TEXT_VALUE((ctx_), (custom_id_))), \
        (message_) \
    )
#define DCC_CTX_REQUIRE_FORM_STRING_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_TEXT_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_FIELD_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_FIELD_TEXT_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_TEXT_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_FIELD_STRING_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_STRING_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_STRING_FIELD(var_, ctx_, custom_id_, message_) \
    const char *var_ = DCC_CTX_FIELD_TEXT((ctx_), (custom_id_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_TEXT_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_STRING_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_STRING_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_STRING_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_TEXT_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_TEXT_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_BOOL_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_form_has_type((ctx_), (custom_id_), DCC_INTERACTION_FORM_VALUE_BOOLEAN), \
        (message_) \
    ); \
    uint8_t var_ = DCC_CTX_FIELD_BOOL((ctx_), (custom_id_))
#define DCC_CTX_REQUIRE_BOOLEAN_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOL_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_CHECKBOX_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOL_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_BOOL_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOL_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_BOOLEAN_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOLEAN_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_CHECKBOX_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_CHECKBOX_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_VALUES_FIELD(values_var_, count_var_, ctx_, custom_id_, message_) \
    size_t count_var_ = 0U; \
    const char **values_var_ = DCC_CTX_FIELD_VALUES((ctx_), (custom_id_), &count_var_); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), values_var_ != NULL && count_var_ != 0U, (message_))
#define DCC_CTX_REQUIRE_VALUES_FORM(values_var_, count_var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_VALUES_FIELD(values_var_, count_var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_FIRST_VALUE_FIELD(var_, ctx_, custom_id_, message_) \
    const char *var_ = DCC_CTX_FIELD_FIRST_VALUE((ctx_), (custom_id_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_FIRST_VALUE_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIRST_VALUE_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_SELECT_BAD_INPUT(ctx_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_SELECTED_COUNT((ctx_)) != 0U, (message_))
#define DCC_CTX_REQUIRED_STRING_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_STRING_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_TEXT_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_TEXT_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_STRING_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_STRING_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_TEXT_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_TEXT_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_BOOL_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_BOOL_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_BOOLEAN_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_BOOLEAN_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_CHECKBOX_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_CHECKBOX_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_BOOL_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_BOOL_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_BOOLEAN_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_BOOLEAN_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_CHECKBOX_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_CHECKBOX_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_VALUES_FIELD(values_var_, count_var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_VALUES_FIELD(values_var_, count_var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_VALUES_FORM(values_var_, count_var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_VALUES_FORM(values_var_, count_var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_FIRST_VALUE_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FIRST_VALUE_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_FIRST_VALUE_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FIRST_VALUE_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_SELECT_MESSAGE)

#define DCC_REQUIRE(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE((ctx_), (condition_), (message_))
#define DCC_REQUIRE_BAD_INPUT(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), (condition_), (message_))
#define DCC_REQUIRE_BAD_INPUT_F(ctx_, condition_, ...) \
    DCC_CTX_REQUIRE_BAD_INPUT_F((ctx_), (condition_), __VA_ARGS__)
#define DCC_REQUIRE_DENY(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE_DENY((ctx_), (condition_), (message_))
#define DCC_REQUIRE_DENY_F(ctx_, condition_, ...) \
    DCC_CTX_REQUIRE_DENY_F((ctx_), (condition_), __VA_ARGS__)
#define DCC_REQUIRE_NOT_FOUND(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE_NOT_FOUND((ctx_), (condition_), (message_))
#define DCC_REQUIRE_NOT_FOUND_F(ctx_, condition_, ...) \
    DCC_CTX_REQUIRE_NOT_FOUND_F((ctx_), (condition_), __VA_ARGS__)
#define DCC_REQUIRE_BUSY(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE_BUSY((ctx_), (condition_), (message_))
#define DCC_REQUIRE_BUSY_F(ctx_, condition_, ...) \
    DCC_CTX_REQUIRE_BUSY_F((ctx_), (condition_), __VA_ARGS__)
#define DCC_REQUIRE_FAIL(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE_FAIL((ctx_), (condition_), (message_))
#define DCC_REQUIRE_FAIL_F(ctx_, condition_, ...) \
    DCC_CTX_REQUIRE_FAIL_F((ctx_), (condition_), __VA_ARGS__)
#define DCC_REQUIRE_INTERNAL(ctx_, condition_) \
    DCC_CTX_REQUIRE_INTERNAL((ctx_), (condition_))
#define DCC_REQUIRE_GUILD(ctx_, message_) DCC_CTX_REQUIRE_GUILD((ctx_), (message_))
#define DCC_REQUIRE_DM(ctx_, message_) DCC_CTX_REQUIRE_DM((ctx_), (message_))
#define DCC_REQUIRE_AUTHOR(ctx_, user_id_, message_) \
    DCC_CTX_REQUIRE_AUTHOR((ctx_), (user_id_), (message_))
#define DCC_REQUIRE_PERMISSIONS(ctx_, permissions_, message_) \
    DCC_CTX_REQUIRE_PERMISSIONS((ctx_), (permissions_), (message_))
#define DCC_REQUIRE_ROLE(ctx_, role_id_, message_) \
    DCC_CTX_REQUIRE_ROLE((ctx_), (role_id_), (message_))
#define DCC_REQUIRE_ANY_ROLE(ctx_, role_ids_, role_id_count_, message_) \
    DCC_CTX_REQUIRE_ANY_ROLE((ctx_), (role_ids_), (role_id_count_), (message_))
#define DCC_REQUIRE_ALL_ROLES(ctx_, role_ids_, role_id_count_, message_) \
    DCC_CTX_REQUIRE_ALL_ROLES((ctx_), (role_ids_), (role_id_count_), (message_))
#define DCC_REQUIRE_ANY_ROLE_IDS(ctx_, message_, ...) \
    DCC_CTX_REQUIRE_ANY_ROLE_IDS((ctx_), (message_), __VA_ARGS__)
#define DCC_REQUIRE_ALL_ROLE_IDS(ctx_, message_, ...) \
    DCC_CTX_REQUIRE_ALL_ROLE_IDS((ctx_), (message_), __VA_ARGS__)
#define DCC_REQUIRE_TEXT(ctx_, value_, message_) \
    DCC_CTX_REQUIRE_TEXT((ctx_), (value_), (message_))
#define DCC_REQUIRE_OPTION(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_OPTION((ctx_), (name_), (message_))
#define DCC_REQUIRE_ARG(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG((ctx_), (name_), (message_))
#define DCC_REQUIRE_ARG_TEXT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_TEXT((ctx_), (name_), (message_))
#define DCC_REQUIRE_ARG_STRING(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_STRING((ctx_), (name_), (message_))
#define DCC_REQUIRE_FIELD(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIELD((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FORM(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FORM_TEXT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_TEXT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FORM_STRING(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_STRING((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_SELECTED(ctx_, message_) DCC_CTX_REQUIRE_SELECTED((ctx_), (message_))
#define DCC_REQUIRE_SELECT(ctx_, message_) DCC_CTX_REQUIRE_SELECT((ctx_), (message_))
#define DCC_REQUIRE_CUSTOM_ID(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_CUSTOM_ID_PREFIX(ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX((ctx_), (prefix_), (message_))
#define DCC_REQUIRED_GUILD(ctx_) DCC_CTX_REQUIRED_GUILD((ctx_))
#define DCC_REQUIRED_DM(ctx_) DCC_CTX_REQUIRED_DM((ctx_))
#define DCC_REQUIRED_AUTHOR(ctx_, user_id_) DCC_CTX_REQUIRED_AUTHOR((ctx_), (user_id_))
#define DCC_REQUIRED_PERMISSIONS(ctx_, permissions_) \
    DCC_CTX_REQUIRED_PERMISSIONS((ctx_), (permissions_))
#define DCC_REQUIRED_ANY_ROLE(ctx_, role_ids_, role_id_count_) \
    DCC_CTX_REQUIRED_ANY_ROLE((ctx_), (role_ids_), (role_id_count_))
#define DCC_REQUIRED_ALL_ROLES(ctx_, role_ids_, role_id_count_) \
    DCC_CTX_REQUIRED_ALL_ROLES((ctx_), (role_ids_), (role_id_count_))
#define DCC_REQUIRED_ANY_ROLE_IDS(ctx_, ...) \
    DCC_CTX_REQUIRED_ANY_ROLE_IDS((ctx_), __VA_ARGS__)
#define DCC_REQUIRED_ALL_ROLE_IDS(ctx_, ...) \
    DCC_CTX_REQUIRED_ALL_ROLE_IDS((ctx_), __VA_ARGS__)
#define DCC_REQUIRED_TEXT(ctx_, value_) DCC_CTX_REQUIRED_TEXT((ctx_), (value_))
#define DCC_REQUIRED_OPTION(ctx_, name_) DCC_CTX_REQUIRED_OPTION((ctx_), (name_))
#define DCC_REQUIRED_ARG(ctx_, name_) DCC_CTX_REQUIRED_ARG((ctx_), (name_))
#define DCC_REQUIRED_ARG_TEXT(ctx_, name_) DCC_CTX_REQUIRED_ARG_TEXT((ctx_), (name_))
#define DCC_REQUIRED_ARG_STRING(ctx_, name_) DCC_CTX_REQUIRED_ARG_STRING((ctx_), (name_))
#define DCC_REQUIRED_FIELD(ctx_, custom_id_) DCC_CTX_REQUIRED_FIELD((ctx_), (custom_id_))
#define DCC_REQUIRED_FORM(ctx_, custom_id_) DCC_CTX_REQUIRED_FORM((ctx_), (custom_id_))
#define DCC_REQUIRED_FORM_TEXT(ctx_, custom_id_) \
    DCC_CTX_REQUIRED_FORM_TEXT((ctx_), (custom_id_))
#define DCC_REQUIRED_FORM_STRING(ctx_, custom_id_) \
    DCC_CTX_REQUIRED_FORM_STRING((ctx_), (custom_id_))
#define DCC_REQUIRED_SELECTED(ctx_) DCC_CTX_REQUIRED_SELECTED((ctx_))
#define DCC_REQUIRED_SELECT(ctx_) DCC_CTX_REQUIRED_SELECT((ctx_))
#define DCC_REQUIRED_CUSTOM_ID(ctx_, custom_id_) \
    DCC_CTX_REQUIRED_CUSTOM_ID((ctx_), (custom_id_))
#define DCC_REQUIRED_CUSTOM_ID_PREFIX(ctx_, prefix_) \
    DCC_CTX_REQUIRED_CUSTOM_ID_PREFIX((ctx_), (prefix_))
#define DCC_REQUIRE_CUSTOM_ID_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_CUSTOM_ID_PREFIX_BAD_INPUT(ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX_BAD_INPUT((ctx_), (prefix_), (message_))
#define DCC_REQUIRE_CUSTOM_ID_VALUE(var_, ctx_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_VALUE(var_, (ctx_), (message_))
#define DCC_REQUIRE_CUSTOM_ID_SUFFIX(var_, ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_SUFFIX(var_, (ctx_), (prefix_), (message_))
#define DCC_REQUIRE_ACTION(var_, ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_ACTION(var_, (ctx_), (prefix_), (message_))
#define DCC_REQUIRE_COMPONENT_ACTION(var_, ctx_, namespace_, message_) \
    DCC_CTX_REQUIRE_COMPONENT_ACTION(var_, (ctx_), (namespace_), (message_))
#define DCC_REQUIRE_ACTION_IN_NS(var_, ctx_, namespace_, message_) \
    DCC_CTX_REQUIRE_ACTION_IN_NS(var_, (ctx_), (namespace_), (message_))
#define DCC_REQUIRE_ACTION_IN_NAMESPACE(var_, ctx_, namespace_, message_) \
    DCC_CTX_REQUIRE_ACTION_IN_NAMESPACE(var_, (ctx_), (namespace_), (message_))
#define DCC_REQUIRE_COMPONENT_IS(ctx_, namespace_, action_, message_) \
    DCC_CTX_REQUIRE_COMPONENT_IS((ctx_), (namespace_), (action_), (message_))
#define DCC_REQUIRED_CUSTOM_ID_VALUE(var_, ctx_) \
    DCC_CTX_REQUIRED_CUSTOM_ID_VALUE(var_, (ctx_))
#define DCC_REQUIRED_CUSTOM_ID_SUFFIX(var_, ctx_, prefix_) \
    DCC_CTX_REQUIRED_CUSTOM_ID_SUFFIX(var_, (ctx_), (prefix_))
#define DCC_REQUIRED_ACTION(var_, ctx_, prefix_) \
    DCC_CTX_REQUIRED_ACTION(var_, (ctx_), (prefix_))
#define DCC_REQUIRED_COMPONENT_ACTION(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRED_COMPONENT_ACTION(var_, (ctx_), (namespace_))
#define DCC_REQUIRED_ACTION_IN_NS(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRED_ACTION_IN_NS(var_, (ctx_), (namespace_))
#define DCC_REQUIRED_ACTION_IN_NAMESPACE(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRED_ACTION_IN_NAMESPACE(var_, (ctx_), (namespace_))
#define DCC_REQUIRED_COMPONENT_IS(ctx_, namespace_, action_) \
    DCC_CTX_REQUIRED_COMPONENT_IS((ctx_), (namespace_), (action_))
#define DCC_REQUIRE_ARG_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_ARG_TEXT_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_ARG_STRING_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_STRING_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_OPTION_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_OPTION_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_OPTION_TEXT_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_OPTION_TEXT_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_OPTION_STRING_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_OPTION_STRING_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_STRING_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_STRING_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_TEXT_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_TEXT_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_INT_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_INT_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_INTEGER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_INTEGER_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_NUMBER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_NUMBER_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_BOOL_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BOOL_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_BOOLEAN_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BOOLEAN_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_SNOWFLAKE_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_SNOWFLAKE_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_USER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_USER_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_MEMBER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_MEMBER_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_ROLE_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ROLE_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_CHANNEL_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_CHANNEL_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_MESSAGE_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_MESSAGE_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_ATTACHMENT_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ATTACHMENT_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRED_STRING_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_STRING_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_TEXT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_TEXT_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_INT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_INT_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_INTEGER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_INTEGER_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_NUMBER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_NUMBER_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_BOOL_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_BOOL_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_BOOLEAN_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_BOOLEAN_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_SNOWFLAKE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_SNOWFLAKE_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_USER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_USER_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_MEMBER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_MEMBER_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_ROLE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_ROLE_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_CHANNEL_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_CHANNEL_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_MESSAGE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_MESSAGE_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_ATTACHMENT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_ATTACHMENT_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRE_FORM_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FORM_TEXT_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_TEXT_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FORM_STRING_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_STRING_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FIELD_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIELD_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FIELD_TEXT_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIELD_TEXT_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FIELD_STRING_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIELD_STRING_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_STRING_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_STRING_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_TEXT_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_TEXT_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_STRING_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_STRING_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_TEXT_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_TEXT_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_BOOL_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOL_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_BOOLEAN_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOLEAN_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_CHECKBOX_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_CHECKBOX_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_BOOL_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOL_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_BOOLEAN_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOLEAN_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_CHECKBOX_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_CHECKBOX_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_VALUES_FIELD(values_var_, count_var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_VALUES_FIELD(values_var_, count_var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_VALUES_FORM(values_var_, count_var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_VALUES_FORM(values_var_, count_var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FIRST_VALUE_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIRST_VALUE_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FIRST_VALUE_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIRST_VALUE_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_SELECT_BAD_INPUT(ctx_, message_) \
    DCC_CTX_REQUIRE_SELECT_BAD_INPUT((ctx_), (message_))
#define DCC_REQUIRED_STRING_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_STRING_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_TEXT_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_TEXT_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_STRING_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_STRING_FORM(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_TEXT_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_TEXT_FORM(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_BOOL_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_BOOL_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_BOOLEAN_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_BOOLEAN_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_CHECKBOX_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_CHECKBOX_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_BOOL_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_BOOL_FORM(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_BOOLEAN_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_BOOLEAN_FORM(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_CHECKBOX_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_CHECKBOX_FORM(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_VALUES_FIELD(values_var_, count_var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_VALUES_FIELD(values_var_, count_var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_VALUES_FORM(values_var_, count_var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_VALUES_FORM(values_var_, count_var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_FIRST_VALUE_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_FIRST_VALUE_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_FIRST_VALUE_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_FIRST_VALUE_FORM(var_, (ctx_), (custom_id_))

#define DCC_CTX_ADD_MEMBER_ROLE(ctx_, user_id_, role_id_) \
    dcc_ctx_add_member_role((ctx_), (user_id_), (role_id_), NULL, NULL)
#define DCC_CTX_ADD_MEMBER_ROLE_CB(ctx_, user_id_, role_id_, cb_, user_data_) \
    dcc_ctx_add_member_role((ctx_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_REMOVE_MEMBER_ROLE(ctx_, user_id_, role_id_) \
    dcc_ctx_remove_member_role((ctx_), (user_id_), (role_id_), NULL, NULL)
#define DCC_CTX_REMOVE_MEMBER_ROLE_CB(ctx_, user_id_, role_id_, cb_, user_data_) \
    dcc_ctx_remove_member_role((ctx_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_ADD_AUTHOR_ROLE(ctx_, role_id_) \
    dcc_ctx_add_author_role((ctx_), (role_id_), NULL, NULL)
#define DCC_CTX_ADD_AUTHOR_ROLE_CB(ctx_, role_id_, cb_, user_data_) \
    dcc_ctx_add_author_role((ctx_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_REMOVE_AUTHOR_ROLE(ctx_, role_id_) \
    dcc_ctx_remove_author_role((ctx_), (role_id_), NULL, NULL)
#define DCC_CTX_REMOVE_AUTHOR_ROLE_CB(ctx_, role_id_, cb_, user_data_) \
    dcc_ctx_remove_author_role((ctx_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_MEMBER_ADD_ROLE(ctx_, user_id_, role_id_) \
    DCC_CTX_ADD_MEMBER_ROLE((ctx_), (user_id_), (role_id_))
#define DCC_CTX_MEMBER_ADD_ROLE_CB(ctx_, user_id_, role_id_, cb_, user_data_) \
    DCC_CTX_ADD_MEMBER_ROLE_CB((ctx_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_MEMBER_REMOVE_ROLE(ctx_, user_id_, role_id_) \
    DCC_CTX_REMOVE_MEMBER_ROLE((ctx_), (user_id_), (role_id_))
#define DCC_CTX_MEMBER_REMOVE_ROLE_CB(ctx_, user_id_, role_id_, cb_, user_data_) \
    DCC_CTX_REMOVE_MEMBER_ROLE_CB((ctx_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_AUTHOR_ADD_ROLE(ctx_, role_id_) DCC_CTX_ADD_AUTHOR_ROLE((ctx_), (role_id_))
#define DCC_AUTHOR_ADD_ROLE_CB(ctx_, role_id_, cb_, user_data_) \
    DCC_CTX_ADD_AUTHOR_ROLE_CB((ctx_), (role_id_), (cb_), (user_data_))
#define DCC_AUTHOR_REMOVE_ROLE(ctx_, role_id_) DCC_CTX_REMOVE_AUTHOR_ROLE((ctx_), (role_id_))
#define DCC_AUTHOR_REMOVE_ROLE_CB(ctx_, role_id_, cb_, user_data_) \
    DCC_CTX_REMOVE_AUTHOR_ROLE_CB((ctx_), (role_id_), (cb_), (user_data_))
#define DCC_CTX_THREAD_FROM_MESSAGE(ctx_, name_) \
    dcc_ctx_create_thread_from_message_name((ctx_), (name_), NULL, NULL)
#define DCC_CTX_THREAD_FROM_MESSAGE_CB(ctx_, name_, cb_, user_data_) \
    dcc_ctx_create_thread_from_message_name((ctx_), (name_), (cb_), (user_data_))
#define DCC_CTX_THREAD_FROM_MESSAGE_PARAMS(ctx_, params_) \
    dcc_ctx_create_thread_from_message((ctx_), (params_), NULL, NULL)
#define DCC_CTX_THREAD_FROM_MESSAGE_PARAMS_CB(ctx_, params_, cb_, user_data_) \
    dcc_ctx_create_thread_from_message((ctx_), (params_), (cb_), (user_data_))
#define DCC_CTX_CREATE_THREAD(ctx_, name_) DCC_CTX_THREAD_FROM_MESSAGE((ctx_), (name_))
#define DCC_CTX_CREATE_THREAD_CB(ctx_, name_, cb_, user_data_) \
    DCC_CTX_THREAD_FROM_MESSAGE_CB((ctx_), (name_), (cb_), (user_data_))
#define DCC_CTX_CREATE_THREAD_PARAMS(ctx_, params_) \
    DCC_CTX_THREAD_FROM_MESSAGE_PARAMS((ctx_), (params_))
#define DCC_CTX_CREATE_THREAD_PARAMS_CB(ctx_, params_, cb_, user_data_) \
    DCC_CTX_THREAD_FROM_MESSAGE_PARAMS_CB((ctx_), (params_), (cb_), (user_data_))
#define DCC_CTX_ARCHIVE_THREAD(ctx_) dcc_ctx_archive_current_thread((ctx_), NULL, NULL)
#define DCC_CTX_ARCHIVE_THREAD_CB(ctx_, cb_, user_data_) \
    dcc_ctx_archive_current_thread((ctx_), (cb_), (user_data_))
#define DCC_CTX_LOCK_THREAD(ctx_) dcc_ctx_lock_current_thread((ctx_), NULL, NULL)
#define DCC_CTX_LOCK_THREAD_CB(ctx_, cb_, user_data_) \
    dcc_ctx_lock_current_thread((ctx_), (cb_), (user_data_))
#define DCC_CTX_UNLOCK_THREAD(ctx_) dcc_ctx_unlock_current_thread((ctx_), NULL, NULL)
#define DCC_CTX_UNLOCK_THREAD_CB(ctx_, cb_, user_data_) \
    dcc_ctx_unlock_current_thread((ctx_), (cb_), (user_data_))
#define DCC_CTX_CLOSE_THREAD(ctx_) DCC_CTX_ARCHIVE_THREAD((ctx_))
#define DCC_CTX_CLOSE_THREAD_CB(ctx_, cb_, user_data_) \
    DCC_CTX_ARCHIVE_THREAD_CB((ctx_), (cb_), (user_data_))

#endif
