#ifndef DCC_GATEWAY_PAYLOAD_BUFFER_INTERNAL_H
#define DCC_GATEWAY_PAYLOAD_BUFFER_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_gateway_payload_buffer {
    char *data;
    size_t len;
    size_t cap;
} dcc_gateway_payload_buffer_t;

void dcc_gateway_payload_buffer_deinit(dcc_gateway_payload_buffer_t *buf);
int dcc_gateway_payload_buffer_append_mem(dcc_gateway_payload_buffer_t *buf, const char *data, size_t len);
int dcc_gateway_payload_buffer_append_cstr(dcc_gateway_payload_buffer_t *buf, const char *value);
int dcc_gateway_payload_buffer_append_u64(dcc_gateway_payload_buffer_t *buf, uint64_t value);
int dcc_gateway_payload_buffer_append_json_string(dcc_gateway_payload_buffer_t *buf, const char *value);

#ifdef __cplusplus
}
#endif

#endif
