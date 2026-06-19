#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_interaction_followup_create_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t application_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_webhook_fields(interaction, &application_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_followup_create(client, application_id, token, json_body, cb, user_data)
        : status;
}

dcc_status_t dcc_rest_interaction_followup_create_from_interaction_builder(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t application_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_webhook_fields(interaction, &application_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_followup_create_builder(client, application_id, token, message, cb, user_data)
        : status;
}
