#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_interaction_response_create(
    dcc_client_t *client,
    dcc_snowflake_t interaction_id,
    const char *interaction_token,
    const dcc_rest_interaction_response_t *response,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_SENSITIVE;
    (void)DCC_ENDPOINT_ROUTE_KEY_OPAQUE;
    DCC_ENDPOINT_SENSITIVE_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_NONE, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_INTERACTION_CALLBACK, DCC_REST_POST, "dcc_rest_interaction_response_create");
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || interaction_id == 0U ||
        interaction_token == NULL || interaction_token[0] == '\0')
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_endpoint_interaction_view_t view;
    status = dcc_endpoint_interaction_response_preflight(response, &view);
    if (status != DCC_OK) return status;
    dcc_endpoint_body_t body = {0};
    status = dcc_endpoint_allocation_probe();
    if (status == DCC_OK) status = dcc_endpoint_build_interaction_body(response, &body);
    dcc_rest_buffer_t query = {0};
    if (status == DCC_OK &&
        (view.record.present & DCC_REST_INTERACTION_RESPONSE_PRESENT_WITH_RESPONSE) != 0U)
        status = dcc_rest_query_append_bool(&query, "with_response", view.with_response);
    char *token = NULL;
    char *base = NULL;
    char *path = NULL;
    if (status == DCC_OK) status = dcc_rest_escape_path_segment(interaction_token, &token);
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(
        &base, DCC_REST_ROUTE_INTERACTION_CALLBACK,
        (unsigned long long)interaction_id, token
    );
    if (status == DCC_OK) status = dcc_endpoint_path_with_query(base, &query, &path);
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_POST, path, &body, &resolved, out_request
    );
    free(token);
    free(base);
    free(path);
    dcc_rest_buffer_deinit(&query);
    dcc_endpoint_body_deinit(&body);
    return status;
}
