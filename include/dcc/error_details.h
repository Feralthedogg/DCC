/**
 * @file include/dcc/error_details.h
 * @brief Structured error views delivered by DCC observers.
 */

#ifndef DCC_ERROR_DETAILS_H
#define DCC_ERROR_DETAILS_H

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Current layout version for dcc_error_t. */
#define DCC_ERROR_VERSION 1U

/** Identifies the subsystem that produced a structured error. */
typedef enum dcc_error_origin {
    DCC_ERROR_HANDLER = 1,
    DCC_ERROR_REST,
    DCC_ERROR_GATEWAY,
    DCC_ERROR_RUNTIME
} dcc_error_origin_t;

/**
 * Borrowed error details passed to a client or App error observer.
 *
 * Every pointer is owned by DCC and remains valid only until the observer
 * returns. `body` is a byte span and is not required to be NUL-terminated.
 */
typedef struct dcc_error {
    size_t size;                 /**< Size of this structure. */
    uint32_t version;            /**< DCC_ERROR_VERSION. */
    dcc_error_origin_t origin;   /**< Subsystem that produced the error. */
    dcc_status_t status;         /**< DCC status after completion mapping. */
    uint16_t http_status;        /**< HTTP status, or zero without a response. */
    int32_t discord_code;        /**< Discord JSON error code, or zero. */
    const char *operation;       /**< Borrowed operation or request path. */
    const char *message;         /**< Borrowed decoded message, when available. */
    const char *body;            /**< Borrowed raw response bytes, when available. */
    size_t body_len;             /**< Exact byte length of body. */
} dcc_error_t;

#ifdef __cplusplus
}
#endif

#endif
