#include <dcc/rest.h>

dcc_status_t dcc_rest_request_async(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_async_priority(
        client,
        method,
        path,
        body,
        DCC_REST_PRIORITY_NORMAL,
        cb,
        user_data
    );
}

dcc_status_t dcc_rest_request_method_async(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_method_async_priority(
        client,
        method,
        path,
        body,
        DCC_REST_PRIORITY_NORMAL,
        cb,
        user_data
    );
}

dcc_status_t dcc_rest_request_method_async_priority(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *body,
    dcc_rest_priority_t priority,
    dcc_rest_cb cb,
    void *user_data
) {
    const char *method_name = dcc_rest_method_name(method);
    if (method_name == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_request_async_priority(client, method_name, path, body, priority, cb, user_data);
}
