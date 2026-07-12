#include <dcc/replay.h>

#include <string.h>

dcc_status_t dcc_replay_validate_file(
    const char *path,
    dcc_replay_validation_result_t *out
) {
    if (path == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_replay_validation_result_t result;
    memset(&result, 0, sizeof(result));
    result.size = sizeof(result);
    result.timestamps_monotonic = 1U;

    dcc_replay_player_t player;
    dcc_status_t status = dcc_replay_player_open(path, &player);
    if (status != DCC_OK) return status;
    for (;;) {
        dcc_replay_record_t record;
        status = dcc_replay_player_next(&player, &record);
        if (status == DCC_ERR_CANCELED) {
            status = DCC_OK;
            break;
        }
        if (status != DCC_OK) break;
        if (result.records == 0U) {
            result.first_ts_ms = record.ts_ms;
        } else if (record.ts_ms < result.last_ts_ms) {
            result.timestamps_monotonic = 0U;
        }
        result.last_ts_ms = record.ts_ms;
        result.records++;
        if (record.kind == DCC_REPLAY_GATEWAY) result.gateway_records++;
        if (record.kind == DCC_REPLAY_INTERACTION) result.interaction_records++;
        dcc_replay_record_deinit(&record);
    }
    dcc_replay_player_close(&player);
    *out = result;
    if (status == DCC_OK && result.timestamps_monotonic == 0U) return DCC_ERR_STATE;
    return status;
}
