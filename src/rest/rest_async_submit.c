#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_async_drain_internal.h"
#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_async_signal_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"

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
    return client->rest_intercept(
        client,
        method,
        path,
        body,
        body_len,
        body_len != 0U ? "application/json" : NULL,
        cb,
        user_data,
        client->rest_intercept_user_data
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
    if (client->rest_intercept != NULL) {
        return dcc_rest_async_intercept(client, method, path, body, cb, user_data);
    }
    if (!client->runtime.initialized || atomic_load_explicit(&client->stopping, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }

    dcc_rest_async_request_t *request = dcc_rest_async_request_new(
        client,
        method,
        path,
        body,
        priority,
        cb,
        user_data
    );
    if (request == NULL) {
        return DCC_ERR_NOMEM;
    }

    dcc_rest_lock(client);
    dcc_rest_async_push_tail_locked(client, request);
    dcc_status_t status = dcc_rest_async_drain_locked(client);
    dcc_rest_unlock(client);
    dcc_rest_async_signal(client);

    return status;
}
