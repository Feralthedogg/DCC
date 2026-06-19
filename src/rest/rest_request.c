#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_firewall_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

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
    void *cancel_user_data
) {
    if (client == NULL || method == NULL || path == NULL || (body_len != 0 && body == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    if (client->rest_intercept != NULL) {
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

        if (cb != NULL) {
            dcc_rest_response_t rest_response = {
                .size = sizeof(rest_response),
                .status = http_response.status,
                .error = limits.response_error,
                .body = http_response.body,
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
