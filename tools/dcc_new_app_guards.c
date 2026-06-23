#include "internal/dcc_new_app.h"

#include <stdio.h>
#include <string.h>

static int dcc_new_app_guard_expr_safe(const char *value) {
    if (value == NULL || value[0] == '\0') {
        return 0;
    }
    for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; ++p) {
        if (*p < 0x20U || *p == '"' || *p == '\'' || *p == '\\' ||
            *p == ';' || *p == '{' || *p == '}') {
            return 0;
        }
    }
    return 1;
}

static int dcc_new_app_guard_has_policy(const dcc_new_app_options_t *options) {
    return options->guard_guild_only ||
        options->guard_dm_only ||
        options->guard_nsfw_only ||
        options->guard_owner_arg != NULL ||
        options->guard_role_arg != NULL ||
        options->guard_any_role_arg != NULL ||
        options->guard_permission_arg != NULL ||
        options->guard_cooldown_user_arg != NULL ||
        options->guard_cooldown_global_arg != NULL ||
        options->guard_cooldown_channel_arg != NULL ||
        options->guard_cooldown_guild_arg != NULL;
}

static int dcc_new_app_guard_mode_supported(dcc_new_app_mode_t mode) {
    return mode == DCC_NEW_APP_MODE_ADD_COMMAND ||
        mode == DCC_NEW_APP_MODE_ADD_SUBCOMMAND ||
        mode == DCC_NEW_APP_MODE_ADD_BUTTON ||
        mode == DCC_NEW_APP_MODE_ADD_SELECT ||
        mode == DCC_NEW_APP_MODE_ADD_MODAL ||
        mode == DCC_NEW_APP_MODE_ADD_AUTOCOMPLETE ||
        mode == DCC_NEW_APP_MODE_ADD_VIEW ||
        mode == DCC_NEW_APP_MODE_ADD_LATEST_MESSAGE ||
        mode == DCC_NEW_APP_MODE_ADD_ACTION ||
        mode == DCC_NEW_APP_MODE_ADD_CONTEXT_MENU ||
        mode == DCC_NEW_APP_MODE_ADD_PRESET;
}

static int dcc_new_app_guard_append(
    char *out,
    size_t out_len,
    size_t *used,
    int *count,
    const char *text
) {
    int written = snprintf(out + *used, out_len - *used, "%s%s", *count > 0 ? ", " : "", text);
    if (written < 0 || (size_t)written >= out_len - *used) {
        return -1;
    }
    *used += (size_t)written;
    ++(*count);
    return 0;
}

static int dcc_new_app_guard_append_expr(
    char *out,
    size_t out_len,
    size_t *used,
    int *count,
    const char *macro,
    const char *expr
) {
    char item[320];
    if (!dcc_new_app_guard_expr_safe(expr)) {
        return -1;
    }
    snprintf(item, sizeof(item), "%s(%s)", macro, expr);
    return dcc_new_app_guard_append(out, out_len, used, count, item);
}

static int dcc_new_app_guard_append_cooldown(
    char *out,
    size_t out_len,
    size_t *used,
    int *count,
    const char *macro,
    const char *value
) {
    char item[360];
    const char *colon = value != NULL ? strchr(value, ':') : NULL;
    if (colon == NULL || colon == value || colon[1] == '\0' ||
        !dcc_new_app_guard_expr_safe(value)) {
        return -1;
    }
    snprintf(item, sizeof(item), "%s(%.*sU, %sU)", macro, (int)(colon - value), value, colon + 1);
    return dcc_new_app_guard_append(out, out_len, used, count, item);
}

int dcc_new_app_guard_options_validate(const dcc_new_app_options_t *options) {
    if (!dcc_new_app_guard_has_policy(options)) {
        return 0;
    }
    if (!dcc_new_app_guard_mode_supported(options->mode)) {
        fprintf(stderr, "guard options are only supported for interaction routes\n");
        return -1;
    }
    if (options->guard_guild_only && options->guard_dm_only) {
        fprintf(stderr, "--guild-only and --dm-only cannot be used together\n");
        return -1;
    }
    if (options->guard_owner_arg != NULL && !dcc_new_app_guard_expr_safe(options->guard_owner_arg)) {
        fprintf(stderr, "invalid --owner expression\n");
        return -1;
    }
    if (options->guard_role_arg != NULL && !dcc_new_app_guard_expr_safe(options->guard_role_arg)) {
        fprintf(stderr, "invalid --role expression\n");
        return -1;
    }
    if (options->guard_any_role_arg != NULL && !dcc_new_app_guard_expr_safe(options->guard_any_role_arg)) {
        fprintf(stderr, "invalid --any-role expression\n");
        return -1;
    }
    if (options->guard_permission_arg != NULL && !dcc_new_app_guard_expr_safe(options->guard_permission_arg)) {
        fprintf(stderr, "invalid --permission expression\n");
        return -1;
    }
    return 0;
}

