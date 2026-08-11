#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_async_drain_internal.h"
#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_async_signal_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_request_raw_internal.h"
#include "internal/rest/dcc_rest_state_internal.h"

#include <string.h>

static dcc_status_t dcc_rest_async_intercept(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
) {
    size_t body_len = body != NULL ? strlen(body) : 0U;
    return dcc_rest_request_raw_impl(
        client,
        method,
        path,
        body,
        body_len,
        body_len != 0U ? "application/json" : NULL,
        DCC_REST_AUTH_DEFAULT,
        NULL,
        NULL,
        0U,
        0,
        cb,
        user_data,
        NULL,
        NULL,
        NULL,
        1,
        1,
        path,
        0U
    );
}

dcc_status_t dcc_rest_request_async_priority(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_priority_t priority,
    dcc_rest_cb cb,
    void *user_data
) {
    if (client == NULL || method == NULL || path == NULL || !dcc_rest_priority_valid(priority)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t operation_status = dcc_rest_operation_begin(client);
    if (operation_status != DCC_OK) {
        return operation_status;
    }
    if (client->rest_intercept != NULL) {
        dcc_status_t status = dcc_rest_async_intercept(
            client,
            method,
            path,
            body,
            cb,
            user_data
        );
        dcc_rest_operation_end(client);
        return status;
    }
    if (!client->runtime.initialized || atomic_load_explicit(&client->stopping, memory_order_acquire)) {
        dcc_rest_operation_end(client);
        return DCC_ERR_STATE;
    }

    dcc_rest_async_request_t *request = dcc_rest_async_request_new(
        client,
        method,
        path,
        path,
        body,
        body != NULL ? strlen(body) : 0U,
        body != NULL ? "application/json" : NULL,
        NULL,
        DCC_REST_AUTH_DEFAULT,
        NULL,
        0U,
        0U,
        priority,
        cb,
        user_data,
        NULL
    );
    if (request == NULL) {
        dcc_rest_operation_end(client);
        return DCC_ERR_NOMEM;
    }

    dcc_rest_lock(client);
    dcc_rest_async_push_tail_locked(client, request);
    dcc_rest_async_request_t *rejected = NULL;
    dcc_status_t status = dcc_rest_async_drain_admission_locked(
        client,
        request,
        &rejected
    );
    dcc_rest_unlock(client);
    dcc_rest_async_signal(client);

    dcc_rest_async_request_free(rejected);

    dcc_rest_operation_end(client);

    return status;
}
