#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_query_webhooks_internal.h"
#include "internal/rest/dcc_rest_request_payload_multipart_internal.h"
#include "internal/rest/dcc_rest_request_webhooks_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_execute_webhook_multipart(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *query,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_webhook_token_path(&path, webhook_id, webhook_token, NULL, 0);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_request_payload_files_multipart(
        client,
        DCC_REST_POST,
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

dcc_status_t dcc_rest_execute_webhook_multipart_options(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_webhook_options_query(&query, wait, thread_id, with_components);
    if (status == DCC_OK) {
        status = dcc_rest_execute_webhook_multipart(
            client,
            webhook_id,
            webhook_token,
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
