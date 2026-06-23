#include "internal/dcc_hot_reload_host.h"

#include <string.h>

static void dcc_hot_reload_host_options_defaults(dcc_hot_reload_host_options_t *options) {
    memset(options, 0, sizeof(*options));
    options->intents = DCC_INTENT_GUILDS;
    options->health_address = "127.0.0.1";
    options->poll_interval_ms = 250U;
    options->settle_interval_ms = 100U;
    options->worker_health_timeout_ms = 2000U;
    options->worker_drain_timeout_ms = 500U;
}

static int dcc_hot_reload_host_has_help_arg(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            return 1;
        }
    }
    return 0;
}

static int dcc_hot_reload_host_take_positional(
    dcc_hot_reload_host_options_t *options,
    const char *arg,
    int *positional
) {
    if (*positional == 0) {
        options->module_path = arg;
    } else if (*positional == 1) {
        options->worker_path = arg;
    } else {
        fprintf(stderr, "too many positional arguments\n");
        return -1;
    }
    ++(*positional);
    return 0;
}

static void dcc_hot_reload_host_resolve_env_paths(dcc_hot_reload_host_options_t *options) {
    if (options->module_path == NULL) {
        options->module_path = dcc_hot_reload_host_env_nonempty("DCC_BOT_MODULE");
    }
    if (options->worker_path == NULL) {
        options->worker_path = dcc_hot_reload_host_env_nonempty("DCC_HOT_RELOAD_WORKER");
    }
}

int dcc_hot_reload_host_parse_options(
    int argc,
    char **argv,
    dcc_hot_reload_host_options_t *options
) {
    int positional = 0;
    dcc_hot_reload_host_options_defaults(options);

    if (dcc_hot_reload_host_has_help_arg(argc, argv)) {
        dcc_hot_reload_host_usage(stdout, argv[0]);
        return 1;
    }
    if (dcc_hot_reload_host_parse_env_options(options) != 0) {
        return -1;
    }

    for (int i = 1; i < argc; ++i) {
        int handled = 0;
        if (dcc_hot_reload_host_parse_option_arg(argc, argv, &i, options, &handled) != 0) {
            return -1;
        }
        if (handled) {
            continue;
        }
        if (dcc_hot_reload_host_take_positional(options, argv[i], &positional) != 0) {
            return -1;
        }
    }

    dcc_hot_reload_host_resolve_env_paths(options);
    return dcc_hot_reload_host_apply_scope_env(options);
}

const char *dcc_hot_reload_host_token(void) {
    const char *token = NULL;
    return dcc_app_env_get_token(NULL, &token) == DCC_OK ? token : NULL;
}

const char *dcc_hot_reload_host_resolved_worker(const dcc_hot_reload_host_options_t *options) {
    if (options->worker_path != NULL && options->worker_path[0] != '\0') {
        return options->worker_path;
    }
    return DCC_HOT_RELOAD_DEFAULT_WORKER;
}
