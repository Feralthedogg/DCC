#include <dcc/dcc.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct replay_seen {
    int count;
    uint64_t ts_sum;
} replay_seen_t;

static dcc_status_t replay_cb(
    dcc_client_t *client,
    const dcc_replay_record_t *record,
    void *user_data
) {
    (void)client;
    replay_seen_t *seen = (replay_seen_t *)user_data;
    if (seen == NULL || record == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (seen->count == 0 &&
        (record->kind != DCC_REPLAY_GATEWAY ||
         record->event == NULL ||
         strcmp(record->event, "MESSAGE_CREATE") != 0 ||
         strcmp(record->payload, "{\"id\":\"1\",\"content\":\"hi\"}") != 0)) {
        return DCC_ERR_STATE;
    }
    if (seen->count == 1 &&
        (record->kind != DCC_REPLAY_INTERACTION ||
         record->event != NULL ||
         strcmp(record->payload, "{\"id\":\"2\",\"type\":2}") != 0)) {
        return DCC_ERR_STATE;
    }
    seen->count++;
    seen->ts_sum += record->ts_ms;
    return DCC_OK;
}

static int make_temp_path(char *path, size_t path_len) {
    if (path_len < 32U) {
        return 0;
    }
    snprintf(path, path_len, "/tmp/dcc_replay_smoke_XXXXXX");
    int fd = mkstemp(path);
    if (fd < 0) {
        return 0;
    }
    close(fd);
    return 1;
}

int main(void) {
    char path[128];
    if (!make_temp_path(path, sizeof(path))) {
        fprintf(stderr, "failed to allocate replay temp path\n");
        return 1;
    }

    dcc_replay_recorder_t recorder;
    const char gateway_payload[] = "{\"id\":\"1\",\"content\":\"hi\"}";
    const char interaction_payload[] = "{\"id\":\"2\",\"type\":2}";
    if (dcc_replay_recorder_open(path, &recorder) != DCC_OK ||
        dcc_replay_recorder_capture_gateway(
            &recorder,
            "MESSAGE_CREATE",
            10U,
            gateway_payload,
            sizeof(gateway_payload) - 1U
        ) != DCC_OK ||
        dcc_replay_recorder_capture_interaction(
            &recorder,
            11U,
            interaction_payload,
            sizeof(interaction_payload) - 1U
        ) != DCC_OK ||
        dcc_replay_recorder_capture_interaction(&recorder, 12U, "not-json", 8U) != DCC_ERR_INVALID_ARG ||
        dcc_replay_recorder_capture_interaction(&recorder, 12U, "{not-json}", 10U) != DCC_ERR_INVALID_ARG ||
        dcc_replay_recorder_capture_interaction(&recorder, 12U, "{} trailing", 11U) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "failed to write replay smoke file\n");
        dcc_replay_recorder_close(&recorder);
        unlink(path);
        return 1;
    }
    dcc_replay_recorder_close(&recorder);

    dcc_replay_player_t player;
    dcc_replay_record_t record;
    if (dcc_replay_player_open(path, &player) != DCC_OK ||
        dcc_replay_player_next(&player, &record) != DCC_OK ||
        record.kind != DCC_REPLAY_GATEWAY ||
        record.ts_ms != 10U ||
        record.event == NULL ||
        strcmp(record.event, "MESSAGE_CREATE") != 0 ||
        strcmp(record.payload, gateway_payload) != 0) {
        fprintf(stderr, "failed to read first replay record\n");
        dcc_replay_player_close(&player);
        unlink(path);
        return 1;
    }
    dcc_replay_record_deinit(&record);

    if (dcc_replay_player_next(&player, &record) != DCC_OK ||
        record.kind != DCC_REPLAY_INTERACTION ||
        record.ts_ms != 11U ||
        record.event != NULL ||
        strcmp(record.payload, interaction_payload) != 0) {
        fprintf(stderr, "failed to read second replay record\n");
        dcc_replay_player_close(&player);
        unlink(path);
        return 1;
    }
    dcc_replay_record_deinit(&record);
    if (dcc_replay_player_next(&player, &record) != DCC_ERR_CANCELED) {
        fprintf(stderr, "expected replay EOF\n");
        dcc_replay_player_close(&player);
        unlink(path);
        return 1;
    }
    dcc_replay_player_close(&player);

    replay_seen_t seen = {0};
    if (dcc_replay_player_open(path, &player) != DCC_OK ||
        dcc_replay_player_run(&player, NULL, replay_cb, &seen) != DCC_OK ||
        seen.count != 2 ||
        seen.ts_sum != 21U) {
        fprintf(stderr, "failed to run replay callbacks\n");
        dcc_replay_player_close(&player);
        unlink(path);
        return 1;
    }
    dcc_replay_player_close(&player);

    dcc_replay_validation_result_t validation = {
        .size = sizeof(validation),
    };
    if (dcc_replay_validate_file(path, &validation) != DCC_OK ||
        validation.records != 2U ||
        validation.gateway_records != 1U ||
        validation.interaction_records != 1U ||
        validation.first_ts_ms != 10U ||
        validation.last_ts_ms != 11U ||
        validation.timestamps_monotonic != 1U) {
        fprintf(stderr, "failed replay validation result\n");
        unlink(path);
        return 1;
    }
    unlink(path);
    return 0;
}

#endif
