#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_join_thread(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu/thread-members/@me", (unsigned long long)thread_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_leave_thread(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu/thread-members/@me", (unsigned long long)thread_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_add_thread_member(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/thread-members/%llu",
        (unsigned long long)thread_id,
        (unsigned long long)user_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_add_thread_member_params(
    dcc_client_t *client,
    const dcc_thread_member_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL || params->size < sizeof(*params) || params->thread_id == 0 || params->user_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_add_thread_member(client, params->thread_id, params->user_id, cb, user_data);
}

dcc_status_t dcc_rest_remove_thread_member(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/thread-members/%llu",
        (unsigned long long)thread_id,
        (unsigned long long)user_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_thread_member(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/thread-members/%llu",
        (unsigned long long)thread_id,
        (unsigned long long)user_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_thread_members(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu/thread-members", (unsigned long long)thread_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}
