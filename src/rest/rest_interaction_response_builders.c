#include "internal/rest/dcc_rest_interaction_response_body_internal.h"

#include <dcc/autocomplete.h>
#include <dcc/modal.h>
#include <dcc/rest/interactions/responses.h>

#include <stdlib.h>

dcc_status_t dcc_rest_interaction_response_create_message_builder(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_interaction_response_message_body(type, message, &body);
    if (status == DCC_OK) {
        status = dcc_rest_interaction_response_create(client, interaction_id, interaction_token, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_interaction_response_create_type(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    dcc_interaction_response_type_t type,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_interaction_response_type_body(type, &body);
    if (status == DCC_OK) {
        status = dcc_rest_interaction_response_create(client, interaction_id, interaction_token, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_interaction_response_create_modal(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    const dcc_modal_builder_t *modal,
    dcc_rest_cb cb,
    void *user_data
) {
    if (modal == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *json_body = NULL;
    dcc_status_t status = dcc_modal_builder_build_json(modal, &json_body);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_rest_interaction_response_create(
        client,
        interaction_id,
        interaction_token,
        json_body,
        cb,
        user_data
    );
    dcc_modal_builder_json_free(json_body);
    return status;
}

dcc_status_t dcc_rest_interaction_response_create_autocomplete(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    const dcc_autocomplete_builder_t *autocomplete,
    dcc_rest_cb cb,
    void *user_data
) {
    if (autocomplete == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *json_body = NULL;
    dcc_status_t status = dcc_autocomplete_builder_build_json(autocomplete, &json_body);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_rest_interaction_response_create(
        client,
        interaction_id,
        interaction_token,
        json_body,
        cb,
        user_data
    );
    dcc_autocomplete_builder_json_free(json_body);
    return status;
}
