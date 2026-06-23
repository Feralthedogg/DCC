#include "internal/dcc_new_app.h"

void dcc_new_app_usage(FILE *stream, const char *argv0) {
    fprintf(
        stream,
        "usage:\n"
        "  %s PATH [options]\n"
        "  %s add feature PATH FEATURE_NAME [options]\n"
        "  %s add command PATH FEATURE_NAME COMMAND_NAME [options]\n"
        "  %s add subcommand PATH FEATURE_NAME COMMAND_NAME SUBCOMMAND_NAME [options]\n"
        "  %s add button PATH FEATURE_NAME BUTTON_NAME [options]\n"
        "  %s add select PATH FEATURE_NAME SELECT_NAME [options]\n"
        "  %s add modal PATH FEATURE_NAME MODAL_NAME [options]\n"
        "  %s add autocomplete PATH FEATURE_NAME COMMAND_NAME [options]\n"
        "  %s add event PATH FEATURE_NAME EVENT_NAME [options]\n"
        "  %s add view PATH FEATURE_NAME VIEW_NAME [options]\n"
        "  %s add config PATH FIELD_NAME string|bool|u64|i64|channel|role|guild|snowflake ENV_NAME [options]\n"
        "  %s add error-handler PATH ERROR_HANDLER_NAME [friendly|simple|verbose] [options]\n"
        "  %s add latest-message PATH FEATURE_NAME COMMAND_NAME CHANNEL_FIELD [options]\n"
        "  %s add scheduled-latest PATH FEATURE_NAME TASK_NAME CHANNEL_FIELD [options]\n"
        "  %s add message-command PATH FEATURE_NAME COMMAND_NAME [options]\n"
        "  %s add task PATH FEATURE_NAME TASK_NAME [options]\n"
        "  %s add action PATH FEATURE_NAME ACTION_NAME send|gateway|dm|role-add|role-remove|voice-regions [options]\n"
        "  %s add context-menu PATH FEATURE_NAME CONTEXT_MENU_NAME user|message [options]\n"
        "  %s add middleware PATH FEATURE_NAME MIDDLEWARE_NAME [pass|config|guild|dm] [options]\n"
        "  %s add preset PATH FEATURE_NAME announcement|confirm|paginator|form|settings|wizard|counter|crud|help|menu|poll|profile|roles|ticket|welcome|flow [options]\n"
        "\n"
        "options:\n"
        "  -n, --name NAME     override generated C identifier/project name where supported\n"
        "  -f, --force         overwrite generated files if they already exist\n"
        "  --prefix PREFIX     prefix for add message-command; default: !\n"
        "  --every-ms N        schedule add task every N milliseconds\n"
        "  --every-seconds N   schedule add task every N seconds; default: 60\n"
        "  --every-minutes N   schedule add task every N minutes\n"
        "  --every-hours N     schedule add task every N hours\n"
        "  --daily-kst HH:MM   schedule add task once per day in KST\n"
        "  --guild-only        add an interaction route guard for guild usage\n"
        "  --dm-only           add an interaction route guard for DM usage\n"
        "  --owner USER_ID     guard an interaction route to one owner\n"
        "  --role ROLE_ID      require one role on an interaction route\n"
        "  --any-role LIST     require any role; example: 1ULL,2ULL\n"
        "  --permission EXPR   require permissions; example: DCC_PERMISSION_MANAGE_GUILD\n"
        "  --cooldown-user L:M add per-user cooldown with limit L and window M milliseconds\n"
        "  -h, --help          show this help\n"
        "\n"
        "aliases:\n"
        "  add cog and add module are accepted as compatibility aliases for add feature\n"
        "\n"
        "generated files:\n"
        "  app: CMakeLists.txt, src/main.c, src/config.h, src/ping.c, src/ping.h, commands.json,\n"
        "       .env.example, .gitignore, README.md\n"
        "  feature: src/FEATURE_NAME.c, src/FEATURE_NAME.h, plus generated app markers when present\n"
        "  command: appends a handler and slash route to src/FEATURE_NAME.c when markers exist\n"
        "  config: appends typed config to src/config.h, src/main.c, and .env.example\n"
        "  error-handler: appends a friendly/simple/verbose error handler and registration\n"
        "  latest-message/scheduled-latest: publish store-backed latest messages\n"
        "  action: append a ready-to-edit REST/action shortcut command\n"
        "  context-menu: append a user or message app context menu route\n"
        "  middleware: append a feature middleware function and registration\n"
        "  subcommand/button/select/modal/autocomplete/event/view/message-command/task/preset: append handlers and feature routes\n",
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0,
        argv0
    );
}
