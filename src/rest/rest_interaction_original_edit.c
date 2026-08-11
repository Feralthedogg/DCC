#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_interaction_original_response_edit(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const dcc_rest_message_payload_t *payload,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || application_id == 0U ||
        interaction_token == NULL || interaction_token[0] == '\0')
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_endpoint_body_t body = {0};
    status = dcc_endpoint_build_message_body(payload, &body);
    char *token = NULL;
    char *path = NULL;
    if (status == DCC_OK) status = dcc_rest_escape_path_segment(interaction_token, &token);
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(
        &path, DCC_REST_ROUTE_INTERACTION_ORIGINAL_RESPONSE,
        (unsigned long long)application_id, token
    );
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_PATCH, path, &body, &resolved, out_request
    );
    free(token);
    free(path);
    dcc_endpoint_body_deinit(&body);
    return status;
}
