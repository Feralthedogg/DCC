#ifndef DCC_REPLAY_INTERNAL_H
#define DCC_REPLAY_INTERNAL_H

#include <dcc/replay.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_replay_recorder_state {
    FILE *file;
} dcc_replay_recorder_state_t;

typedef struct dcc_replay_player_state {
    FILE *file;
} dcc_replay_player_state_t;

#define DCC_REPLAY_MAX_LINE_SIZE (8U * 1024U * 1024U)

dcc_status_t dcc_replay_json_escape(FILE *file, const char *text);
dcc_status_t dcc_replay_read_line(FILE *file, char **out_line, size_t *out_len);
dcc_status_t dcc_replay_parse_line(const char *line, size_t line_len, dcc_replay_record_t *out);
int dcc_replay_payload_shape_ok(const char *payload, size_t payload_len);

#ifdef __cplusplus
}
#endif

#endif
