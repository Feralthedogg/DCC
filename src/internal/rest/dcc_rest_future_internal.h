#ifndef DCC_REST_FUTURE_INTERNAL_H
#define DCC_REST_FUTURE_INTERNAL_H

#include "internal/client/dcc_client_state_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

#include <dcc/dcc.h>
#include <llam/runtime.h>

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#if !defined(_WIN32)
#include <pthread.h>
#endif

struct dcc_rest_future {
    llam_channel_t *done;
    atomic_bool completed;
#if !defined(_WIN32)
    pthread_mutex_t wait_mutex;
    pthread_cond_t wait_cond;
    uint8_t wait_initialized;
#endif
    dcc_rest_response_t response;
    char *body;
    size_t body_len;
};

dcc_status_t dcc_rest_future_create(dcc_rest_future_t **out);
dcc_status_t dcc_rest_future_wait_channel(dcc_rest_future_t *future, uint32_t timeout_ms);
void dcc_rest_future_deinit_completed(dcc_rest_future_t *future);
void dcc_rest_future_mark_completed(dcc_rest_future_t *future);
void dcc_rest_future_store_response(
    dcc_rest_future_t *future,
    const dcc_rest_response_t *response
);

#endif
