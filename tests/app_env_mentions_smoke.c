#include <dcc/sugar.h>

#include <stdio.h>
#include <stdlib.h>

static int test_set_env(const char *name, const char *value) {
#if defined(_WIN32)
    return _putenv_s(name, value) == 0;
#else
    return setenv(name, value, 1) == 0;
#endif
}

static int test_unset_env(const char *name) {
#if defined(_WIN32)
    return _putenv_s(name, "") == 0;
#else
    return unsetenv(name) == 0;
#endif
}

static int expect_id(const char *label, dcc_snowflake_t actual, dcc_snowflake_t expected) {
    if (actual != expected) {
        fprintf(
            stderr,
            "%s mismatch: actual=%llu expected=%llu\n",
            label,
            (unsigned long long)actual,
            (unsigned long long)expected
        );
        return 1;
    }
    return 0;
}

static int check_direct_env(void) {
    dcc_snowflake_t id = 0U;

    if (!test_set_env("DCC_ENV_MENTION_CHANNEL", "<#123>") ||
        !test_set_env("DCC_ENV_MENTION_ROLE", "<@&456>") ||
        !test_set_env("DCC_ENV_MENTION_USER", "<@789>") ||
        !test_set_env("DCC_ENV_MENTION_MEMBER", "<@!790>") ||
        !test_set_env("DCC_ENV_MENTION_GUILD", "111") ||
        !test_unset_env("DCC_ENV_MENTION_MISSING")) {
        return 1;
    }

    if (DCC_ENV_CHANNEL("DCC_ENV_MENTION_CHANNEL", &id) != DCC_OK ||
        expect_id("channel", id, 123U) ||
        DCC_ENV_ROLE("DCC_ENV_MENTION_ROLE", &id) != DCC_OK ||
        expect_id("role", id, 456U) ||
        DCC_ENV_USER("DCC_ENV_MENTION_USER", &id) != DCC_OK ||
        expect_id("user", id, 789U) ||
        DCC_ENV_USER("DCC_ENV_MENTION_MEMBER", &id) != DCC_OK ||
        expect_id("member", id, 790U) ||
        DCC_ENV_GUILD("DCC_ENV_MENTION_GUILD", &id) != DCC_OK ||
        expect_id("guild", id, 111U) ||
        DCC_ENV_CHANNEL_OR("DCC_ENV_MENTION_MISSING", 222U, &id) != DCC_OK ||
        expect_id("channel fallback", id, 222U)) {
        return 1;
    }

    return 0;
}

static int check_config_bindings(void) {
    if (!test_set_env("DCC_ENV_MENTION_CHANNEL", "<#123>") ||
        !test_set_env("DCC_ENV_MENTION_ROLE", "<@&456>") ||
        !test_set_env("DCC_ENV_MENTION_USER", "<@!790>") ||
        !test_set_env("DCC_ENV_MENTION_GUILD", "111")) {
        return 1;
    }

    dcc_snowflake_t channel_id = 0U;
    dcc_snowflake_t role_id = 0U;
    dcc_snowflake_t user_id = 0U;
    dcc_snowflake_t guild_id = 0U;

    if (DCC_CONFIG(
            DCC_CONFIG_CHANNEL("DCC_ENV_MENTION_CHANNEL", &channel_id),
            DCC_CONFIG_ROLE("DCC_ENV_MENTION_ROLE", &role_id),
            DCC_CONFIG_USER("DCC_ENV_MENTION_USER", &user_id),
            DCC_CONFIG_GUILD("DCC_ENV_MENTION_GUILD", &guild_id)
        ) != DCC_OK) {
        return 1;
    }

    return
        expect_id("config channel", channel_id, 123U) ||
        expect_id("config role", role_id, 456U) ||
        expect_id("config user", user_id, 790U) ||
        expect_id("config guild", guild_id, 111U);
}

static int check_rejections(void) {
    dcc_snowflake_t id = 0U;

    return
        !test_set_env("DCC_ENV_MENTION_CHANNEL", "<@&456>") ||
        DCC_ENV_CHANNEL("DCC_ENV_MENTION_CHANNEL", &id) == DCC_OK ||
        !test_set_env("DCC_ENV_MENTION_ROLE", "<#123>") ||
        DCC_ENV_ROLE("DCC_ENV_MENTION_ROLE", &id) == DCC_OK ||
        !test_set_env("DCC_ENV_MENTION_USER", "<#123>") ||
        DCC_ENV_USER("DCC_ENV_MENTION_USER", &id) == DCC_OK ||
        !test_set_env("DCC_ENV_MENTION_GUILD", "<#123>") ||
        DCC_ENV_GUILD("DCC_ENV_MENTION_GUILD", &id) == DCC_OK ||
        !test_set_env("DCC_ENV_MENTION_GENERIC", "<#123>") ||
        DCC_ENV_SNOWFLAKE("DCC_ENV_MENTION_GENERIC", &id) == DCC_OK;
}

int main(void) {
    if (check_direct_env() != 0) {
        return 1;
    }
    if (check_config_bindings() != 0) {
        return 1;
    }
    if (check_rejections() != 0) {
        return 1;
    }
    return 0;
}
