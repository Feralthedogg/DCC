#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stdlib.h>

static dcc_status_t archive_submit(
    dcc_client_t *client, const char *operation, const char *base,
    dcc_rest_buffer_t *query, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_BOT,
        DCC_ENDPOINT_AUDIT_REASON_DENIED
    );
    if (status != DCC_OK || client == NULL)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_endpoint_path_with_query(base, query, &path);
    if (status == DCC_OK) status = dcc_endpoint_submit_named(
        client, operation, DCC_REST_GET, path, NULL, &resolved,
        DCC_ENDPOINT_PATH_PUBLIC, out_request
    );
    free(path);
    return status;
}

dcc_status_t dcc_rest_get_active_threads(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_THREADS_GET_ACTIVE, DCC_REST_GET);
    if (guild_id == 0U) {
        if (out_request != NULL) *out_request = NULL;
        return DCC_ERR_INVALID_ARG;
    }
    char path[88];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/guilds/%llu/threads/active",
        (unsigned long long)guild_id
    );
    dcc_rest_buffer_t query = {0};
    return status == DCC_OK ? archive_submit(
        client, "dcc_rest_get_active_threads", path, &query, options, out_request
    ) : status;
}

static dcc_status_t archived(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const char *operation, const char *suffix,
    const dcc_rest_thread_archive_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    if (out_request != NULL) *out_request = NULL;
    const uint64_t known = DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE |
        DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT;
    dcc_endpoint_record_view_t view = {0};
    if (channel_id == 0U) return DCC_ERR_INVALID_ARG;
    if (query != NULL &&
        (dcc_endpoint_record_read(
            query, offsetof(dcc_rest_thread_archive_query_t, version),
            offsetof(dcc_rest_thread_archive_query_t, present),
            DCC_ENDPOINT_FIELD_END(dcc_rest_thread_archive_query_t, present),
            DCC_REST_THREAD_ARCHIVE_QUERY_VERSION, known, &view
        ) != DCC_OK ||
         dcc_endpoint_field_partially_covered(
             view.size, offsetof(dcc_rest_thread_archive_query_t, before),
             sizeof(query->before)
         ) ||
         dcc_endpoint_field_partially_covered(
             view.size, offsetof(dcc_rest_thread_archive_query_t, limit),
             sizeof(query->limit)
         ) ||
         !dcc_endpoint_present_field_covered(
             &view, DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE,
             offsetof(dcc_rest_thread_archive_query_t, before),
             sizeof(query->before)
         ) ||
         !dcc_endpoint_present_field_covered(
             &view, DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT,
             offsetof(dcc_rest_thread_archive_query_t, limit),
             sizeof(query->limit)
         ) ||
         ((view.present & DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE) != 0U &&
            (query->before == NULL || query->before[0] == '\0')) ||
         ((view.present & DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT) != 0U &&
            query->limit == 0U))) return DCC_ERR_INVALID_ARG;
    char base[128];
    dcc_status_t status = dcc_rest_format_path(
        base, sizeof(base), "/channels/%llu/%s",
        (unsigned long long)channel_id, suffix
    );
    dcc_rest_buffer_t text = {0};
    if (status == DCC_OK && query != NULL &&
        (view.present & DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE) != 0U)
        status = dcc_rest_query_append_string(&text, "before", query->before);
    if (status == DCC_OK && query != NULL &&
        (view.present & DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT) != 0U)
        status = dcc_rest_query_append_u64_value(&text, "limit", query->limit);
    if (status == DCC_OK) status = archive_submit(
        client, operation, base, &text, options, out_request
    );
    dcc_rest_buffer_deinit(&text);
    return status;
}

dcc_status_t dcc_rest_get_public_archived_threads(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_rest_thread_archive_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_THREADS_GET_PUBLIC_ARCHIVED, DCC_REST_GET);
    return archived(client, channel_id, "dcc_rest_get_public_archived_threads",
        "threads/archived/public", query, options, out_request);
}

dcc_status_t dcc_rest_get_private_archived_threads(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_rest_thread_archive_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_THREADS_GET_PRIVATE_ARCHIVED, DCC_REST_GET);
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    return archived(client, channel_id, "dcc_rest_get_private_archived_threads",
        "threads/archived/private", query, options, out_request);
}

dcc_status_t dcc_rest_get_joined_private_archived_threads(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_rest_joined_thread_archive_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_THREADS_GET_JOINED_PRIVATE_ARCHIVED, DCC_REST_GET);
    if (out_request != NULL) *out_request = NULL;
    const uint64_t known = DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE |
        DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT;
    dcc_endpoint_record_view_t view = {0};
    if (channel_id == 0U) return DCC_ERR_INVALID_ARG;
    if (query != NULL &&
        (dcc_endpoint_record_read(
            query, offsetof(dcc_rest_joined_thread_archive_query_t, version),
            offsetof(dcc_rest_joined_thread_archive_query_t, present),
            DCC_ENDPOINT_FIELD_END(dcc_rest_joined_thread_archive_query_t, present),
            DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_VERSION, known, &view
        ) != DCC_OK ||
         dcc_endpoint_field_partially_covered(
             view.size,
             offsetof(dcc_rest_joined_thread_archive_query_t, before),
             sizeof(query->before)
         ) ||
         dcc_endpoint_field_partially_covered(
             view.size,
             offsetof(dcc_rest_joined_thread_archive_query_t, limit),
             sizeof(query->limit)
         ) ||
         !dcc_endpoint_present_field_covered(
             &view, DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE,
             offsetof(dcc_rest_joined_thread_archive_query_t, before),
             sizeof(query->before)
         ) ||
         !dcc_endpoint_present_field_covered(
             &view, DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT,
             offsetof(dcc_rest_joined_thread_archive_query_t, limit),
             sizeof(query->limit)
         ) ||
         ((view.present & DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE) != 0U && query->before == 0U) ||
         ((view.present & DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT) != 0U && query->limit == 0U)))
        return DCC_ERR_INVALID_ARG;
    char base[128];
    dcc_status_t status = dcc_rest_format_path(
        base, sizeof(base), "/channels/%llu/users/@me/threads/archived/private",
        (unsigned long long)channel_id
    );
    dcc_rest_buffer_t text = {0};
    if (status == DCC_OK && query != NULL &&
        (view.present & DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE) != 0U)
        status = dcc_rest_query_append_u64_value(&text, "before", query->before);
    if (status == DCC_OK && query != NULL &&
        (view.present & DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT) != 0U)
        status = dcc_rest_query_append_u64_value(&text, "limit", query->limit);
    if (status == DCC_OK) status = archive_submit(
        client, "dcc_rest_get_joined_private_archived_threads", base, &text,
        options, out_request
    );
    dcc_rest_buffer_deinit(&text);
    return status;
}
