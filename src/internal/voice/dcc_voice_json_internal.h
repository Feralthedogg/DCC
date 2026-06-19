#ifndef DCC_VOICE_JSON_INTERNAL_H
#define DCC_VOICE_JSON_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_voice_json_buffer {
    char *data;
    size_t len;
    size_t cap;
    int overflow;
} dcc_voice_json_buffer_t;

int dcc_voice_json_append_mem(dcc_voice_json_buffer_t *buf, const char *data, size_t len);
int dcc_voice_json_append_cstr(dcc_voice_json_buffer_t *buf, const char *value);
int dcc_voice_json_append_u64(dcc_voice_json_buffer_t *buf, uint64_t value);
int dcc_voice_json_append_i32(dcc_voice_json_buffer_t *buf, int32_t value);
int dcc_voice_json_append_string(dcc_voice_json_buffer_t *buf, const char *value);
dcc_status_t dcc_voice_json_finish(dcc_voice_json_buffer_t *buf, size_t *out_len);

#ifdef __cplusplus
}
#endif

#endif
