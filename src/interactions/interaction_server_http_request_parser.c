#include "internal/interactions/dcc_interaction_server_request_internal.h"

#include <stdlib.h>
#include <string.h>

static char *dcc_interaction_trim_header_value(char *value) {
    while (*value == ' ' || *value == '\t') {
        value++;
    }
    char *end = value + strlen(value);
    while (end > value && (end[-1] == ' ' || end[-1] == '\t')) {
        end--;
    }
    *end = '\0';
    return value;
}

static dcc_status_t dcc_interaction_parse_request_line(dcc_interaction_request_t *request, char *line) {
    char *method_end = strchr(line, ' ');
    if (method_end == NULL) {
        return DCC_ERR_JSON;
    }
    *method_end = '\0';
    char *path = method_end + 1;
    while (*path == ' ') {
        path++;
    }
    char *path_end = strchr(path, ' ');
    if (path_end == NULL) {
        return DCC_ERR_JSON;
    }
    *path_end = '\0';

    size_t method_len = strlen(line);
    size_t path_len = strlen(path);
    if (method_len == 0U || method_len >= sizeof(request->method) ||
        path_len == 0U || path_len >= sizeof(request->path)) {
        return DCC_ERR_JSON;
    }
    memcpy(request->method, line, method_len + 1U);
    memcpy(request->path, path, path_len + 1U);
    return DCC_OK;
}

dcc_status_t dcc_interaction_parse_headers(
    dcc_interaction_request_t *request,
    const char *headers,
    size_t header_len
) {
    if (header_len == SIZE_MAX) {
        return DCC_ERR_NOMEM;
    }
    request->header_block = (char *)malloc(header_len + 1U);
    if (request->header_block == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(request->header_block, headers, header_len);
    request->header_block[header_len] = '\0';

    char *line = request->header_block;
    char *line_end = strstr(line, "\r\n");
    if (line_end == NULL) {
        return DCC_ERR_JSON;
    }
    *line_end = '\0';
    dcc_status_t st = dcc_interaction_parse_request_line(request, line);
    if (st != DCC_OK) {
        return st;
    }

    line = line_end + 2;
    while (line < request->header_block + header_len) {
        line_end = strstr(line, "\r\n");
        if (line_end == NULL) {
            break;
        }
        *line_end = '\0';
        if (*line == '\0') {
            break;
        }

        char *colon = strchr(line, ':');
        if (colon != NULL) {
            *colon = '\0';
            char *name = line;
            char *name_end = name + strlen(name);
            while (name_end > name && (name_end[-1] == ' ' || name_end[-1] == '\t')) {
                name_end--;
            }
            *name_end = '\0';
            char *value = dcc_interaction_trim_header_value(colon + 1);
            if (*name != '\0' && request->header_count < DCC_INTERACTION_HEADER_CAP) {
                request->headers[request->header_count].name = name;
                request->headers[request->header_count].value = value;
                request->header_count++;
            }
        }
        line = line_end + 2;
    }
    return DCC_OK;
}
