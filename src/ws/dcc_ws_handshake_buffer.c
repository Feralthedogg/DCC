#include "internal/ws/dcc_ws_handshake_helpers_internal.h"

#include <stdlib.h>
#include <string.h>

int dcc_ws_buffer_append(dcc_ws_buffer_t *buf, const void *data, size_t len) {
    if (len > (size_t)-1 - buf->len) {
        return -1;
    }
    size_t needed = buf->len + len;
    if (needed > buf->cap) {
        size_t next_cap = buf->cap == 0 ? 1024 : buf->cap;
        while (next_cap < needed) {
            if (next_cap > (size_t)-1 / 2) {
                return -1;
            }
            next_cap *= 2;
        }
        unsigned char *next = (unsigned char *)realloc(buf->data, next_cap);
        if (next == NULL) {
            return -1;
        }
        buf->data = next;
        buf->cap = next_cap;
    }
    memcpy(buf->data + buf->len, data, len);
    buf->len += len;
    return 0;
}

void dcc_ws_buffer_deinit(dcc_ws_buffer_t *buf) {
    free(buf->data);
    memset(buf, 0, sizeof(*buf));
}
