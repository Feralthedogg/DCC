#ifndef DCC_CLUSTER_JSON_INTERNAL_H
#define DCC_CLUSTER_JSON_INTERNAL_H

#include "internal/dcc_core_internal.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_cluster_json_buffer {
    char *data;
    size_t len;
    size_t cap;
    uint8_t overflow;
} dcc_cluster_json_buffer_t;

int dcc_cluster_json_append_raw(
    dcc_cluster_json_buffer_t *json,
    const char *data,
    size_t data_len
);
int dcc_cluster_json_append_cstr(dcc_cluster_json_buffer_t *json, const char *text);
int dcc_cluster_json_append_u64(dcc_cluster_json_buffer_t *json, uint64_t value);
int dcc_cluster_json_append_string(dcc_cluster_json_buffer_t *json, const char *text);
dcc_status_t dcc_cluster_json_finish(dcc_cluster_json_buffer_t *json, size_t *out_len);

#ifdef __cplusplus
}
#endif

#endif
