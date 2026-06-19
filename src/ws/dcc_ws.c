#include "internal/ws/dcc_ws.h"

#include <llam/runtime.h>

#include <limits.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

static void dcc_ws_wait_send_idle(dcc_ws_t *ws) {
    while (atomic_flag_test_and_set_explicit(&ws->send_lock, memory_order_acquire)) {
        llam_yield();
    }
}

dcc_status_t dcc_ws_retain(dcc_ws_t *ws) {
    if (ws == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    unsigned int refs = atomic_load_explicit(&ws->ref_count, memory_order_relaxed);
    for (;;) {
        if (refs == 0U || refs == UINT_MAX) {
            return DCC_ERR_STATE;
        }
        if (atomic_compare_exchange_weak_explicit(
                &ws->ref_count,
                &refs,
                refs + 1U,
                memory_order_acquire,
                memory_order_relaxed
            )) {
            return DCC_OK;
        }
    }
}
void dcc_ws_message_deinit(dcc_ws_message_t *message) {
    if (message == NULL) {
        return;
    }
    free(message->data);
    memset(message, 0, sizeof(*message));
}
intptr_t dcc_ws_fd(const dcc_ws_t *ws) {
    return ws != NULL ? (intptr_t)ws->conn.fd : (intptr_t)-1;
}
uint16_t dcc_ws_last_close_code(const dcc_ws_t *ws) {
    return ws != NULL ? ws->close_code : 0;
}
const char *dcc_ws_last_close_reason(const dcc_ws_t *ws) {
    return ws != NULL ? ws->close_reason : "";
}
void dcc_ws_abort(dcc_ws_t *ws) {
    if (ws == NULL) {
        return;
    }
    if (atomic_exchange_explicit(&ws->open, 0, memory_order_acq_rel) != 0) {
        dcc_conn_close(&ws->conn);
    }
}
void dcc_ws_close(dcc_ws_t *ws, uint16_t code, const char *reason) {
    if (ws == NULL || atomic_load_explicit(&ws->open, memory_order_acquire) == 0) {
        return;
    }

    unsigned char payload[128];
    size_t reason_len = reason != NULL ? strlen(reason) : 0;
    if (reason_len > sizeof(payload) - 2) {
        reason_len = sizeof(payload) - 2;
    }
    dcc_ws_write_u16(payload, code);
    if (reason_len > 0) {
        memcpy(payload + 2, reason, reason_len);
    }
    (void)dcc_ws_send(ws, DCC_WS_CLOSE, payload, reason_len + 2);
    atomic_store_explicit(&ws->open, 0, memory_order_release);
}
void dcc_ws_destroy(dcc_ws_t *ws) {
    if (ws == NULL) {
        return;
    }
    if (atomic_fetch_sub_explicit(&ws->ref_count, 1U, memory_order_acq_rel) != 1U) {
        return;
    }
    atomic_store_explicit(&ws->open, 0, memory_order_release);
    dcc_ws_wait_send_idle(ws);
    dcc_conn_close(&ws->conn);
    free(ws->rx_buf);
    free(ws->host);
    free(ws->path);
    free(ws);
}
