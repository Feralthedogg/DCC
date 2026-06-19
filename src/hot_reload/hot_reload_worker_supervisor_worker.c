#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <llam/runtime.h>

#include <stdlib.h>

void dcc_hot_reload_worker_supervisor_sleep_ms(uint32_t ms) {
    uint32_t remaining = ms == 0U ? 1U : ms;
    while (remaining > 0U) {
        uint32_t slice = remaining > 100U ? 100U : remaining;
        if (llam_sleep_ns((uint64_t)slice * UINT64_C(1000000)) != 0) {
            break;
        }
        remaining -= slice;
    }
}

uint8_t dcc_hot_reload_worker_supervisor_probe(
    dcc_hot_reload_worker_process_t *worker,
    uint32_t timeout_ms
) {
    if (worker == NULL || !dcc_hot_reload_worker_process_is_alive(worker)) {
        return 0U;
    }
    return dcc_hot_reload_worker_process_healthcheck(worker, timeout_ms) == DCC_OK ? 1U : 0U;
}

void dcc_hot_reload_worker_supervisor_retire(
    dcc_hot_reload_worker_process_t *worker,
    uint32_t timeout_ms
) {
    if (worker == NULL) {
        return;
    }
    dcc_hot_reload_worker_process_stop(worker, timeout_ms);
    free(worker);
}
