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
        "  -m, --module PATH        hot-reload bot module to replay into\n"
        "  -w, --worker PATH        isolated worker executable; defaults to %s\n"
        "      --isolated           use isolated worker mode; default for --module\n"
        "      --in-process         run module in the replay process\n"
        "      --token TOKEN        REST token for in-process module handlers\n"
        "      --worker-health-ms MS isolated worker health timeout; default 2000\n"
        "      --worker-drain-ms MS isolated worker drain timeout; default 500\n"
        "      --realtime           preserve captured timestamp gaps while replaying\n"
        "  -h, --help               show this help\n"
        "\n"
        "input:\n"
        "  Gateway records may store either a full gateway frame or a dispatch d object.\n"
        "  Interaction records are wrapped as INTERACTION_CREATE dispatch frames.\n",
        argv0,
        DCC_HOT_RELOAD_DEFAULT_WORKER
    );
}
