#include "internal/dcc_replay_tool.h"

#include <llam/runtime.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static void dcc_replay_tool_sleep_gap(
    uint64_t previous_ts,
    uint64_t current_ts,
    uint32_t max_gap_ms
) {
    if (current_ts <= previous_ts) {
        return;
    }
    uint64_t gap_ms = current_ts - previous_ts;
    if (gap_ms > max_gap_ms) gap_ms = max_gap_ms;
    if (gap_ms > UINT64_MAX / UINT64_C(1000000)) {
        return;
    }
    (void)llam_sleep_ns(gap_ms * UINT64_C(1000000));
}

static dcc_status_t dcc_replay_tool_check_expectations(
    const dcc_replay_tool_options_t *options,
    const dcc_replay_tool_stats_t *stats
) {
    if ((options->has_expect_records && stats->records != options->expect_records) ||
        (options->has_expect_gateway && stats->gateway_records != options->expect_gateway) ||
        (options->has_expect_interactions &&
            stats->interaction_records != options->expect_interactions)) {
        fprintf(
            stderr,
            "replay expectation failed: records=%zu gateway=%zu interactions=%zu\n",
            stats->records,
            stats->gateway_records,
            stats->interaction_records
        );
        return DCC_ERR_STATE;
    }
    return DCC_OK;
}

dcc_status_t dcc_replay_tool_run(const dcc_replay_tool_options_t *options) {
    dcc_replay_player_t player;
    dcc_replay_tool_runtime_t runtime;
    dcc_replay_tool_stats_t stats = {0};
    dcc_status_t status = dcc_replay_player_open(options->file_path, &player);
    int runtime_loaded = 0;
    uint64_t previous_ts = 0;

    memset(&runtime, 0, sizeof(runtime));
    if (status == DCC_OK && options->module_path != NULL) {
        status = dcc_replay_tool_runtime_init(options, &runtime);
        runtime_loaded = status == DCC_OK;
    }
    while (status == DCC_OK) {
        dcc_replay_record_t record;
        status = dcc_replay_player_next(&player, &record);
        if (status == DCC_ERR_CANCELED) {
            status = DCC_OK;
            break;
        }
        if (status != DCC_OK) {
            break;
        }
        if (stats.records >= options->max_records) {
            dcc_replay_record_deinit(&record);
            status = DCC_ERR_STATE;
            break;
        }
        if (!options->allow_nonmonotonic && stats.records != 0U && record.ts_ms < previous_ts) {
            dcc_replay_record_deinit(&record);
            status = DCC_ERR_STATE;
            break;
        }
        if (options->realtime && stats.records != 0U) {
            dcc_replay_tool_sleep_gap(previous_ts, record.ts_ms, options->max_gap_ms);
        }
        previous_ts = record.ts_ms;
        dcc_replay_tool_stats_record(&stats, &record);
        if (runtime_loaded) {
            char *frame = NULL;
            size_t frame_len = 0;
            status = dcc_replay_tool_frame_for_record(&record, &frame, &frame_len);
            if (status == DCC_OK) {
                status = dcc_replay_tool_dispatch_frame(&runtime, frame, frame_len);
            }
            free(frame);
        }
        dcc_replay_record_deinit(&record);
    }
    dcc_replay_player_close(&player);
    dcc_replay_tool_runtime_deinit(&runtime);
    if (status == DCC_OK) status = dcc_replay_tool_check_expectations(options, &stats);
    if (status == DCC_OK && (options->summary || options->validate)) {
        dcc_replay_tool_stats_print(options, &stats);
        if (options->validate) {
            fputs("validate=ok\n", stdout);
        }
    }
    return status;
}
