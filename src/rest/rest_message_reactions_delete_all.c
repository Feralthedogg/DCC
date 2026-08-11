#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_delete_all_message_reactions(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U || message_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_rest_alloc_formatted_path(
        &path, DCC_REST_ROUTE_CHANNEL_MESSAGE_REACTIONS,
        (unsigned long long)channel_id, (unsigned long long)message_id
    );
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_DELETE, path, NULL, &resolved, out_request
    );
    free(path);
    return status;
}

dcc_status_t dcc_rest_delete_all_message_reactions_for_emoji(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U || message_id == 0U ||
        reaction == NULL || reaction[0] == '\0')
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *escaped = NULL;
    char *path = NULL;
    status = dcc_rest_escape_path_segment(reaction, &escaped);
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(
        &path, DCC_REST_ROUTE_CHANNEL_MESSAGE_REACTIONS_EMOJI,
        (unsigned long long)channel_id, (unsigned long long)message_id, escaped
    );
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_DELETE, path, NULL, &resolved, out_request
    );
    free(escaped);
    free(path);
    return status;
}
