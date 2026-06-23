#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static const char *dcc_new_app_basename(const char *path) {
    const char *base = path;
    const char *cursor = path;
    if (path == NULL) {
        return NULL;
    }
    while (*cursor != '\0') {
        if (*cursor == '/' || *cursor == '\\') {
            base = cursor + 1;
        }
        ++cursor;
    }
    return base;
}

static void dcc_new_app_make_name(const char *input, char *out, size_t out_len) {
    size_t written = 0U;
    int previous_underscore = 0;

    if (out_len == 0U) {
        return;
    }
    out[0] = '\0';
    if (input == NULL || input[0] == '\0') {
        input = "dcc_app";
    }
    if (isdigit((unsigned char)input[0])) {
        const char prefix[] = "dcc_";
        size_t prefix_len = sizeof(prefix) - 1U;
        if (prefix_len < out_len) {
            memcpy(out, prefix, prefix_len);
            written = prefix_len;
        }
    }
    for (const char *p = input; *p != '\0' && written + 1U < out_len; ++p) {
        unsigned char ch = (unsigned char)*p;
        if (isalnum(ch)) {
            out[written++] = (char)tolower(ch);
            previous_underscore = 0;
        } else if (!previous_underscore && written > 0U) {
            out[written++] = '_';
            previous_underscore = 1;
        }
    }
    while (written > 0U && out[written - 1U] == '_') {
        --written;
    }
    if (written == 0U) {
        const char fallback[] = "dcc_app";
        size_t len = sizeof(fallback) - 1U;
        if (len >= out_len) {
            len = out_len - 1U;
        }
        memcpy(out, fallback, len);
        written = len;
    }
    out[written] = '\0';
}

static void dcc_new_app_make_path(const char *input, char *out, size_t out_len) {
    size_t written = 0U;
    int previous_separator = 0;
    int previous_slash = 0;

    if (out_len == 0U) {
        return;
    }
    out[0] = '\0';
    if (input == NULL || input[0] == '\0') {
        input = "subcommand";
    }
    for (const char *p = input; *p != '\0' && written + 1U < out_len; ++p) {
        unsigned char ch = (unsigned char)*p;
        if (isalnum(ch)) {
            out[written++] = (char)tolower(ch);
            previous_separator = 0;
            previous_slash = 0;
        } else if ((ch == '/' || ch == ':') && written > 0U && !previous_slash) {
            while (written > 0U && out[written - 1U] == '_') {
                --written;
            }
            if (written > 0U && written + 1U < out_len) {
                out[written++] = '/';
                previous_slash = 1;
                previous_separator = 1;
            }
        } else if (!previous_separator && written > 0U) {
            out[written++] = '_';
            previous_separator = 1;
            previous_slash = 0;
        }
    }
    while (written > 0U && (out[written - 1U] == '_' || out[written - 1U] == '/')) {
        --written;
    }
    if (written == 0U) {
        const char fallback[] = "subcommand";
        size_t len = sizeof(fallback) - 1U;
        if (len >= out_len) {
            len = out_len - 1U;
        }
        memcpy(out, fallback, len);
        written = len;
    }
    out[written] = '\0';
}

static int dcc_new_app_take_value(int argc, char **argv, int *index, const char **out) {
    if (*index + 1 >= argc) {
        fprintf(stderr, "%s requires a value\n", argv[*index]);
        return -1;
    }
    ++(*index);
    *out = argv[*index];
    return 0;
}

static int dcc_new_app_mode_adds_to_cog(dcc_new_app_mode_t mode) {
    return mode == DCC_NEW_APP_MODE_ADD_COG ||
           mode == DCC_NEW_APP_MODE_ADD_COMMAND ||
           mode == DCC_NEW_APP_MODE_ADD_SUBCOMMAND ||
           mode == DCC_NEW_APP_MODE_ADD_BUTTON ||
           mode == DCC_NEW_APP_MODE_ADD_SELECT ||
           mode == DCC_NEW_APP_MODE_ADD_MODAL ||
           mode == DCC_NEW_APP_MODE_ADD_AUTOCOMPLETE ||
           mode == DCC_NEW_APP_MODE_ADD_EVENT ||
           mode == DCC_NEW_APP_MODE_ADD_VIEW ||
           mode == DCC_NEW_APP_MODE_ADD_LATEST_MESSAGE ||
           mode == DCC_NEW_APP_MODE_ADD_MESSAGE_COMMAND ||
           mode == DCC_NEW_APP_MODE_ADD_TASK ||
           mode == DCC_NEW_APP_MODE_ADD_ACTION ||
           mode == DCC_NEW_APP_MODE_ADD_CONTEXT_MENU ||
           mode == DCC_NEW_APP_MODE_ADD_MIDDLEWARE ||
           mode == DCC_NEW_APP_MODE_ADD_PRESET;
}

