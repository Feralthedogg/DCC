#include "internal/runtime/dcc_task_group_internal.h"

#include <stdint.h>

uint64_t dcc_task_group_deadline_from_timeout(uint32_t timeout_ms) {
    uint64_t now = llam_now_ns();
    uint64_t add = (uint64_t)timeout_ms * UINT64_C(1000000);
    if (UINT64_MAX - now < add) {
        return UINT64_MAX;
    }
    return now + add;
}

int dcc_task_group_client_accepts_work(const dcc_client_t *client) {
    return client != NULL &&
        client->runtime.initialized &&
        atomic_load_explicit(&client->started, memory_order_acquire) &&
        !atomic_load_explicit(&client->stopping, memory_order_acquire);
}
