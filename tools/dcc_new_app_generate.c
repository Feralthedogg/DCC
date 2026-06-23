#include "internal/dcc_new_app.h"

#include <stdio.h>
#include <stdlib.h>

static int dcc_new_app_write_cmake(const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_join(options->path, "CMakeLists.txt");
    if (path == NULL) {
        return -1;
    }
    FILE *file = dcc_new_app_open_output(path, options->force);
    free(path);
    if (file == NULL) {
        return -1;
    }
    fprintf(
        file,
        "cmake_minimum_required(VERSION 3.20)\n"
        "\n"
        "project(%s VERSION 0.1.0 LANGUAGES C)\n"
        "\n"
        "find_package(dcc CONFIG REQUIRED)\n"
        "\n"
        "add_executable(%s\n"
        "    src/main.c\n"
        "    src/ping.c\n"
        "    # dcc_new_app:feature-sources\n"
        ")\n"
        "target_link_libraries(%s PRIVATE dcc::dcc)\n"
        "target_compile_features(%s PRIVATE c_std_11)\n",
        options->name,
        options->name,
        options->name,
        options->name
    );
    return fclose(file) == 0 ? 0 : -1;
}

static int dcc_new_app_write_main(const dcc_new_app_options_t *options) {
    char *src_dir = dcc_new_app_join(options->path, "src");
    if (src_dir == NULL) {
        return -1;
    }
    int status = dcc_new_app_mkdirs(src_dir);
    char *path = status == 0 ? dcc_new_app_join(src_dir, "main.c") : NULL;
    free(src_dir);
    if (path == NULL) {
        return -1;
    }
    FILE *file = dcc_new_app_open_output(path, options->force);
    free(path);
    if (file == NULL) {
        return -1;
    }
    fputs(
        "#include <dcc/dcc.h>\n"
        "#include <dcc/sugar.h>\n"
        "\n"
        "#include \"config.h\"\n"
        "#include \"ping.h\"\n"
        "/* dcc_new_app:feature-includes */\n"
        "\n"
        "#include <stdio.h>\n"
        "\n"
        "/* dcc_new_app:error-handlers */\n"
        "\n"
        "int main(void) {\n"
        "    bot_config_t config = {0};\n"
        "\n"
        "    dcc_status_t status = dcc_app_load_dotenv();\n"
        "    if (status != DCC_OK && status != DCC_ERR_NOT_FOUND) {\n"
        "        fprintf(stderr, \"DCC dotenv failed: %s\\n\", dcc_status_string(status));\n"
        "        return 1;\n"
        "    }\n"
        "\n"
        "    status =\n"
        "        DCC_CONFIG(\n"
        "            DCC_CONFIG_BOOL_DEFAULT(\"BOT_DEBUG\", 0U, &config.debug),\n"
        "            /* dcc_new_app:config-bindings */\n"
        "        );\n"
        "    if (status != DCC_OK) {\n"
        "        fprintf(stderr, \"DCC config failed: %s\\n\", dcc_status_string(status));\n"
        "        return 1;\n"
        "    }\n"
        "\n"
        "    status =\n"
        "        DCC_RUN_APP_ENV(\n"
        "            NULL,\n"
        "            DCC_BOT(\n"
        "                \"bot\",\n"
        "                DCC_APP_USE(\n"
        "                    DCC_USE_FEATURE(ping, &config),\n"
        "                    /* dcc_new_app:feature-modules */\n"
        "                ),\n"
        "                /* dcc_new_app:error-handler-option */\n"
        "            )\n"
        "        );\n"
        "    if (status != DCC_OK) {\n"
        "        fprintf(stderr, \"DCC failed: %s\\n\", dcc_status_string(status));\n"
        "    }\n"
        "    return status == DCC_OK ? 0 : 1;\n"
        "}\n",
        file
    );
    return fclose(file) == 0 ? 0 : -1;
}

static int dcc_new_app_write_config_header(const dcc_new_app_options_t *options) {
    char *src_dir = dcc_new_app_join(options->path, "src");
    if (src_dir == NULL) {
        return -1;
    }
    int status = dcc_new_app_mkdirs(src_dir);
    char *path = status == 0 ? dcc_new_app_join(src_dir, "config.h") : NULL;
    free(src_dir);
    if (path == NULL) {
        return -1;
    }
    FILE *file = dcc_new_app_open_output(path, options->force);
    free(path);
    if (file == NULL) {
        return -1;
    }
    fputs(
        "#ifndef DCC_GENERATED_CONFIG_H\n"
        "#define DCC_GENERATED_CONFIG_H\n"
        "\n"
        "#include <dcc/dcc.h>\n"
        "\n"
        "#include <stdint.h>\n"
        "\n"
        "typedef struct bot_config {\n"
        "    uint8_t debug;\n"
        "    /* dcc_new_app:config-fields */\n"
        "} bot_config_t;\n"
        "\n"
        "static inline bot_config_t *bot_config_from_user_data(void *user_data) {\n"
        "    return (bot_config_t *)user_data;\n"
        "}\n"
        "\n"
        "static inline bot_config_t *bot_config_from_ctx(const dcc_ctx_t *ctx) {\n"
        "    return (bot_config_t *)dcc_ctx_user_data(ctx);\n"
        "}\n"
        "\n"
        "#define BOT_CONFIG(user_data_) bot_config_from_user_data((user_data_))\n"
        "#define BOT_CTX_CONFIG(ctx_) bot_config_from_ctx((ctx_))\n"
        "\n"
        "#endif\n",
        file
    );
    return fclose(file) == 0 ? 0 : -1;
}

