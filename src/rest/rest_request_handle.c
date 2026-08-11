#include "internal/rest/dcc_rest_request_handle_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/rest/dcc_rest_async_cancel_internal.h"
#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_async_signal_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

#include <llam/runtime.h>

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#if defined(_WIN32)
#include <windows.h>
#endif

typedef struct dcc_rest_request_delivery_frame {
    const dcc_rest_request_t *request;
    struct dcc_rest_request_delivery_frame *previous;
} dcc_rest_request_delivery_frame_t;

static _Thread_local dcc_rest_request_delivery_frame_t *dcc_rest_request_delivery;

void dcc_rest_call_options_init(dcc_rest_call_options_t *options) {
    if (options != NULL) {
        *options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
    }
}

void dcc_rest_request_desc_init(dcc_rest_request_desc_t *description) {
    if (description != NULL) {
        *description = (dcc_rest_request_desc_t)DCC_REST_REQUEST_DESC_INIT;
    }
}

dcc_status_t dcc_rest_request_handle_create(
    dcc_client_t *client,
    dcc_rest_result_fn callback,
    void *callback_user_data,
    uint8_t caller_reference,
    dcc_rest_request_t **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    dcc_rest_request_t *request = (dcc_rest_request_t *)calloc(1U, sizeof(*request));
    if (request == NULL) {
        return DCC_ERR_NOMEM;
    }
    atomic_init(&request->references, caller_reference ? 2U : 1U);
    atomic_init(&request->terminal_claimed, false);
    atomic_init(&request->completed, false);
    atomic_init(&request->caller_reference_released, !caller_reference);
    atomic_init(&request->async_request, NULL);
    request->client = client;
    request->callback = callback;
    request->callback_user_data = callback_user_data;
#if !defined(_WIN32)
    if (pthread_mutex_init(&request->wait_mutex, NULL) != 0) {
        free(request);
        return DCC_ERR_RUNTIME;
    }
    if (pthread_cond_init(&request->wait_cond, NULL) != 0) {
        (void)pthread_mutex_destroy(&request->wait_mutex);
        free(request);
        return DCC_ERR_RUNTIME;
    }
    request->wait_initialized = 1U;
#endif
    *out = request;
    return DCC_OK;
}

void dcc_rest_request_handle_attach(
    dcc_rest_request_t *request,
    struct dcc_rest_async_request *async_request
) {
    if (request != NULL) {
        atomic_store_explicit(&request->async_request, async_request, memory_order_release);
    }
}

void dcc_rest_request_handle_release(dcc_rest_request_t *request) {
    if (request == NULL) {
        return;
    }
    if (atomic_fetch_sub_explicit(&request->references, 1U, memory_order_acq_rel) == 1U) {
        dcc_rest_result_free(request->result);
#if !defined(_WIN32)
        if (request->wait_initialized) {
            request->wait_initialized = 0U;
            (void)pthread_cond_destroy(&request->wait_cond);
            (void)pthread_mutex_destroy(&request->wait_mutex);
        }
#endif
        free(request);
    }
}

void dcc_rest_request_handle_finalize(
    dcc_rest_request_t *request,
    const dcc_rest_terminal_completion_t *completion
) {
    if (request == NULL || completion == NULL) {
        return;
    }
    bool expected = false;
    if (!atomic_compare_exchange_strong_explicit(
            &request->terminal_claimed,
            &expected,
            true,
            memory_order_acq_rel,
            memory_order_acquire
        )) {
        return;
    }

    if (dcc_rest_terminal_result_clone(completion, &request->result) != DCC_OK) {
        request->fallback_result = (dcc_rest_result_t){
            .size = sizeof(request->fallback_result),
            .version = DCC_REST_RESULT_VERSION,
            .transport_status = DCC_ERR_NOMEM,
        };
    }
    const dcc_rest_result_t *result = request->result != NULL
        ? request->result
        : &request->fallback_result;

    dcc_rest_request_delivery_frame_t frame = {
        .request = request,
        .previous = dcc_rest_request_delivery,
    };
    dcc_rest_request_delivery = &frame;
    dcc_rest_deliver_terminal_result(
        request->client,
        completion->operation,
        result,
        completion->legacy_error,
        NULL,
        NULL,
        request->callback,
        request->callback_user_data
    );
    dcc_rest_request_delivery = frame.previous;

    atomic_store_explicit(&request->async_request, NULL, memory_order_release);
#if !defined(_WIN32)
    if (request->wait_initialized && pthread_mutex_lock(&request->wait_mutex) == 0) {
        atomic_store_explicit(&request->completed, true, memory_order_release);
        (void)pthread_cond_broadcast(&request->wait_cond);
        (void)pthread_mutex_unlock(&request->wait_mutex);
    } else
#endif
    {
        atomic_store_explicit(&request->completed, true, memory_order_release);
    }
}

static int dcc_rest_request_delivery_active(const dcc_rest_request_t *request) {
    for (dcc_rest_request_delivery_frame_t *frame = dcc_rest_request_delivery;
         frame != NULL;
         frame = frame->previous) {
        if (frame->request == request) {
            return 1;
        }
    }
    return 0;
}

