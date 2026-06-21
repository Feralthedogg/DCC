#include "internal/ws/dcc_ws.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_ws_append_fragment(
    unsigned char **buffer,
    size_t *buffer_len,
    const unsigned char *payload,
    size_t payload_len
) {
    if (buffer == NULL || buffer_len == NULL || (payload == NULL && payload_len > 0)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (payload_len > (size_t)DCC_WS_MAX_MESSAGE_BYTES ||
        *buffer_len > (size_t)DCC_WS_MAX_MESSAGE_BYTES - payload_len) {
        return DCC_ERR_NETWORK;
    }

    size_t next_len = *buffer_len + payload_len;
    unsigned char *next = (unsigned char *)realloc(*buffer, next_len + 1U);
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    if (payload_len > 0) {
        memcpy(next + *buffer_len, payload, payload_len);
    }
    next[next_len] = '\0';
    *buffer = next;
    *buffer_len = next_len;
    return DCC_OK;
}

static dcc_status_t dcc_ws_fail_fragment(unsigned char *fragment, unsigned char *payload) {
    free(fragment);
    free(payload);
    return DCC_ERR_NETWORK;
}

dcc_status_t dcc_ws_recv(dcc_ws_t *ws, dcc_ws_message_t *out) {
    if (ws == NULL ||
        out == NULL ||
        atomic_load_explicit(&ws->open, memory_order_acquire) == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));

    unsigned char *fragment = NULL;
    size_t fragment_len = 0;
    dcc_ws_opcode_t fragment_opcode = DCC_WS_TEXT;
    int in_fragment = 0;

    for (;;) {
        unsigned char header[2];
        if (dcc_ws_read_exact(ws, header, sizeof(header)) != 0) {
            free(fragment);
            return DCC_ERR_NETWORK;
        }

        int fin = (header[0] & 0x80U) != 0;
        dcc_ws_opcode_t opcode = (dcc_ws_opcode_t)(header[0] & 0x0fU);
        int masked = (header[1] & 0x80U) != 0;
        uint64_t payload_len = (uint64_t)(header[1] & 0x7fU);

        if ((header[0] & 0x70U) != 0) {
            free(fragment);
            return DCC_ERR_NETWORK;
        }
        if (payload_len == 126U) {
            unsigned char ext[2];
            if (dcc_ws_read_exact(ws, ext, sizeof(ext)) != 0) {
                free(fragment);
                return DCC_ERR_NETWORK;
            }
            payload_len = dcc_ws_read_u16(ext);
        } else if (payload_len == 127U) {
            unsigned char ext[8];
            if (dcc_ws_read_exact(ws, ext, sizeof(ext)) != 0) {
                free(fragment);
                return DCC_ERR_NETWORK;
            }
            payload_len = dcc_ws_read_u64(ext);
        }

        if (payload_len > DCC_WS_MAX_MESSAGE_BYTES) {
            free(fragment);
            return DCC_ERR_NETWORK;
        }
        if ((unsigned)opcode >= 0x8U && (!fin || payload_len > 125U)) {
            free(fragment);
            return DCC_ERR_NETWORK;
        }

        unsigned char mask[4] = {0};
        if (masked && dcc_ws_read_exact(ws, mask, sizeof(mask)) != 0) {
            free(fragment);
            return DCC_ERR_NETWORK;
        }

        unsigned char *payload = NULL;
        if (payload_len > 0) {
            payload = (unsigned char *)malloc((size_t)payload_len + 1);
            if (payload == NULL) {
                free(fragment);
                return DCC_ERR_NOMEM;
            }
            if (dcc_ws_read_exact(ws, payload, (size_t)payload_len) != 0) {
                return dcc_ws_fail_fragment(fragment, payload);
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
                free(fragment);
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
            } else {
                ws->close_code = 1005U;
                ws->close_reason[0] = '\0';
            }
            (void)dcc_ws_send(ws, DCC_WS_CLOSE, payload, (size_t)payload_len);
            atomic_store_explicit(&ws->open, 0, memory_order_release);
            free(fragment);
            free(payload);
            return DCC_ERR_CANCELED;
        }
        if (opcode == DCC_WS_TEXT || opcode == DCC_WS_BINARY) {
            if (in_fragment) {
                return dcc_ws_fail_fragment(fragment, payload);
            }
            if (fin) {
                out->opcode = opcode;
                out->data = payload;
                out->len = (size_t)payload_len;
                return DCC_OK;
            }

            dcc_status_t status =
                dcc_ws_append_fragment(&fragment, &fragment_len, payload, (size_t)payload_len);
            free(payload);
            if (status != DCC_OK) {
                free(fragment);
                return status;
            }
            fragment_opcode = opcode;
            in_fragment = 1;
            continue;
        }
        if (opcode == DCC_WS_CONTINUATION) {
            if (!in_fragment) {
                return dcc_ws_fail_fragment(fragment, payload);
            }

            dcc_status_t status =
                dcc_ws_append_fragment(&fragment, &fragment_len, payload, (size_t)payload_len);
            free(payload);
            if (status != DCC_OK) {
                free(fragment);
                return status;
            }
            if (!fin) {
                continue;
            }

            out->opcode = fragment_opcode;
            out->data = fragment;
            out->len = fragment_len;
            return DCC_OK;
        }

        return dcc_ws_fail_fragment(fragment, payload);
    }
}
