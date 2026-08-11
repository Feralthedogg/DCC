#include "internal/client/dcc_client_state_internal.h"
#include "internal/rest/dcc_rest_async_drain_internal.h"
#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_async_signal_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_request_handle_internal.h"
#include "internal/rest/dcc_rest_state_internal.h"

#include <dcc/rest/request.h>

#include <stdatomic.h>

static int dcc_rest_call_options_valid(const dcc_rest_call_options_t *options) {
    return options != NULL &&
        options->size >= sizeof(*options) &&
        options->version == DCC_REST_CALL_OPTIONS_VERSION &&
        dcc_rest_priority_valid(options->priority) &&
        (options->callback != NULL || options->user_data == NULL);
}

static int dcc_rest_request_desc_valid(
    const dcc_rest_request_desc_t *description,
    const char **out_method
) {
    if (description == NULL || description->size < sizeof(*description) ||
        description->version != DCC_REST_REQUEST_DESC_VERSION ||
        description->path == NULL || description->path[0] == '\0' ||
        (description->body_len != 0U && description->body == NULL) ||
        !dcc_rest_call_options_valid(&description->options)) {
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

dcc_status_t dcc_rest_submit(
    dcc_client_t *client,
    const dcc_rest_request_desc_t *description,
    dcc_rest_request_t **out_request
) {
    if (out_request != NULL) {
        *out_request = NULL;
    }
    const char *method = NULL;
    if (client == NULL || !dcc_rest_request_desc_valid(description, &method)) {
        return DCC_ERR_INVALID_ARG;
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
        description->options.callback,
        description->options.user_data,
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
        description->path,
        description->body,
        description->body_len,
        description->content_type,
        description->options.priority,
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
