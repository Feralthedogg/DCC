#ifndef DCC_REST_REQUEST_PAYLOAD_MULTIPART_INTERNAL_H
#define DCC_REST_REQUEST_PAYLOAD_MULTIPART_INTERNAL_H

#include <dcc/rest.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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
);

#ifdef __cplusplus
}
#endif

#endif
