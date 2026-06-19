#include "internal/rest/dcc_rest_future_internal.h"

#include <llam/runtime.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

void dcc_rest_future_mark_completed(dcc_rest_future_t *future) {
    if (future == NULL) {
        return;
    }
#if !defined(_WIN32)
    if (future->wait_initialized && pthread_mutex_lock(&future->wait_mutex) == 0) {
        atomic_store_explicit(&future->completed, true, memory_order_release);
        (void)pthread_cond_broadcast(&future->wait_cond);
        (void)pthread_mutex_unlock(&future->wait_mutex);
    } else
#endif
    {
        atomic_store_explicit(&future->completed, true, memory_order_release);
    }
    (void)llam_channel_try_send(future->done, future);
}

dcc_status_t dcc_rest_future_create(dcc_rest_future_t **out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_rest_future_t *future = (dcc_rest_future_t *)calloc(1, sizeof(*future));
    if (future == NULL) {
        return DCC_ERR_NOMEM;
    }
    future->done = llam_channel_create(1);
    if (future->done == NULL) {
        dcc_status_t status = dcc_rest_wait_status_from_errno(errno);
        free(future);
        return status;
    }
#if !defined(_WIN32)
    if (pthread_mutex_init(&future->wait_mutex, NULL) != 0) {
        (void)llam_channel_close(future->done);
        (void)llam_channel_destroy(future->done);
        free(future);
        return DCC_ERR_RUNTIME;
    }
    if (pthread_cond_init(&future->wait_cond, NULL) != 0) {
        (void)pthread_mutex_destroy(&future->wait_mutex);
        (void)llam_channel_close(future->done);
        (void)llam_channel_destroy(future->done);
        free(future);
        return DCC_ERR_RUNTIME;
    }
    future->wait_initialized = 1;
#endif
    atomic_init(&future->completed, false);
    future->response.size = sizeof(future->response);
    *out = future;
    return DCC_OK;
}

void dcc_rest_future_deinit_completed(dcc_rest_future_t *future) {
    if (future == NULL) {
        return;
    }
    if (future->done != NULL) {
        void *value = NULL;
        while (llam_channel_try_recv_result(future->done, &value) == 0) {
            value = NULL;
        }
        (void)llam_channel_close(future->done);
        (void)llam_channel_destroy(future->done);
        future->done = NULL;
    }
#if !defined(_WIN32)
    if (future->wait_initialized) {
        future->wait_initialized = 0;
        (void)pthread_cond_destroy(&future->wait_cond);
        (void)pthread_mutex_destroy(&future->wait_mutex);
    }
#endif
    free(future->body);
    future->body = NULL;
    future->body_len = 0;
}
