#include "internal/ws/dcc_ws.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_ws_recv(dcc_ws_t *ws, dcc_ws_message_t *out) {
    if (ws == NULL ||
        out == NULL ||
        atomic_load_explicit(&ws->open, memory_order_acquire) == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));

    for (;;) {
        unsigned char header[2];
        if (dcc_ws_read_exact(ws, header, sizeof(header)) != 0) {
            return DCC_ERR_NETWORK;
        }

        int fin = (header[0] & 0x80U) != 0;
        dcc_ws_opcode_t opcode = (dcc_ws_opcode_t)(header[0] & 0x0fU);
        int masked = (header[1] & 0x80U) != 0;
        uint64_t payload_len = (uint64_t)(header[1] & 0x7fU);

        if (!fin) {
            return DCC_ERR_NETWORK;
        }
        if (payload_len == 126U) {
            unsigned char ext[2];
            if (dcc_ws_read_exact(ws, ext, sizeof(ext)) != 0) {
                return DCC_ERR_NETWORK;
            }
            payload_len = dcc_ws_read_u16(ext);
        } else if (payload_len == 127U) {
            unsigned char ext[8];
            if (dcc_ws_read_exact(ws, ext, sizeof(ext)) != 0) {
                return DCC_ERR_NETWORK;
            }
            payload_len = dcc_ws_read_u64(ext);
        }

        if (payload_len > DCC_WS_MAX_MESSAGE_BYTES) {
            return DCC_ERR_NETWORK;
        }

        unsigned char mask[4] = {0};
        if (masked && dcc_ws_read_exact(ws, mask, sizeof(mask)) != 0) {
            return DCC_ERR_NETWORK;
        }

        unsigned char *payload = NULL;
        if (payload_len > 0) {
            payload = (unsigned char *)malloc((size_t)payload_len + 1);
            if (payload == NULL) {
                return DCC_ERR_NOMEM;
            }
            if (dcc_ws_read_exact(ws, payload, (size_t)payload_len) != 0) {
                free(payload);
                return DCC_ERR_NETWORK;
            }
            if (masked) {
                for (size_t i = 0; i < (size_t)payload_len; ++i) {
                    payload[i] ^= mask[i % 4];
                }
            }
            payload[payload_len] = '\0';
        }

        if (opcode == DCC_WS_PING) {
            dcc_status_t status = dcc_ws_send(ws, DCC_WS_PONG, payload, (size_t)payload_len);
            free(payload);
            if (status != DCC_OK) {
                return status;
            }
            continue;
        }
        if (opcode == DCC_WS_PONG) {
            free(payload);
            continue;
        }
        if (opcode == DCC_WS_CLOSE) {
            if (payload_len >= 2) {
                ws->close_code = dcc_ws_read_u16(payload);
                size_t reason_len = (size_t)payload_len - 2U;
                if (reason_len >= sizeof(ws->close_reason)) {
                    reason_len = sizeof(ws->close_reason) - 1U;
                }
                if (reason_len > 0) {
                    memcpy(ws->close_reason, payload + 2, reason_len);
                }
                ws->close_reason[reason_len] = '\0';
            }
            (void)dcc_ws_send(ws, DCC_WS_CLOSE, payload, (size_t)payload_len);
            atomic_store_explicit(&ws->open, 0, memory_order_release);
            free(payload);
            return DCC_ERR_CANCELED;
        }

        out->opcode = opcode;
        out->data = payload;
        out->len = (size_t)payload_len;
        return DCC_OK;
    }
}
