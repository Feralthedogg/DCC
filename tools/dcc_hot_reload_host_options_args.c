#include "internal/dcc_hot_reload_host.h"

#include <string.h>

static int dcc_hot_reload_host_value_option_name(const char *arg) {
    return strcmp(arg, "-m") == 0 ||
           strcmp(arg, "--module") == 0 ||
           strcmp(arg, "-w") == 0 ||
           strcmp(arg, "--worker") == 0 ||
           strcmp(arg, "-i") == 0 ||
           strcmp(arg, "--intents") == 0 ||
           strcmp(arg, "-g") == 0 ||
           strcmp(arg, "--guild") == 0 ||
           strcmp(arg, "--health-address") == 0 ||
           strcmp(arg, "--health-port") == 0 ||
           strcmp(arg, "--poll-ms") == 0 ||
           strcmp(arg, "--settle-ms") == 0 ||
           strcmp(arg, "--worker-health-ms") == 0 ||
           strcmp(arg, "--worker-drain-ms") == 0;
}

int dcc_hot_reload_host_parse_option_arg(
    int argc,
    char **argv,
    int *index,
    dcc_hot_reload_host_options_t *options,
    int *out_handled
) {
    const char *arg = argv[*index];
    *out_handled = 1;

    if (dcc_hot_reload_host_value_option_name(arg)) {
        return dcc_hot_reload_host_parse_value_option(argc, argv, index, options);
    }
    if (strcmp(arg, "--global") == 0) {
        options->guild_id = 0;
        options->guild_id_set = -1;
        return 0;
    }
    if (strcmp(arg, "--check") == 0) {
        options->check_only = 1;
        return 0;
    }
    if (arg[0] == '-') {
        fprintf(stderr, "unknown option: %s\n", arg);
        return -1;
    }

    *out_handled = 0;
    return 0;
}
