#ifndef DCC_REST_CORE_FUTURE_H
#define DCC_REST_CORE_FUTURE_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DCC_REST_LEGACY_INTERNAL)
#define DCC_REST_LEGACY_API
#else
#define DCC_REST_LEGACY_API DCC_API
#endif

DCC_REST_LEGACY_API dcc_status_t dcc_rest_request_future(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_future_t **out
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_request_future_priority(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_priority_t priority,
    dcc_rest_future_t **out
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_request_method_future(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *body,
    dcc_rest_future_t **out
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_request_method_future_priority(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *body,
    dcc_rest_priority_t priority,
    dcc_rest_future_t **out
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_future_wait(
    dcc_rest_future_t *future,
    uint32_t timeout_ms,
    dcc_rest_response_t *out
);

DCC_REST_LEGACY_API uint8_t dcc_rest_future_completed(const dcc_rest_future_t *future);
DCC_REST_LEGACY_API dcc_status_t dcc_rest_future_destroy(dcc_rest_future_t *future);

#ifdef __cplusplus
}
#endif

#undef DCC_REST_LEGACY_API

#endif
