#include "internal/client/dcc_client_state_internal.h"
#include "internal/rest/dcc_rest_async_drain_internal.h"
#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_async_signal_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_request_handle_internal.h"
#include "internal/rest/dcc_rest_state_internal.h"
#include "internal/rest/dcc_rest_submit_internal.h"
#include "internal/rest/dcc_rest_task7_probe_internal.h"

#include <dcc/rest/request.h>

#include <stdatomic.h>
#include <stddef.h>
#include <string.h>

static int dcc_rest_field_covered(size_t size, size_t offset, size_t width) {
    return size >= offset && width <= size - offset;
}

static int dcc_rest_field_partial(size_t size, size_t offset, size_t width) {
    return size > offset && !dcc_rest_field_covered(size, offset, width);
}

dcc_status_t dcc_rest_call_options_normalize(
    const dcc_rest_call_options_t *options,
    dcc_rest_call_options_t *out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
    if (options == NULL) {
        return DCC_OK;
    }
    const size_t required = offsetof(dcc_rest_call_options_t, audit_log_reason);
    if (options->size < required ||
        options->version != DCC_REST_CALL_OPTIONS_VERSION ||
        !dcc_rest_priority_valid(options->priority) ||
        (options->callback == NULL && options->user_data != NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    out->priority = options->priority;
    out->callback = options->callback;
    out->user_data = options->user_data;

#define DCC_READ_OPTION_FIELD(field_) \
    do { \
        const size_t offset = offsetof(dcc_rest_call_options_t, field_); \
        const size_t width = sizeof(options->field_); \
        if (dcc_rest_field_partial(options->size, offset, width)) { \
            return DCC_ERR_INVALID_ARG; \
        } \
        if (dcc_rest_field_covered(options->size, offset, width)) { \
            out->field_ = options->field_; \
        } \
    } while (0)
    DCC_READ_OPTION_FIELD(audit_log_reason);
    DCC_READ_OPTION_FIELD(auth_mode);
    DCC_READ_OPTION_FIELD(auth_token);
    DCC_READ_OPTION_FIELD(flags);
#undef DCC_READ_OPTION_FIELD

    if (out->auth_mode < DCC_REST_AUTH_DEFAULT ||
        out->auth_mode > DCC_REST_AUTH_BEARER ||
        (out->flags & ~(DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY |
            DCC_REST_CALL_FLAG_SENSITIVE_RESULT_BODY)) != 0U ||
        (out->auth_mode == DCC_REST_AUTH_BEARER &&
            (out->auth_token == NULL || out->auth_token[0] == '\0')) ||
        (out->auth_mode != DCC_REST_AUTH_BEARER && out->auth_token != NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static int dcc_rest_request_desc_valid(
    const dcc_rest_request_desc_t *description,
    const char **out_method
) {
    const size_t required = offsetof(dcc_rest_request_desc_t, options) +
        sizeof(description->options);
    if (description == NULL || description->size < required ||
        description->version != DCC_REST_REQUEST_DESC_VERSION ||
        description->path == NULL || description->path[0] == '\0' ||
        (description->body_len != 0U && description->body == NULL)) {
        return 0;
    }
    const char *method = dcc_rest_method_name(description->method);
    if (method == NULL) {
        return 0;
    }
    if (out_method != NULL) {
        *out_method = method;
    }
    return 1;
}

static void dcc_rest_request_handle_release_unpublished(
    dcc_rest_request_t *request,
    int caller_reference,
    int runtime_reference
) {
    if (runtime_reference) {
        dcc_rest_request_handle_release(request);
    }
    if (caller_reference) {
        dcc_rest_request_handle_release(request);
    }
}

dcc_status_t dcc_rest_submit_operation_with_post_hook(
    dcc_client_t *client,
    const dcc_rest_request_desc_t *description,
    const char *operation,
    uint8_t sensitive_path,
    dcc_rest_request_post_hook_fn post_hook,
    void *post_hook_user_data,
    dcc_rest_request_t **out_request
) {
    if (out_request != NULL) {
        *out_request = NULL;
    }
    const char *method = NULL;
    dcc_rest_call_options_t options;
    if (client == NULL || operation == NULL || operation[0] == '\0' ||
        !dcc_rest_request_desc_valid(description, &method) ||
        dcc_rest_call_options_normalize(description->options, &options) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t forced = dcc_endpoint_task7_probe_consume_failure();
    if (forced != DCC_OK) {
        return forced;
    }
    const int absolute = strncmp(description->path, "https://", 8U) == 0 ||
        strncmp(description->path, "http://", 7U) == 0;
    if (absolute) {
        if (options.auth_mode != DCC_REST_AUTH_DEFAULT ||
            options.audit_log_reason != NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        options.auth_mode = DCC_REST_AUTH_NONE;
    } else if (options.auth_mode == DCC_REST_AUTH_DEFAULT) {
        options.auth_mode = DCC_REST_AUTH_BOT;
    }

    dcc_status_t status = dcc_rest_operation_begin(client);
    if (status != DCC_OK) {
        return status;
    }
    if (!client->runtime.initialized ||
        atomic_load_explicit(&client->stopping, memory_order_acquire)) {
        dcc_rest_operation_end(client);
        return DCC_ERR_STATE;
    }

    const int caller_reference = out_request != NULL;
    dcc_rest_request_t *handle = NULL;
    status = dcc_rest_request_handle_create(
        client,
        options.callback,
        options.user_data,
        post_hook,
        post_hook_user_data,
        (uint8_t)caller_reference,
        &handle
    );
    if (status != DCC_OK) {
        dcc_rest_operation_end(client);
        return status;
    }

    dcc_rest_async_request_t *request = dcc_rest_async_request_new(
        client,
        method,
        operation,
        description->path,
        description->body,
        description->body_len,
        description->content_type,
        options.audit_log_reason,
        options.auth_mode,
        options.auth_token,
        options.flags,
        sensitive_path,
        options.priority,
        NULL,
        NULL,
        handle
    );
    if (request == NULL) {
        /* The failed job constructor dropped the runtime reference. */
        dcc_rest_request_handle_release_unpublished(handle, caller_reference, 0);
        dcc_rest_operation_end(client);
        return DCC_ERR_NOMEM;
    }

    /* Publish the caller reference before the worker can become runnable. */
    if (out_request != NULL) {
        *out_request = handle;
    }

    dcc_rest_lock(client);
    dcc_rest_async_push_tail_locked(client, request);
    dcc_rest_async_request_t *rejected = NULL;
    status = dcc_rest_async_drain_admission_locked(client, request, &rejected);
    dcc_rest_unlock(client);
    dcc_rest_async_signal(client);

    if (rejected != NULL) {
        dcc_rest_async_request_free(rejected);
    }
    if (status != DCC_OK && caller_reference) {
        *out_request = NULL;
        dcc_rest_request_handle_release(handle);
    }
    dcc_rest_operation_end(client);
    return status;
}

dcc_status_t dcc_rest_submit_operation(
    dcc_client_t *client,
    const dcc_rest_request_desc_t *description,
    const char *operation,
    uint8_t sensitive_path,
    dcc_rest_request_t **out_request
) {
    return dcc_rest_submit_operation_with_post_hook(
        client,
        description,
        operation,
        sensitive_path,
        NULL,
        NULL,
        out_request
    );
}

dcc_status_t dcc_rest_submit(
    dcc_client_t *client,
    const dcc_rest_request_desc_t *description,
    dcc_rest_request_t **out_request
) {
    return dcc_rest_submit_operation(
        client, description, "dcc_rest_submit", 0U, out_request
    );
}
