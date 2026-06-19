#ifndef DCC_HTTP_RESPONSE_INTERNAL_H
#define DCC_HTTP_RESPONSE_INTERNAL_H

#include "internal/http/dcc_http_buffer_internal.h"

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_http_response {
    uint16_t status;
    char *headers;
    size_t headers_len;
    char *body;
    size_t body_len;
} dcc_http_response_t;

void dcc_http_response_deinit(dcc_http_response_t *response);
dcc_status_t dcc_http_parse_response(dcc_http_buffer_t *raw, dcc_http_response_t *out);
dcc_status_t dcc_http_decode_chunked_body(const char *body, size_t body_len, dcc_http_buffer_t *out);
int dcc_http_headers_token_contains(const char *headers, size_t headers_len, const char *name, const char *token);
char *dcc_http_find_header_end(const char *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
