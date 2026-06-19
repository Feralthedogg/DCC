#include "internal/voice/dcc_voice_internal.h"

#include <stdio.h>
#include <string.h>

int dcc_voice_json_append_mem(dcc_voice_json_buffer_t *buf, const char *data, size_t len) {
    if (buf == NULL || (data == NULL && len != 0)) {
        return -1;
    }
    if (len > SIZE_MAX - buf->len) {
        buf->overflow = 1;
        return -1;
    }
    if (buf->data == NULL || buf->cap == 0 || len >= buf->cap - buf->len) {
        buf->overflow = 1;
        buf->len += len;
        return -1;
    }
    memcpy(buf->data + buf->len, data, len);
    buf->len += len;
    return 0;
}

int dcc_voice_json_append_cstr(dcc_voice_json_buffer_t *buf, const char *value) {
    return value != NULL ? dcc_voice_json_append_mem(buf, value, strlen(value)) : -1;
}

int dcc_voice_json_append_u64(dcc_voice_json_buffer_t *buf, uint64_t value) {
    char tmp[32];
    int n = snprintf(tmp, sizeof(tmp), "%llu", (unsigned long long)value);
    if (n <= 0 || (size_t)n >= sizeof(tmp)) {
        if (buf != NULL) {
            buf->overflow = 1;
        }
        return -1;
    }
    return dcc_voice_json_append_mem(buf, tmp, (size_t)n);
}

int dcc_voice_json_append_i32(dcc_voice_json_buffer_t *buf, int32_t value) {
    char tmp[16];
    int n = snprintf(tmp, sizeof(tmp), "%ld", (long)value);
    if (n <= 0 || (size_t)n >= sizeof(tmp)) {
        if (buf != NULL) {
            buf->overflow = 1;
        }
        return -1;
    }
    return dcc_voice_json_append_mem(buf, tmp, (size_t)n);
}
