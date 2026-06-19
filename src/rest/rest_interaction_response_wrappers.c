#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_interaction_response_create_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t interaction_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_callback_fields(interaction, &interaction_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_response_create(client, interaction_id, token, json_body, cb, user_data)
        : status;
}

dcc_status_t dcc_rest_interaction_response_create_from_interaction_message_builder(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t interaction_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_callback_fields(interaction, &interaction_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_response_create_message_builder(client, interaction_id, token, type, message, cb, user_data)
        : status;
}

dcc_status_t dcc_rest_interaction_response_create_type_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_interaction_response_type_t type,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t interaction_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_callback_fields(interaction, &interaction_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_response_create_type(client, interaction_id, token, type, cb, user_data)
        : status;
}

dcc_status_t dcc_rest_interaction_response_create_modal_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_modal_builder_t *modal,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t interaction_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_callback_fields(interaction, &interaction_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_response_create_modal(client, interaction_id, token, modal, cb, user_data)
        : status;
}

dcc_status_t dcc_rest_interaction_response_create_autocomplete_from_interaction(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_autocomplete_builder_t *autocomplete,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t interaction_id = 0;
    const char *token = NULL;
    dcc_status_t status = dcc_rest_interaction_require_callback_fields(interaction, &interaction_id, &token);
    return status == DCC_OK
        ? dcc_rest_interaction_response_create_autocomplete(client, interaction_id, token, autocomplete, cb, user_data)
        : status;
}
