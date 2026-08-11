#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>

static dcc_status_t dcc_reaction_path(
    char **out,
    const char *route,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_snowflake_t user_id
) {
    if (reaction == NULL || reaction[0] == '\0') return DCC_ERR_INVALID_ARG;
    char *escaped = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(reaction, &escaped);
    if (status == DCC_OK) {
        status = user_id == 0U
            ? dcc_rest_alloc_formatted_path(out, route,
                (unsigned long long)channel_id, (unsigned long long)message_id,
                escaped)
            : dcc_rest_alloc_formatted_path(out, route,
                (unsigned long long)channel_id, (unsigned long long)message_id,
                escaped, (unsigned long long)user_id);
    }
    free(escaped);
    return status;
}

dcc_status_t dcc_rest_add_message_reaction(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id, const char *reaction,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U || message_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_reaction_path(&path, DCC_REST_ROUTE_MESSAGE_REACTION_SELF,
        channel_id, message_id, reaction, 0U);
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_PUT, path, NULL, &resolved, out_request);
    free(path);
    return status;
}

dcc_status_t dcc_rest_delete_own_message_reaction(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id, const char *reaction,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U || message_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_reaction_path(&path, DCC_REST_ROUTE_MESSAGE_REACTION_SELF,
        channel_id, message_id, reaction, 0U);
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_DELETE, path, NULL, &resolved, out_request);
    free(path);
    return status;
}

dcc_status_t dcc_rest_delete_user_message_reaction(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id, const char *reaction, dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U ||
        message_id == 0U || user_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_reaction_path(&path, DCC_REST_ROUTE_MESSAGE_REACTION_USER,
        channel_id, message_id, reaction, user_id);
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_DELETE, path, NULL, &resolved, out_request);
    free(path);
    return status;
}
