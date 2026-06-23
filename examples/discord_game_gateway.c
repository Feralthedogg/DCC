#include "discord_game_internal.h"

DCC_PUBLIC_READY_FN(on_ready) {
    (void)app;
    (void)user_data;
    printf("Discord game bot ready: shard %u/%u\n", ready->shard_id, ready->shard_count);
}

DCC_PUBLIC_EVENT_FN(on_log) {
    (void)app;
    (void)user_data;
    const dcc_log_event_t *log = dcc_event_log(event);
    if (log != NULL) {
        fprintf(stderr, "[dcc:%d] %s\n", (int)log->level, log->message);
    }
}

DCC_PUBLIC_EVENT_FN(on_socket_close) {
    (void)app;
    (void)user_data;
    const dcc_socket_close_event_t *close_ev = dcc_event_socket_close(event);
    if (close_ev != NULL) {
        printf("SOCKET CLOSED! Code: %u, Reason: %s\n", close_ev->code, close_ev->reason ? close_ev->reason : "null");
    }
}
