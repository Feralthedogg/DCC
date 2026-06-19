#include "internal/ws/dcc_ws.h"

#include <llam/runtime.h>
#include <openssl/rand.h>

#include <limits.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_ws_send_lock(dcc_ws_t *ws) {
    while (atomic_flag_test_and_set_explicit(&ws->send_lock, memory_order_acquire)) {
        if (atomic_load_explicit(&ws->open, memory_order_acquire) == 0) {
            return DCC_ERR_CANCELED;
        }
        llam_yield();
    }
    return DCC_OK;
}
static void dcc_ws_send_unlock(dcc_ws_t *ws) {
    atomic_flag_clear_explicit(&ws->send_lock, memory_order_release);
}
dcc_status_t dcc_ws_send(dcc_ws_t *ws, dcc_ws_opcode_t opcode, const void *data, size_t len) {
    if (ws == NULL || (data == NULL && len > 0)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t retain_status = dcc_ws_retain(ws);
    if (retain_status != DCC_OK) {
        return retain_status;
    }

    if (atomic_load_explicit(&ws->open, memory_order_acquire) == 0) {
        dcc_ws_destroy(ws);
        return DCC_ERR_INVALID_ARG;
    }

    size_t header_len = 2 + 4;
    if (len > UINT16_MAX) {
        header_len += 8;
    } else if (len >= 126U) {
        header_len += 2;
    }

    if (len > (size_t)-1 - header_len) {
        dcc_ws_destroy(ws);
        return DCC_ERR_NOMEM;
    }
    size_t frame_len = header_len + len;
    unsigned char *frame = (unsigned char *)malloc(frame_len);
    if (frame == NULL) {
        dcc_ws_destroy(ws);
        return DCC_ERR_NOMEM;
    }

    size_t pos = 0;
    frame[pos++] = (unsigned char)(0x80U | ((unsigned)opcode & 0x0fU));
    if (len < 126U) {
        frame[pos++] = (unsigned char)(0x80U | len);
    } else if (len <= UINT16_MAX) {
        frame[pos++] = 0x80U | 126U;
        dcc_ws_write_u16(frame + pos, (uint16_t)len);
        pos += 2;
    } else {
        frame[pos++] = 0x80U | 127U;
        dcc_ws_write_u64(frame + pos, (uint64_t)len);
        pos += 8;
    }

    unsigned char mask[4];
    if (RAND_bytes(mask, sizeof(mask)) != 1) {
        free(frame);
        dcc_ws_destroy(ws);
        return DCC_ERR_TLS;
    }
    memcpy(frame + pos, mask, sizeof(mask));
    pos += sizeof(mask);

    const unsigned char *payload = (const unsigned char *)data;
    for (size_t i = 0; i < len; ++i) {
        frame[pos + i] = (unsigned char)(payload[i] ^ mask[i % 4]);
    }

    dcc_status_t lock_status = dcc_ws_send_lock(ws);
    if (lock_status != DCC_OK) {
        free(frame);
        dcc_ws_destroy(ws);
        return lock_status;
    }
    int rc = atomic_load_explicit(&ws->open, memory_order_acquire) != 0
        ? dcc_conn_write_all(&ws->conn, frame, frame_len)
        : -1;
    dcc_ws_send_unlock(ws);
    free(frame);
    dcc_ws_destroy(ws);
    return rc == 0 ? DCC_OK : DCC_ERR_NETWORK;
}
dcc_status_t dcc_ws_send_text(dcc_ws_t *ws, const char *text, size_t len) {
    return dcc_ws_send(ws, DCC_WS_TEXT, text, len);
}
