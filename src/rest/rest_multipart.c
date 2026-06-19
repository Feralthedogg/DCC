#include "internal/rest/dcc_rest_multipart_internal.h"
#include "internal/rest/dcc_rest_request_core_internal.h"
#include "internal/rest/dcc_rest_request_payload_multipart_internal.h"

#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_rest_request_with_query_multipart(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *query,
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    if (query == NULL || query[0] == '\0') {
        return dcc_rest_request_method_multipart(client, method, path, fields, field_count, files, file_count, cb, user_data);
    }

    const char *query_part = query[0] == '?' ? query + 1 : query;
    if (query_part[0] == '\0') {
        return dcc_rest_request_method_multipart(client, method, path, fields, field_count, files, file_count, cb, user_data);
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

    dcc_status_t status = dcc_rest_request_method_multipart(
        client,
        method,
        full_path,
        fields,
        field_count,
        files,
        file_count,
        cb,
        user_data
    );
    free(full_path);
    return status;
}

dcc_status_t dcc_rest_request_payload_files_multipart(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *query,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_multipart_field_t payload = {
        .name = "payload_json",
        .value = payload_json,
    };
    const dcc_rest_multipart_field_t *fields = NULL;
    size_t field_count = 0;

    if (payload_json != NULL) {
        fields = &payload;
        field_count = 1;
    }

    return dcc_rest_request_with_query_multipart(
        client,
        method,
        path,
        query,
        fields,
        field_count,
        files,
        file_count,
        cb,
        user_data
    );
}
