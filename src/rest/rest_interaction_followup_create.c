#include "internal/rest/dcc_rest_request_core_internal.h"
#include "internal/rest/dcc_rest_request_interactions_internal.h"

dcc_status_t dcc_rest_interaction_followup_create(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_interaction_webhook_path(&path, application_id, interaction_token, NULL, 0);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_interaction_followup_create_builder(
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
        status = dcc_rest_interaction_followup_create(client, application_id, interaction_token, json, cb, user_data);
    }
    dcc_message_builder_json_free(json);
    return status;
}
