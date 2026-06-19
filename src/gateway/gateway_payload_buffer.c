#include "internal/gateway/dcc_gateway_payload_buffer_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dcc_gateway_payload_buffer_deinit(dcc_gateway_payload_buffer_t *buf) {
    if (buf == NULL) {
        return;
    }
    free(buf->data);
    memset(buf, 0, sizeof(*buf));
}

static int dcc_gateway_payload_buffer_reserve(dcc_gateway_payload_buffer_t *buf, size_t extra) {
    if (extra > (size_t)-1 - buf->len) {
        return -1;
    }
    size_t needed = buf->len + extra + 1U;
    if (needed <= buf->cap) {
        return 0;
    }

    size_t next_cap = buf->cap == 0 ? 256U : buf->cap;
    while (next_cap < needed) {
        if (next_cap > (size_t)-1 / 2U) {
            return -1;
        }
        next_cap *= 2U;
    }

    char *next = (char *)realloc(buf->data, next_cap);
    if (next == NULL) {
        return -1;
    }
    buf->data = next;
    buf->cap = next_cap;
    return 0;
}

int dcc_gateway_payload_buffer_append_mem(dcc_gateway_payload_buffer_t *buf, const char *data, size_t len) {
    if (dcc_gateway_payload_buffer_reserve(buf, len) != 0) {
        return -1;
    }
    memcpy(buf->data + buf->len, data, len);
    buf->len += len;
    buf->data[buf->len] = '\0';
    return 0;
}

int dcc_gateway_payload_buffer_append_cstr(dcc_gateway_payload_buffer_t *buf, const char *value) {
    return dcc_gateway_payload_buffer_append_mem(buf, value, strlen(value));
}

int dcc_gateway_payload_buffer_append_u64(dcc_gateway_payload_buffer_t *buf, uint64_t value) {
    char tmp[32];
    int n = snprintf(tmp, sizeof(tmp), "%llu", (unsigned long long)value);
    if (n <= 0 || (size_t)n >= sizeof(tmp)) {
        return -1;
    }
    return dcc_gateway_payload_buffer_append_mem(buf, tmp, (size_t)n);
}
