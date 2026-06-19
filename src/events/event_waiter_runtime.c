#include "internal/events/dcc_events_internal.h"

#include <errno.h>

dcc_status_t dcc_event_wait_status_from_errno(int err) {
    switch (err) {
        case ETIMEDOUT:
        case EAGAIN:
            return DCC_ERR_TIMEOUT;
        case ECANCELED:
        case EPIPE:
            return DCC_ERR_CANCELED;
        case ENOMEM:
            return DCC_ERR_NOMEM;
        case EINVAL:
            return DCC_ERR_INVALID_ARG;
        case ENOTSUP:
        case EXDEV:
            return DCC_ERR_STATE;
        default:
            return DCC_ERR_RUNTIME;
    }
}

void dcc_event_waiter_remove_locked(dcc_event_bus_t *bus, dcc_event_waiter_node_t *waiter) {
    if (bus == NULL || waiter == NULL) {
        return;
    }
    dcc_event_waiter_node_t **cursor = &bus->waiters;
    while (*cursor != NULL) {
        if (*cursor == waiter) {
            *cursor = waiter->next;
            waiter->next = NULL;
            return;
        }
        cursor = &(*cursor)->next;
    }
}

void dcc_event_waiter_drain_channel(llam_channel_t *channel) {
    if (channel == NULL) {
        return;
    }
    void *value = NULL;
    while (llam_channel_try_recv_result(channel, &value) == 0) {
        value = NULL;
    }
}

dcc_status_t dcc_event_waiter_wait_channel(
    dcc_event_waiter_node_t *waiter,
    uint32_t timeout_ms
) {
    if (waiter == NULL || waiter->done == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (waiter->completed) {
        return DCC_OK;
    }

    if (llam_current_task() != NULL) {
        void *value = NULL;
        int rc = timeout_ms == 0
            ? llam_channel_recv_result(waiter->done, &value)
            : llam_channel_recv_until_result(waiter->done, dcc_event_waiter_deadline_ns(timeout_ms), &value);
        if (rc != 0) {
            return dcc_event_wait_status_from_errno(errno);
        }
        return value == waiter && waiter->completed ? DCC_OK : DCC_ERR_RUNTIME;
    }

    uint64_t start_ms = llam_now_ns() / UINT64_C(1000000);
    for (;;) {
        if (waiter->completed) {
            return DCC_OK;
        }
        void *value = NULL;
        if (llam_channel_try_recv_result(waiter->done, &value) == 0) {
            return value == waiter && waiter->completed ? DCC_OK : DCC_ERR_RUNTIME;
        }
        int err = errno;
        if (err != EAGAIN) {
            return dcc_event_wait_status_from_errno(err);
        }
        if (timeout_ms != 0) {
            uint64_t now_ms = llam_now_ns() / UINT64_C(1000000);
            if (now_ms - start_ms >= timeout_ms) {
                return DCC_ERR_TIMEOUT;
            }
        }
        dcc_event_waiter_sleep_ms(1);
    }
}
