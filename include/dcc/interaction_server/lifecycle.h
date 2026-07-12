#ifndef DCC_INTERACTION_SERVER_LIFECYCLE_H
#define DCC_INTERACTION_SERVER_LIFECYCLE_H

#include <dcc/interaction_server/health.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_interaction_server_options {
    size_t size;
    const char *public_key_hex;
    const char *address;
    uint16_t port;
    uint32_t backlog;
    size_t max_header_size;
    size_t max_body_size;
    dcc_interaction_server_cb callback;
    void *user_data;
    uint64_t max_active_requests;
    uint32_t response_deadline_ms;
    uint32_t replay_window_ms;
} dcc_interaction_server_options_t;

DCC_API dcc_status_t dcc_interaction_server_create(
    const dcc_interaction_server_options_t *options,
    dcc_interaction_server_t **out
);
DCC_API void dcc_interaction_server_destroy(dcc_interaction_server_t *server);

DCC_API dcc_status_t dcc_interaction_server_start(dcc_interaction_server_t *server);
DCC_API dcc_status_t dcc_interaction_server_stop(dcc_interaction_server_t *server);
DCC_API dcc_status_t dcc_interaction_server_drain(
    dcc_interaction_server_t *server,
    uint32_t timeout_ms,
    dcc_interaction_server_state_t *out
);
DCC_API dcc_status_t dcc_interaction_server_wait_until_ready(
    dcc_interaction_server_t *server,
    uint32_t timeout_ms,
    dcc_interaction_server_health_snapshot_t *out
);
DCC_API dcc_status_t dcc_interaction_server_wait_until_drained(
    dcc_interaction_server_t *server,
    uint32_t timeout_ms,
    dcc_interaction_server_health_snapshot_t *out
);
DCC_API dcc_status_t dcc_interaction_server_wait(dcc_interaction_server_t *server);
DCC_API uint16_t dcc_interaction_server_port(const dcc_interaction_server_t *server);
DCC_API const char *dcc_interaction_server_last_error(const dcc_interaction_server_t *server);

#ifdef __cplusplus
}
#endif

#endif
