#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_multipart_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_request_multipart(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    size_t body_len = 0;
    dcc_status_t status = dcc_rest_build_multipart_body(fields, field_count, files, file_count, &body, &body_len);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_rest_request_raw(
        client,
        method,
        path,
        body,
        body_len,
        DCC_REST_MULTIPART_CONTENT_TYPE,
        cb,
        user_data
    );
    free(body);
    return status;
}

dcc_status_t dcc_rest_request_method_multipart(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    const char *method_name = dcc_rest_method_name(method);
    if (method_name == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_request_multipart(client, method_name, path, fields, field_count, files, file_count, cb, user_data);
}
