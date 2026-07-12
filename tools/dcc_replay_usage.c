#include "internal/dcc_replay_tool.h"

void dcc_replay_tool_usage(FILE *stream, const char *argv0) {
    fprintf(
        stream,
        "usage: %s --file events.jsonl [options]\n"
        "\n"
        "options:\n"
        "  -f, --file FILE          replay JSONL file\n"
        "      --summary            print record counts and timestamp span; default without --module\n"
        "      --validate           parse the file and fail on malformed records\n"
        "      --expect-records N   fail unless exactly N records are parsed\n"
        "      --expect-gateway N   fail unless exactly N Gateway records are parsed\n"
        "      --expect-interactions N fail unless exactly N interaction records are parsed\n"
        "      --max-records N      resource cap; default 1000000\n"
        "  -m, --module PATH        hot-reload bot module to replay into\n"
        "  -w, --worker PATH        isolated worker executable; defaults to %s\n"
        "      --isolated           use isolated worker mode; default for --module\n"
        "      --in-process         run module in the replay process\n"
        "      --token TOKEN        REST token; DCC_TOKEN, BOT_TOKEN, or DISCORD_TOKEN are accepted\n"
        "      --worker-health-ms MS isolated worker health timeout; default 2000\n"
        "      --worker-drain-ms MS isolated worker drain timeout; default 500\n"
        "      --realtime           preserve captured timestamp gaps while replaying\n"
        "      --max-gap-ms MS      cap each realtime sleep; default 5000\n"
        "      --allow-nonmonotonic accept decreasing capture timestamps\n"
        "  -h, --help               show this help\n"
        "\n"
        "input:\n"
        "  Gateway records may store either a full gateway frame or a dispatch d object.\n"
        "  Interaction records are wrapped as INTERACTION_CREATE dispatch frames.\n",
        argv0,
        DCC_HOT_RELOAD_DEFAULT_WORKER
    );
}
