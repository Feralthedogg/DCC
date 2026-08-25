#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_async_drain_internal.h"
#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_async_signal_internal.h"
#include "internal/rest/dcc_rest_intercept_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_request_raw_internal.h"
#include "internal/rest/dcc_rest_state_internal.h"
#include "internal/rest/dcc_rest_submit_internal.h"

#include <string.h>
#include <stdlib.h>

typedef struct dcc_rest_async_legacy_bridge {
    dcc_rest_cb callback;
    void *user_data;
} dcc_rest_async_legacy_bridge_t;

static void dcc_rest_async_legacy_result(
    dcc_client_t *client,
    const dcc_rest_result_t *result,
    void *user_data
) {
    dcc_rest_async_legacy_bridge_t *bridge = user_data;
    if (bridge != NULL && bridge->callback != NULL && result != NULL) {
        dcc_rest_response_t response = {
            .size = sizeof(response),
            .status = result->http_status,
            .error = dcc_rest_result_status(result),
            .body = result->body,
            .body_len = result->body_len,
        };
        bridge->callback(client, &response, bridge->user_data);
    }
    free(bridge);
}

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
    if (client->rest_intercept != NULL) {
        dcc_status_t operation_status = dcc_rest_operation_begin(client);
        if (operation_status != DCC_OK) {
            return operation_status;
        }
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
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.priority = priority;
    dcc_rest_request_desc_t description = DCC_REST_REQUEST_DESC_INIT;
    description.method = strcmp(method, "GET") == 0
                             ? DCC_REST_GET
                             : strcmp(method, "POST") == 0
                                   ? DCC_REST_POST
                                   : strcmp(method, "PUT") == 0
                                         ? DCC_REST_PUT
                                         : strcmp(method, "PATCH") == 0
                                               ? DCC_REST_PATCH
                                               : strcmp(method, "DELETE") == 0
                                                     ? DCC_REST_DELETE
                                                     : (dcc_rest_method_t)-1;
    if (dcc_rest_method_name(description.method) == NULL)
        return DCC_ERR_INVALID_ARG;
    dcc_rest_async_legacy_bridge_t *bridge = NULL;
    if (cb != NULL) {
        bridge = malloc(sizeof(*bridge));
        if (bridge == NULL)
            return DCC_ERR_NOMEM;
        bridge->callback = cb;
        bridge->user_data = user_data;
        options.callback = dcc_rest_async_legacy_result;
        options.user_data = bridge;
    }
    description.path = path;
    description.body = body;
    description.body_len = body != NULL ? strlen(body) : 0U;
    description.content_type = body != NULL ? "application/json" : NULL;
    description.options = &options;
    dcc_status_t status = dcc_rest_submit_operation(
        client, &description, path, 0U, NULL);
    if (status != DCC_OK)
        free(bridge);
    return status;
}