static int dcc_new_app_guard_literal(
    const dcc_new_app_options_t *options,
    char *out,
    size_t out_len,
    int wrapped
) {
    size_t used = 0U;
    int count = 0;
    if (out == NULL || out_len == 0U) {
        return -1;
    }
    out[0] = '\0';
    if (!dcc_new_app_guard_has_policy(options)) {
        return 0;
    }
    if (wrapped) {
        int written = snprintf(out, out_len, "DCC_POLICY(");
        if (written < 0 || (size_t)written >= out_len) {
            return -1;
        }
        used = (size_t)written;
    }
    if (options->guard_guild_only &&
        dcc_new_app_guard_append(out, out_len, &used, &count, "DCC_ONLY_GUILD()") != 0) {
        return -1;
    }
    if (options->guard_dm_only &&
        dcc_new_app_guard_append(out, out_len, &used, &count, "DCC_ONLY_DM()") != 0) {
        return -1;
    }
    if (options->guard_nsfw_only &&
        dcc_new_app_guard_append(out, out_len, &used, &count, "DCC_ONLY_NSFW()") != 0) {
        return -1;
    }
    if (options->guard_owner_arg != NULL &&
        dcc_new_app_guard_append_expr(out, out_len, &used, &count, "DCC_OWNER_ONLY", options->guard_owner_arg) != 0) {
        return -1;
    }
    if (options->guard_role_arg != NULL &&
        dcc_new_app_guard_append_expr(out, out_len, &used, &count, "DCC_HAS_ROLE", options->guard_role_arg) != 0) {
        return -1;
    }
    if (options->guard_any_role_arg != NULL &&
        dcc_new_app_guard_append_expr(out, out_len, &used, &count, "DCC_HAS_ANY_ROLE", options->guard_any_role_arg) != 0) {
        return -1;
    }
    if (options->guard_permission_arg != NULL &&
        dcc_new_app_guard_append_expr(out, out_len, &used, &count, "DCC_CAN", options->guard_permission_arg) != 0) {
        return -1;
    }
    if (options->guard_cooldown_user_arg != NULL &&
        dcc_new_app_guard_append_cooldown(out, out_len, &used, &count, "DCC_RATE_LIMIT_USER", options->guard_cooldown_user_arg) != 0) {
        return -1;
    }
    if (options->guard_cooldown_global_arg != NULL &&
        dcc_new_app_guard_append_cooldown(out, out_len, &used, &count, "DCC_RATE_LIMIT_GLOBAL", options->guard_cooldown_global_arg) != 0) {
        return -1;
    }
    if (options->guard_cooldown_channel_arg != NULL &&
        dcc_new_app_guard_append_cooldown(out, out_len, &used, &count, "DCC_RATE_LIMIT_CHANNEL", options->guard_cooldown_channel_arg) != 0) {
        return -1;
    }
    if (options->guard_cooldown_guild_arg != NULL &&
        dcc_new_app_guard_append_cooldown(out, out_len, &used, &count, "DCC_RATE_LIMIT_GUILD", options->guard_cooldown_guild_arg) != 0) {
        return -1;
    }
    if (!wrapped) {
        return count > 0 ? 1 : 0;
    }
    int written = snprintf(out + used, out_len - used, ")");
    return written >= 0 && (size_t)written < out_len - used ? 1 : -1;
}

int dcc_new_app_guard_policy_literal(
    const dcc_new_app_options_t *options,
    char *out,
    size_t out_len
) {
    return dcc_new_app_guard_literal(options, out, out_len, 1);
}

int dcc_new_app_guard_items_literal(
    const dcc_new_app_options_t *options,
    char *out,
    size_t out_len
) {
    return dcc_new_app_guard_literal(options, out, out_len, 0);
}
