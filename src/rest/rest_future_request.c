#include "internal/rest/dcc_rest_future_internal.h"

#include <stdlib.h>

static void dcc_rest_future_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    dcc_rest_future_t *future = (dcc_rest_future_t *)user_data;
    if (future == NULL || response == NULL) {
        return;
    }

    dcc_rest_future_store_response(future, response);
    dcc_rest_future_mark_completed(future);
}

dcc_status_t dcc_rest_request_future(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_future_t **out
) {
    return dcc_rest_request_future_priority(client, method, path, body, DCC_REST_PRIORITY_NORMAL, out);
}

dcc_status_t dcc_rest_request_future_priority(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_priority_t priority,
    dcc_rest_future_t **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (client == NULL || method == NULL || path == NULL || !dcc_rest_priority_valid(priority)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!client->runtime.initialized || atomic_load_explicit(&client->stopping, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }

    dcc_rest_future_t *future = NULL;
    dcc_status_t status = dcc_rest_future_create(&future);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_rest_request_async_priority(
        client,
        method,
        path,
        body,
        priority,
        dcc_rest_future_cb,
        future
    );
    if (status != DCC_OK) {
        dcc_rest_future_deinit_completed(future);
        free(future);
        return status;
    }

    *out = future;
    return DCC_OK;
}

dcc_status_t dcc_rest_request_method_future(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *body,
    dcc_rest_future_t **out
) {
    return dcc_rest_request_method_future_priority(client, method, path, body, DCC_REST_PRIORITY_NORMAL, out);
}

dcc_status_t dcc_rest_request_method_future_priority(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *body,
    dcc_rest_priority_t priority,
    dcc_rest_future_t **out
) {
    const char *method_name = dcc_rest_method_name(method);
    if (method_name == NULL) {
        if (out != NULL) {
            *out = NULL;
        }
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_request_future_priority(client, method_name, path, body, priority, out);
}
