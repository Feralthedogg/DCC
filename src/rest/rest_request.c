#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_firewall_internal.h"
#include "internal/rest/dcc_rest_error_observer_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

#include <stddef.h>
#include <stdint.h>

typedef struct dcc_rest_intercept_delivery {
    dcc_client_t *client;
    const char *operation;
    dcc_rest_cb callback;
    void *callback_user_data;
    uint8_t called;
} dcc_rest_intercept_delivery_t;

static void dcc_rest_intercept_delivery_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    dcc_rest_intercept_delivery_t *delivery =
        (dcc_rest_intercept_delivery_t *)user_data;
    if (delivery == NULL || delivery->called) {
        return;
    }
    delivery->called = 1U;
    dcc_rest_terminal_completion_t completion = {
        .operation = delivery->operation,
    };
    const size_t required_response_size =
        offsetof(dcc_rest_response_t, body_len) + sizeof(response->body_len);
    if (client != delivery->client || response == NULL ||
        response->size < required_response_size ||
        (response->body_len != 0U && response->body == NULL)) {
        completion.transport_status = DCC_ERR_RUNTIME;
        completion.legacy_error = DCC_ERR_RUNTIME;
    } else if (response->status == 0U) {
        completion.transport_status = response->error != DCC_OK
            ? response->error
            : DCC_ERR_RUNTIME;
        completion.legacy_error = completion.transport_status;
    } else {
        completion.transport_status = DCC_OK;
        completion.http_status = response->status;
        completion.legacy_error = response->error;
        completion.body = response->body;
        completion.body_len = response->body_len;
    }
    dcc_rest_deliver_terminal(
        delivery->client,
        &completion,
        delivery->callback,
        delivery->callback_user_data
    );
}

dcc_status_t dcc_rest_request_raw_impl(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    uint32_t max_rate_limit_retries,
    int wait_for_route,
    dcc_rest_cb cb,
    void *user_data,
    int (*is_canceled)(void *user_data),
    llam_fd_t (*swap_fd)(void *user_data, llam_fd_t fd),
    void *cancel_user_data,
    int observe_terminal,
    int silent_admission_failure
) {
    if (client == NULL || method == NULL || path == NULL || (body_len != 0 && body == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    if (client->rest_intercept != NULL) {
        if (!observe_terminal) {
            return client->rest_intercept(
                client,
                method,
                path,
                body,
                body_len,
                content_type,
                cb,
                user_data,
                client->rest_intercept_user_data
            );
        }
        dcc_rest_intercept_delivery_t delivery = {
            .client = client,
            .operation = path,
            .callback = cb,
            .callback_user_data = user_data,
        };
        dcc_status_t intercept_status = client->rest_intercept(
            client,
            method,
            path,
            body,
            body_len,
            content_type,
            dcc_rest_intercept_delivery_cb,
            &delivery,
            client->rest_intercept_user_data
        );
        if (intercept_status != DCC_OK && !delivery.called &&
            !silent_admission_failure) {
            dcc_rest_terminal_completion_t completion = {
                .operation = path,
                .transport_status = intercept_status,
                .legacy_error = intercept_status,
            };
            dcc_rest_deliver_terminal(client, &completion, NULL, NULL);
        }
        if (silent_admission_failure) {
            return delivery.called
                ? DCC_OK
                : intercept_status != DCC_OK ? intercept_status : DCC_ERR_RUNTIME;
        }
        return intercept_status;
    }

    char route[DCC_REST_ROUTE_KEY_CAP];
    dcc_rest_route_key(method, path, route, sizeof(route));

    dcc_status_t firewall_status = dcc_rest_firewall_before_request(client, method, path);
    if (firewall_status != DCC_OK) {
        return firewall_status;
    }

    dcc_rest_prepared_request_t prepared;
    dcc_status_t prepare_status = dcc_rest_prepare_http_request(
        &prepared,
        client,
        method,
        path,
        body,
        body_len,
        content_type,
        is_canceled,
        swap_fd,
        cancel_user_data
    );
    if (prepare_status != DCC_OK) {
        return prepare_status;
    }

    for (uint32_t attempt = 0; attempt <= max_rate_limit_retries; ++attempt) {
        if (wait_for_route) {
            dcc_rest_wait_for_route(client, route);
        }

        dcc_http_response_t http_response;
        dcc_status_t st = dcc_http_perform(&prepared.http, &http_response);

        if (st != DCC_OK) {
            dcc_rest_prepared_request_deinit(&prepared);
            dcc_set_error(client, dcc_status_string(st));
            if (observe_terminal) {
                dcc_rest_terminal_completion_t completion = {
                    .operation = path,
                    .transport_status = st,
                    .legacy_error = st,
                };
                dcc_rest_deliver_terminal(client, &completion, NULL, NULL);
            }
            return st;
        }

        dcc_rest_request_response_limits_t limits =
            dcc_rest_request_update_response_limits(client, route, &http_response);
        dcc_rest_firewall_after_response(client, http_response.status);

        if (limits.response_error == DCC_ERR_RATE_LIMITED) {
            dcc_rest_request_emit_rate_limited(client, method, path, &http_response, &limits);
            if (attempt < max_rate_limit_retries) {
                dcc_http_response_deinit(&http_response);
                continue;
            }
            dcc_set_error(client, dcc_status_string(limits.response_error));
        }

        if (observe_terminal) {
            uint64_t retry_after_ms = 0U;
            if (limits.retry_after_seconds > 0.0 &&
                limits.retry_after_seconds <= (double)UINT64_MAX / 1000.0) {
                retry_after_ms = (uint64_t)(limits.retry_after_seconds * 1000.0);
            }
            dcc_rest_terminal_completion_t completion = {
                .operation = path,
                .transport_status = DCC_OK,
                .http_status = http_response.status,
                .legacy_error = limits.response_error,
                .body = http_response.body,
                .body_len = http_response.body_len,
                .retry_after_ms = retry_after_ms,
            };
            dcc_rest_deliver_terminal(client, &completion, cb, user_data);
        } else if (cb != NULL) {
            dcc_rest_response_t rest_response = {
                .size = sizeof(rest_response), .status = http_response.status,
                .error = limits.response_error, .body = http_response.body,
                .body_len = http_response.body_len,
            };
            cb(client, &rest_response, user_data);
        }

        dcc_http_response_deinit(&http_response);
        dcc_rest_prepared_request_deinit(&prepared);
        return DCC_OK;
    }

    dcc_rest_prepared_request_deinit(&prepared);
    return DCC_OK;
}
