#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_message_reactions(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    const dcc_rest_reaction_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_CHANNEL_MESSAGE_REACTIONS_EMOJI, DCC_REST_GET);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U || message_id == 0U ||
        reaction == NULL || reaction[0] == '\0')
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_endpoint_record_view_t view;
    status = dcc_endpoint_reaction_query_preflight(query, &view);
    if (status != DCC_OK) return status;
    dcc_rest_buffer_t text = {0};
    if (query != NULL && (view.present & DCC_REST_REACTION_QUERY_PRESENT_TYPE) != 0U)
        status = dcc_rest_query_append_u64_value(&text, "type", (uint64_t)query->type);
    if (status == DCC_OK && query != NULL && (view.present & DCC_REST_REACTION_QUERY_PRESENT_AFTER) != 0U)
        status = dcc_rest_query_append_u64_value(&text, "after", query->after);
    if (status == DCC_OK && query != NULL && (view.present & DCC_REST_REACTION_QUERY_PRESENT_LIMIT) != 0U)
        status = dcc_rest_query_append_u64_value(&text, "limit", query->limit);
    char *escaped = NULL;
    char *base = NULL;
    char *path = NULL;
    if (status == DCC_OK) status = dcc_rest_escape_path_segment(reaction, &escaped);
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(
        &base, DCC_REST_ROUTE_CHANNEL_MESSAGE_REACTIONS_EMOJI,
        (unsigned long long)channel_id, (unsigned long long)message_id, escaped
    );
    if (status == DCC_OK) status = dcc_endpoint_path_with_query(base, &text, &path);
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_GET, path, NULL, &resolved, out_request
    );
    free(escaped);
    free(base);
    free(path);
    dcc_rest_buffer_deinit(&text);
    return status;
}
