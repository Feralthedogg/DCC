#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_interaction_followup_get_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t application_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_webhook_fields(interaction, &application_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_followup_get(client, application_id, token, message_id, cb, user_data)
        : status;
}

dcc_status_t dcc_rest_interaction_followup_edit_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_snowflake_t message_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t application_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_webhook_fields(interaction, &application_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_followup_edit(client, application_id, token, message_id, json_body, cb, user_data)
        : status;
}

dcc_status_t dcc_rest_interaction_followup_edit_from_interaction_builder(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_snowflake_t message_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t application_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_webhook_fields(interaction, &application_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_followup_edit_builder(client, application_id, token, message_id, message, cb, user_data)
        : status;
}

dcc_status_t dcc_rest_interaction_followup_delete_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t application_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_webhook_fields(interaction, &application_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_followup_delete(client, application_id, token, message_id, cb, user_data)
        : status;
}
