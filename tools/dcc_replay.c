#include "internal/dcc_replay_tool.h"

int main(int argc, char **argv) {
    dcc_replay_tool_options_t options;
    int parse_status = dcc_replay_tool_parse_options(argc, argv, &options);
    if (parse_status > 0) {
        return 0;
    }
    if (parse_status < 0 || dcc_replay_tool_validate_options(&options) != 0) {
        dcc_replay_tool_usage(stderr, argv[0]);
        return 2;
    }

    dcc_status_t status = dcc_replay_tool_run(&options);
    if (status != DCC_OK) {
        fprintf(stderr, "replay failed: %s\n", dcc_status_string(status));
        return 1;
    }
    return 0;
}
