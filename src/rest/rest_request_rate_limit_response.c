#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_rest_request_response_limits_t dcc_rest_request_update_response_limits(
    dcc_client_t *client,
    const char *route,
    const dcc_http_response_t *http_response
) {
    size_t retry_after_len = 0;
    const char *retry_after = dcc_rest_response_header(http_response, "Retry-After", &retry_after_len);
    size_t global_len = 0;
    const char *global = dcc_rest_response_header(http_response, "X-RateLimit-Global", &global_len);

    dcc_rest_request_response_limits_t limits = {
        .response_error = http_response->status == 429 ? DCC_ERR_RATE_LIMITED : DCC_OK,
        .retry_after_seconds = dcc_rest_parse_header_double(retry_after, retry_after_len),
        .is_global = dcc_rest_header_bool_true(global, global_len),
    };
    dcc_rest_update_rate_limit(client, route, http_response, limits.retry_after_seconds, limits.is_global);
    return limits;
}

void dcc_rest_request_emit_rate_limited(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const dcc_http_response_t *http_response,
    const dcc_rest_request_response_limits_t *limits
) {
    if (client == NULL || http_response == NULL || limits == NULL) {
        return;
    }

    dcc_rate_limited_event_t limited = {
        .status = http_response->status,
        .retry_after = limits->retry_after_seconds,
        .global = limits->is_global,
        .method = method,
        .path = path,
        .body = http_response->body,
        .body_len = http_response->body_len,
    };
    dcc_emit_rate_limited(client, &limited);
}
