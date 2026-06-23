#include "internal/dcc_command_sync.h"

#include <string.h>

int dcc_command_sync_parse_snowflake(const char *text, dcc_snowflake_t *out) {
    dcc_snowflake_t value = 0;
    if (dcc_snowflake_parse(text, &value) != DCC_OK || value == 0U || out == NULL) {
        return -1;
    }
    *out = value;
    return 0;
}

static void dcc_command_sync_defaults(dcc_command_sync_options_t *options) {
    memset(options, 0, sizeof(*options));
    options->plan = 1;
}

static int dcc_command_sync_take_value(
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

static int dcc_command_sync_parse_value_option(
    int argc,
    char **argv,
    int *index,
    dcc_command_sync_options_t *options
) {
    const char *arg = argv[*index];
    const char *value = NULL;
    if (dcc_command_sync_take_value(argc, argv, index, &value) != 0) {
        return -1;
    }
    if (strcmp(arg, "-c") == 0 || strcmp(arg, "--commands") == 0) {
        options->commands_path = value;
        return 0;
    }
    if (strcmp(arg, "--remote") == 0) {
        options->remote_path = value;
        return 0;
    }
    if (strcmp(arg, "-a") == 0 || strcmp(arg, "--application-id") == 0) {
        if (dcc_command_sync_parse_snowflake(value, &options->application_id) != 0) {
            fprintf(stderr, "invalid application id: %s\n", value);
            return -1;
        }
        return 0;
    }
    if (strcmp(arg, "-g") == 0 || strcmp(arg, "--guild") == 0) {
        if (dcc_command_sync_parse_snowflake(value, &options->guild_id) != 0) {
            fprintf(stderr, "invalid guild id: %s\n", value);
            return -1;
        }
        options->guild_scope_set = 1;
        return 0;
    }
    if (strcmp(arg, "--token") == 0) {
        options->token = value;
        return 0;
    }
    return -1;
}

static int dcc_command_sync_is_value_option(const char *arg) {
    return strcmp(arg, "-c") == 0 ||
           strcmp(arg, "--commands") == 0 ||
           strcmp(arg, "--remote") == 0 ||
           strcmp(arg, "-a") == 0 ||
           strcmp(arg, "--application-id") == 0 ||
           strcmp(arg, "-g") == 0 ||
           strcmp(arg, "--guild") == 0 ||
           strcmp(arg, "--token") == 0;
}

static void dcc_command_sync_apply_env(dcc_command_sync_options_t *options) {
    if (options->token == NULL) {
        const char *token = NULL;
        if (dcc_app_env_get_token(NULL, &token) == DCC_OK) {
            options->token = token;
        }
    }
    if (options->application_id == 0) {
        dcc_snowflake_t application_id = 0;
        if (dcc_app_env_get_snowflake("DCC_APPLICATION_ID", &application_id) == DCC_OK ||
            dcc_app_env_get_snowflake("DISCORD_APPLICATION_ID", &application_id) == DCC_OK) {
            options->application_id = application_id;
        }
    }
    if (!options->guild_scope_set && options->guild_id == 0) {
        dcc_snowflake_t guild_id = 0;
        if (dcc_app_env_get_guild("DCC_COMMAND_GUILD_ID", &guild_id) == DCC_OK ||
            dcc_app_env_get_guild("DCC_COMMAND_SYNC_GUILD_ID", &guild_id) == DCC_OK) {
            options->guild_id = guild_id;
            options->guild_scope_set = 1;
        }
    }
}

int dcc_command_sync_parse_options(
    int argc,
    char **argv,
    dcc_command_sync_options_t *options
) {
    dcc_command_sync_defaults(options);
    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            dcc_command_sync_usage(stdout, argv[0]);
            return 1;
        }
        if (dcc_command_sync_is_value_option(arg)) {
            if (dcc_command_sync_parse_value_option(argc, argv, &i, options) != 0) {
                return -1;
            }
            continue;
        }
        if (strcmp(arg, "--global") == 0) {
            options->guild_id = 0;
            options->guild_scope_set = 1;
        } else if (strcmp(arg, "--delete-stale") == 0) {
            options->delete_stale = 1;
        } else if (strcmp(arg, "--dry-run") == 0) {
            options->dry_run = 1;
        } else if (strcmp(arg, "--apply") == 0) {
            options->apply = 1;
        } else if (strcmp(arg, "--plan") == 0) {
            options->plan = 1;
        } else if (strcmp(arg, "--check") == 0) {
            options->check_only = 1;
        } else if (arg[0] != '-' && options->commands_path == NULL) {
            options->commands_path = arg;
        } else {
            fprintf(stderr, "unknown option: %s\n", arg);
            return -1;
        }
    }
    dcc_command_sync_apply_env(options);
    if (!options->apply) {
        options->dry_run = 1;
    }
    return 0;
}

int dcc_command_sync_validate_options(const dcc_command_sync_options_t *options) {
    if (options->commands_path == NULL || options->commands_path[0] == '\0') {
        fprintf(stderr, "missing --commands FILE\n");
        return -1;
    }
    if (options->remote_path == NULL && options->application_id == 0) {
        fprintf(stderr, "missing --application-id ID for REST remote fetch\n");
        return -1;
    }
    if ((options->remote_path == NULL || options->apply) &&
        (options->token == NULL || options->token[0] == '\0')) {
        fprintf(stderr, "missing bot token; set DCC_TOKEN, BOT_TOKEN, DISCORD_TOKEN, or --token\n");
        return -1;
    }
    return 0;
}
