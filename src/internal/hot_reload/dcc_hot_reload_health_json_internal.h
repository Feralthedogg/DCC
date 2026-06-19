#ifndef DCC_HOT_RELOAD_HEALTH_JSON_INTERNAL_H
#define DCC_HOT_RELOAD_HEALTH_JSON_INTERNAL_H

#include "internal/dcc_core_internal.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_hot_reload_json_buffer {
    char *data;
    size_t len;
    size_t cap;
    uint8_t overflow;
} dcc_hot_reload_json_buffer_t;

int dcc_hot_reload_json_append_raw(
    dcc_hot_reload_json_buffer_t *json,
    const char *data,
    size_t data_len
);
int dcc_hot_reload_json_append_cstr(dcc_hot_reload_json_buffer_t *json, const char *text);
int dcc_hot_reload_json_append_u64(dcc_hot_reload_json_buffer_t *json, uint64_t value);
int dcc_hot_reload_json_append_string(dcc_hot_reload_json_buffer_t *json, const char *text);
dcc_status_t dcc_hot_reload_json_finish(dcc_hot_reload_json_buffer_t *json, size_t *out_len);

#ifdef __cplusplus
}
#endif

#endif
