#include "internal/interactions/dcc_interaction_server_request_internal.h"

#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_interaction_buffer_reserve(dcc_interaction_buffer_t *buffer, size_t need) {
    if (need <= buffer->cap) {
        return DCC_OK;
    }
    size_t next = buffer->cap == 0U ? DCC_INTERACTION_READ_CHUNK : buffer->cap;
    while (next < need) {
        if (next > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next *= 2U;
    }
    char *data = (char *)realloc(buffer->data, next);
    if (data == NULL) {
        return DCC_ERR_NOMEM;
    }
    buffer->data = data;
    buffer->cap = next;
    return DCC_OK;
}

dcc_status_t dcc_interaction_buffer_append(
    dcc_interaction_buffer_t *buffer,
    const char *data,
    size_t len
) {
    if (len > SIZE_MAX - buffer->len) {
        return DCC_ERR_NOMEM;
    }
    size_t next_len = buffer->len + len;
    if (next_len == SIZE_MAX) {
        return DCC_ERR_NOMEM;
    }
    dcc_status_t st = dcc_interaction_buffer_reserve(buffer, next_len + 1U);
    if (st != DCC_OK) {
        return st;
    }
    memcpy(buffer->data + buffer->len, data, len);
    buffer->len += len;
    buffer->data[buffer->len] = '\0';
    return DCC_OK;
}
