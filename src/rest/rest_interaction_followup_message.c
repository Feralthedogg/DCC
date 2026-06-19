#include "internal/rest/dcc_rest_request_core_internal.h"
#include "internal/rest/dcc_rest_request_interactions_internal.h"

dcc_status_t dcc_rest_interaction_followup_get(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_interaction_webhook_path(&path, application_id, interaction_token, "messages", message_id);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_interaction_followup_edit(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_interaction_webhook_path(&path, application_id, interaction_token, "messages", message_id);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_interaction_followup_edit_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_message_builder_build_json(message, &json);
    if (status == DCC_OK) {
        status = dcc_rest_interaction_followup_edit(
            client,
            application_id,
            interaction_token,
            message_id,
            json,
            cb,
            user_data
        );
    }
    dcc_message_builder_json_free(json);
    return status;
}

dcc_status_t dcc_rest_interaction_followup_delete(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_interaction_webhook_path(&path, application_id, interaction_token, "messages", message_id);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
