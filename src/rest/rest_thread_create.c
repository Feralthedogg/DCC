#include "internal/rest/dcc_rest_threads_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_create_thread(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu/threads", (unsigned long long)channel_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_create_thread_params(
    dcc_client_t *client,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_thread_body(params, DCC_REST_THREAD_BODY_CREATE, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_thread(client, params->channel_id, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_create_forum_thread_params(
    dcc_client_t *client,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_thread_body(params, DCC_REST_THREAD_BODY_CREATE_FORUM, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_thread(client, params->channel_id, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_create_thread_from_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/messages/%llu/threads",
        (unsigned long long)channel_id,
        (unsigned long long)message_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_create_thread_from_message_params(
    dcc_client_t *client,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_thread_body(params, DCC_REST_THREAD_BODY_CREATE_FROM_MESSAGE, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_thread_from_message(
            client,
            params->channel_id,
            params->message_id,
            body,
            cb,
            user_data
        );
    }
    free(body);
    return status;
}
