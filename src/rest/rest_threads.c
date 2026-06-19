#include "internal/rest/dcc_rest_threads_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_thread(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[64];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu", (unsigned long long)thread_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_thread(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[64];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu", (unsigned long long)thread_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_thread_params(
    dcc_client_t *client,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_thread_body(params, DCC_REST_THREAD_BODY_MODIFY, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_thread(client, params->thread_id, body, cb, user_data);
    }
    free(body);
    return status;
}
