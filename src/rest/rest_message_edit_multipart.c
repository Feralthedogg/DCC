#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_message_builders_internal.h"
#include "internal/rest/dcc_rest_request_payload_multipart_internal.h"

dcc_status_t dcc_rest_edit_message_multipart(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/messages/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)message_id
    );
    return status == DCC_OK
        ? dcc_rest_request_payload_files_multipart(
              client,
              DCC_REST_PATCH,
              path,
              NULL,
              payload_json,
              files,
              file_count,
              cb,
              user_data
          )
        : status;
}

dcc_status_t dcc_rest_edit_message_builder_multipart(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_message_builder_t *message,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/messages/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)message_id
    );
    return status == DCC_OK
        ? dcc_rest_request_message_builder_multipart(
              client,
              DCC_REST_PATCH,
              path,
              NULL,
              message,
              files,
              file_count,
              cb,
              user_data
          )
        : status;
}
