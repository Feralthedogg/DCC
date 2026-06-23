#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200112L
#endif

#include <dcc/app.h>
#include <dcc/sugar.h>

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

static int dcc_app_env_streq(const char *a, const char *b) {
    if (a == NULL || b == NULL) {
        return 0;
    }
    while (*a != '\0' && *b != '\0') {
        char ca = *a;
        char cb = *b;
        if (ca >= 'A' && ca <= 'Z') {
            ca = (char)(ca - 'A' + 'a');
        }
        if (cb >= 'A' && cb <= 'Z') {
            cb = (char)(cb - 'A' + 'a');
        }
        if (ca != cb) {
            return 0;
        }
        ++a;
        ++b;
    }
    return *a == '\0' && *b == '\0';
}

static uint8_t dcc_app_env_bool(const char *value, uint8_t fallback) {
    if (value == NULL || value[0] == '\0') {
        return fallback;
    }
    if (dcc_app_env_streq(value, "1") ||
        dcc_app_env_streq(value, "true") ||
        dcc_app_env_streq(value, "yes") ||
        dcc_app_env_streq(value, "on")) {
        return 1U;
    }
    if (dcc_app_env_streq(value, "0") ||
        dcc_app_env_streq(value, "false") ||
        dcc_app_env_streq(value, "no") ||
        dcc_app_env_streq(value, "off")) {
        return 0U;
    }
    return fallback;
}

static uint64_t dcc_app_env_u64(const char *value, uint64_t fallback) {
    if (value == NULL || value[0] == '\0') {
        return fallback;
    }
    char *end = NULL;
    unsigned long long parsed = strtoull(value, &end, 10);
    return end != value && end != NULL && *end == '\0' ? (uint64_t)parsed : fallback;
}

static int dcc_app_env_command_sync_mode(const char *value) {
    if (value == NULL || value[0] == '\0' ||
        dcc_app_env_streq(value, "0") ||
        dcc_app_env_streq(value, "false") ||
        dcc_app_env_streq(value, "no") ||
        dcc_app_env_streq(value, "off") ||
        dcc_app_env_streq(value, "none") ||
        dcc_app_env_streq(value, "disabled")) {
        return 0;
    }
    if (dcc_app_env_streq(value, "plan") ||
        dcc_app_env_streq(value, "check") ||
        dcc_app_env_streq(value, "dry-run") ||
        dcc_app_env_streq(value, "dry_run")) {
        return 1;
    }
    if (dcc_app_env_streq(value, "1") ||
        dcc_app_env_streq(value, "true") ||
        dcc_app_env_streq(value, "yes") ||
        dcc_app_env_streq(value, "on") ||
        dcc_app_env_streq(value, "apply") ||
        dcc_app_env_streq(value, "sync")) {
        return 2;
    }
    return -1;
}

static char *dcc_app_env_trim_left(char *text) {
    while (*text == ' ' || *text == '\t' || *text == '\r' || *text == '\n') {
        ++text;
    }
    return text;
}

static void dcc_app_env_trim_right(char *text) {
    size_t len = strlen(text);
    while (len > 0U) {
        char ch = text[len - 1U];
        if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') {
            break;
        }
        text[--len] = '\0';
    }
}

static void dcc_app_env_unquote(char **value) {
    char *start = dcc_app_env_trim_left(*value);
    dcc_app_env_trim_right(start);
    size_t len = strlen(start);
    if (len >= 2U &&
        ((start[0] == '"' && start[len - 1U] == '"') ||
         (start[0] == '\'' && start[len - 1U] == '\''))) {
        start[len - 1U] = '\0';
        ++start;
    }
    *value = start;
}

static int dcc_app_env_set(const char *name, const char *value, uint8_t overwrite) {
#if defined(_WIN32)
    if (!overwrite && getenv(name) != NULL) {
        return 0;
    }
    return _putenv_s(name, value);
#else
    return setenv(name, value, overwrite ? 1 : 0);
#endif
}

