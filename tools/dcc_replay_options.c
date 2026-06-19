#include "internal/dcc_replay_tool.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

const char *dcc_replay_tool_env_nonempty(const char *name) {
    const char *value = getenv(name);
    return value != NULL && value[0] != '\0' ? value : NULL;
}

int dcc_replay_tool_parse_u32(const char *text, uint32_t *out) {
    if (text == NULL || text[0] == '\0' || out == NULL) {
        return -1;
    }
    errno = 0;
    char *end = NULL;
    unsigned long value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value > UINT32_MAX) {
        return -1;
    }
    *out = (uint32_t)value;
    return 0;
}

static void dcc_replay_tool_defaults(dcc_replay_tool_options_t *options) {
    memset(options, 0, sizeof(*options));
    options->worker_path = DCC_HOT_RELOAD_DEFAULT_WORKER;
    options->worker_health_timeout_ms = 2000U;
    options->worker_drain_timeout_ms = 500U;
    options->isolated = 1;
}

static int dcc_replay_tool_take_value(
    int argc,
    char **argv,
    int *index,
    const char **out
) {
    if (*index + 1 >= argc) {
        fprintf(stderr, "missing value for %s\n", argv[*index]);
        return -1;
    }
    *out = argv[++(*index)];
    return 0;
}

static int dcc_replay_tool_parse_value_option(
    int argc,
    char **argv,
    int *index,
    dcc_replay_tool_options_t *options
) {
    const char *arg = argv[*index];
    const char *value = NULL;
    if (dcc_replay_tool_take_value(argc, argv, index, &value) != 0) {
        return -1;
    }
    if (strcmp(arg, "-f") == 0 || strcmp(arg, "--file") == 0) {
        options->file_path = value;
        return 0;
    }
    if (strcmp(arg, "-m") == 0 || strcmp(arg, "--module") == 0) {
        options->module_path = value;
        return 0;
    }
    if (strcmp(arg, "-w") == 0 || strcmp(arg, "--worker") == 0) {
        options->worker_path = value;
        options->isolated = 1;
        options->in_process = 0;
        return 0;
    }
    if (strcmp(arg, "--token") == 0) {
        options->token = value;
        return 0;
    }
    if (strcmp(arg, "--worker-health-ms") == 0) {
        return dcc_replay_tool_parse_u32(value, &options->worker_health_timeout_ms);
    }
    if (strcmp(arg, "--worker-drain-ms") == 0) {
        return dcc_replay_tool_parse_u32(value, &options->worker_drain_timeout_ms);
    }
    return -1;
}

static int dcc_replay_tool_is_value_option(const char *arg) {
    return strcmp(arg, "-f") == 0 ||
           strcmp(arg, "--file") == 0 ||
           strcmp(arg, "-m") == 0 ||
           strcmp(arg, "--module") == 0 ||
           strcmp(arg, "-w") == 0 ||
           strcmp(arg, "--worker") == 0 ||
           strcmp(arg, "--token") == 0 ||
           strcmp(arg, "--worker-health-ms") == 0 ||
           strcmp(arg, "--worker-drain-ms") == 0;
}

int dcc_replay_tool_parse_options(
    int argc,
    char **argv,
    dcc_replay_tool_options_t *options
) {
    dcc_replay_tool_defaults(options);
    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            dcc_replay_tool_usage(stdout, argv[0]);
            return 1;
        }
        if (dcc_replay_tool_is_value_option(arg)) {
            if (dcc_replay_tool_parse_value_option(argc, argv, &i, options) != 0) {
                fprintf(stderr, "invalid value for %s\n", arg);
                return -1;
            }
            continue;
        }
        if (strcmp(arg, "--summary") == 0) {
            options->summary = 1;
        } else if (strcmp(arg, "--validate") == 0) {
            options->validate = 1;
        } else if (strcmp(arg, "--realtime") == 0) {
            options->realtime = 1;
        } else if (strcmp(arg, "--isolated") == 0) {
            options->isolated = 1;
            options->in_process = 0;
        } else if (strcmp(arg, "--in-process") == 0) {
            options->in_process = 1;
            options->isolated = 0;
        } else if (arg[0] != '-' && options->file_path == NULL) {
            options->file_path = arg;
        } else {
            fprintf(stderr, "unknown option: %s\n", arg);
            return -1;
        }
    }
    if (options->token == NULL) {
        const char *token = dcc_replay_tool_env_nonempty("BOT_TOKEN");
        options->token = token != NULL ? token : dcc_replay_tool_env_nonempty("DISCORD_TOKEN");
    }
    if (options->module_path == NULL && !options->summary) {
        options->summary = 1;
    }
    return 0;
}

int dcc_replay_tool_validate_options(const dcc_replay_tool_options_t *options) {
    if (options->file_path == NULL || options->file_path[0] == '\0') {
        fprintf(stderr, "missing --file FILE\n");
        return -1;
    }
    if (options->module_path != NULL &&
        options->isolated &&
        (options->worker_path == NULL || options->worker_path[0] == '\0')) {
        fprintf(stderr, "missing --worker PATH for isolated replay\n");
        return -1;
    }
    if (options->worker_health_timeout_ms == 0U) {
        fprintf(stderr, "--worker-health-ms must be greater than zero\n");
        return -1;
    }
    return 0;
}
