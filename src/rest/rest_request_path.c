#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_request_owned_path(
    dcc_client_t *client,
    dcc_rest_method_t method,
    char *path,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (path == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_rest_request_method(client, method, path, body, cb, user_data);
    free(path);
    return status;
}

dcc_status_t dcc_rest_request_with_query(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *query,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
) {
    if (query == NULL || query[0] == '\0') {
        return dcc_rest_request_method(client, method, path, body, cb, user_data);
    }

    const char *query_part = query[0] == '?' ? query + 1 : query;
    if (query_part[0] == '\0') {
        return dcc_rest_request_method(client, method, path, body, cb, user_data);
    }

    size_t path_len = strlen(path);
    size_t query_len = strlen(query_part);
    if (path_len > SIZE_MAX - query_len - 2U) {
        return DCC_ERR_NOMEM;
    }
    size_t full_len = path_len + query_len + 1U;
    char *full_path = (char *)malloc(full_len + 1U);
    if (full_path == NULL) {
        return DCC_ERR_NOMEM;
    }

    memcpy(full_path, path, path_len);
    full_path[path_len] = '?';
    memcpy(full_path + path_len + 1U, query_part, query_len);
    full_path[full_len] = '\0';

    dcc_status_t status = dcc_rest_request_method(client, method, full_path, body, cb, user_data);
    free(full_path);
    return status;
}
