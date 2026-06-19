#include "internal/dcc_hot_reload_host.h"

#include <string.h>

static int dcc_hot_reload_host_take_value(int argc, char **argv, int *index, const char **out) {
    if (*index + 1 >= argc) {
        return -1;
    }
    *out = argv[++(*index)];
    return 0;
}

int dcc_hot_reload_host_parse_value_option(
    int argc,
    char **argv,
    int *index,
    dcc_hot_reload_host_options_t *options
) {
    const char *arg = argv[*index];
    const char *value = NULL;
    if (dcc_hot_reload_host_take_value(argc, argv, index, &value) != 0) {
        fprintf(stderr, "%s requires a value\n", arg);
        return -1;
    }

    if (strcmp(arg, "-m") == 0 || strcmp(arg, "--module") == 0) {
        options->module_path = value;
        return 0;
    }
    if (strcmp(arg, "-w") == 0 || strcmp(arg, "--worker") == 0) {
        options->worker_path = value;
        return 0;
    }
    if (strcmp(arg, "-i") == 0 || strcmp(arg, "--intents") == 0) {
        return dcc_hot_reload_host_parse_intents_arg(value, options);
    }
    if (strcmp(arg, "-g") == 0 || strcmp(arg, "--guild") == 0) {
        return dcc_hot_reload_host_parse_guild_arg(value, options);
    }
    if (strcmp(arg, "--health-address") == 0) {
        options->health_address = value;
        return 0;
    }
    if (strcmp(arg, "--health-port") == 0) {
        return dcc_hot_reload_host_parse_health_port_arg(value, options);
    }
    if (strcmp(arg, "--poll-ms") == 0) {
        return dcc_hot_reload_host_parse_u32_arg(value, &options->poll_interval_ms, "poll interval", 1);
    }
    if (strcmp(arg, "--settle-ms") == 0) {
        return dcc_hot_reload_host_parse_settle_arg(value, options);
    }
    if (strcmp(arg, "--worker-health-ms") == 0) {
        return dcc_hot_reload_host_parse_u32_arg(
            value,
            &options->worker_health_timeout_ms,
            "worker health timeout",
            1
        );
    }
    if (strcmp(arg, "--worker-drain-ms") == 0) {
        return dcc_hot_reload_host_parse_u32_arg(
            value,
            &options->worker_drain_timeout_ms,
            "worker drain timeout",
            0
        );
    }
    return -1;
}
