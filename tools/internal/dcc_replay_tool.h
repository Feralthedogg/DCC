#ifndef DCC_REPLAY_TOOL_INTERNAL_H
#define DCC_REPLAY_TOOL_INTERNAL_H

#include <dcc/dcc.h>

#include "internal/gateway/dcc_gateway_session_receive_internal.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct dcc_replay_tool_options {
    const char *file_path;
    const char *module_path;
    const char *worker_path;
    const char *token;
    uint32_t worker_health_timeout_ms;
    uint32_t worker_drain_timeout_ms;
    uint32_t max_gap_ms;
    uint64_t max_records;
    uint64_t expect_records;
    uint64_t expect_gateway;
    uint64_t expect_interactions;
    int summary;
    int validate;
    int realtime;
    int isolated;
    int in_process;
    int allow_nonmonotonic;
    int has_expect_records;
    int has_expect_gateway;
    int has_expect_interactions;
} dcc_replay_tool_options_t;

typedef struct dcc_replay_tool_stats {
    size_t records;
    size_t gateway_records;
    size_t interaction_records;
    uint64_t first_ts_ms;
    uint64_t last_ts_ms;
    int has_first_ts;
} dcc_replay_tool_stats_t;

typedef struct dcc_replay_tool_runtime {
    dcc_client_t *client;
    dcc_hot_reload_t *hot_reload;
    dcc_gateway_session_t gateway_session;
} dcc_replay_tool_runtime_t;

void dcc_replay_tool_usage(FILE *stream, const char *argv0);
int dcc_replay_tool_parse_options(
    int argc,
    char **argv,
    dcc_replay_tool_options_t *options
);
int dcc_replay_tool_validate_options(const dcc_replay_tool_options_t *options);
int dcc_replay_tool_parse_u32(const char *text, uint32_t *out);

void dcc_replay_tool_stats_record(
    dcc_replay_tool_stats_t *stats,
    const dcc_replay_record_t *record
);
void dcc_replay_tool_stats_print(
    const dcc_replay_tool_options_t *options,
    const dcc_replay_tool_stats_t *stats
);

dcc_status_t dcc_replay_tool_frame_for_record(
    const dcc_replay_record_t *record,
    char **out_json,
    size_t *out_len
);
dcc_status_t dcc_replay_tool_runtime_init(
    const dcc_replay_tool_options_t *options,
    dcc_replay_tool_runtime_t *runtime
);
void dcc_replay_tool_runtime_deinit(dcc_replay_tool_runtime_t *runtime);
dcc_status_t dcc_replay_tool_dispatch_frame(
    dcc_replay_tool_runtime_t *runtime,
    const char *json,
    size_t json_len
);
dcc_status_t dcc_replay_tool_run(const dcc_replay_tool_options_t *options);

#endif
