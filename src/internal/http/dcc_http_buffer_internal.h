#ifndef DCC_HTTP_BUFFER_INTERNAL_H
#define DCC_HTTP_BUFFER_INTERNAL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_http_buffer {
    char *data;
    size_t len;
    size_t cap;
} dcc_http_buffer_t;

void dcc_http_buffer_deinit(dcc_http_buffer_t *buf);
int dcc_http_buffer_append(dcc_http_buffer_t *buf, const void *data, size_t len);
int dcc_http_buffer_append_cstr(dcc_http_buffer_t *buf, const char *data);
char *dcc_http_strndup(const char *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif
