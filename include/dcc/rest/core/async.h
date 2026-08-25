#ifndef DCC_REST_CORE_ASYNC_H
#define DCC_REST_CORE_ASYNC_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DCC_REST_LEGACY_INTERNAL)
#define DCC_REST_LEGACY_API
#else
#define DCC_REST_LEGACY_API DCC_API
#endif

/**
 * Submits an asynchronous REST request.
 *
 * A non-OK return rejects admission: DCC retains neither the request nor its
 * callback/user data and will not deliver a terminal callback or observation
 * for that submission. DCC_OK transfers callback/user-data lifetime to DCC
 * until exactly one terminal completion.
 */
DCC_REST_LEGACY_API dcc_status_t dcc_rest_request_async(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
);

/** Same admission and ownership contract as dcc_rest_request_async(). */
DCC_REST_LEGACY_API dcc_status_t dcc_rest_request_async_priority(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_priority_t priority,
    dcc_rest_cb cb,
    void *user_data
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_request_method_async(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_request_method_async_priority(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *body,
    dcc_rest_priority_t priority,
    dcc_rest_cb cb,
    void *user_data
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_async_status(
    dcc_client_t *client,
    dcc_rest_async_status_t *out
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_async_pending(
    dcc_client_t *client,
    size_t *out_pending,
    size_t *out_active
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_async_cancel_pending(
    dcc_client_t *client,
    size_t *out_canceled
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_async_cancel_active(
    dcc_client_t *client,
    size_t *out_canceled
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_async_cancel_all(
    dcc_client_t *client,
    size_t *out_canceled
);

DCC_REST_LEGACY_API dcc_status_t dcc_rest_async_wait(
    dcc_client_t *client,
    uint32_t timeout_ms
);

#ifdef __cplusplus
}
#endif

#undef DCC_REST_LEGACY_API

#endif
