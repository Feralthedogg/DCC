#ifndef DCC_REPLAY_H
#define DCC_REPLAY_H

#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/rest/base.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_replay_kind {
    DCC_REPLAY_GATEWAY = 0,
    DCC_REPLAY_INTERACTION = 1
} dcc_replay_kind_t;

typedef struct dcc_replay_record {
    size_t size;
    dcc_replay_kind_t kind;
    uint64_t ts_ms;
    const char *event;
    const char *payload;
    size_t payload_len;
} dcc_replay_record_t;

typedef struct dcc_replay_recorder {
    size_t size;
    void *state;
} dcc_replay_recorder_t;

typedef struct dcc_replay_player {
    size_t size;
    void *state;
} dcc_replay_player_t;

typedef struct dcc_replay_validation_result {
    size_t size;
    uint64_t records;
    uint64_t gateway_records;
    uint64_t interaction_records;
    uint64_t first_ts_ms;
    uint64_t last_ts_ms;
    uint8_t timestamps_monotonic;
    uint8_t reserved[7];
} dcc_replay_validation_result_t;

typedef dcc_status_t (*dcc_replay_event_cb)(
    dcc_client_t *client,
    const dcc_replay_record_t *record,
    void *user_data
);

DCC_API const char *dcc_replay_kind_string(dcc_replay_kind_t kind);
DCC_API dcc_status_t dcc_replay_kind_from_string(const char *text, dcc_replay_kind_t *out);

DCC_API dcc_status_t dcc_replay_recorder_open(
    const char *path,
    dcc_replay_recorder_t *out
);
DCC_API void dcc_replay_recorder_close(dcc_replay_recorder_t *recorder);
DCC_API dcc_status_t dcc_replay_recorder_write(
    dcc_replay_recorder_t *recorder,
    const dcc_replay_record_t *record
);
DCC_API dcc_status_t dcc_replay_recorder_capture_gateway(
    dcc_replay_recorder_t *recorder,
    const char *event,
    uint64_t ts_ms,
    const char *payload,
    size_t payload_len
);
DCC_API dcc_status_t dcc_replay_recorder_capture_interaction(
    dcc_replay_recorder_t *recorder,
    uint64_t ts_ms,
    const char *payload,
    size_t payload_len
);

DCC_API dcc_status_t dcc_replay_player_open(
    const char *path,
    dcc_replay_player_t *out
);
DCC_API void dcc_replay_player_close(dcc_replay_player_t *player);
DCC_API dcc_status_t dcc_replay_player_next(
    dcc_replay_player_t *player,
    dcc_replay_record_t *out
);
DCC_API dcc_status_t dcc_replay_player_run(
    dcc_replay_player_t *player,
    dcc_client_t *client,
    dcc_replay_event_cb cb,
    void *user_data
);
DCC_API void dcc_replay_record_deinit(dcc_replay_record_t *record);
DCC_API dcc_status_t dcc_replay_validate_file(
    const char *path,
    dcc_replay_validation_result_t *out
);

#ifdef __cplusplus
}
#endif

#endif
