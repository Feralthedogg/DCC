#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"

#include <llam/io.h>

#include <stdlib.h>

dcc_rest_async_request_t *dcc_rest_async_request_new(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_priority_t priority,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_async_request_t *request = (dcc_rest_async_request_t *)calloc(1, sizeof(*request));
    if (request == NULL) {
        return NULL;
    }

    request->client = client;
    request->method = dcc_strdup(method);
    request->path = dcc_strdup(path);
    request->body = body != NULL ? dcc_strdup(body) : NULL;
    request->priority = priority;
    dcc_rest_route_key(method, path, request->route, sizeof(request->route));
    request->cb = cb;
    request->user_data = user_data;
    atomic_init(&request->cancel_requested, false);
    atomic_init(&request->active_fd, LLAM_INVALID_FD);

    if (request->method == NULL || request->path == NULL || (body != NULL && request->body == NULL)) {
        dcc_rest_async_request_free(request);
        return NULL;
    }
    return request;
}