static int dcc_new_app_mode_adds_cog_item(dcc_new_app_mode_t mode) {
    return mode == DCC_NEW_APP_MODE_ADD_COMMAND ||
           mode == DCC_NEW_APP_MODE_ADD_SUBCOMMAND ||
           mode == DCC_NEW_APP_MODE_ADD_BUTTON ||
           mode == DCC_NEW_APP_MODE_ADD_SELECT ||
           mode == DCC_NEW_APP_MODE_ADD_MODAL ||
           mode == DCC_NEW_APP_MODE_ADD_AUTOCOMPLETE ||
           mode == DCC_NEW_APP_MODE_ADD_EVENT ||
           mode == DCC_NEW_APP_MODE_ADD_VIEW ||
           mode == DCC_NEW_APP_MODE_ADD_LATEST_MESSAGE ||
           mode == DCC_NEW_APP_MODE_ADD_MESSAGE_COMMAND ||
           mode == DCC_NEW_APP_MODE_ADD_TASK ||
           mode == DCC_NEW_APP_MODE_ADD_ACTION ||
           mode == DCC_NEW_APP_MODE_ADD_CONTEXT_MENU ||
           mode == DCC_NEW_APP_MODE_ADD_MIDDLEWARE ||
           mode == DCC_NEW_APP_MODE_ADD_PRESET;
}

static const char *dcc_new_app_mode_item_label(dcc_new_app_mode_t mode) {
    switch (mode) {
        case DCC_NEW_APP_MODE_ADD_COMMAND:
            return "COMMAND_NAME";
        case DCC_NEW_APP_MODE_ADD_SUBCOMMAND:
            return "COMMAND_NAME";
        case DCC_NEW_APP_MODE_ADD_BUTTON:
            return "BUTTON_NAME";
        case DCC_NEW_APP_MODE_ADD_SELECT:
            return "SELECT_NAME";
        case DCC_NEW_APP_MODE_ADD_MODAL:
            return "MODAL_NAME";
        case DCC_NEW_APP_MODE_ADD_AUTOCOMPLETE:
            return "COMMAND_NAME";
        case DCC_NEW_APP_MODE_ADD_EVENT:
            return "EVENT_NAME";
        case DCC_NEW_APP_MODE_ADD_VIEW:
            return "VIEW_NAME";
        case DCC_NEW_APP_MODE_ADD_LATEST_MESSAGE:
            return "COMMAND_NAME";
        case DCC_NEW_APP_MODE_ADD_MESSAGE_COMMAND:
            return "MESSAGE_COMMAND_NAME";
        case DCC_NEW_APP_MODE_ADD_TASK:
            return "TASK_NAME";
        case DCC_NEW_APP_MODE_ADD_ACTION:
            return "ACTION_NAME";
        case DCC_NEW_APP_MODE_ADD_CONTEXT_MENU:
            return "CONTEXT_MENU_NAME";
        case DCC_NEW_APP_MODE_ADD_MIDDLEWARE:
            return "MIDDLEWARE_NAME";
        case DCC_NEW_APP_MODE_ADD_ERROR_HANDLER:
            return "ERROR_HANDLER_NAME";
        case DCC_NEW_APP_MODE_ADD_PRESET:
            return "PRESET_NAME";
        default:
            return "NAME";
    }
}

