#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdint.h>

dcc_status_t dcc_hot_reload_wait_for_generation(
    dcc_hot_reload_t *hot_reload,
    uint64_t generation,
    uint32_t timeout_ms
) {
    if (hot_reload == NULL || generation == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    uint64_t deadline_ms = UINT64_MAX;
    if (timeout_ms != 0U) {
        uint64_t now = dcc_hot_reload_now_ms();
        deadline_ms = UINT64_MAX - now < (uint64_t)timeout_ms ? UINT64_MAX : now + (uint64_t)timeout_ms;
    }

    dcc_hot_reload_lock(hot_reload);
    while (hot_reload->generation < generation) {
        uint32_t wait_ms = 100U;
        if (timeout_ms != 0U) {
            uint64_t now = dcc_hot_reload_now_ms();
            if (now >= deadline_ms) {
                dcc_hot_reload_unlock(hot_reload);
                return DCC_ERR_TIMEOUT;
            }
            uint64_t remaining = deadline_ms - now;
            if (remaining < wait_ms) {
                wait_ms = (uint32_t)remaining;
            }
            if (wait_ms == 0U) {
                dcc_hot_reload_unlock(hot_reload);
                return DCC_ERR_TIMEOUT;
            }
        }
        dcc_status_t status = dcc_hot_reload_wait_timeout_locked(hot_reload, wait_ms);
        if (status != DCC_OK && status != DCC_ERR_TIMEOUT) {
            dcc_hot_reload_unlock(hot_reload);
            return status;
        }
    }
    dcc_hot_reload_unlock(hot_reload);
    return DCC_OK;
}

dcc_status_t dcc_hot_reload_wait_until_loaded(
    dcc_hot_reload_t *hot_reload,
    uint32_t timeout_ms,
    dcc_hot_reload_health_snapshot_t *out
) {
    if (hot_reload == NULL || (out != NULL && out->size < sizeof(*out))) {
        return DCC_ERR_INVALID_ARG;
    }

    uint64_t deadline_ms = UINT64_MAX;
    if (timeout_ms != 0U) {
        uint64_t now = dcc_hot_reload_now_ms();
        deadline_ms = UINT64_MAX - now < (uint64_t)timeout_ms ? UINT64_MAX : now + (uint64_t)timeout_ms;
    }

    for (;;) {
        dcc_hot_reload_health_snapshot_t snapshot = {
            .size = sizeof(snapshot),
        };
        dcc_status_t status = dcc_hot_reload_health_snapshot(hot_reload, &snapshot);
        if (status != DCC_OK) {
            return status;
        }
        if (out != NULL) {
            *out = snapshot;
        }
        if (snapshot.loaded != 0U) {
            return DCC_OK;
        }
        if (snapshot.health == DCC_HOT_RELOAD_HEALTH_DEGRADED) {
            return DCC_ERR_STATE;
        }

        uint32_t wait_ms = 100U;
        if (timeout_ms != 0U) {
            uint64_t now = dcc_hot_reload_now_ms();
            if (now >= deadline_ms) {
                return DCC_ERR_TIMEOUT;
            }
            uint64_t remaining = deadline_ms - now;
            if (remaining < wait_ms) {
                wait_ms = (uint32_t)remaining;
            }
            if (wait_ms == 0U) {
                return DCC_ERR_TIMEOUT;
            }
        }
        dcc_hot_reload_lock(hot_reload);
        status = dcc_hot_reload_wait_timeout_locked(hot_reload, wait_ms);
        dcc_hot_reload_unlock(hot_reload);
        if (status != DCC_OK && status != DCC_ERR_TIMEOUT) {
            return status;
        }
    }
}
