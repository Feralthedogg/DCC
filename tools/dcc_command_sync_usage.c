#include "internal/dcc_command_sync.h"

void dcc_command_sync_usage(FILE *stream, const char *argv0) {
    fprintf(
        stream,
        "usage: %s --commands commands.json [options]\n"
        "\n"
        "options:\n"
        "  -c, --commands FILE       local Discord application command JSON array\n"
        "      --remote FILE         remote command JSON array fixture; avoids REST fetch\n"
        "  -a, --application-id ID   Discord application id; env DCC_APPLICATION_ID is accepted\n"
        "  -g, --guild ID            reconcile a single guild command scope\n"
        "      --global              reconcile global commands; this is the default\n"
        "      --delete-stale        delete remote commands missing from local JSON\n"
        "      --plan                print the create/update/delete/noop plan; default action\n"
        "      --apply               apply the plan through Discord REST\n"
        "      --dry-run             force no REST writes even with --apply\n"
        "      --token TOKEN         bot token; BOT_TOKEN or DISCORD_TOKEN are accepted\n"
        "      --check               validate inputs and print resolved scope without writes\n"
        "  -h, --help                show this help\n"
        "\n"
        "notes:\n"
        "  The default scope is global. Use --guild only when you want guild-scoped commands.\n"
        "  Global command changes can take time to propagate across Discord.\n",
        argv0
    );
}
