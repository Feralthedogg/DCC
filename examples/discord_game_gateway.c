#include "discord_game_internal.h"

dcc_status_t on_ready(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data) {
    (void)app;
    (void)user_data;
    printf("Discord game bot ready: shard %u/%u\n", ready->shard_id, ready->shard_count);
    return DCC_OK;
}

dcc_status_t on_log(dcc_app_t *app, const dcc_event_t *event, void *user_data) {
    (void)app;
    (void)user_data;
    const dcc_log_event_t *log = dcc_event_log(event);
    if (log != NULL) {
        fprintf(stderr, "[dcc:%d] %s\n", (int)log->level, log->message);
    }
    return DCC_OK;
}

dcc_status_t on_socket_close(dcc_app_t *app, const dcc_event_t *event, void *user_data) {
    (void)app;
    (void)user_data;
    const dcc_socket_close_event_t *close_ev = dcc_event_socket_close(event);
    if (close_ev != NULL) {
        printf("SOCKET CLOSED! Code: %u, Reason: %s\n", close_ev->code, close_ev->reason ? close_ev->reason : "null");
    }
    return DCC_OK;
}
