/**
 * @file include/dcc/rest/request.h
 * @brief Canonical asynchronous REST request handles.
 */

#ifndef DCC_REST_REQUEST_H
#define DCC_REST_REQUEST_H

#include <dcc/rest/base.h>
#include <dcc/rest/result.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Current layout version for dcc_rest_call_options_t. */
#define DCC_REST_CALL_OPTIONS_VERSION 1U

/** Current layout version for dcc_rest_request_desc_t. */
#define DCC_REST_REQUEST_DESC_VERSION 1U

/** Opaque owner of one accepted asynchronous REST operation and its result. */
typedef struct dcc_rest_request dcc_rest_request_t;

/**
 * Receives one borrowed terminal REST result.
 *
 * The callback may cancel or destroy its request, but waiting on that same
 * request returns DCC_ERR_STATE. The result view is valid only until the
 * callback returns; use dcc_rest_result_clone() to retain it. No thread
 * affinity is promised: normal completion runs on a runtime worker, while
 * pending cancellation or teardown may deliver on the calling thread.
 */
typedef void (*dcc_rest_result_fn)(
    dcc_client_t *client,
    const dcc_rest_result_t *result,
    void *user_data
);

/** Versioned scheduling and completion options for one REST operation. */
typedef struct dcc_rest_call_options {
    size_t size;                   /**< Size of this structure. */
    uint32_t version;              /**< DCC_REST_CALL_OPTIONS_VERSION. */
    dcc_rest_priority_t priority;  /**< Queue priority. */
    dcc_rest_result_fn callback;   /**< Optional exactly-once terminal callback. */
    void *user_data;               /**< Borrowed until callback return. */
} dcc_rest_call_options_t;

/** Constant initializer for dcc_rest_call_options_t. */
#define DCC_REST_CALL_OPTIONS_INIT \
    { sizeof(dcc_rest_call_options_t), DCC_REST_CALL_OPTIONS_VERSION, \
      DCC_REST_PRIORITY_NORMAL, NULL, NULL }

/** Versioned raw REST submission description. */
typedef struct dcc_rest_request_desc {
    size_t size;                    /**< Size of this structure. */
    uint32_t version;               /**< DCC_REST_REQUEST_DESC_VERSION. */
    dcc_rest_method_t method;       /**< HTTP method. */
    const char *path;               /**< Non-empty path or absolute URL; copied. */
    const char *content_type;       /**< Optional NUL-terminated value; copied. */
    const void *body;               /**< Optional exact body byte span; copied. */
    size_t body_len;                /**< Exact byte count, including embedded NULs. */
    dcc_rest_call_options_t options; /**< Scheduling and callback options. */
} dcc_rest_request_desc_t;

/** Constant initializer for dcc_rest_request_desc_t. */
#define DCC_REST_REQUEST_DESC_INIT \
    { sizeof(dcc_rest_request_desc_t), DCC_REST_REQUEST_DESC_VERSION, \
      DCC_REST_GET, NULL, NULL, NULL, 0U, DCC_REST_CALL_OPTIONS_INIT }

/** Initializes call options to normal priority with no callback; NULL is allowed. */
DCC_API void dcc_rest_call_options_init(dcc_rest_call_options_t *options);

/**
 * Initializes a raw descriptor and its nested call options; NULL is allowed.
 * The initialized descriptor uses GET and has no path, content type, or body.
 */
DCC_API void dcc_rest_request_desc_init(dcc_rest_request_desc_t *description);

/**
 * Validates, copies, and queues one raw REST operation.
 *
 * A non-OK return is an admission rejection: `*out_request` is NULL and no
 * callback or error observer runs. DCC_OK guarantees exactly one terminal
 * result. Path, content type, and exact body bytes are copied before
 * admission; callback and user-data values remain borrowed. When
 * `out_request` is NULL, DCC releases the request automatically. Otherwise
 * the output is published before callback delivery becomes eligible, and
 * delivery may race the tail of this function's successful return.
 */
DCC_API dcc_status_t dcc_rest_submit(
    dcc_client_t *client,
    const dcc_rest_request_desc_t *description,
    dcc_rest_request_t **out_request
);

/**
 * Waits for callback and observer delivery, then returns the retained result.
 *
 * Zero waits indefinitely. A non-zero timeout returns DCC_ERR_TIMEOUT and
 * leaves the handle usable. Any terminal HTTP or transport outcome returns
 * DCC_OK; inspect it with dcc_rest_result_status(). `out_result` is set to
 * NULL before waiting and receives a view valid until request destruction.
 */
DCC_API dcc_status_t dcc_rest_request_wait(
    dcc_rest_request_t *request,
    uint32_t timeout_ms,
    const dcc_rest_result_t **out_result
);

/**
 * Requests nonblocking, idempotent cancellation of one operation.
 * Completion may win the race and retain its original result. NULL returns
 * DCC_ERR_INVALID_ARG.
 */
DCC_API dcc_status_t dcc_rest_request_cancel(dcc_rest_request_t *request);

/** Returns non-zero after terminal callback and observer delivery; NULL is false. */
DCC_API uint8_t dcc_rest_request_completed(const dcc_rest_request_t *request);

/**
 * Requests cancellation and releases the caller reference without waiting.
 *
 * NULL is allowed. The caller must not race destruction against wait, cancel,
 * completed, or another destroy call using the same caller-owned handle.
 * Destruction never suppresses an installed callback.
 */
DCC_API void dcc_rest_request_destroy(dcc_rest_request_t *request);

#ifdef __cplusplus
}
#endif

#endif
