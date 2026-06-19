#include "internal/replay/dcc_replay_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_replay_player_open(
    const char *path,
    dcc_replay_player_t *out
) {
    if (path == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));
    dcc_replay_player_state_t *state =
        (dcc_replay_player_state_t *)calloc(1, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->file = fopen(path, "rb");
    if (state->file == NULL) {
        free(state);
        return DCC_ERR_RUNTIME;
    }
    out->size = sizeof(*out);
    out->state = state;
    return DCC_OK;
}

void dcc_replay_player_close(dcc_replay_player_t *player) {
    if (player == NULL) {
        return;
    }
    dcc_replay_player_state_t *state =
        (dcc_replay_player_state_t *)player->state;
    if (state != NULL) {
        if (state->file != NULL) {
            (void)fclose(state->file);
        }
        free(state);
    }
    memset(player, 0, sizeof(*player));
}

dcc_status_t dcc_replay_player_next(
    dcc_replay_player_t *player,
    dcc_replay_record_t *out
) {
    dcc_replay_player_state_t *state =
        player != NULL ? (dcc_replay_player_state_t *)player->state : NULL;
    if (state == NULL || state->file == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    char *line = NULL;
    size_t line_len = 0;
    dcc_status_t status = dcc_replay_read_line(state->file, &line, &line_len);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_replay_parse_line(line, line_len, out);
    free(line);
    return status;
}

dcc_status_t dcc_replay_player_run(
    dcc_replay_player_t *player,
    dcc_client_t *client,
    dcc_replay_event_cb cb,
    void *user_data
) {
    if (player == NULL || cb == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    for (;;) {
        dcc_replay_record_t record;
        dcc_status_t status = dcc_replay_player_next(player, &record);
        if (status == DCC_ERR_CANCELED) {
            return DCC_OK;
        }
        if (status != DCC_OK) {
            return status;
        }
        status = cb(client, &record, user_data);
        dcc_replay_record_deinit(&record);
        if (status != DCC_OK) {
            return status;
        }
    }
}

void dcc_replay_record_deinit(dcc_replay_record_t *record) {
    if (record == NULL) {
        return;
    }
    free((char *)record->event);
    free((char *)record->payload);
    memset(record, 0, sizeof(*record));
}
