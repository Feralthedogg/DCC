#include "internal/dcc_replay_tool.h"

void dcc_replay_tool_stats_record(
    dcc_replay_tool_stats_t *stats,
    const dcc_replay_record_t *record
) {
    if (stats == NULL || record == NULL) {
        return;
    }
    if (!stats->has_first_ts) {
        stats->first_ts_ms = record->ts_ms;
        stats->has_first_ts = 1;
    }
    stats->last_ts_ms = record->ts_ms;
    stats->records++;
    if (record->kind == DCC_REPLAY_GATEWAY) {
        stats->gateway_records++;
    } else if (record->kind == DCC_REPLAY_INTERACTION) {
        stats->interaction_records++;
    }
}

void dcc_replay_tool_stats_print(
    const dcc_replay_tool_options_t *options,
    const dcc_replay_tool_stats_t *stats
) {
    fprintf(
        stdout,
        "replay file=%s records=%zu gateway=%zu interactions=%zu first_ts=%llu last_ts=%llu\n",
        options != NULL && options->file_path != NULL ? options->file_path : "",
        stats != NULL ? stats->records : 0U,
        stats != NULL ? stats->gateway_records : 0U,
        stats != NULL ? stats->interaction_records : 0U,
        (unsigned long long)(stats != NULL ? stats->first_ts_ms : 0U),
        (unsigned long long)(stats != NULL ? stats->last_ts_ms : 0U)
    );
}