dcc_status_t dcc_rest_request_wait(
    dcc_rest_request_t *request,
    uint32_t timeout_ms,
    const dcc_rest_result_t **out_result
) {
    if (out_result != NULL) {
        *out_result = NULL;
    }
    if (request == NULL || out_result == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_rest_request_delivery_active(request)) {
        return DCC_ERR_STATE;
    }

    if (!atomic_load_explicit(&request->completed, memory_order_acquire)) {
#if !defined(_WIN32)
        if (llam_current_task() == NULL && request->wait_initialized) {
            int rc = pthread_mutex_lock(&request->wait_mutex);
            if (rc != 0) {
                return dcc_rest_status_from_pthread(rc);
            }
            dcc_status_t status = DCC_OK;
            if (timeout_ms == 0U) {
                while (!atomic_load_explicit(&request->completed, memory_order_acquire) &&
                       status == DCC_OK) {
                    status = dcc_rest_status_from_pthread(
                        pthread_cond_wait(&request->wait_cond, &request->wait_mutex)
                    );
                }
            } else {
                struct timespec deadline;
                status = dcc_rest_make_deadline_ms(timeout_ms, &deadline);
                while (!atomic_load_explicit(&request->completed, memory_order_acquire) &&
                       status == DCC_OK) {
                    rc = pthread_cond_timedwait(
                        &request->wait_cond,
                        &request->wait_mutex,
                        &deadline
                    );
                    if (rc == ETIMEDOUT) {
                        status = atomic_load_explicit(
                            &request->completed,
                            memory_order_acquire
                        ) ? DCC_OK : DCC_ERR_TIMEOUT;
                    } else {
                        status = dcc_rest_status_from_pthread(rc);
                    }
                }
            }
            (void)pthread_mutex_unlock(&request->wait_mutex);
            if (status != DCC_OK) {
                return status;
            }
        } else
#endif
        {
            uint64_t started_ms = dcc_rest_now_ms();
            while (!atomic_load_explicit(&request->completed, memory_order_acquire)) {
                if (timeout_ms != 0U && dcc_rest_now_ms() - started_ms >= timeout_ms) {
                    return DCC_ERR_TIMEOUT;
                }
#if defined(_WIN32)
                if (llam_current_task() == NULL) {
                    Sleep(1U);
                } else
#endif
                {
                    dcc_rest_sleep_ms(1U);
                }
            }
        }
    }
    *out_result = request->result != NULL
        ? request->result
        : &request->fallback_result;
    return DCC_OK;
}

uint8_t dcc_rest_request_completed(const dcc_rest_request_t *request) {
    return request != NULL &&
        atomic_load_explicit(&request->completed, memory_order_acquire)
        ? 1U
        : 0U;
}

static int dcc_rest_request_detach_pending_locked(
    dcc_client_t *client,
    dcc_rest_async_request_t *target
) {
    for (uint32_t priority = 0U; priority < DCC_REST_PRIORITY_LEVELS; ++priority) {
        dcc_rest_async_request_t *previous = NULL;
        dcc_rest_async_request_t *request = client->rest_async_heads[priority];
        while (request != NULL) {
            if (request == target) {
                if (previous != NULL) {
                    previous->next = request->next;
                } else {
                    client->rest_async_heads[priority] = request->next;
                }
                if (client->rest_async_tails[priority] == request) {
                    client->rest_async_tails[priority] = previous;
                }
                request->next = NULL;
                return 1;
            }
            previous = request;
            request = request->next;
        }
    }
    return 0;
}

static int dcc_rest_request_is_active_locked(
    const dcc_client_t *client,
    const dcc_rest_async_request_t *target
) {
    for (const dcc_rest_async_request_t *request = client->rest_async_active_head;
         request != NULL;
         request = request->active_next) {
        if (request == target) {
            return 1;
        }
    }
    return 0;
}

dcc_status_t dcc_rest_request_cancel(dcc_rest_request_t *request) {
    if (request == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (atomic_load_explicit(&request->completed, memory_order_acquire) ||
        atomic_load_explicit(&request->terminal_claimed, memory_order_acquire)) {
        return DCC_OK;
    }

    dcc_client_t *client = request->client;
    dcc_rest_async_request_t *async_request = atomic_load_explicit(
        &request->async_request,
        memory_order_acquire
    );
    if (client == NULL || async_request == NULL) {
        return DCC_OK;
    }

    llam_fd_t fd = LLAM_INVALID_FD;
    size_t fd_count = 0U;
    int pending = 0;
    dcc_rest_lock(client);
    if (!atomic_load_explicit(&request->terminal_claimed, memory_order_acquire)) {
        pending = dcc_rest_request_detach_pending_locked(client, async_request);
        if (!pending && dcc_rest_request_is_active_locked(client, async_request)) {
            (void)dcc_rest_async_request_cancel(async_request, &fd, &fd_count);
        }
    }
    dcc_rest_unlock(client);
    dcc_rest_async_signal(client);

    if (fd_count != 0U && !LLAM_FD_IS_INVALID(fd)) {
        (void)llam_close(fd);
    }
    if (pending) {
        dcc_rest_terminal_completion_t completion = {
            .operation = async_request->path,
            .transport_status = DCC_ERR_CANCELED,
            .legacy_error = DCC_ERR_CANCELED,
        };
        dcc_rest_request_handle_finalize(request, &completion);
        dcc_rest_async_request_free(async_request);
    }
    return DCC_OK;
}

void dcc_rest_request_destroy(dcc_rest_request_t *request) {
    if (request == NULL) {
        return;
    }
    bool expected = false;
    if (atomic_compare_exchange_strong_explicit(
            &request->caller_reference_released,
            &expected,
            true,
            memory_order_acq_rel,
            memory_order_acquire
        )) {
        (void)dcc_rest_request_cancel(request);
        dcc_rest_request_handle_release(request);
    }
}
