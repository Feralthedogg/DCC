#ifndef DCC_HTTP_URL_INTERNAL_H
#define DCC_HTTP_URL_INTERNAL_H

#include "internal/http/dcc_http_buffer_internal.h"

#include <dcc/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_http_url_parts {
    int use_tls;
    char *host;
    char *port;
    char *path;
} dcc_http_url_parts_t;

dcc_status_t dcc_http_parse_url(const char *url, dcc_http_url_parts_t *out);
void dcc_http_url_parts_deinit(dcc_http_url_parts_t *parts);

#ifdef __cplusplus
}
#endif

#endif
