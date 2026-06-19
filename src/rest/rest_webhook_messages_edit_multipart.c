#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"
#include "internal/rest/dcc_rest_request_payload_multipart_internal.h"
#include "internal/rest/dcc_rest_request_webhooks_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_modify_webhook_message_multipart(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_webhook_token_path(&path, webhook_id, webhook_token, "messages", message_id);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_request_payload_files_multipart(
        client,
        DCC_REST_PATCH,
        path,
        query,
        payload_json,
        files,
        file_count,
        cb,
        user_data
    );
    free(path);
    return status;
}

dcc_status_t dcc_rest_modify_webhook_message_multipart_thread(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_u64(&query, "thread_id", thread_id);
    if (status == DCC_OK) {
        status = dcc_rest_modify_webhook_message_multipart(
            client,
            webhook_id,
            webhook_token,
            message_id,
            query.data,
            payload_json,
            files,
            file_count,
            cb,
            user_data
        );
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}
