#include "internal/ws/dcc_ws.h"

#include <stdlib.h>
#include <string.h>

int dcc_ws_read_exact(dcc_ws_t *ws, void *buf, size_t len) {
    unsigned char *cursor = (unsigned char *)buf;
    size_t remaining = len;

    if (ws->rx_pos < ws->rx_len) {
        size_t available = ws->rx_len - ws->rx_pos;
        size_t take = available < remaining ? available : remaining;
        memcpy(cursor, ws->rx_buf + ws->rx_pos, take);
        ws->rx_pos += take;
        cursor += take;
        remaining -= take;

        if (ws->rx_pos == ws->rx_len) {
            free(ws->rx_buf);
            ws->rx_buf = NULL;
            ws->rx_len = 0;
            ws->rx_pos = 0;
        }
    }

    while (remaining > 0) {
        ssize_t n = dcc_conn_read(&ws->conn, cursor, remaining);
        if (n <= 0) {
            return -1;
        }
        cursor += (size_t)n;
        remaining -= (size_t)n;
    }
    return 0;
}
void dcc_ws_write_u16(unsigned char *out, uint16_t value) {
    out[0] = (unsigned char)((value >> 8) & 0xffU);
    out[1] = (unsigned char)(value & 0xffU);
}
void dcc_ws_write_u64(unsigned char *out, uint64_t value) {
    for (int i = 7; i >= 0; --i) {
        out[7 - i] = (unsigned char)((value >> (i * 8)) & 0xffU);
    }
}
uint16_t dcc_ws_read_u16(const unsigned char *data) {
    return (uint16_t)(((uint16_t)data[0] << 8) | (uint16_t)data[1]);
}
uint64_t dcc_ws_read_u64(const unsigned char *data) {
    uint64_t value = 0;
    for (size_t i = 0; i < 8; ++i) {
        value = (value << 8) | (uint64_t)data[i];
    }
    return value;
}
