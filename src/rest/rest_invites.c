#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_guild_invites(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_GUILD_GET_INVITES, DCC_REST_GET);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_BOT,
        DCC_ENDPOINT_AUDIT_REASON_DENIED
    );
    if (status != DCC_OK || client == NULL || guild_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char path[80];
    status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/invites",
        (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_endpoint_submit_named(
        client, "dcc_rest_get_guild_invites", DCC_REST_GET, path, NULL,
        &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request
    ) : status;
}

dcc_status_t dcc_rest_get_invite(
    dcc_client_t *client, const char *invite_code,
    const dcc_rest_invite_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_NONE_OR_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_INVITE_GET, DCC_REST_GET);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_NONE_OR_BOT,
        DCC_ENDPOINT_AUDIT_REASON_DENIED
    );
    if (status != DCC_OK || client == NULL || invite_code == NULL || invite_code[0] == '\0')
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_endpoint_record_view_t query_view = {0};
    if (query != NULL &&
        (dcc_endpoint_record_read(
            query, offsetof(dcc_rest_invite_query_t, version),
            offsetof(dcc_rest_invite_query_t, present),
            DCC_ENDPOINT_FIELD_END(dcc_rest_invite_query_t, present),
            DCC_REST_INVITE_QUERY_VERSION,
            DCC_REST_INVITE_QUERY_PRESENT_WITH_COUNTS |
                DCC_REST_INVITE_QUERY_PRESENT_GUILD_SCHEDULED_EVENT_ID,
            &query_view
        ) != DCC_OK ||
         dcc_endpoint_field_partially_covered(
             query_view.size, offsetof(dcc_rest_invite_query_t, with_counts),
             sizeof(query->with_counts)
         ) ||
         dcc_endpoint_field_partially_covered(
             query_view.size,
             offsetof(dcc_rest_invite_query_t, guild_scheduled_event_id),
             sizeof(query->guild_scheduled_event_id)
         ) ||
         !dcc_endpoint_present_field_covered(
             &query_view, DCC_REST_INVITE_QUERY_PRESENT_WITH_COUNTS,
             offsetof(dcc_rest_invite_query_t, with_counts),
             sizeof(query->with_counts)
         ) ||
         !dcc_endpoint_present_field_covered(
             &query_view,
             DCC_REST_INVITE_QUERY_PRESENT_GUILD_SCHEDULED_EVENT_ID,
             offsetof(dcc_rest_invite_query_t, guild_scheduled_event_id),
             sizeof(query->guild_scheduled_event_id)
         ) ||
         ((query_view.present & DCC_REST_INVITE_QUERY_PRESENT_WITH_COUNTS) != 0U &&
            query->with_counts > 1U) ||
         ((query_view.present & DCC_REST_INVITE_QUERY_PRESENT_GUILD_SCHEDULED_EVENT_ID) != 0U &&
            query->guild_scheduled_event_id == 0U))) return DCC_ERR_INVALID_ARG;

    char *code = NULL;
    char *base = NULL;
    char *path = NULL;
    dcc_rest_buffer_t query_text = {0};
    status = dcc_rest_escape_path_segment(invite_code, &code);
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(&base, "/invites/%s", code);
    if (status == DCC_OK && query != NULL &&
        (query_view.present & DCC_REST_INVITE_QUERY_PRESENT_WITH_COUNTS) != 0U)
        status = dcc_rest_query_append_bool(&query_text, "with_counts", query->with_counts);
    if (status == DCC_OK && query != NULL &&
        (query_view.present & DCC_REST_INVITE_QUERY_PRESENT_GUILD_SCHEDULED_EVENT_ID) != 0U)
        status = dcc_rest_query_append_u64_value(
            &query_text, "guild_scheduled_event_id", query->guild_scheduled_event_id
        );
    if (status == DCC_OK) status = dcc_endpoint_path_with_query(base, &query_text, &path);
    if (status == DCC_OK) status = dcc_endpoint_submit_named(
        client, "dcc_rest_get_invite", DCC_REST_GET, path, NULL, &resolved,
        DCC_ENDPOINT_PATH_PUBLIC, out_request
    );
    free(code);
    free(base);
    free(path);
    dcc_rest_buffer_deinit(&query_text);
    return status;
}

dcc_status_t dcc_rest_delete_invite(
    dcc_client_t *client, const char *invite_code,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_INVITE_DELETE, DCC_REST_DELETE);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_BOT,
        DCC_ENDPOINT_AUDIT_REASON_ALLOWED
    );
    if (status != DCC_OK || client == NULL || invite_code == NULL || invite_code[0] == '\0')
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *code = NULL;
    char *path = NULL;
    status = dcc_rest_escape_path_segment(invite_code, &code);
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(&path, "/invites/%s", code);
    if (status == DCC_OK) status = dcc_endpoint_submit_named(
        client, "dcc_rest_delete_invite", DCC_REST_DELETE, path, NULL,
        &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request
    );
    free(code);
    free(path);
    return status;
}
