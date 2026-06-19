#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <string.h>

dcc_status_t dcc_rest_request(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
) {
    size_t body_len = body != NULL ? strlen(body) : 0;
    return dcc_rest_request_raw_impl(
        client,
        method,
        path,
        body,
        body_len,
        body_len != 0 ? "application/json" : NULL,
        DCC_REST_MAX_RATE_LIMIT_RETRIES,
        1,
        cb,
        user_data,
        NULL,
        NULL,
        NULL
    );
}

dcc_status_t dcc_rest_request_raw(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_raw_impl(
        client,
        method,
        path,
        body,
        body_len,
        content_type,
        DCC_REST_MAX_RATE_LIMIT_RETRIES,
        1,
        cb,
        user_data,
        NULL,
        NULL,
        NULL
    );
}

dcc_status_t dcc_rest_request_method(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
) {
    const char *method_name = dcc_rest_method_name(method);
    if (method_name == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_request(client, method_name, path, body, cb, user_data);
}

dcc_status_t dcc_rest_request_method_raw(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    dcc_rest_cb cb,
    void *user_data
) {
    const char *method_name = dcc_rest_method_name(method);
    if (method_name == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_request_raw(client, method_name, path, body, body_len, content_type, cb, user_data);
}