int dcc_new_app_parse_options(int argc, char **argv, dcc_new_app_options_t *options) {
    memset(options, 0, sizeof(*options));
    options->mode = DCC_NEW_APP_MODE_CREATE;
    options->prefix_arg = "!";
    options->task_interval_arg = "60";
    options->task_schedule = DCC_NEW_APP_TASK_SECONDS;

    int i = 1;
    if (i < argc && strcmp(argv[i], "add") == 0) {
        ++i;
        if (i >= argc) {
            fprintf(stderr, "add requires a resource type\n");
            return -1;
        }
        if (strcmp(argv[i], "feature") == 0 ||
            strcmp(argv[i], "module") == 0 ||
            strcmp(argv[i], "cog") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_COG;
        } else if (strcmp(argv[i], "command") == 0 || strcmp(argv[i], "slash") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_COMMAND;
        } else if (strcmp(argv[i], "subcommand") == 0 || strcmp(argv[i], "sub") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_SUBCOMMAND;
        } else if (strcmp(argv[i], "button") == 0 || strcmp(argv[i], "component") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_BUTTON;
        } else if (strcmp(argv[i], "select") == 0 || strcmp(argv[i], "menu") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_SELECT;
        } else if (strcmp(argv[i], "modal") == 0 || strcmp(argv[i], "form") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_MODAL;
        } else if (strcmp(argv[i], "autocomplete") == 0 || strcmp(argv[i], "complete") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_AUTOCOMPLETE;
        } else if (strcmp(argv[i], "event") == 0 || strcmp(argv[i], "listener") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_EVENT;
        } else if (strcmp(argv[i], "view") == 0 || strcmp(argv[i], "persistent-view") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_VIEW;
        } else if (strcmp(argv[i], "config") == 0 || strcmp(argv[i], "env") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_CONFIG;
        } else if (strcmp(argv[i], "latest-message") == 0 || strcmp(argv[i], "latest") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_LATEST_MESSAGE;
        } else if (strcmp(argv[i], "scheduled-latest") == 0 || strcmp(argv[i], "latest-task") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_LATEST_MESSAGE;
            options->subcommand_arg = "scheduled";
        } else if (strcmp(argv[i], "message-command") == 0 ||
                   strcmp(argv[i], "prefix-command") == 0 ||
                   strcmp(argv[i], "msg") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_MESSAGE_COMMAND;
        } else if (strcmp(argv[i], "task") == 0 || strcmp(argv[i], "loop") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_TASK;
        } else if (strcmp(argv[i], "action") == 0 ||
                   strcmp(argv[i], "rest-action") == 0 ||
                   strcmp(argv[i], "shortcut") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_ACTION;
        } else if (strcmp(argv[i], "context-menu") == 0 ||
                   strcmp(argv[i], "context") == 0 ||
                   strcmp(argv[i], "app-command") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_CONTEXT_MENU;
        } else if (strcmp(argv[i], "user-menu") == 0 ||
                   strcmp(argv[i], "user-context") == 0 ||
                   strcmp(argv[i], "user_context") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_CONTEXT_MENU;
            options->subcommand_arg = "user";
        } else if (strcmp(argv[i], "message-menu") == 0 ||
                   strcmp(argv[i], "message-context") == 0 ||
                   strcmp(argv[i], "message_context") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_CONTEXT_MENU;
            options->subcommand_arg = "message";
        } else if (strcmp(argv[i], "middleware") == 0 ||
                   strcmp(argv[i], "check") == 0 ||
                   strcmp(argv[i], "cog-check") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_MIDDLEWARE;
        } else if (strcmp(argv[i], "error-handler") == 0 ||
                   strcmp(argv[i], "errors") == 0 ||
                   strcmp(argv[i], "on-error") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_ERROR_HANDLER;
        } else if (strcmp(argv[i], "preset") == 0 || strcmp(argv[i], "ui-preset") == 0) {
            options->mode = DCC_NEW_APP_MODE_ADD_PRESET;
        } else {
            fprintf(stderr, "unknown add resource: %s\n", argv[i]);
            return -1;
        }
        ++i;
    }

    for (; i < argc; ++i) {
        const char *arg = argv[i];
        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            options->help = 1U;
            return 1;
        }
        if (strcmp(arg, "-f") == 0 || strcmp(arg, "--force") == 0) {
            options->force = 1U;
            continue;
        }
        if (strcmp(arg, "-n") == 0 || strcmp(arg, "--name") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->name_arg) != 0) {
                return -1;
            }
            continue;
        }
        if (strcmp(arg, "--prefix") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->prefix_arg) != 0) {
                return -1;
            }
            continue;
        }
        if (strcmp(arg, "--every-ms") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->task_interval_arg) != 0) {
                return -1;
            }
            options->task_schedule = DCC_NEW_APP_TASK_MS;
            continue;
        }
        if (strcmp(arg, "--every-seconds") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->task_interval_arg) != 0) {
                return -1;
            }
            options->task_schedule = DCC_NEW_APP_TASK_SECONDS;
            continue;
        }
        if (strcmp(arg, "--every-minutes") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->task_interval_arg) != 0) {
                return -1;
            }
            options->task_schedule = DCC_NEW_APP_TASK_MINUTES;
            continue;
        }
        if (strcmp(arg, "--every-hours") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->task_interval_arg) != 0) {
                return -1;
            }
            options->task_schedule = DCC_NEW_APP_TASK_HOURS;
            continue;
        }
        if (strcmp(arg, "--daily-kst") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->task_interval_arg) != 0) {
                return -1;
            }
            options->task_schedule = DCC_NEW_APP_TASK_DAILY_KST;
            continue;
        }
        if (strcmp(arg, "--guild-only") == 0) {
            options->guard_guild_only = 1U;
            continue;
        }
        if (strcmp(arg, "--dm-only") == 0) {
            options->guard_dm_only = 1U;
            continue;
        }
        if (strcmp(arg, "--nsfw-only") == 0) {
            options->guard_nsfw_only = 1U;
            continue;
        }
        if (strcmp(arg, "--owner") == 0 || strcmp(arg, "--guard-owner") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->guard_owner_arg) != 0) {
                return -1;
            }
            continue;
        }
        if (strcmp(arg, "--role") == 0 || strcmp(arg, "--guard-role") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->guard_role_arg) != 0) {
                return -1;
            }
            continue;
        }
        if (strcmp(arg, "--any-role") == 0 || strcmp(arg, "--guard-any-role") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->guard_any_role_arg) != 0) {
                return -1;
            }
            continue;
        }
        if (strcmp(arg, "--permission") == 0 || strcmp(arg, "--permissions") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->guard_permission_arg) != 0) {
                return -1;
            }
            continue;
        }
        if (strcmp(arg, "--cooldown-user") == 0 || strcmp(arg, "--cooldown") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->guard_cooldown_user_arg) != 0) {
                return -1;
            }
            continue;
        }
        if (strcmp(arg, "--cooldown-global") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->guard_cooldown_global_arg) != 0) {
                return -1;
            }
            continue;
        }
        if (strcmp(arg, "--cooldown-channel") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->guard_cooldown_channel_arg) != 0) {
                return -1;
            }
            continue;
        }
        if (strcmp(arg, "--cooldown-guild") == 0) {
            if (dcc_new_app_take_value(argc, argv, &i, &options->guard_cooldown_guild_arg) != 0) {
                return -1;
            }
            continue;
        }
        if (arg[0] == '-') {
            fprintf(stderr, "unknown option: %s\n", arg);
            return -1;
        }
        if (options->path == NULL) {
            options->path = arg;
            continue;
        }
        if (options->mode == DCC_NEW_APP_MODE_ADD_CONFIG) {
            if (options->command_arg == NULL) {
                options->command_arg = arg;
                continue;
            }
            if (options->config_type_arg == NULL) {
                options->config_type_arg = arg;
                continue;
            }
            if (options->config_env_arg == NULL) {
                options->config_env_arg = arg;
                continue;
            }
        }
        if (options->mode == DCC_NEW_APP_MODE_ADD_COG && options->cog_arg == NULL) {
            options->cog_arg = arg;
            continue;
        }
        if (options->mode == DCC_NEW_APP_MODE_ADD_ERROR_HANDLER) {
            if (options->command_arg == NULL) {
                options->command_arg = arg;
                continue;
            }
            if (options->subcommand_arg == NULL) {
                options->subcommand_arg = arg;
                continue;
            }
        }
        if (dcc_new_app_mode_adds_cog_item(options->mode)) {
            if (options->cog_arg == NULL) {
                options->cog_arg = arg;
                continue;
            }
            if (options->command_arg == NULL) {
                options->command_arg = arg;
                continue;
            }
            if (options->mode == DCC_NEW_APP_MODE_ADD_SUBCOMMAND &&
                options->subcommand_arg == NULL) {
                options->subcommand_arg = arg;
                continue;
            }
            if (options->mode == DCC_NEW_APP_MODE_ADD_LATEST_MESSAGE &&
                options->config_type_arg == NULL) {
                options->config_type_arg = arg;
                continue;
            }
            if (options->mode == DCC_NEW_APP_MODE_ADD_ACTION &&
                options->subcommand_arg == NULL) {
                options->subcommand_arg = arg;
                continue;
            }
            if (options->mode == DCC_NEW_APP_MODE_ADD_CONTEXT_MENU &&
                options->subcommand_arg == NULL) {
                options->subcommand_arg = arg;
                continue;
            }
            if (options->mode == DCC_NEW_APP_MODE_ADD_MIDDLEWARE &&
                options->subcommand_arg == NULL) {
                options->subcommand_arg = arg;
                continue;
            }
        }
        fprintf(stderr, "too many positional arguments\n");
        return -1;
    }
    return 0;
}

