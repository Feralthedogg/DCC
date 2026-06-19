#include "internal/rest/dcc_rest_request_core_internal.h"
#include "internal/rest/dcc_rest_request_interactions_internal.h"
#include "internal/rest/dcc_rest_request_payload_multipart_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_interaction_original_response_edit(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_interaction_webhook_path(
        &path,
        application_id,
        interaction_token,
        "messages/@original",
        0
    );
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_interaction_original_response_edit_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &json);
    if (status == DCC_OK) {
        status = dcc_rest_interaction_original_response_edit(
            client,
            application_id,
            interaction_token,
            json,
            cb,
            user_data
        );
    }
    dcc_message_builder_json_free(json);
    return status;
}

dcc_status_t dcc_rest_interaction_original_response_edit_multipart(
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
    dcc_status_t status = dcc_rest_interaction_webhook_path(
        &path,
        application_id,
        interaction_token,
        "messages/@original",
        0
    );
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
