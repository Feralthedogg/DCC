#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_request_handle_internal.h"

#include <llam/io.h>

#include <stdlib.h>
#include <string.h>

dcc_rest_async_request_t *dcc_rest_async_request_new(
    dcc_client_t *client,
    const char *method,
    const char *operation,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    const char *audit_log_reason,
    dcc_rest_auth_mode_t auth_mode,
    const char *auth_token,
    uint64_t flags,
    uint8_t sensitive_path,
    dcc_rest_priority_t priority,
    dcc_rest_cb cb,
    void *user_data,
    dcc_rest_request_t *request_handle
) {
    dcc_rest_async_request_t *request = (dcc_rest_async_request_t *)calloc(1, sizeof(*request));
    if (request == NULL) {
        dcc_rest_request_handle_release(request_handle);
        return NULL;
    }

    request->client = client;
    request->method = dcc_strdup(method);
    request->operation = dcc_strdup(operation);
    request->wire_path = dcc_strdup(path);
    if (body_len != 0U) {
        request->body = (char *)malloc(body_len);
        if (request->body != NULL) {
            memcpy(request->body, body, body_len);
        }
    }
    request->body_len = body_len;
    request->content_type = content_type != NULL ? dcc_strdup(content_type) : NULL;
    request->audit_log_reason = audit_log_reason != NULL
        ? dcc_strdup(audit_log_reason)
        : NULL;
    request->auth_token = auth_token != NULL ? dcc_strdup(auth_token) : NULL;
    request->auth_mode = auth_mode;
    request->flags = flags;
    request->sensitive_path = sensitive_path;
    request->priority = priority;
    dcc_rest_route_key(method, path, request->route, sizeof(request->route));
    request->cb = cb;
    request->user_data = user_data;
    request->request_handle = request_handle;
    atomic_init(&request->cancel_requested, false);
    atomic_init(&request->active_fd, LLAM_INVALID_FD);

    if (request->method == NULL || request->operation == NULL || request->wire_path == NULL ||
        (body_len != 0U && request->body == NULL) ||
        (content_type != NULL && request->content_type == NULL) ||
        (audit_log_reason != NULL && request->audit_log_reason == NULL) ||
        (auth_token != NULL && request->auth_token == NULL)) {
        dcc_rest_async_request_free(request);
        return NULL;
    }
    dcc_rest_request_handle_attach(request_handle, request);
    return request;
}
