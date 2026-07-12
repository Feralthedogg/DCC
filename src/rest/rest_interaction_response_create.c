#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_interaction_response_create(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_interaction_response_create_options(
        client,
        interaction_id,
        interaction_token,
        json_body,
        0U,
        cb,
        user_data
    );
}

dcc_status_t dcc_rest_interaction_response_create_options(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    const char *json_body,
    uint8_t with_response,
    dcc_rest_cb cb,
    void *user_data
) {
    if (with_response > 1U) {
        return DCC_ERR_INVALID_ARG;
    }
    char *token = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(interaction_token, &token);
    if (status != DCC_OK) {
        return status;
    }
    char *path = NULL;
    status = dcc_rest_alloc_formatted_path(
        &path,
        with_response
            ? "/interactions/%llu/%s/callback?with_response=true"
            : "/interactions/%llu/%s/callback",
        (unsigned long long)interaction_id,
        token
    );
    free(token);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}
