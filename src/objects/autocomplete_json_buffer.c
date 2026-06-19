#include "internal/objects/dcc_autocomplete_json_buffer_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_autocomplete_json_buffer_deinit(dcc_autocomplete_json_buffer_t *buffer) {
    if (buffer == NULL) {
        return;
    }
    free(buffer->data);
    buffer->data = NULL;
    buffer->len = 0;
    buffer->cap = 0;
}

static dcc_status_t dcc_autocomplete_json_buffer_reserve(
    dcc_autocomplete_json_buffer_t *buffer,
    size_t extra
) {
    if (buffer == NULL || extra > SIZE_MAX - buffer->len) {
        return DCC_ERR_NOMEM;
    }

    size_t needed = buffer->len + extra;
    if (needed <= buffer->cap) {
        return DCC_OK;
    }

    size_t next_cap = buffer->cap != 0 ? buffer->cap : 256U;
    while (next_cap < needed) {
        if (next_cap > SIZE_MAX / 2U) {
            next_cap = needed;
            break;
        }
        next_cap *= 2U;
    }
    if (next_cap < needed) {
        return DCC_ERR_NOMEM;
    }

    char *next = (char *)realloc(buffer->data, next_cap);
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    buffer->data = next;
    buffer->cap = next_cap;
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_json_append(
    dcc_autocomplete_json_buffer_t *buffer,
    const void *data,
    size_t len
) {
    if (buffer == NULL || (len != 0 && data == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (buffer->len > SIZE_MAX - 1U || len > SIZE_MAX - buffer->len - 1U) {
        return DCC_ERR_NOMEM;
    }

    dcc_status_t status = dcc_autocomplete_json_buffer_reserve(buffer, len + 1U);
    if (status != DCC_OK) {
        return status;
    }
    if (len != 0) {
        memcpy(buffer->data + buffer->len, data, len);
        buffer->len += len;
    }
    buffer->data[buffer->len] = '\0';
    return DCC_OK;
}

dcc_status_t dcc_autocomplete_json_append_cstr(
    dcc_autocomplete_json_buffer_t *buffer,
    const char *value
) {
    if (value == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_autocomplete_json_append(buffer, value, strlen(value));
}