static int dcc_new_app_write_literal(
    const dcc_new_app_options_t *options,
    const char *relative_path,
    const char *content
) {
    char *path = dcc_new_app_join(options->path, relative_path);
    if (path == NULL) {
        return -1;
    }
    FILE *file = dcc_new_app_open_output(path, options->force);
    free(path);
    if (file == NULL) {
        return -1;
    }
    fputs(content, file);
    return fclose(file) == 0 ? 0 : -1;
}

static int dcc_new_app_write_readme(const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_join(options->path, "README.md");
    if (path == NULL) {
        return -1;
    }
    FILE *file = dcc_new_app_open_output(path, options->force);
    free(path);
    if (file == NULL) {
        return -1;
    }
    fprintf(
        file,
        "# %s\n"
        "\n"
        "Generated by `dcc_new_app`.\n"
        "\n"
        "## Configure\n"
        "\n"
        "```sh\n"
        "cp .env.example .env\n"
        "$EDITOR .env\n"
        "```\n"
        "\n"
        "Use `DCC_COMMAND_GUILD_ID` during development when you want faster guild-scoped\n"
        "slash command updates. Leave it commented out for global commands.\n"
        "\n"
        "`src/main.c` loads `.env`, binds typed feature config with `DCC_CONFIG(...)`,\n"
        "then runs the app with `DCC_RUN_APP_ENV(...)`. Add channel IDs, role IDs,\n"
        "booleans, and numeric settings with `dcc_new_app add config ...` instead of\n"
        "scattering `getenv()` parsing through handlers. The generated `src/config.h`\n"
        "exposes `bot_config_t`, `BOT_CONFIG(user_data)`, and `BOT_CTX_CONFIG(ctx)`.\n"
        "The template starts with `BOT_DEBUG=false` as a small typed boolean config example.\n"
        "\n"
        "The generated `DCC_BOT(...)` definition includes `DCC_APP_DEV_MODE()`, so the\n"
        "app reconciles local slash commands on READY, infers the application id from the\n"
        "Gateway READY event, auto-defers slow interactions, and enables friendly default\n"
        "errors. Editing a feature file such as `src/ping.c` and restarting the app is enough\n"
        "for normal development.\n"
        "\n"
        "Slash and subcommand routes use typed args by default: generated feature files declare\n"
        "a small args struct plus a `*_PARAMS` list, then register through\n"
        "`DCC_FEATURE_COMMAND_ROUTES(...)`, `DCC_COMMAND_ROUTE_NO_OPTIONS*`, `DCC_COMMAND_ROUTE*`,\n"
        "`DCC_FEATURE_SUBCOMMAND_ROUTES(...)`, `DCC_SUBCOMMAND_ROUTE*`,\n"
        "`DCC_FEATURE_AUTOCOMPLETE_ROUTES(...)`, and `DCC_AUTOCOMPLETE_PARAMS*`.\n"
        "Generated button, select, and modal routes use small args structs plus\n"
        "`*_PARAMS` binding lists through `DCC_BUTTON_PARAMS*`,\n"
        "`DCC_SELECT_PARAMS*`, and `DCC_MODAL_PARAMS*`.\n"
        "Handlers are declared with `DCC_COMMAND_IMPL`, `DCC_HANDLER`, `DCC_COMMAND_ARGS_IMPL`,\n"
        "`DCC_BUTTON_ARGS_IMPL`, `DCC_SELECT_ARGS_IMPL`, `DCC_MODAL_ARGS_IMPL`,\n"
        "`DCC_AUTOCOMPLETE_ARGS_IMPL`, `DCC_READY_FN`, `DCC_EVENT_FN`,\n"
        "`DCC_MESSAGE_COMMAND_FN`, and `DCC_TASK_FN`, then reply through\n"
        "the context-first `DCC_CTX_*` aliases, formatted replies such as\n"
        "`DCC_CTX_REPLY_TEXT_F(ctx, \"pong #%%llu\", (unsigned long long)count)`, and Components v2 UI\n"
        "with the short `DCC_UI_*` aliases such as `DCC_CTX_REPLY_UI(ctx, DCC_UI_TEXT(reply))`.\n"
        "Feature files are public feature modules declared with `DCC_DEFINE_PUBLIC_FEATURE(...)`\n"
        "and wired from `src/main.c` with `DCC_USE_FEATURE(...)` inside\n"
        "`DCC_APP_USE(...)`.\n"
        "Simple component routes still use the sugar-first `DCC_ON_*` aliases, so\n"
        "hand-written button or modal handlers can stay as short as\n"
        "`DCC_BUTTON(\"custom.id\", on_click)` or `DCC_MODAL(\"custom.id\", on_submit)`.\n"
        "\n"
        "Add a new feature file with:\n"
        "\n"
        "```sh\n"
        "dcc_new_app add feature . profile\n"
        "dcc_new_app add command . profile greet\n"
        "dcc_new_app add subcommand . profile greet stats\n"
        "dcc_new_app add button . profile refresh\n"
        "dcc_new_app add select . profile role\n"
        "dcc_new_app add modal . profile edit\n"
        "dcc_new_app add autocomplete . profile profile\n"
        "dcc_new_app add context-menu . profile inspect_user user --guild-only\n"
        "dcc_new_app add context-menu . profile quote_message message\n"
        "dcc_new_app add middleware . profile require_config config\n"
        "dcc_new_app add event . profile ready\n"
        "dcc_new_app add view . profile dashboard\n"
        "dcc_new_app add config . log_channel channel LOG_CHANNEL\n"
        "dcc_new_app add error-handler . bot_error friendly\n"
        "dcc_new_app add latest-message . profile status log_channel\n"
        "dcc_new_app add scheduled-latest . profile daily_status log_channel --daily-kst 09:00\n"
        "dcc_new_app add message-command . profile hello --prefix '!'\n"
        "dcc_new_app add task . profile heartbeat --every-seconds 60\n"
        "dcc_new_app add action . profile gateway_status gateway --guild-only\n"
        "dcc_new_app add action . profile dm_user dm --guild-only\n"
        "dcc_new_app add action . profile grant_role role-add --guild-only --permission DCC_PERMISSION_MANAGE_ROLES\n"
        "dcc_new_app add preset . profile announcement --guild-only\n"
        "dcc_new_app add preset . profile confirm\n"
        "dcc_new_app add preset . profile paginator\n"
        "dcc_new_app add preset . profile form\n"
        "dcc_new_app add preset . profile settings\n"
        "dcc_new_app add preset . profile wizard\n"
        "dcc_new_app add preset . profile counter\n"
        "dcc_new_app add preset . profile crud\n"
        "dcc_new_app add preset . profile help\n"
        "dcc_new_app add preset . profile menu\n"
        "dcc_new_app add preset . profile poll\n"
        "dcc_new_app add preset . profile profile --name profile_card\n"
        "dcc_new_app add preset . profile roles --guild-only\n"
        "dcc_new_app add preset . profile ticket\n"
        "dcc_new_app add preset . profile welcome\n"
        "dcc_new_app add preset . profile flow\n"
        "```\n"
        "\n"
        "Generated apps include markers in `CMakeLists.txt` and `src/main.c`, so the new\n"
        "feature wiring is automatic when those markers are still present. Feature source\n"
        "files also include route markers, so `add command`, `add subcommand`,\n"
        "`add button`, `add select`, `add modal`, `add autocomplete`, `add event`,\n"
        "`add view`, `add latest-message`, `add message-command`, `add task`, `add action`,\n"
        "`add context-menu`, `add middleware`, and `add preset` append handlers and routes\n"
        "without touching `src/main.c`.\n"
        "`add config` and `add error-handler` are the app-level exceptions:\n"
        "`add config` appends a typed field to `bot_config_t`, wires the matching\n"
        "`DCC_CONFIG_*` binding, and adds the placeholder to `.env.example`.\n"
        "`add error-handler` appends a `DCC_FRIENDLY_ERROR_FN(...)`,\n"
        "`DCC_SIMPLE_ERROR_FN(...)`, or `DCC_VERBOSE_ERROR_FN(...)` handler to\n"
        "`src/main.c` and registers it with `DCC_APP_ERROR_HANDLER(...)`.\n"
        "\n"
        "`DCC_STORE_FILE` enables the app-owned file store. The generated ping handler\n"
        "uses `DCC_CTX_STORE_U64_OR(...)` and `DCC_CTX_STORE_SET_U64(...)` to keep a\n"
        "tiny persistent ping count without hand-written store plumbing.\n"
        "\n"
        "## Run\n"
        "\n"
        "```sh\n"
        "cmake -S . -B build -DCMAKE_PREFIX_PATH=\"$HOME/.local\"\n"
        "cmake --build build\n"
        "./build/%s\n"
        "```\n"
        "\n"
        "## Slash command sync\n"
        "\n"
        "Slash commands are synced automatically by `DCC_APP_DEV_MODE()`. The generated\n"
        "`commands.json` is only needed when you want to inspect or apply a command plan\n"
        "manually with the standalone CLI.\n"
        "\n"
        "```sh\n"
        "set -a\n"
        ". ./.env\n"
        "set +a\n"
        "DCC_APPLICATION_ID=your-application-id\n"
        "dcc_command_sync --commands commands.json --application-id \"$DCC_APPLICATION_ID\" --plan\n"
        "dcc_command_sync --commands commands.json --application-id \"$DCC_APPLICATION_ID\" --apply\n"
        "# or for development guild commands:\n"
        "dcc_command_sync --commands commands.json --application-id \"$DCC_APPLICATION_ID\" --guild \"$DCC_COMMAND_GUILD_ID\" --apply\n"
        "```\n",
        options->name,
        options->name
    );
    return fclose(file) == 0 ? 0 : -1;
}

