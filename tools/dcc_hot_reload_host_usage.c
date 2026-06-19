#include "internal/dcc_hot_reload_host.h"

void dcc_hot_reload_host_usage(FILE *stream, const char *argv0) {
    fprintf(
        stream,
        "usage: BOT_TOKEN=... %s [options] [module.so] [worker]\n"
        "\n"
        "options:\n"
        "  -m, --module PATH       bot module path; defaults to DCC_BOT_MODULE\n"
        "  -w, --worker PATH       worker path/name; defaults to DCC_HOT_RELOAD_WORKER or %s\n"
        "  -i, --intents VALUE     numeric intents or comma-separated names\n"
        "  -g, --guild ID          set DCC_HOT_RELOAD_GUILD_ID for guild-scoped commands\n"
        "      --global            clear DCC_HOT_RELOAD_GUILD_ID and register globally\n"
        "      --health-address IP bind health sidecar address; default is 127.0.0.1\n"
        "      --health-port PORT  enable unsigned GET /hot-reloadz health sidecar\n"
        "      --poll-ms MS        module file poll interval; default is 250\n"
        "      --settle-ms MS      module file settle interval; use none for immediate reload\n"
        "      --worker-health-ms MS worker IPC/health timeout; default is 2000\n"
        "      --worker-drain-ms MS  retired worker drain timeout; default is 500\n"
        "      --check             validate resolved config without connecting\n"
        "  -h, --help              show this help\n"
        "\n"
        "environment:\n"
        "  BOT_TOKEN               Discord bot token; DISCORD_TOKEN is accepted as fallback\n"
        "  DCC_BOT_MODULE          bot module path\n"
        "  DCC_HOT_RELOAD_WORKER   worker executable override\n"
        "  DCC_HOT_RELOAD_INTENTS  same format as --intents; default is guilds\n"
        "  DCC_HOT_RELOAD_GUILD_ID optional guild snowflake for modules that use guild scoping\n"
        "  DCC_HOT_RELOAD_HEALTH_ADDRESS health bind address; default is 127.0.0.1\n"
        "  DCC_HOT_RELOAD_HEALTH_PORT enable unsigned GET /hot-reloadz health sidecar\n"
        "  DCC_HOT_RELOAD_POLL_MS module file poll interval; default is 250\n"
        "  DCC_HOT_RELOAD_SETTLE_MS module settle interval; use none for immediate reload\n"
        "  DCC_HOT_RELOAD_WORKER_HEALTH_MS worker IPC/health timeout; default is 2000\n"
        "  DCC_HOT_RELOAD_WORKER_DRAIN_MS retired worker drain timeout; default is 500\n"
        "\n"
        "dotenv:\n"
        "  .env in the current directory is loaded without overriding exported values\n"
        "\n"
        "intent example:\n"
        "  DCC_HOT_RELOAD_INTENTS=guilds,guild_messages,message_content\n",
        argv0,
        DCC_HOT_RELOAD_DEFAULT_WORKER
    );
}
