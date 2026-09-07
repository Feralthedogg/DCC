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
    const char *strings[] = {method, operation, path, content_type, audit_log_reason, auth_token};
    size_t lengths[sizeof(strings) / sizeof(strings[0])] = {0};
    size_t allocation_size = sizeof(dcc_rest_async_request_t);
    if (method == NULL || operation == NULL || path == NULL) {
        dcc_rest_request_handle_release(request_handle);
        return NULL;
    }
    for (size_t i = 0U; i < sizeof(strings) / sizeof(strings[0]); ++i) {
        if (strings[i] == NULL) {
            continue;
        }
        size_t length = strlen(strings[i]);
        if (length >= SIZE_MAX - allocation_size) {
            dcc_rest_request_handle_release(request_handle);
            return NULL;
        }
        lengths[i] = length + 1U;
        allocation_size += lengths[i];
    }
    dcc_rest_async_request_t *request = (dcc_rest_async_request_t *)calloc(1, allocation_size);
    if (request == NULL) {
        dcc_rest_request_handle_release(request_handle);
        return NULL;
    }

    request->client = client;
    request->metadata_len = allocation_size - sizeof(*request);
    char **destinations[] = {&request->method, &request->operation, &request->wire_path,
                            &request->content_type, &request->audit_log_reason, &request->auth_token};
    char *cursor = (char *)(request + 1);
    for (size_t i = 0U; i < sizeof(strings) / sizeof(strings[0]); ++i) {
        if (lengths[i] != 0U) {
            *destinations[i] = cursor;
            memcpy(cursor, strings[i], lengths[i]);
            cursor += lengths[i];
        }
    }
    if (body_len != 0U) {
        request->body = (char *)malloc(body_len);
        if (request->body != NULL) {
            memcpy(request->body, body, body_len);
        }
    }
    request->body_len = body_len;
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

    if (body_len != 0U && request->body == NULL) {
        dcc_rest_async_request_free(request);
        return NULL;
    }
    dcc_rest_request_handle_attach(request_handle, request);
    return request;
}
