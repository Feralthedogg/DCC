#include "internal/hot_reload/dcc_hot_reload_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"

static dcc_status_t dcc_worker_rest_intercept(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    dcc_rest_cb cb,
    void *user_data,
    void *intercept_user_data
) {
    dcc_hot_reload_worker_capture_t *capture =
        (dcc_hot_reload_worker_capture_t *)intercept_user_data;
    dcc_status_t status =
        dcc_hot_reload_worker_capture_push(capture, method, path, body, body_len, content_type);
    if (cb != NULL) {
        const char ok_body[] = "{\"captured\":true}";
        dcc_rest_response_t response = {
            .size = sizeof(response),
            .status = status == DCC_OK ? 202U : 0U,
            .error = status,
            .body = status == DCC_OK ? ok_body : NULL,
            .body_len = status == DCC_OK ? sizeof(ok_body) - 1U : 0U,
        };
        cb(client, &response, user_data);
    }
    return status;
}

void dcc_hot_reload_worker_capture_start(
    dcc_client_t *client,
    dcc_hot_reload_worker_capture_t *capture
) {
    dcc_rest_set_interceptor(client, dcc_worker_rest_intercept, capture);
}

void dcc_hot_reload_worker_capture_stop(dcc_client_t *client) {
    dcc_rest_set_interceptor(client, NULL, NULL);
}