int dcc_new_app_validate_options(dcc_new_app_options_t *options) {
    if (options == NULL || options->path == NULL || options->path[0] == '\0') {
        fprintf(stderr, "PATH is required\n");
        return -1;
    }
    if (dcc_new_app_mode_adds_to_cog(options->mode) &&
        (options->cog_arg == NULL || options->cog_arg[0] == '\0')) {
        fprintf(
            stderr,
            "FEATURE_NAME is required for add feature/command/subcommand/button/select/modal/autocomplete/event/view/latest-message/message-command/task/action/context-menu/middleware/preset\n"
        );
        return -1;
    }
    if (dcc_new_app_mode_adds_cog_item(options->mode) &&
        (options->command_arg == NULL || options->command_arg[0] == '\0')) {
        fprintf(stderr, "%s is required\n", dcc_new_app_mode_item_label(options->mode));
        return -1;
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_SUBCOMMAND &&
        (options->subcommand_arg == NULL || options->subcommand_arg[0] == '\0')) {
        fprintf(stderr, "SUBCOMMAND_NAME is required\n");
        return -1;
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_LATEST_MESSAGE &&
        (options->config_type_arg == NULL || options->config_type_arg[0] == '\0')) {
        fprintf(stderr, "CHANNEL_FIELD is required\n");
        return -1;
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_ACTION) {
        if (options->subcommand_arg == NULL || options->subcommand_arg[0] == '\0') {
            fprintf(stderr, "ACTION_KIND is required\n");
            return -1;
        }
        if (strcmp(options->subcommand_arg, "send") != 0 &&
            strcmp(options->subcommand_arg, "gateway") != 0 &&
            strcmp(options->subcommand_arg, "dm") != 0 &&
            strcmp(options->subcommand_arg, "direct-message") != 0 &&
            strcmp(options->subcommand_arg, "direct_message") != 0 &&
            strcmp(options->subcommand_arg, "role-add") != 0 &&
            strcmp(options->subcommand_arg, "role_add") != 0 &&
            strcmp(options->subcommand_arg, "add-role") != 0 &&
            strcmp(options->subcommand_arg, "role-remove") != 0 &&
            strcmp(options->subcommand_arg, "role_remove") != 0 &&
            strcmp(options->subcommand_arg, "remove-role") != 0 &&
            strcmp(options->subcommand_arg, "voice") != 0 &&
            strcmp(options->subcommand_arg, "voice-regions") != 0 &&
            strcmp(options->subcommand_arg, "voice_regions") != 0 &&
            strcmp(options->subcommand_arg, "guild-voice-regions") != 0) {
            fprintf(stderr, "ACTION_KIND must be send, gateway, dm, role-add, role-remove, or voice-regions\n");
            return -1;
        }
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_CONTEXT_MENU) {
        if (options->subcommand_arg == NULL || options->subcommand_arg[0] == '\0') {
            fprintf(stderr, "CONTEXT_MENU_KIND is required\n");
            return -1;
        }
        if (strcmp(options->subcommand_arg, "user") != 0 &&
            strcmp(options->subcommand_arg, "user-menu") != 0 &&
            strcmp(options->subcommand_arg, "user-context") != 0 &&
            strcmp(options->subcommand_arg, "user_context") != 0 &&
            strcmp(options->subcommand_arg, "message") != 0 &&
            strcmp(options->subcommand_arg, "message-menu") != 0 &&
            strcmp(options->subcommand_arg, "message-context") != 0 &&
            strcmp(options->subcommand_arg, "message_context") != 0) {
            fprintf(stderr, "CONTEXT_MENU_KIND must be user or message\n");
            return -1;
        }
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_MIDDLEWARE && options->subcommand_arg != NULL) {
        if (strcmp(options->subcommand_arg, "pass") != 0 &&
            strcmp(options->subcommand_arg, "noop") != 0 &&
            strcmp(options->subcommand_arg, "config") != 0 &&
            strcmp(options->subcommand_arg, "state") != 0 &&
            strcmp(options->subcommand_arg, "guild") != 0 &&
            strcmp(options->subcommand_arg, "guild-only") != 0 &&
            strcmp(options->subcommand_arg, "dm") != 0 &&
            strcmp(options->subcommand_arg, "dm-only") != 0) {
            fprintf(stderr, "MIDDLEWARE_KIND must be pass, config, guild, or dm\n");
            return -1;
        }
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_ERROR_HANDLER) {
        if (options->command_arg == NULL || options->command_arg[0] == '\0') {
            fprintf(stderr, "ERROR_HANDLER_NAME is required\n");
            return -1;
        }
        if (options->subcommand_arg != NULL &&
            strcmp(options->subcommand_arg, "friendly") != 0 &&
            strcmp(options->subcommand_arg, "simple") != 0 &&
            strcmp(options->subcommand_arg, "verbose") != 0) {
            fprintf(stderr, "ERROR_HANDLER_KIND must be friendly, simple, or verbose\n");
            return -1;
        }
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_CONFIG) {
        if (options->command_arg == NULL || options->command_arg[0] == '\0') {
            fprintf(stderr, "FIELD_NAME is required\n");
            return -1;
        }
        if (options->config_type_arg == NULL || options->config_type_arg[0] == '\0') {
            fprintf(stderr, "CONFIG_TYPE is required\n");
            return -1;
        }
        if (options->config_env_arg == NULL || options->config_env_arg[0] == '\0') {
            fprintf(stderr, "ENV_NAME is required\n");
            return -1;
        }
    }
    dcc_new_app_make_name(
        options->name_arg != NULL ? options->name_arg : dcc_new_app_basename(options->path),
        options->name,
        sizeof(options->name)
    );
    if (options->mode == DCC_NEW_APP_MODE_ADD_COG) {
        dcc_new_app_make_name(
            options->name_arg != NULL ? options->name_arg : options->cog_arg,
            options->cog_name,
            sizeof(options->cog_name)
        );
    } else if (dcc_new_app_mode_adds_cog_item(options->mode)) {
        dcc_new_app_make_name(options->cog_arg, options->cog_name, sizeof(options->cog_name));
        dcc_new_app_make_name(options->command_arg, options->command_name, sizeof(options->command_name));
        if (options->mode == DCC_NEW_APP_MODE_ADD_SUBCOMMAND) {
            dcc_new_app_make_name(
                options->name_arg != NULL ? options->name_arg : options->subcommand_arg,
                options->subcommand_name,
                sizeof(options->subcommand_name)
            );
            dcc_new_app_make_path(options->subcommand_arg, options->subcommand_path, sizeof(options->subcommand_path));
        } else {
            dcc_new_app_make_name(
                options->name_arg != NULL ? options->name_arg : options->command_arg,
                options->command_name,
                sizeof(options->command_name)
            );
        }
    } else if (options->mode == DCC_NEW_APP_MODE_ADD_CONFIG) {
        dcc_new_app_make_name(
            options->name_arg != NULL ? options->name_arg : options->command_arg,
            options->command_name,
            sizeof(options->command_name)
        );
    } else if (options->mode == DCC_NEW_APP_MODE_ADD_ERROR_HANDLER) {
        dcc_new_app_make_name(
            options->name_arg != NULL ? options->name_arg : options->command_arg,
            options->command_name,
            sizeof(options->command_name)
        );
    }
    return dcc_new_app_guard_options_validate(options);
}
