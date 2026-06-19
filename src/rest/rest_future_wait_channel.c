#include "internal/rest/dcc_rest_future_internal.h"

#include <llam/runtime.h>

#include <errno.h>

dcc_status_t dcc_rest_future_wait_channel(dcc_rest_future_t *future, uint32_t timeout_ms) {
    if (future == NULL || future->done == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (atomic_load_explicit(&future->completed, memory_order_acquire)) {
        return DCC_OK;
    }

    if (llam_current_task() != NULL) {
        void *value = NULL;
        int rc = timeout_ms == 0
            ? llam_channel_recv_result(future->done, &value)
            : llam_channel_recv_until_result(future->done, dcc_rest_timeout_deadline_ns(timeout_ms), &value);
        if (rc != 0) {
            return dcc_rest_wait_status_from_errno(errno);
        }
        return value == future && atomic_load_explicit(&future->completed, memory_order_acquire)
            ? DCC_OK
            : DCC_ERR_RUNTIME;
    }

#if !defined(_WIN32)
    if (future->wait_initialized) {
        int rc = pthread_mutex_lock(&future->wait_mutex);
        if (rc != 0) {
            return dcc_rest_status_from_pthread(rc);
        }
        dcc_status_t status = DCC_OK;
        if (timeout_ms == 0) {
            while (!atomic_load_explicit(&future->completed, memory_order_acquire) && status == DCC_OK) {
                status = dcc_rest_status_from_pthread(pthread_cond_wait(&future->wait_cond, &future->wait_mutex));
            }
        } else {
            struct timespec deadline;
            status = dcc_rest_make_deadline_ms(timeout_ms, &deadline);
            while (!atomic_load_explicit(&future->completed, memory_order_acquire) && status == DCC_OK) {
                rc = pthread_cond_timedwait(&future->wait_cond, &future->wait_mutex, &deadline);
                if (rc == ETIMEDOUT) {
                    status = atomic_load_explicit(&future->completed, memory_order_acquire)
                        ? DCC_OK
                        : DCC_ERR_TIMEOUT;
                } else {
                    status = dcc_rest_status_from_pthread(rc);
                }
            }
        }
        (void)pthread_mutex_unlock(&future->wait_mutex);
        return status;
    }
#endif

    uint64_t start_ms = dcc_rest_now_ms();
    for (;;) {
        if (atomic_load_explicit(&future->completed, memory_order_acquire)) {
            return DCC_OK;
        }
        void *value = NULL;
        if (llam_channel_try_recv_result(future->done, &value) == 0) {
            return value == future && atomic_load_explicit(&future->completed, memory_order_acquire)
                ? DCC_OK
                : DCC_ERR_RUNTIME;
        }
        int err = errno;
        if (err != EAGAIN) {
            return dcc_rest_wait_status_from_errno(err);
        }
        if (timeout_ms != 0 && dcc_rest_now_ms() - start_ms >= timeout_ms) {
            return DCC_ERR_TIMEOUT;
        }
        dcc_rest_sleep_ms(1);
    }
}
