#include "internal/rest/dcc_rest_request_core_internal.h"
#include "internal/rest/dcc_rest_request_message_builders_internal.h"
#include "internal/rest/dcc_rest_request_payload_multipart_internal.h"

dcc_status_t dcc_rest_request_message_builder_json(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &json);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_request_method(client, method, path, json, cb, user_data);
    dcc_message_builder_json_free(json);
    return status;
}

dcc_status_t dcc_rest_request_message_builder_multipart(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *query,
    const dcc_message_builder_t *message,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &json);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_request_payload_files_multipart(
        client,
        method,
        path,
        query,
        json,
        files,
        file_count,
        cb,
        user_data
    );
    dcc_message_builder_json_free(json);
    return status;
}
