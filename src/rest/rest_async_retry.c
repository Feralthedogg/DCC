#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_async_drain_internal.h"
#include "internal/rest/dcc_rest_async_retry_internal.h"
#include "internal/rest/dcc_rest_async_signal_internal.h"
#include "internal/rest/dcc_rest_async_worker_lifecycle_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

#include <stdlib.h>

typedef struct dcc_rest_async_retry_wake {
    dcc_client_t *client;
    uint64_t not_before_ms;
} dcc_rest_async_retry_wake_t;

static void dcc_rest_async_retry_wake_worker(void *arg) {
    dcc_rest_async_retry_wake_t *wake = (dcc_rest_async_retry_wake_t *)arg;
    if (wake == NULL) {
        return;
    }
    dcc_client_t *client = wake->client;
    uint64_t not_before_ms = wake->not_before_ms;
    free(wake);

    for (;;) {
        uint64_t now = dcc_rest_now_ms();
        if (now >= not_before_ms) {
            break;
        }
        dcc_rest_sleep_ms(not_before_ms - now);
    }

    if (client == NULL) {
        return;
    }
    dcc_rest_lock(client);
    if (!atomic_load_explicit(&client->stopping, memory_order_acquire)) {
        (void)dcc_rest_async_drain_locked(client);
    }
    dcc_rest_unlock(client);
    dcc_rest_async_signal(client);
}

uint64_t dcc_rest_async_retry_not_before_ms(dcc_rest_async_request_t *request) {
    dcc_client_t *client = request != NULL ? request->client : NULL;
    uint64_t now = dcc_rest_now_ms();
    uint64_t wait_ms = 1U;
    if (client == NULL) {
        return dcc_rest_add_ms(now, wait_ms);
    }

    dcc_rest_lock(client);
    uint64_t route_wait_ms = dcc_rest_route_wait_ms_locked(client, request->route, now, 0);
    dcc_rest_unlock(client);

    if (route_wait_ms > wait_ms) {
        wait_ms = route_wait_ms;
    }
    return dcc_rest_add_ms(now, wait_ms);
}

int dcc_rest_async_requeue_retry(dcc_rest_async_request_t *request, uint64_t not_before_ms) {
    dcc_client_t *client = request != NULL ? request->client : NULL;
    if (client == NULL) {
        return 0;
    }
    dcc_rest_async_retry_wake_t *wake = (dcc_rest_async_retry_wake_t *)malloc(sizeof(*wake));
    if (wake == NULL) {
        return 0;
    }
    wake->client = client;
    wake->not_before_ms = not_before_ms;

    dcc_rest_lock(client);
    if (atomic_load_explicit(&client->stopping, memory_order_acquire)) {
        dcc_rest_unlock(client);
        free(wake);
        return 0;
    }
    dcc_rest_async_finish_active_locked(client, request);
    request->not_before_ms = not_before_ms;
    request->callback_called = 0;
    (void)atomic_exchange_explicit(&request->active_fd, LLAM_INVALID_FD, memory_order_acq_rel);
    dcc_rest_async_push_tail_locked(client, request);
    (void)dcc_rest_async_drain_locked(client);
    dcc_status_t wake_status = dcc_runtime_spawn(&client->runtime, dcc_rest_async_retry_wake_worker, wake);
    if (wake_status != DCC_OK) {
        free(wake);
        dcc_set_error(client, dcc_status_string(wake_status));
    }
    dcc_rest_unlock(client);
    dcc_rest_async_signal(client);
    return 1;
}
