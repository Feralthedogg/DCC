#include "internal/http/dcc_http.h"

#include <stdlib.h>
#include <string.h>

void dcc_http_url_parts_deinit(dcc_http_url_parts_t *parts) {
    if (parts == NULL) {
        return;
    }
    free(parts->host);
    free(parts->port);
    free(parts->path);
    memset(parts, 0, sizeof(*parts));
}

dcc_status_t dcc_http_parse_url(const char *url, dcc_http_url_parts_t *out) {
    if (url == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));

    const char *cursor = NULL;
    if (strncmp(url, "https://", 8) == 0) {
        out->use_tls = 1;
        cursor = url + 8;
    } else if (strncmp(url, "http://", 7) == 0) {
        out->use_tls = 0;
        cursor = url + 7;
    } else {
        return DCC_ERR_INVALID_ARG;
    }

    const char *path = strchr(cursor, '/');
    const char *authority_end = path != NULL ? path : cursor + strlen(cursor);
    const char *port_sep = NULL;

    if (cursor < authority_end && *cursor == '[') {
        const char *close = memchr(cursor, ']', (size_t)(authority_end - cursor));
        if (close == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        out->host = dcc_http_strndup(cursor + 1, (size_t)(close - cursor - 1));
        if (close + 1 < authority_end && close[1] == ':') {
            port_sep = close + 1;
        }
    } else {
        port_sep = memchr(cursor, ':', (size_t)(authority_end - cursor));
        const char *host_end = port_sep != NULL ? port_sep : authority_end;
        out->host = dcc_http_strndup(cursor, (size_t)(host_end - cursor));
    }

    if (out->host == NULL || out->host[0] == '\0') {
        dcc_http_url_parts_deinit(out);
        return DCC_ERR_NOMEM;
    }

    if (port_sep != NULL) {
        out->port = dcc_http_strndup(port_sep + 1, (size_t)(authority_end - port_sep - 1));
    } else {
        const char *default_port = out->use_tls ? "443" : "80";
        out->port = dcc_http_strndup(default_port, strlen(default_port));
    }
    out->path = dcc_http_strndup(path != NULL ? path : "/", strlen(path != NULL ? path : "/"));

    if (out->port == NULL || out->path == NULL || out->port[0] == '\0') {
        dcc_http_url_parts_deinit(out);
        return DCC_ERR_NOMEM;
    }

    return DCC_OK;
}
