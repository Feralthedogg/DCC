#include "internal/interactions/dcc_interaction_server_internal.h"

#include <llam/io.h>

dcc_status_t dcc_interaction_server_stop(dcc_interaction_server_t *server) {
    if (server == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    atomic_store_explicit(&server->stopping, true, memory_order_release);
    dcc_interaction_server_close_listener(server);
    return dcc_runtime_request_stop(&server->runtime);
}

dcc_status_t dcc_interaction_server_drain(
    dcc_interaction_server_t *server,
    uint32_t timeout_ms,
    dcc_interaction_server_state_t *out
) {
    if (server == NULL || (out != NULL && out->size < sizeof(*out))) {
        return DCC_ERR_INVALID_ARG;
    }

    atomic_store_explicit(&server->stopping, true, memory_order_release);
    dcc_interaction_server_close_listener(server);

    uint64_t deadline = UINT64_MAX;
    if (timeout_ms != 0U) {
        uint64_t add = (uint64_t)timeout_ms * UINT64_C(1000000);
        uint64_t now = llam_now_ns();
        deadline = UINT64_MAX - now < add ? UINT64_MAX : now + add;
    }

    for (;;) {
        if (atomic_load_explicit(&server->active_requests, memory_order_acquire) == 0U) {
            dcc_status_t status = dcc_runtime_request_stop(&server->runtime);
            if (out != NULL) {
                dcc_interaction_server_fill_state(server, out);
            }
            return status;
        }
        if (timeout_ms != 0U && llam_now_ns() >= deadline) {
            if (out != NULL) {
                dcc_interaction_server_fill_state(server, out);
            }
            return DCC_ERR_TIMEOUT;
        }
        (void)llam_sleep_ns(UINT64_C(1000000));
    }
}

dcc_status_t dcc_interaction_server_wait(dcc_interaction_server_t *server) {
    if (server == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!atomic_load_explicit(&server->started, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }
    dcc_status_t st = dcc_runtime_run(&server->runtime);
    atomic_store_explicit(&server->started, false, memory_order_release);
    return st == DCC_ERR_CANCELED ? DCC_OK : st;
}