int dcc_new_app_generate(const dcc_new_app_options_t *options) {
    if (options->mode == DCC_NEW_APP_MODE_ADD_COG) {
        return dcc_new_app_generate_cog(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_COMMAND) {
        return dcc_new_app_generate_command(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_SUBCOMMAND) {
        return dcc_new_app_generate_subcommand(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_BUTTON) {
        return dcc_new_app_generate_button(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_SELECT) {
        return dcc_new_app_generate_select(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_MODAL) {
        return dcc_new_app_generate_modal(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_AUTOCOMPLETE) {
        return dcc_new_app_generate_autocomplete(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_EVENT) {
        return dcc_new_app_generate_event(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_VIEW) {
        return dcc_new_app_generate_view(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_CONFIG) {
        return dcc_new_app_generate_config(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_ERROR_HANDLER) {
        return dcc_new_app_generate_error_handler(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_LATEST_MESSAGE) {
        return dcc_new_app_generate_latest_message(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_MESSAGE_COMMAND) {
        return dcc_new_app_generate_message_command(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_TASK) {
        return dcc_new_app_generate_task(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_ACTION) {
        return dcc_new_app_generate_action(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_CONTEXT_MENU) {
        return dcc_new_app_generate_context_menu(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_MIDDLEWARE) {
        return dcc_new_app_generate_middleware(options);
    }
    if (options->mode == DCC_NEW_APP_MODE_ADD_PRESET) {
        return dcc_new_app_generate_known_preset(options);
    }
    if (!options->force && dcc_new_app_path_exists(options->path)) {
        fprintf(stderr, "%s already exists; pass --force to write into it\n", options->path);
        return -1;
    }
    if (dcc_new_app_mkdirs(options->path) != 0) {
        return -1;
    }
    if (dcc_new_app_write_cmake(options) != 0 ||
        dcc_new_app_write_config_header(options) != 0 ||
        dcc_new_app_write_main(options) != 0 ||
        dcc_new_app_write_default_cogs(options) != 0) {
        return -1;
    }
    if (dcc_new_app_write_literal(
            options,
            ".env.example",
            "DCC_TOKEN=replace-me\n"
            "DCC_INTENTS=default\n"
            "DCC_AUTO_DEFER_MS=1500\n"
            "DCC_AUTO_DEFER_EPHEMERAL=true\n"
            "BOT_DEBUG=false\n"
            "DCC_STORE_FILE=bot-state.kv\n"
            "# DCC_COMMAND_GUILD_ID=123456789012345678\n"
            "# dcc_new_app:config-env\n"
        ) != 0) {
        return -1;
    }
    if (dcc_new_app_write_literal(
            options,
            ".gitignore",
            "build/\n"
            ".env\n"
            "bot-state.kv\n"
            "*.o\n"
            "*.a\n"
            "*.so\n"
            "*.dylib\n"
            "*.dll\n"
            "*.exe\n"
            "compile_commands.json\n"
        ) != 0) {
        return -1;
    }
    if (dcc_new_app_write_literal(
            options,
            "commands.json",
            "[\n"
            "  {\n"
            "    \"name\": \"ping\",\n"
            "    \"description\": \"Reply with pong\",\n"
            "    \"type\": 1\n"
            "  }\n"
            "]\n"
        ) != 0) {
        return -1;
    }
    if (dcc_new_app_write_readme(options) != 0) {
        return -1;
    }
    printf("created DCC app %s in %s\n", options->name, options->path);
    return 0;
}
