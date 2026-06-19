#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_request_url_internal.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DCC_DISCORD_API_BASE_DEFAULT "https://discord.com/api/v10"

dcc_status_t dcc_rest_build_url(const char *path, char **out_url, int *out_absolute) {
    if (path == NULL || out_url == NULL || out_absolute == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    const int absolute_url = strncmp(path, "http://", 7) == 0 || strncmp(path, "https://", 8) == 0;
    *out_absolute = absolute_url;

    if (absolute_url) {
        char *url = dcc_strdup(path);
        if (url == NULL) {
            return DCC_ERR_NOMEM;
        }
        *out_url = url;
        return DCC_OK;
    }

    const char *base = getenv("DCC_DISCORD_API_BASE");
    if (base == NULL || base[0] == '\0') {
        base = DCC_DISCORD_API_BASE_DEFAULT;
    }

    size_t base_len = strlen(base);
    const char *path_part = path;
    size_t path_len = strlen(path_part);
    int need_slash = 0;

    if (base_len > 0 && base[base_len - 1] == '/' && path_part[0] == '/') {
        path_part++;
        path_len--;
    } else if ((base_len == 0 || base[base_len - 1] != '/') && path_part[0] != '/') {
        need_slash = 1;
    }

    if (base_len > SIZE_MAX - path_len - (size_t)need_slash - 1U) {
        return DCC_ERR_NOMEM;
    }
    size_t url_len = base_len + path_len + (size_t)need_slash;
    char *url = (char *)malloc(url_len + 1U);
    if (url == NULL) {
        return DCC_ERR_NOMEM;
    }

    size_t pos = 0;
    memcpy(url + pos, base, base_len);
    pos += base_len;
    if (need_slash) {
        url[pos++] = '/';
    }
    memcpy(url + pos, path_part, path_len);
    pos += path_len;
    url[pos] = '\0';

    *out_url = url;
    return DCC_OK;
}
