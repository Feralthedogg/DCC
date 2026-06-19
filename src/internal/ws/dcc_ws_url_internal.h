#ifndef DCC_WS_URL_INTERNAL_H
#define DCC_WS_URL_INTERNAL_H

#include <dcc/error.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_ws_url {
    int use_tls;
    char *host;
    char *port;
    char *path;
} dcc_ws_url_t;

char *dcc_ws_strndup(const char *data, size_t len);
void dcc_ws_url_deinit(dcc_ws_url_t *url);
dcc_status_t dcc_ws_parse_url(const char *raw, dcc_ws_url_t *out);

#ifdef __cplusplus
}
#endif

#endif
