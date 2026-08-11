/**
 * @file include/dcc/rest/result.h
 * @brief Versioned REST completion results and ownership helpers.
 */

#ifndef DCC_REST_RESULT_H
#define DCC_REST_RESULT_H

#include <dcc/error.h>
#include <dcc/export.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Current layout version for dcc_rest_result_t. */
#define DCC_REST_RESULT_VERSION 1U

/** Result flags. */
enum {
    DCC_REST_RESULT_FLAG_SENSITIVE_BODY = UINT64_C(1)
};

/**
 * Borrowed REST completion result.
 *
 * Callback results are owned by DCC for the duration of the callback. A
 * caller that needs to retain one uses dcc_rest_result_clone(). `body` is an
 * exact byte span and may contain embedded NUL bytes.
 */
typedef struct dcc_rest_result {
    size_t size;                    /**< Size of this structure. */
    uint32_t version;               /**< DCC_REST_RESULT_VERSION. */
    dcc_status_t transport_status;  /**< Transport completion status. */
    uint16_t http_status;           /**< HTTP status, or zero without a response. */
    int32_t discord_code;           /**< Discord JSON error code, or zero. */
    const char *discord_message;    /**< Borrowed decoded Discord message. */
    const char *body;               /**< Borrowed raw response bytes. */
    size_t body_len;                /**< Exact byte length of body. */
    uint64_t retry_after_ms;        /**< Terminal retry delay, when known. */
    uint64_t flags;                 /**< DCC_REST_RESULT_FLAG_* bits. */
} dcc_rest_result_t;

/** Returns non-zero only for a valid transport-successful 2xx result. */
DCC_API uint8_t dcc_rest_result_ok(const dcc_rest_result_t *result);

/**
 * Maps a valid result to DCC_OK, its transport failure, or DCC_ERR_DISCORD.
 * Invalid metadata returns DCC_ERR_INVALID_ARG.
 */
DCC_API dcc_status_t dcc_rest_result_status(const dcc_rest_result_t *result);

/**
 * Creates an owned deep copy of a full version-1 result.
 *
 * `out` is set to NULL before validation and on every failure. The clone owns
 * exact `body_len` bytes (including embedded NULs) and its decoded message.
 */
DCC_API dcc_status_t dcc_rest_result_clone(
    const dcc_rest_result_t *source,
    dcc_rest_result_t **out
);

/** Releases a result returned by dcc_rest_result_clone(); NULL is allowed. */
DCC_API void dcc_rest_result_free(dcc_rest_result_t *result);

#ifdef __cplusplus
}
#endif

#endif
