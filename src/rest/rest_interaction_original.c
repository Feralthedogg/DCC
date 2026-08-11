#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>

static dcc_status_t dcc_interaction_original_path(
    char **out,
    const char *route,
    dcc_snowflake_t application_id,
    const char *interaction_token
) {
    if (application_id == 0U || interaction_token == NULL || interaction_token[0] == '\0')
        return DCC_ERR_INVALID_ARG;
    char *token = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(interaction_token, &token);
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(
        out, route,
        (unsigned long long)application_id, token
    );
    free(token);
    return status;
}

dcc_status_t dcc_rest_interaction_original_response_get(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || application_id == 0U ||
        interaction_token == NULL || interaction_token[0] == '\0')
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_interaction_original_path(
        &path, DCC_REST_ROUTE_INTERACTION_ORIGINAL_RESPONSE,
        application_id, interaction_token
    );
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_GET, path, NULL, &resolved, out_request
    );
    free(path);
    return status;
}

dcc_status_t dcc_rest_interaction_original_response_delete(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || application_id == 0U ||
        interaction_token == NULL || interaction_token[0] == '\0')
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_interaction_original_path(
        &path, DCC_REST_ROUTE_INTERACTION_ORIGINAL_RESPONSE,
        application_id, interaction_token
    );
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_DELETE, path, NULL, &resolved, out_request
    );
    free(path);
    return status;
}
