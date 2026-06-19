#include <dcc/dcc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
static int test_setenv(const char *name, const char *value) {
    return _putenv_s(name, value);
}

static void test_unsetenv(const char *name) {
    (void)_putenv_s(name, "");
}
#else
static int test_setenv(const char *name, const char *value) {
    return setenv(name, value, 1);
}

static void test_unsetenv(const char *name) {
    (void)unsetenv(name);
}
#endif

static int expect_status(const char *name, dcc_status_t got, dcc_status_t expected) {
    if (got != expected) {
        fprintf(
            stderr,
            "%s: got %s, expected %s\n",
            name,
            dcc_status_string(got),
            dcc_status_string(expected)
        );
        return -1;
    }
    return 0;
}

int main(void) {
    test_unsetenv("DCC_HOT_RELOAD_WORKER");
    test_unsetenv("DCC_BOT_MODULE");
    test_unsetenv("BOT_TOKEN");

    if (expect_status(
            "env isolated runner uses default worker before token validation",
            dcc_hot_reload_run_env_isolated(DCC_INTENT_GUILDS),
            DCC_ERR_INVALID_ARG
        ) != 0) {
        return 1;
    }

    if (expect_status(
            "simple isolated runner accepts empty worker default before token validation",
            dcc_hot_reload_run_simple_isolated("module.so", "", NULL, DCC_INTENT_GUILDS),
            DCC_ERR_INVALID_ARG
        ) != 0) {
        return 1;
    }

    if (test_setenv("DCC_HOT_RELOAD_WORKER", DCC_HOT_RELOAD_DEFAULT_WORKER) != 0) {
        perror("setenv DCC_HOT_RELOAD_WORKER");
        return 1;
    }
    if (expect_status(
            "simple isolated runner falls back to worker env before token validation",
            dcc_hot_reload_run_simple_isolated("module.so", NULL, NULL, DCC_INTENT_GUILDS),
            DCC_ERR_INVALID_ARG
        ) != 0) {
        return 1;
    }

    dcc_hot_reload_run_options_t options;
    memset(&options, 0, sizeof(options));
    options.size = sizeof(options);
    options.client_options.size = sizeof(options.client_options);
    options.client_options.intents = DCC_INTENT_GUILDS;
    options.library_path = "module.so";
    options.hot_reload_options.size = sizeof(options.hot_reload_options);
    options.hot_reload_options.backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER;
    test_unsetenv("DCC_HOT_RELOAD_WORKER");
    if (expect_status(
            "full isolated runner uses default worker before token validation",
            dcc_hot_reload_run(&options),
            DCC_ERR_INVALID_ARG
        ) != 0) {
        return 1;
    }

    return 0;
}