dcc_status_t dcc_app_load_env_file(const char *path, uint8_t overwrite) {
    if (path == NULL || path[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return DCC_ERR_NOT_FOUND;
    }

    char line[4096];
    dcc_status_t status = DCC_OK;
    while (fgets(line, sizeof(line), file) != NULL) {
        char *key = dcc_app_env_trim_left(line);
        if (key[0] == '\0' || key[0] == '#') {
            continue;
        }
        if (strncmp(key, "export", 6U) == 0 &&
            (key[6U] == ' ' || key[6U] == '\t')) {
            key = dcc_app_env_trim_left(key + 6U);
        }

        char *eq = strchr(key, '=');
        if (eq == NULL) {
            continue;
        }
        *eq = '\0';
        char *value = eq + 1;
        dcc_app_env_trim_right(key);
        dcc_app_env_unquote(&value);
        if (key[0] == '\0') {
            continue;
        }
        if (dcc_app_env_set(key, value, overwrite ? 1U : 0U) != 0) {
            status = DCC_ERR_RUNTIME;
            break;
        }
    }

    if (fclose(file) != 0 && status == DCC_OK) {
        status = DCC_ERR_RUNTIME;
    }
    return status;
}

dcc_status_t dcc_app_load_dotenv(void) {
    return dcc_app_load_env_file(".env", 1U);
}

static int dcc_app_env_missing(const char *value) {
    return value == NULL || value[0] == '\0';
}

dcc_status_t dcc_app_env_get_token(const char *preferred_env, const char **out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    *out = NULL;
    const char *token = NULL;
    if (preferred_env != NULL && preferred_env[0] != '\0') {
        token = getenv(preferred_env);
    }
    if (dcc_app_env_missing(token)) {
        token = getenv("DCC_TOKEN");
    }
    if (dcc_app_env_missing(token)) {
        token = getenv("BOT_TOKEN");
    }
    if (dcc_app_env_missing(token)) {
        token = getenv("DISCORD_TOKEN");
    }
    if (dcc_app_env_missing(token)) {
        return DCC_ERR_NOT_FOUND;
    }

    *out = token;
    return DCC_OK;
}

static dcc_status_t dcc_app_env_lookup(const char *name, const char **out) {
    if (name == NULL || name[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = getenv(name);
    if (dcc_app_env_missing(value)) {
        return DCC_ERR_NOT_FOUND;
    }
    *out = value;
    return DCC_OK;
}

static dcc_status_t dcc_app_env_parse_bool_strict(const char *value, uint8_t *out) {
    if (value == NULL || out == NULL || value[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_app_env_streq(value, "1") ||
        dcc_app_env_streq(value, "true") ||
        dcc_app_env_streq(value, "yes") ||
        dcc_app_env_streq(value, "on")) {
        *out = 1U;
        return DCC_OK;
    }
    if (dcc_app_env_streq(value, "0") ||
        dcc_app_env_streq(value, "false") ||
        dcc_app_env_streq(value, "no") ||
        dcc_app_env_streq(value, "off")) {
        *out = 0U;
        return DCC_OK;
    }
    return DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_app_env_parse_u64_strict(const char *value, uint64_t *out) {
    if (value == NULL || out == NULL || value[0] == '\0' || value[0] == '-' || value[0] == '+') {
        return DCC_ERR_INVALID_ARG;
    }

    errno = 0;
    char *end = NULL;
    unsigned long long parsed = strtoull(value, &end, 10);
    if (end == value || end == NULL || *end != '\0' || errno == ERANGE || parsed > UINT64_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (uint64_t)parsed;
    return DCC_OK;
}

static dcc_status_t dcc_app_env_parse_i64_strict(const char *value, int64_t *out) {
    if (value == NULL || out == NULL || value[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    errno = 0;
    char *end = NULL;
    long long parsed = strtoll(value, &end, 10);
    if (end == value ||
        end == NULL ||
        *end != '\0' ||
        errno == ERANGE ||
        parsed < INT64_MIN ||
        parsed > INT64_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (int64_t)parsed;
    return DCC_OK;
}

static int dcc_app_env_intent_char_equal(char a, char b) {
    if (a >= 'A' && a <= 'Z') {
        a = (char)(a - 'A' + 'a');
    }
    if (b >= 'A' && b <= 'Z') {
        b = (char)(b - 'A' + 'a');
    }
    if (a == '-' || a == '.' || a == ' ') {
        a = '_';
    }
    if (b == '-' || b == '.' || b == ' ') {
        b = '_';
    }
    return a == b;
}

static int dcc_app_env_intent_token_eq(const char *start, size_t len, const char *literal) {
    if (start == NULL || literal == NULL) {
        return 0;
    }
    for (size_t i = 0U; i < len; ++i) {
        if (literal[i] == '\0' || !dcc_app_env_intent_char_equal(start[i], literal[i])) {
            return 0;
        }
    }
    return literal[len] == '\0';
}

static dcc_status_t dcc_app_env_parse_intents_number(
    const char *start,
    size_t len,
    dcc_intents_t *out
) {
    if (start == NULL || out == NULL || len == 0U || len >= 64U || start[0] == '-' || start[0] == '+') {
        return DCC_ERR_INVALID_ARG;
    }

    char buf[64];
    memcpy(buf, start, len);
    buf[len] = '\0';

    errno = 0;
    char *end = NULL;
    unsigned long long parsed = strtoull(buf, &end, 0);
    if (end == buf || end == NULL || *end != '\0' || errno == ERANGE || parsed > UINT64_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (dcc_intents_t)parsed;
    return DCC_OK;
}

static dcc_status_t dcc_app_env_parse_intents_token(
    const char *start,
    size_t len,
    dcc_intents_t *out
) {
    if (start == NULL || out == NULL || len == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    if (start[0] >= '0' && start[0] <= '9') {
        return dcc_app_env_parse_intents_number(start, len, out);
    }

    if (dcc_app_env_intent_token_eq(start, len, "all")) {
        *out = DCC_INTENTS_ALL;
    } else if (dcc_app_env_intent_token_eq(start, len, "none")) {
        *out = DCC_INTENTS_NONE;
    } else if (dcc_app_env_intent_token_eq(start, len, "default") ||
               dcc_app_env_intent_token_eq(start, len, "guilds")) {
        *out = DCC_INTENTS_DEFAULT;
    } else if (dcc_app_env_intent_token_eq(start, len, "messages")) {
        *out = DCC_INTENTS_MESSAGES;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_messages")) {
        *out = DCC_INTENTS_GUILD_MESSAGES;
    } else if (dcc_app_env_intent_token_eq(start, len, "direct_messages")) {
        *out = DCC_INTENTS_DIRECT_MESSAGES;
    } else if (dcc_app_env_intent_token_eq(start, len, "message_reactions")) {
        *out = DCC_INTENTS_MESSAGE_REACTIONS;
    } else if (dcc_app_env_intent_token_eq(start, len, "message_typing")) {
        *out = DCC_INTENTS_MESSAGE_TYPING;
    } else if (dcc_app_env_intent_token_eq(start, len, "message_polls")) {
        *out = DCC_INTENTS_MESSAGE_POLLS;
    } else if (dcc_app_env_intent_token_eq(start, len, "message_content")) {
        *out = DCC_INTENTS_MESSAGES | DCC_INTENT_MESSAGE_CONTENT;
    } else if (dcc_app_env_intent_token_eq(start, len, "privileged")) {
        *out = DCC_INTENTS_PRIVILEGED;
    } else if (dcc_app_env_intent_token_eq(start, len, "unprivileged")) {
        *out = DCC_INTENTS_UNPRIVILEGED;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_members")) {
        *out = DCC_INTENT_GUILD_MEMBERS;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_moderation") ||
               dcc_app_env_intent_token_eq(start, len, "moderation")) {
        *out = DCC_INTENT_GUILD_MODERATION;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_expressions") ||
               dcc_app_env_intent_token_eq(start, len, "expressions")) {
        *out = DCC_INTENT_GUILD_EXPRESSIONS;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_integrations") ||
               dcc_app_env_intent_token_eq(start, len, "integrations")) {
        *out = DCC_INTENT_GUILD_INTEGRATIONS;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_webhooks") ||
               dcc_app_env_intent_token_eq(start, len, "webhooks")) {
        *out = DCC_INTENT_GUILD_WEBHOOKS;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_invites") ||
               dcc_app_env_intent_token_eq(start, len, "invites")) {
        *out = DCC_INTENT_GUILD_INVITES;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_voice_states") ||
               dcc_app_env_intent_token_eq(start, len, "voice_states") ||
               dcc_app_env_intent_token_eq(start, len, "voice")) {
        *out = DCC_INTENT_GUILD_VOICE_STATES;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_presences") ||
               dcc_app_env_intent_token_eq(start, len, "presences")) {
        *out = DCC_INTENT_GUILD_PRESENCES;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_message_reactions")) {
        *out = DCC_INTENT_GUILD_MESSAGE_REACTIONS;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_message_typing")) {
        *out = DCC_INTENT_GUILD_MESSAGE_TYPING;
    } else if (dcc_app_env_intent_token_eq(start, len, "direct_message_reactions")) {
        *out = DCC_INTENT_DIRECT_MESSAGE_REACTIONS;
    } else if (dcc_app_env_intent_token_eq(start, len, "direct_message_typing")) {
        *out = DCC_INTENT_DIRECT_MESSAGE_TYPING;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_scheduled_events") ||
               dcc_app_env_intent_token_eq(start, len, "scheduled_events")) {
        *out = DCC_INTENT_GUILD_SCHEDULED_EVENTS;
    } else if (dcc_app_env_intent_token_eq(start, len, "auto_moderation_configuration")) {
        *out = DCC_INTENT_AUTO_MODERATION_CONFIGURATION;
    } else if (dcc_app_env_intent_token_eq(start, len, "auto_moderation_execution")) {
        *out = DCC_INTENT_AUTO_MODERATION_EXECUTION;
    } else if (dcc_app_env_intent_token_eq(start, len, "guild_message_polls")) {
        *out = DCC_INTENT_GUILD_MESSAGE_POLLS;
    } else if (dcc_app_env_intent_token_eq(start, len, "direct_message_polls")) {
        *out = DCC_INTENT_DIRECT_MESSAGE_POLLS;
    } else {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static int dcc_app_env_intents_separator(char ch) {
    return ch == ',' || ch == '|' || ch == '+';
}

static dcc_status_t dcc_app_env_parse_intents_strict(const char *value, dcc_intents_t *out) {
    if (value == NULL || out == NULL || value[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_intents_t parsed = 0U;
    const char *cursor = value;
    while (*cursor != '\0') {
        while (*cursor == ' ' || *cursor == '\t' || *cursor == '\r' || *cursor == '\n') {
            ++cursor;
        }

        const char *start = cursor;
        while (*cursor != '\0' && !dcc_app_env_intents_separator(*cursor)) {
            ++cursor;
        }

        const char *end = cursor;
        while (end > start &&
               (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r' || end[-1] == '\n')) {
            --end;
        }

        dcc_intents_t token = 0U;
        dcc_status_t status = dcc_app_env_parse_intents_token(start, (size_t)(end - start), &token);
        if (status != DCC_OK) {
            return status;
        }
        parsed |= token;

        if (*cursor != '\0') {
            ++cursor;
        }
    }

    *out = parsed;
    return DCC_OK;
}

dcc_status_t dcc_app_env_get_string(const char *name, const char **out) {
    return dcc_app_env_lookup(name, out);
}

dcc_status_t dcc_app_env_get_string_or(
    const char *name,
    const char *fallback,
    const char **out
) {
    if (name == NULL || name[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = getenv(name);
    *out = dcc_app_env_missing(value) ? fallback : value;
    return DCC_OK;
}

dcc_status_t dcc_app_env_get_bool(const char *name, uint8_t *out) {
    const char *value = NULL;
    dcc_status_t status = dcc_app_env_lookup(name, &value);
    return status == DCC_OK ? dcc_app_env_parse_bool_strict(value, out) : status;
}

dcc_status_t dcc_app_env_get_bool_or(
    const char *name,
    uint8_t fallback,
    uint8_t *out
) {
    if (name == NULL || name[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = getenv(name);
    if (dcc_app_env_missing(value)) {
        *out = fallback ? 1U : 0U;
        return DCC_OK;
    }
    return dcc_app_env_parse_bool_strict(value, out);
}

dcc_status_t dcc_app_env_get_u64(const char *name, uint64_t *out) {
    const char *value = NULL;
    dcc_status_t status = dcc_app_env_lookup(name, &value);
    return status == DCC_OK ? dcc_app_env_parse_u64_strict(value, out) : status;
}

dcc_status_t dcc_app_env_get_u64_or(
    const char *name,
    uint64_t fallback,
    uint64_t *out
) {
    if (name == NULL || name[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = getenv(name);
    if (dcc_app_env_missing(value)) {
        *out = fallback;
        return DCC_OK;
    }
    return dcc_app_env_parse_u64_strict(value, out);
}

static dcc_status_t dcc_app_env_range_u32(
    uint32_t value,
    uint32_t min_value,
    uint32_t max_value,
    uint32_t *out
) {
    if (out == NULL || min_value > max_value || value < min_value || value > max_value) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = value;
    return DCC_OK;
}

dcc_status_t dcc_app_env_get_u32(const char *name, uint32_t *out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    uint64_t parsed = 0U;
    dcc_status_t status = dcc_app_env_get_u64(name, &parsed);
    if (status != DCC_OK) {
        return status;
    }
    if (parsed > UINT32_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (uint32_t)parsed;
    return DCC_OK;
}

dcc_status_t dcc_app_env_get_u32_or(
    const char *name,
    uint32_t fallback,
    uint32_t *out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    uint64_t parsed = fallback;
    dcc_status_t status = dcc_app_env_get_u64_or(name, fallback, &parsed);
    if (status != DCC_OK) {
        return status;
    }
    if (parsed > UINT32_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (uint32_t)parsed;
    return DCC_OK;
}

dcc_status_t dcc_app_env_get_u32_range(
    const char *name,
    uint32_t min_value,
    uint32_t max_value,
    uint32_t *out
) {
    uint32_t parsed = 0U;
    dcc_status_t status = dcc_app_env_get_u32(name, &parsed);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_app_env_range_u32(parsed, min_value, max_value, out);
}

dcc_status_t dcc_app_env_get_u32_range_or(
    const char *name,
    uint32_t fallback,
    uint32_t min_value,
    uint32_t max_value,
    uint32_t *out
) {
    if (name == NULL || name[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_app_env_range_u32(fallback, min_value, max_value, out);
    if (status != DCC_OK) {
        return status;
    }

    const char *value = getenv(name);
    if (dcc_app_env_missing(value)) {
        return DCC_OK;
    }

    uint32_t parsed = 0U;
    status = dcc_app_env_get_u32(name, &parsed);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_app_env_range_u32(parsed, min_value, max_value, out);
}

dcc_status_t dcc_app_env_get_i64(const char *name, int64_t *out) {
    const char *value = NULL;
    dcc_status_t status = dcc_app_env_lookup(name, &value);
    return status == DCC_OK ? dcc_app_env_parse_i64_strict(value, out) : status;
}

dcc_status_t dcc_app_env_get_i64_or(
    const char *name,
    int64_t fallback,
    int64_t *out
) {
    if (name == NULL || name[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = getenv(name);
    if (dcc_app_env_missing(value)) {
        *out = fallback;
        return DCC_OK;
    }
    return dcc_app_env_parse_i64_strict(value, out);
}

static dcc_status_t dcc_app_env_parse_snowflake_strict(const char *value, dcc_snowflake_t *out) {
    uint64_t parsed = 0U;
    dcc_status_t status = dcc_app_env_parse_u64_strict(value, &parsed);
    if (status != DCC_OK) {
        return status;
    }
    if (parsed == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (dcc_snowflake_t)parsed;
    return DCC_OK;
}

static dcc_status_t dcc_app_env_parse_channel(const char *value, dcc_snowflake_t *out) {
    dcc_status_t status = dcc_app_env_parse_snowflake_strict(value, out);
    return status == DCC_OK ? status : dcc_snowflake_parse_channel_mention(value, out);
}

static dcc_status_t dcc_app_env_parse_role(const char *value, dcc_snowflake_t *out) {
    dcc_status_t status = dcc_app_env_parse_snowflake_strict(value, out);
    return status == DCC_OK ? status : dcc_snowflake_parse_role_mention(value, out);
}

static dcc_status_t dcc_app_env_parse_user(const char *value, dcc_snowflake_t *out) {
    dcc_status_t status = dcc_app_env_parse_snowflake_strict(value, out);
    return status == DCC_OK ? status : dcc_snowflake_parse_user_mention(value, out);
}

dcc_status_t dcc_app_env_get_snowflake(const char *name, dcc_snowflake_t *out) {
    const char *value = NULL;
    dcc_status_t status = dcc_app_env_lookup(name, &value);
    return status == DCC_OK ? dcc_app_env_parse_snowflake_strict(value, out) : status;
}

dcc_status_t dcc_app_env_get_snowflake_or(
    const char *name,
    dcc_snowflake_t fallback,
    dcc_snowflake_t *out
) {
    if (name == NULL || name[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = getenv(name);
    if (dcc_app_env_missing(value)) {
        *out = fallback;
        return DCC_OK;
    }
    return dcc_app_env_parse_snowflake_strict(value, out);
}

dcc_status_t dcc_app_env_get_channel(const char *name, dcc_snowflake_t *out) {
    const char *value = NULL;
    dcc_status_t status = dcc_app_env_lookup(name, &value);
    return status == DCC_OK ? dcc_app_env_parse_channel(value, out) : status;
}

dcc_status_t dcc_app_env_get_channel_or(
    const char *name,
    dcc_snowflake_t fallback,
    dcc_snowflake_t *out
) {
    if (name == NULL || name[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = getenv(name);
    if (dcc_app_env_missing(value)) {
        *out = fallback;
        return DCC_OK;
    }
    return dcc_app_env_parse_channel(value, out);
}

dcc_status_t dcc_app_env_get_role(const char *name, dcc_snowflake_t *out) {
    const char *value = NULL;
    dcc_status_t status = dcc_app_env_lookup(name, &value);
    return status == DCC_OK ? dcc_app_env_parse_role(value, out) : status;
}

dcc_status_t dcc_app_env_get_role_or(
    const char *name,
    dcc_snowflake_t fallback,
    dcc_snowflake_t *out
) {
    if (name == NULL || name[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = getenv(name);
    if (dcc_app_env_missing(value)) {
        *out = fallback;
        return DCC_OK;
    }
    return dcc_app_env_parse_role(value, out);
}

dcc_status_t dcc_app_env_get_user(const char *name, dcc_snowflake_t *out) {
    const char *value = NULL;
    dcc_status_t status = dcc_app_env_lookup(name, &value);
    return status == DCC_OK ? dcc_app_env_parse_user(value, out) : status;
}

dcc_status_t dcc_app_env_get_user_or(
    const char *name,
    dcc_snowflake_t fallback,
    dcc_snowflake_t *out
) {
    if (name == NULL || name[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = getenv(name);
    if (dcc_app_env_missing(value)) {
        *out = fallback;
        return DCC_OK;
    }
    return dcc_app_env_parse_user(value, out);
}

dcc_status_t dcc_app_env_get_guild(const char *name, dcc_snowflake_t *out) {
    return dcc_app_env_get_snowflake(name, out);
}

dcc_status_t dcc_app_env_get_guild_or(
    const char *name,
    dcc_snowflake_t fallback,
    dcc_snowflake_t *out
) {
    return dcc_app_env_get_snowflake_or(name, fallback, out);
}

dcc_status_t dcc_app_env_get_intents(const char *name, dcc_intents_t *out) {
    const char *value = NULL;
    dcc_status_t status = dcc_app_env_lookup(name, &value);
    return status == DCC_OK ? dcc_app_env_parse_intents_strict(value, out) : status;
}

dcc_status_t dcc_app_env_get_intents_or(
    const char *name,
    dcc_intents_t fallback,
    dcc_intents_t *out
) {
    if (name == NULL || name[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = getenv(name);
    if (dcc_app_env_missing(value)) {
        *out = fallback;
        return DCC_OK;
    }
    return dcc_app_env_parse_intents_strict(value, out);
}

static dcc_status_t dcc_app_env_assign_fallback(const dcc_app_env_binding_t *binding) {
    switch (binding->type) {
        case DCC_APP_ENV_BIND_STRING:
            *(const char **)binding->out = binding->fallback_string;
            return DCC_OK;
        case DCC_APP_ENV_BIND_BOOL:
            *(uint8_t *)binding->out = binding->fallback_bool ? 1U : 0U;
            return DCC_OK;
        case DCC_APP_ENV_BIND_U64:
            *(uint64_t *)binding->out = binding->fallback_u64;
            return DCC_OK;
        case DCC_APP_ENV_BIND_I64:
            *(int64_t *)binding->out = binding->fallback_i64;
            return DCC_OK;
        case DCC_APP_ENV_BIND_SNOWFLAKE:
        case DCC_APP_ENV_BIND_CHANNEL:
        case DCC_APP_ENV_BIND_ROLE:
        case DCC_APP_ENV_BIND_USER:
        case DCC_APP_ENV_BIND_GUILD:
            *(dcc_snowflake_t *)binding->out = binding->fallback_snowflake;
            return DCC_OK;
        case DCC_APP_ENV_BIND_INTENTS:
            *(dcc_intents_t *)binding->out = binding->fallback_intents;
            return DCC_OK;
        default:
            return DCC_ERR_INVALID_ARG;
    }
}

static dcc_status_t dcc_app_env_assign_value(
    const dcc_app_env_binding_t *binding,
    const char *value
) {
    switch (binding->type) {
        case DCC_APP_ENV_BIND_STRING:
            *(const char **)binding->out = value;
            return DCC_OK;
        case DCC_APP_ENV_BIND_BOOL:
            return dcc_app_env_parse_bool_strict(value, (uint8_t *)binding->out);
        case DCC_APP_ENV_BIND_U64:
            return dcc_app_env_parse_u64_strict(value, (uint64_t *)binding->out);
        case DCC_APP_ENV_BIND_I64:
            return dcc_app_env_parse_i64_strict(value, (int64_t *)binding->out);
        case DCC_APP_ENV_BIND_SNOWFLAKE:
            return dcc_app_env_parse_snowflake_strict(value, (dcc_snowflake_t *)binding->out);
        case DCC_APP_ENV_BIND_CHANNEL:
            return dcc_app_env_parse_channel(value, (dcc_snowflake_t *)binding->out);
        case DCC_APP_ENV_BIND_ROLE:
            return dcc_app_env_parse_role(value, (dcc_snowflake_t *)binding->out);
        case DCC_APP_ENV_BIND_USER:
            return dcc_app_env_parse_user(value, (dcc_snowflake_t *)binding->out);
        case DCC_APP_ENV_BIND_GUILD:
            return dcc_app_env_parse_snowflake_strict(value, (dcc_snowflake_t *)binding->out);
        case DCC_APP_ENV_BIND_INTENTS:
            return dcc_app_env_parse_intents_strict(value, (dcc_intents_t *)binding->out);
        default:
            return DCC_ERR_INVALID_ARG;
    }
}

dcc_status_t dcc_app_env_bind(const dcc_app_env_binding_t *bindings, size_t count) {
    if (count == 0U) {
        return DCC_OK;
    }
    if (bindings == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0U; i < count; ++i) {
        const dcc_app_env_binding_t *binding = &bindings[i];
        if (binding->size < offsetof(dcc_app_env_binding_t, fallback_snowflake) +
                sizeof(binding->fallback_snowflake) ||
            binding->name == NULL ||
            binding->name[0] == '\0' ||
            binding->out == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        if (binding->type == DCC_APP_ENV_BIND_INTENTS &&
            binding->size < offsetof(dcc_app_env_binding_t, fallback_intents) +
                sizeof(binding->fallback_intents)) {
            return DCC_ERR_INVALID_ARG;
        }

        const char *value = getenv(binding->name);
        if (dcc_app_env_missing(value)) {
            if (binding->has_fallback) {
                dcc_status_t status = dcc_app_env_assign_fallback(binding);
                if (status != DCC_OK) {
                    return status;
                }
                continue;
            }
            if (binding->required) {
                if (binding->description != NULL && binding->description[0] != '\0') {
                    fprintf(
                        stderr,
                        "[dcc] missing required env: %s (%s)\n",
                        binding->name,
                        binding->description
                    );
                } else {
                    fprintf(stderr, "[dcc] missing required env: %s\n", binding->name);
                }
                return DCC_ERR_NOT_FOUND;
            }
            continue;
        }

        dcc_status_t status = dcc_app_env_assign_value(binding, value);
        if (status != DCC_OK) {
            fprintf(stderr, "[dcc] invalid env value: %s\n", binding->name);
            return status;
        }
    }

    return DCC_OK;
}

dcc_status_t dcc_app_options_from_env(dcc_app_options_t *options, const char *token_env) {
    if (options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_options_init(options);
    const char *token = NULL;
    dcc_status_t status = dcc_app_env_get_token(token_env, &token);
    if (status != DCC_OK) {
        return status;
    }
    options->client.token = token;
    status = dcc_app_env_get_intents_or("DCC_INTENTS", DCC_INTENTS_DEFAULT, &options->client.intents);
    if (status != DCC_OK) {
        return status;
    }
    options->client.enable_cache = dcc_app_env_bool(getenv("DCC_ENABLE_CACHE"), options->client.enable_cache);
    options->client.infer_guild_id_from_channel =
        dcc_app_env_bool(getenv("DCC_INFER_GUILD_FROM_CHANNEL"), options->client.infer_guild_id_from_channel);

    uint64_t auto_defer_ms = dcc_app_env_u64(getenv("DCC_AUTO_DEFER_MS"), 0U);
    if (auto_defer_ms != 0U) {
        options->auto_defer_after_ms = auto_defer_ms;
        options->auto_defer_ephemeral = dcc_app_env_bool(getenv("DCC_AUTO_DEFER_EPHEMERAL"), 0U);
    }

    const char *store_file_path = getenv("DCC_STORE_FILE");
    if (store_file_path == NULL || store_file_path[0] == '\0') {
        store_file_path = getenv("DCC_APP_STORE_FILE");
    }
    if (store_file_path != NULL && store_file_path[0] != '\0') {
        options->store_file_path = store_file_path;
    }

    dcc_snowflake_t guild_id =
        (dcc_snowflake_t)dcc_app_env_u64(getenv("DCC_COMMAND_GUILD_ID"), 0U);
    if (guild_id == 0U) {
        guild_id = (dcc_snowflake_t)dcc_app_env_u64(getenv("DCC_COMMAND_SYNC_GUILD_ID"), 0U);
    }
    if (guild_id != 0U) {
        status = dcc_command_registry_options_set_guild(&options->command_registry, guild_id);
    } else {
        status = dcc_command_registry_options_set_global(&options->command_registry);
    }
    if (status != DCC_OK) {
        return status;
    }

    int command_sync_mode = dcc_app_env_command_sync_mode(getenv("DCC_COMMAND_SYNC"));
    if (command_sync_mode < 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (command_sync_mode == 0) {
        return DCC_OK;
    }

    const char *application_id_text = getenv("DCC_APPLICATION_ID");
    if (application_id_text == NULL || application_id_text[0] == '\0') {
        application_id_text = getenv("DISCORD_APPLICATION_ID");
    }
    dcc_snowflake_t application_id =
        (dcc_snowflake_t)dcc_app_env_u64(application_id_text, 0U);
    if (application_id == 0U) {
        return DCC_ERR_NOT_FOUND;
    }

    options->command_sync_on_ready = 1U;
    options->command_sync.application_id = application_id;
    options->command_sync.apply = command_sync_mode == 2 ? 1U : 0U;
    options->command_sync.command_registry = options->command_registry;

    status = dcc_command_registry_options_set_delete_stale(
        &options->command_sync.command_registry,
        dcc_app_env_bool(getenv("DCC_COMMAND_SYNC_DELETE_STALE"), 0U)
    );
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_command_registry_options_set_dry_run(
        &options->command_sync.command_registry,
        options->command_sync.apply ? 0U : 1U
    );
    return status;
}

dcc_status_t dcc_app_create_from_env(const char *token_env, dcc_app_t **out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_options_t options;
    dcc_status_t status = dcc_app_options_from_env(&options, token_env);
    if (status != DCC_OK) {
        *out = NULL;
        return status;
    }
    return dcc_app_create(&options, out);
}
