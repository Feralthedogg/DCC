#include "internal/objects/dcc_message_json_buffer_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static _Thread_local int dcc_message_json_buffer_probe_active;
static _Thread_local size_t dcc_message_json_buffer_probe_calls;

void dcc_message_json_buffer_test_allocation_probe_begin(void) {
    dcc_message_json_buffer_probe_calls = 0U;
    dcc_message_json_buffer_probe_active = 1;
}

size_t dcc_message_json_buffer_test_allocation_probe_end(void) {
    size_t calls = dcc_message_json_buffer_probe_calls;
    dcc_message_json_buffer_probe_calls = 0U;
    dcc_message_json_buffer_probe_active = 0;
    return calls;
}

void dcc_message_json_buffer_init_count(dcc_message_json_buffer_t *buffer) {
    if (buffer != NULL) {
        *buffer = (dcc_message_json_buffer_t){0};
        buffer->count_only = 1U;
    }
}

void dcc_message_json_buffer_deinit(dcc_message_json_buffer_t *buffer) {
    if (buffer == NULL) {
        return;
    }
    free(buffer->data);
    buffer->data = NULL;
    buffer->len = 0;
    buffer->cap = 0;
    buffer->count_only = 0U;
}

dcc_status_t dcc_message_json_buffer_reserve(dcc_message_json_buffer_t *buffer, size_t extra) {
    if (buffer == NULL || extra > SIZE_MAX - buffer->len) {
        return DCC_ERR_NOMEM;
    }

    size_t needed = buffer->len + extra;
    if (needed <= buffer->cap) {
        return DCC_OK;
    }

    if (dcc_message_json_buffer_probe_active) {
        ++dcc_message_json_buffer_probe_calls;
    }

    size_t next_cap = buffer->cap != 0 ? buffer->cap : 256U;
    while (next_cap < needed) {
        if (next_cap > SIZE_MAX / 2U) {
            next_cap = needed;
            break;
        }
        next_cap *= 2U;
    }

    char *next = (char *)realloc(buffer->data, next_cap);
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    buffer->data = next;
    buffer->cap = next_cap;
    return DCC_OK;
}

dcc_status_t dcc_message_json_append(dcc_message_json_buffer_t *buffer, const void *data, size_t len) {
    if (buffer == NULL || (len != 0 && data == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (buffer->len > SIZE_MAX - 1U || len > SIZE_MAX - buffer->len - 1U) {
        return DCC_ERR_NOMEM;
    }

    if (buffer->count_only != 0U) {
        buffer->len += len;
        return DCC_OK;
    }

    dcc_status_t status = dcc_message_json_buffer_reserve(buffer, len + 1U);
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

dcc_status_t dcc_message_json_append_length(
    dcc_message_json_buffer_t *buffer,
    size_t len
) {
    if (buffer == NULL || buffer->count_only == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    if (buffer->len > SIZE_MAX - 1U ||
        len > SIZE_MAX - buffer->len - 1U) {
        return DCC_ERR_NOMEM;
    }
    buffer->len += len;
    return DCC_OK;
}

dcc_status_t dcc_message_json_append_cstr(dcc_message_json_buffer_t *buffer, const char *value) {
    if (value == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_message_json_append(buffer, value, strlen(value));
}

dcc_status_t dcc_message_json_append_u64(dcc_message_json_buffer_t *buffer, uint64_t value) {
    char tmp[32];
    int written = snprintf(tmp, sizeof(tmp), "%llu", (unsigned long long)value);
    if (written < 0 || (size_t)written >= sizeof(tmp)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_message_json_append(buffer, tmp, (size_t)written);
}

dcc_status_t dcc_message_json_append_snowflake_string(
    dcc_message_json_buffer_t *buffer,
    dcc_snowflake_t value
) {
    char tmp[32];
    int written = snprintf(tmp, sizeof(tmp), "\"%llu\"", (unsigned long long)value);
    if (written < 0 || (size_t)written >= sizeof(tmp)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_message_json_append(buffer, tmp, (size_t)written);
}
