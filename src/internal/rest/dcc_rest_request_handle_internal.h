#ifndef DCC_REST_REQUEST_HANDLE_INTERNAL_H
#define DCC_REST_REQUEST_HANDLE_INTERNAL_H

#include "internal/rest/dcc_rest_error_observer_internal.h"

#include <dcc/rest/request.h>

#include <stdatomic.h>
#include <stdint.h>
#if !defined(_WIN32)
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct dcc_rest_async_request;

struct dcc_rest_request {
    atomic_uint references;
    atomic_bool terminal_claimed;
    atomic_bool completed;
    atomic_bool caller_reference_released;
    _Atomic(struct dcc_rest_async_request *) async_request;
    dcc_client_t *client;
    dcc_rest_result_fn callback;
    void *callback_user_data;
    dcc_rest_result_t *result;
    dcc_rest_result_t fallback_result;
#if !defined(_WIN32)
    pthread_mutex_t wait_mutex;
    pthread_cond_t wait_cond;
    uint8_t wait_initialized;
#endif
};

dcc_status_t dcc_rest_request_handle_create(
    dcc_client_t *client,
    dcc_rest_result_fn callback,
    void *callback_user_data,
    uint8_t caller_reference,
    dcc_rest_request_t **out
);
void dcc_rest_request_handle_attach(
    dcc_rest_request_t *request,
    struct dcc_rest_async_request *async_request
);
void dcc_rest_request_handle_release(dcc_rest_request_t *request);
void dcc_rest_request_handle_finalize(
    dcc_rest_request_t *request,
    const dcc_rest_terminal_completion_t *completion
);

#ifdef __cplusplus
}
#endif

#endif
