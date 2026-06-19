#include "internal/replay/dcc_replay_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_replay_recorder_open(
    const char *path,
    dcc_replay_recorder_t *out
) {
    if (path == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));
    dcc_replay_recorder_state_t *state =
        (dcc_replay_recorder_state_t *)calloc(1, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->file = fopen(path, "ab");
    if (state->file == NULL) {
        free(state);
        return DCC_ERR_RUNTIME;
    }
    out->size = sizeof(*out);
    out->state = state;
    return DCC_OK;
}

void dcc_replay_recorder_close(dcc_replay_recorder_t *recorder) {
    if (recorder == NULL) {
        return;
    }
    dcc_replay_recorder_state_t *state =
        (dcc_replay_recorder_state_t *)recorder->state;
    if (state != NULL) {
        if (state->file != NULL) {
            (void)fclose(state->file);
        }
        free(state);
    }
    memset(recorder, 0, sizeof(*recorder));
}

dcc_status_t dcc_replay_recorder_write(
    dcc_replay_recorder_t *recorder,
    const dcc_replay_record_t *record
) {
    dcc_replay_recorder_state_t *state =
        recorder != NULL ? (dcc_replay_recorder_state_t *)recorder->state : NULL;
    const char *kind = record != NULL ? dcc_replay_kind_string(record->kind) : NULL;
    if (state == NULL || state->file == NULL || record == NULL || kind == NULL ||
        !dcc_replay_payload_shape_ok(record->payload, record->payload_len) ||
        (record->kind == DCC_REPLAY_GATEWAY && (record->event == NULL || record->event[0] == '\0'))) {
        return DCC_ERR_INVALID_ARG;
    }

    if (fputs("{\"kind\":\"", state->file) < 0 ||
        fputs(kind, state->file) < 0 ||
        fprintf(state->file, "\",\"ts\":%llu", (unsigned long long)record->ts_ms) < 0) {
        return DCC_ERR_RUNTIME;
    }
    if (record->event != NULL) {
        if (fputs(",\"event\":\"", state->file) < 0 ||
            dcc_replay_json_escape(state->file, record->event) != DCC_OK ||
            fputc('"', state->file) == EOF) {
            return DCC_ERR_RUNTIME;
        }
    }
    if (fputs(",\"payload\":", state->file) < 0 ||
        fwrite(record->payload, 1U, record->payload_len, state->file) != record->payload_len ||
        fputs("}\n", state->file) < 0 ||
        fflush(state->file) != 0) {
        return DCC_ERR_RUNTIME;
    }
    return DCC_OK;
}

dcc_status_t dcc_replay_recorder_capture_gateway(
    dcc_replay_recorder_t *recorder,
    const char *event,
    uint64_t ts_ms,
    const char *payload,
    size_t payload_len
) {
    dcc_replay_record_t record = {
        .size = sizeof(record),
        .kind = DCC_REPLAY_GATEWAY,
        .ts_ms = ts_ms,
        .event = event,
        .payload = payload,
        .payload_len = payload_len,
    };
    return dcc_replay_recorder_write(recorder, &record);
}

dcc_status_t dcc_replay_recorder_capture_interaction(
    dcc_replay_recorder_t *recorder,
    uint64_t ts_ms,
    const char *payload,
    size_t payload_len
) {
    dcc_replay_record_t record = {
        .size = sizeof(record),
        .kind = DCC_REPLAY_INTERACTION,
        .ts_ms = ts_ms,
        .payload = payload,
        .payload_len = payload_len,
    };
    return dcc_replay_recorder_write(recorder, &record);
}
