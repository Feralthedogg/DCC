#include "internal/rest/dcc_rest_request_interactions_internal.h"
#include "internal/rest/dcc_rest_request_payload_multipart_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_interaction_followup_create_multipart(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_interaction_webhook_path(&path, application_id, interaction_token, NULL, 0);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_request_payload_files_multipart(
        client,
        DCC_REST_POST,
        path,
        NULL,
        payload_json,
        files,
        file_count,
        cb,
        user_data
    );
    free(path);
    return status;
}

dcc_status_t dcc_rest_interaction_followup_edit_multipart(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_interaction_webhook_path(&path, application_id, interaction_token, "messages", message_id);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_request_payload_files_multipart(
        client,
        DCC_REST_PATCH,
        path,
        NULL,
        payload_json,
        files,
        file_count,
        cb,
        user_data
    );
    free(path);
    return status;
}
