#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_interaction_followup_create_multipart_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t application_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_webhook_fields(interaction, &application_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_followup_create_multipart(
            client,
            application_id,
            token,
            payload_json,
            files,
            file_count,
            cb,
            user_data
        )
        : status;
}

dcc_status_t dcc_rest_interaction_followup_edit_multipart_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_snowflake_t message_id,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t application_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_webhook_fields(interaction, &application_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_followup_edit_multipart(
            client,
            application_id,
            token,
            message_id,
            payload_json,
            files,
            file_count,
            cb,
            user_data
        )
        : status;
}
