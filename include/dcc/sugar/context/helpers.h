#ifndef DCC_SUGAR_CONTEXT_HELPERS_H
#define DCC_SUGAR_CONTEXT_HELPERS_H

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

#endif /* DCC_SUGAR_CONTEXT_HELPERS_H */
