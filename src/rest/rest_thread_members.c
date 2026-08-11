#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stdlib.h>

static dcc_status_t member_submit(
    dcc_client_t *client, const char *operation, dcc_rest_method_t method,
    const char *base, dcc_rest_buffer_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
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
        client, operation, method, path, NULL, &resolved,
        DCC_ENDPOINT_PATH_PUBLIC, out_request
    );
    free(path);
    return status;
}

static dcc_status_t no_query_member(
    dcc_client_t *client, const char *operation, dcc_rest_method_t method,
    dcc_snowflake_t thread_id, const char *member,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    if (out_request != NULL) *out_request = NULL;
    if (thread_id == 0U) return DCC_ERR_INVALID_ARG;
    char base[128];
    dcc_status_t status = dcc_rest_format_path(
        base, sizeof(base), "/channels/%llu/thread-members/%s",
        (unsigned long long)thread_id, member
    );
    dcc_rest_buffer_t query = {0};
    return status == DCC_OK ? member_submit(
        client, operation, method, base, &query, options, out_request
    ) : status;
}

dcc_status_t dcc_rest_join_thread(
    dcc_client_t *client, dcc_snowflake_t thread_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_CURRENT_USER_JOIN_THREAD, DCC_REST_PUT); return no_query_member(client, "dcc_rest_join_thread", DCC_REST_PUT,
    thread_id, "@me", options, out_request); }

dcc_status_t dcc_rest_leave_thread(
    dcc_client_t *client, dcc_snowflake_t thread_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_CURRENT_USER_LEAVE_THREAD, DCC_REST_DELETE); return no_query_member(client, "dcc_rest_leave_thread", DCC_REST_DELETE,
    thread_id, "@me", options, out_request); }

static dcc_status_t member_id_call(
    dcc_client_t *client, const char *operation, dcc_rest_method_t method,
    dcc_snowflake_t thread_id, dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    if (out_request != NULL) *out_request = NULL;
    if (thread_id == 0U || user_id == 0U) return DCC_ERR_INVALID_ARG;
    char base[128];
    dcc_status_t status = dcc_rest_format_path(
        base, sizeof(base), "/channels/%llu/thread-members/%llu",
        (unsigned long long)thread_id, (unsigned long long)user_id
    );
    dcc_rest_buffer_t query = {0};
    return status == DCC_OK ? member_submit(
        client, operation, method, base, &query, options, out_request
    ) : status;
}

dcc_status_t dcc_rest_add_thread_member(
    dcc_client_t *client, dcc_snowflake_t thread_id, dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_THREAD_MEMBER_ADD, DCC_REST_PUT); return member_id_call(client, "dcc_rest_add_thread_member", DCC_REST_PUT,
    thread_id, user_id, options, out_request); }

dcc_status_t dcc_rest_remove_thread_member(
    dcc_client_t *client, dcc_snowflake_t thread_id, dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_THREAD_MEMBER_REMOVE, DCC_REST_DELETE); return member_id_call(client, "dcc_rest_remove_thread_member", DCC_REST_DELETE,
    thread_id, user_id, options, out_request); }

