#include "internal/hot_reload/dcc_hot_reload_health_json_internal.h"

#include <stdio.h>
#include <string.h>

int dcc_hot_reload_json_append_raw(
    dcc_hot_reload_json_buffer_t *json,
    const char *data,
    size_t data_len
) {
    if (json == NULL || json->data == NULL || data == NULL) {
        return -1;
    }
    if (data_len > json->cap || json->len > json->cap - data_len) {
        json->overflow = 1U;
        if (json->cap != 0U) {
            json->data[json->cap - 1U] = '\0';
        }
        return -1;
    }
    memcpy(json->data + json->len, data, data_len);
    json->len += data_len;
    if (json->len < json->cap) {
        json->data[json->len] = '\0';
    } else if (json->cap != 0U) {
        json->data[json->cap - 1U] = '\0';
    }
    return 0;
}

int dcc_hot_reload_json_append_cstr(dcc_hot_reload_json_buffer_t *json, const char *text) {
    return dcc_hot_reload_json_append_raw(json, text, text != NULL ? strlen(text) : 0U);
}

int dcc_hot_reload_json_append_u64(dcc_hot_reload_json_buffer_t *json, uint64_t value) {
    char buffer[32];
    int n = snprintf(buffer, sizeof(buffer), "%llu", (unsigned long long)value);
    if (n < 0 || (size_t)n >= sizeof(buffer)) {
        json->overflow = 1U;
        return -1;
    }
    return dcc_hot_reload_json_append_raw(json, buffer, (size_t)n);
}

dcc_status_t dcc_hot_reload_json_finish(dcc_hot_reload_json_buffer_t *json, size_t *out_len) {
    if (json == NULL || json->data == NULL || json->cap == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    if (json->overflow || json->len >= json->cap) {
        json->data[json->cap - 1U] = '\0';
        if (out_len != NULL) {
            *out_len = json->len < json->cap ? json->len : json->cap - 1U;
        }
        return DCC_ERR_NOMEM;
    }
    json->data[json->len] = '\0';
    if (out_len != NULL) {
        *out_len = json->len;
    }
    return DCC_OK;
}
