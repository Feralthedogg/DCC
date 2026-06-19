#include "internal/gateway/dcc_gateway_bot_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_gateway_build_url(const char *base, const char *path, char **out) {
    if (base == NULL || path == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    size_t base_len = strlen(base);
    while (base_len > 0 && base[base_len - 1] == '/') {
        base_len--;
    }
    size_t path_len = strlen(path);
    size_t needs_slash = path_len > 0 && path[0] == '/' ? 0U : 1U;
    size_t total = base_len + needs_slash + path_len;
    char *url = (char *)malloc(total + 1U);
    if (url == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(url, base, base_len);
    size_t pos = base_len;
    if (needs_slash) {
        url[pos++] = '/';
    }
    memcpy(url + pos, path, path_len);
    url[total] = '\0';
    *out = url;
    return DCC_OK;
}

dcc_status_t dcc_gateway_url_with_json_query(const char *raw, char **out) {
    if (raw == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    const char *scheme = strstr(raw, "://");
    const char *after_authority = scheme != NULL ? strpbrk(scheme + 3, "/?") : strpbrk(raw, "/?");
    const int has_path = after_authority != NULL && *after_authority == '/';
    const char *query = strchr(raw, '?') == NULL
        ? (has_path ? "?v=10&encoding=json" : "/?v=10&encoding=json")
        : "&v=10&encoding=json";
    size_t raw_len = strlen(raw);
    size_t query_len = strlen(query);
    char *url = (char *)malloc(raw_len + query_len + 1U);
    if (url == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(url, raw, raw_len);
    memcpy(url + raw_len, query, query_len + 1U);
    *out = url;
    return DCC_OK;
}
