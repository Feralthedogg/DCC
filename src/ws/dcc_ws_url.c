#include "internal/ws/dcc_ws.h"

#include <stdlib.h>
#include <string.h>

char *dcc_ws_strndup(const char *data, size_t len) {
    char *copy = (char *)malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, data, len);
    copy[len] = '\0';
    return copy;
}
void dcc_ws_url_deinit(dcc_ws_url_t *url) {
    if (url == NULL) {
        return;
    }
    free(url->host);
    free(url->port);
    free(url->path);
    memset(url, 0, sizeof(*url));
}
dcc_status_t dcc_ws_parse_url(const char *raw, dcc_ws_url_t *out) {
    if (raw == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));

    const char *cursor = NULL;
    if (strncmp(raw, "wss://", 6) == 0) {
        out->use_tls = 1;
        cursor = raw + 6;
    } else if (strncmp(raw, "ws://", 5) == 0) {
        out->use_tls = 0;
        cursor = raw + 5;
    } else {
        return DCC_ERR_INVALID_ARG;
    }

    const char *slash = strchr(cursor, '/');
    const char *query = strchr(cursor, '?');
    const char *path = slash != NULL && (query == NULL || slash < query) ? slash : query;
    const char *authority_end = path != NULL ? path : cursor + strlen(cursor);
    const char *port_sep = NULL;

    if (cursor < authority_end && *cursor == '[') {
        const char *close = memchr(cursor, ']', (size_t)(authority_end - cursor));
        if (close == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        out->host = dcc_ws_strndup(cursor + 1, (size_t)(close - cursor - 1));
        if (close + 1 < authority_end && close[1] == ':') {
            port_sep = close + 1;
        }
    } else {
        port_sep = memchr(cursor, ':', (size_t)(authority_end - cursor));
        const char *host_end = port_sep != NULL ? port_sep : authority_end;
        out->host = dcc_ws_strndup(cursor, (size_t)(host_end - cursor));
    }

    if (out->host == NULL || out->host[0] == '\0') {
        dcc_ws_url_deinit(out);
        return DCC_ERR_NOMEM;
    }

    if (port_sep != NULL) {
        out->port = dcc_ws_strndup(port_sep + 1, (size_t)(authority_end - port_sep - 1));
    } else {
        out->port = dcc_ws_strndup(out->use_tls ? "443" : "80", strlen(out->use_tls ? "443" : "80"));
    }
    if (path == NULL) {
        out->path = dcc_ws_strndup("/", 1);
    } else if (*path == '?') {
        size_t query_len = strlen(path);
        out->path = (char *)malloc(query_len + 2U);
        if (out->path != NULL) {
            out->path[0] = '/';
            memcpy(out->path + 1, path, query_len + 1U);
        }
    } else {
        out->path = dcc_ws_strndup(path, strlen(path));
    }

    if (out->port == NULL || out->path == NULL || out->port[0] == '\0') {
        dcc_ws_url_deinit(out);
        return DCC_ERR_NOMEM;
    }
    return DCC_OK;
}
