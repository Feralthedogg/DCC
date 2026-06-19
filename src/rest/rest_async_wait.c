#include "internal/rest/dcc_rest_async_wait_internal.h"

#if !defined(_WIN32)
#include <errno.h>
#endif

dcc_status_t dcc_rest_async_wait(dcc_client_t *client, uint32_t timeout_ms) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!client->runtime.initialized) {
        return DCC_ERR_STATE;
    }

    uint64_t start_ms = dcc_rest_now_ms();
#if !defined(_WIN32)
    int locked = 0;
    if (client->rest_wait_initialized) {
        int rc = pthread_mutex_lock(&client->rest_wait_mutex);
        if (rc != 0) {
            return dcc_rest_status_from_pthread(rc);
        }
        locked = 1;
    }
#endif
    for (;;) {
        uint64_t now = dcc_rest_now_ms();
        uint64_t next_wake_ms = 0;
        dcc_rest_lock(client);
        size_t pending = dcc_rest_async_pending_count_locked(client);
        size_t active = client->rest_async_active;
        if (pending != 0 && active == 0 && !atomic_load_explicit(&client->stopping, memory_order_acquire)) {
            (void)dcc_rest_async_drain_locked(client);
            pending = dcc_rest_async_pending_count_locked(client);
            active = client->rest_async_active;
        }
        if (pending != 0) {
            next_wake_ms = dcc_rest_async_next_wake_ms_locked(client, now);
        }
        dcc_rest_unlock(client);

        if (pending == 0 && active == 0) {
#if !defined(_WIN32)
            if (locked) {
                (void)pthread_mutex_unlock(&client->rest_wait_mutex);
            }
#endif
            return DCC_OK;
        }
        if (timeout_ms != 0) {
            uint64_t elapsed_ms = now - start_ms;
            if (elapsed_ms >= timeout_ms) {
#if !defined(_WIN32)
                if (locked) {
                    (void)pthread_mutex_unlock(&client->rest_wait_mutex);
                }
#endif
                return DCC_ERR_TIMEOUT;
            }
        }

#if !defined(_WIN32)
        if (locked) {
            uint64_t wait_ms = next_wake_ms;
            if (timeout_ms != 0) {
                uint64_t elapsed_ms = now - start_ms;
                uint64_t remaining_ms = elapsed_ms >= timeout_ms ? 1U : timeout_ms - elapsed_ms;
                if (wait_ms == 0 || remaining_ms < wait_ms) {
                    wait_ms = remaining_ms;
                }
            }
            if (wait_ms == 0) {
                int rc = pthread_cond_wait(&client->rest_wait_cond, &client->rest_wait_mutex);
                if (rc != 0) {
                    (void)pthread_mutex_unlock(&client->rest_wait_mutex);
                    return dcc_rest_status_from_pthread(rc);
                }
            } else {
                struct timespec deadline;
                dcc_status_t deadline_status = dcc_rest_make_deadline_ms(wait_ms, &deadline);
                if (deadline_status != DCC_OK) {
                    (void)pthread_mutex_unlock(&client->rest_wait_mutex);
                    return deadline_status;
                }
                int rc = pthread_cond_timedwait(&client->rest_wait_cond, &client->rest_wait_mutex, &deadline);
                if (rc != 0 && rc != ETIMEDOUT) {
                    (void)pthread_mutex_unlock(&client->rest_wait_mutex);
                    return dcc_rest_status_from_pthread(rc);
                }
            }
            continue;
        }
#endif
        dcc_rest_sleep_ms(1);
    }
}