static dcc_status_t member_query_validate(
    const dcc_rest_thread_member_query_t *query, int list,
    dcc_rest_buffer_t *text
) {
    if (query == NULL) return DCC_OK;
    const uint64_t allowed = list ?
        (DCC_REST_THREAD_MEMBER_QUERY_PRESENT_WITH_MEMBER |
         DCC_REST_THREAD_MEMBER_QUERY_PRESENT_AFTER |
         DCC_REST_THREAD_MEMBER_QUERY_PRESENT_LIMIT) :
        DCC_REST_THREAD_MEMBER_QUERY_PRESENT_WITH_MEMBER;
    dcc_endpoint_record_view_t view;
    if (dcc_endpoint_record_read(
            query, offsetof(dcc_rest_thread_member_query_t, version),
            offsetof(dcc_rest_thread_member_query_t, present),
            DCC_ENDPOINT_FIELD_END(dcc_rest_thread_member_query_t, present),
            DCC_REST_THREAD_MEMBER_QUERY_VERSION, allowed, &view
        ) != DCC_OK ||
        dcc_endpoint_field_partially_covered(
            view.size, offsetof(dcc_rest_thread_member_query_t, with_member),
            sizeof(query->with_member)) ||
        dcc_endpoint_field_partially_covered(
            view.size, offsetof(dcc_rest_thread_member_query_t, after),
            sizeof(query->after)) ||
        dcc_endpoint_field_partially_covered(
            view.size, offsetof(dcc_rest_thread_member_query_t, limit),
            sizeof(query->limit)) ||
        !dcc_endpoint_present_field_covered(&view,
            DCC_REST_THREAD_MEMBER_QUERY_PRESENT_WITH_MEMBER,
            offsetof(dcc_rest_thread_member_query_t, with_member),
            sizeof(query->with_member)) ||
        !dcc_endpoint_present_field_covered(&view,
            DCC_REST_THREAD_MEMBER_QUERY_PRESENT_AFTER,
            offsetof(dcc_rest_thread_member_query_t, after),
            sizeof(query->after)) ||
        !dcc_endpoint_present_field_covered(&view,
            DCC_REST_THREAD_MEMBER_QUERY_PRESENT_LIMIT,
            offsetof(dcc_rest_thread_member_query_t, limit),
            sizeof(query->limit)) ||
        ((view.present & DCC_REST_THREAD_MEMBER_QUERY_PRESENT_WITH_MEMBER) != 0U &&
            query->with_member > 1U) ||
        ((view.present & DCC_REST_THREAD_MEMBER_QUERY_PRESENT_AFTER) != 0U && query->after == 0U) ||
        ((view.present & DCC_REST_THREAD_MEMBER_QUERY_PRESENT_LIMIT) != 0U &&
            (query->limit == 0U || query->limit > 100U)) ||
        ((view.present & (DCC_REST_THREAD_MEMBER_QUERY_PRESENT_AFTER |
            DCC_REST_THREAD_MEMBER_QUERY_PRESENT_LIMIT)) != 0U &&
            (((view.present & DCC_REST_THREAD_MEMBER_QUERY_PRESENT_WITH_MEMBER) == 0U) || !query->with_member)))
        return DCC_ERR_INVALID_ARG;
    dcc_status_t status = DCC_OK;
    if ((view.present & DCC_REST_THREAD_MEMBER_QUERY_PRESENT_WITH_MEMBER) != 0U)
        status = dcc_rest_query_append_bool(text, "with_member", query->with_member);
    if (status == DCC_OK && (view.present & DCC_REST_THREAD_MEMBER_QUERY_PRESENT_AFTER) != 0U)
        status = dcc_rest_query_append_u64_value(text, "after", query->after);
    if (status == DCC_OK && (view.present & DCC_REST_THREAD_MEMBER_QUERY_PRESENT_LIMIT) != 0U)
        status = dcc_rest_query_append_u64_value(text, "limit", query->limit);
    return status;
}

dcc_status_t dcc_rest_get_thread_member(
    dcc_client_t *client, dcc_snowflake_t thread_id, dcc_snowflake_t user_id,
    const dcc_rest_thread_member_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_THREAD_MEMBER_GET, DCC_REST_GET);
    if (out_request != NULL) *out_request = NULL;
    if (thread_id == 0U || user_id == 0U) return DCC_ERR_INVALID_ARG;
    char base[128];
    dcc_status_t status = dcc_rest_format_path(
        base, sizeof(base), "/channels/%llu/thread-members/%llu",
        (unsigned long long)thread_id, (unsigned long long)user_id
    );
    dcc_rest_buffer_t text = {0};
    if (status == DCC_OK) status = member_query_validate(query, 0, &text);
    if (status == DCC_OK) status = member_submit(
        client, "dcc_rest_get_thread_member", DCC_REST_GET, base, &text,
        options, out_request
    );
    dcc_rest_buffer_deinit(&text);
    return status;
}

dcc_status_t dcc_rest_get_thread_members(
    dcc_client_t *client, dcc_snowflake_t thread_id,
    const dcc_rest_thread_member_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_THREAD_MEMBERS_GET, DCC_REST_GET);
    if (out_request != NULL) *out_request = NULL;
    if (thread_id == 0U) return DCC_ERR_INVALID_ARG;
    char base[96];
    dcc_status_t status = dcc_rest_format_path(
        base, sizeof(base), "/channels/%llu/thread-members",
        (unsigned long long)thread_id
    );
    dcc_rest_buffer_t text = {0};
    if (status == DCC_OK) status = member_query_validate(query, 1, &text);
    if (status == DCC_OK) status = member_submit(
        client, "dcc_rest_get_thread_members", DCC_REST_GET, base, &text,
        options, out_request
    );
    dcc_rest_buffer_deinit(&text);
    return status;
}
