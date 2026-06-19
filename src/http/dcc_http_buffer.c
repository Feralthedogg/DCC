#include "internal/http/dcc_http.h"

#include <stdlib.h>
#include <string.h>

void dcc_http_buffer_deinit(dcc_http_buffer_t *buf) {
    if (buf == NULL) {
        return;
    }
    free(buf->data);
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
}

static int dcc_http_buffer_reserve(dcc_http_buffer_t *buf, size_t extra) {
    if (buf == NULL || extra > (size_t)-1 - buf->len) {
        return -1;
    }
    size_t needed = buf->len + extra;
    if (needed <= buf->cap) {
        return 0;
    }

    size_t next_cap = buf->cap == 0 ? 1024 : buf->cap;
    while (next_cap < needed) {
        if (next_cap > (size_t)-1 / 2) {
            return -1;
        }
        next_cap *= 2;
    }

    char *next = (char *)realloc(buf->data, next_cap);
    if (next == NULL) {
        return -1;
    }
    buf->data = next;
    buf->cap = next_cap;
    return 0;
}

int dcc_http_buffer_append(dcc_http_buffer_t *buf, const void *data, size_t len) {
    if (buf == NULL || (data == NULL && len != 0)) {
        return -1;
    }
    if (len >= (size_t)-1 - buf->len || dcc_http_buffer_reserve(buf, len + 1U) != 0) {
        return -1;
    }
    if (len != 0) {
        memcpy(buf->data + buf->len, data, len);
        buf->len += len;
    }
    buf->data[buf->len] = '\0';
    return 0;
}

int dcc_http_buffer_append_cstr(dcc_http_buffer_t *buf, const char *data) {
    return data != NULL ? dcc_http_buffer_append(buf, data, strlen(data)) : -1;
}

char *dcc_http_strndup(const char *data, size_t len) {
    if (data == NULL && len != 0) {
        return NULL;
    }
    if (len == (size_t)-1) {
        return NULL;
    }
    char *copy = (char *)malloc(len + 1U);
    if (copy == NULL) {
        return NULL;
    }
    if (len != 0) {
        memcpy(copy, data, len);
    }
    copy[len] = '\0';
    return copy;
}
