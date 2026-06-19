#include "internal/rest/dcc_rest_request_core_internal.h"
#include "internal/rest/dcc_rest_request_interactions_internal.h"

dcc_status_t dcc_rest_interaction_original_response_get(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
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
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_interaction_original_response_delete(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
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
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
