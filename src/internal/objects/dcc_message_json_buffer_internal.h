#ifndef DCC_MESSAGE_JSON_BUFFER_INTERNAL_H
#define DCC_MESSAGE_JSON_BUFFER_INTERNAL_H

#include <dcc/error.h>
#include <dcc/snowflake.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_message_json_buffer {
    char *data;
    size_t len;
    size_t cap;
} dcc_message_json_buffer_t;

void dcc_message_json_buffer_deinit(dcc_message_json_buffer_t *buffer);
dcc_status_t dcc_message_json_buffer_reserve(dcc_message_json_buffer_t *buffer, size_t extra);
dcc_status_t dcc_message_json_append(dcc_message_json_buffer_t *buffer, const void *data, size_t len);
dcc_status_t dcc_message_json_append_cstr(dcc_message_json_buffer_t *buffer, const char *value);
dcc_status_t dcc_message_json_append_u64(dcc_message_json_buffer_t *buffer, uint64_t value);
dcc_status_t dcc_message_json_append_snowflake_string(
    dcc_message_json_buffer_t *buffer,
    dcc_snowflake_t value
);
dcc_status_t dcc_message_json_append_escaped_string(
    dcc_message_json_buffer_t *buffer,
    const char *value
);

#ifdef __cplusplus
}
#endif

#endif
