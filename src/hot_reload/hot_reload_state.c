#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include "internal/dcc_align.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static int dcc_hot_reload_is_power_of_two(size_t value) {
    return value != 0 && (value & (value - 1U)) == 0;
}

static dcc_status_t dcc_hot_reload_normalize_alignment(size_t *alignment) {
    if (alignment == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (*alignment == 0) {
        *alignment = DCC_MAX_ALIGNMENT;
    }
    if (*alignment < sizeof(void *)) {
        *alignment = sizeof(void *);
    }
    return dcc_hot_reload_is_power_of_two(*alignment) ? DCC_OK : DCC_ERR_INVALID_ARG;
}

static void dcc_hot_reload_state_set_error(
    dcc_hot_reload_t *hot_reload,
    dcc_status_t status,
    const char *message
) {
    dcc_hot_reload_lock(hot_reload);
    dcc_hot_reload_set_error(hot_reload, status, message);
    dcc_hot_reload_broadcast(hot_reload);
    dcc_hot_reload_unlock(hot_reload);
}

void *dcc_hot_reload_state_reserve(dcc_hot_reload_t *hot_reload, size_t size, size_t alignment) {
    if (hot_reload == NULL) {
        return NULL;
    }

    dcc_status_t status = dcc_hot_reload_normalize_alignment(&alignment);
    if (status != DCC_OK) {
        dcc_hot_reload_state_set_error(hot_reload, status, "hot reload state alignment must be a power of two");
        return NULL;
    }

    dcc_hot_reload_lock(hot_reload);
    if (size == 0) {
        void *state = hot_reload->module_state;
        dcc_hot_reload_unlock(hot_reload);
        return state;
    }
    if (hot_reload->module_state != NULL) {
        if (size > hot_reload->module_state_size || alignment > hot_reload->module_state_alignment) {
            dcc_hot_reload_set_error(
                hot_reload,
                DCC_ERR_STATE,
                "hot reload state layout is already fixed for this host run"
            );
            dcc_hot_reload_broadcast(hot_reload);
            dcc_hot_reload_unlock(hot_reload);
            return NULL;
        }
        void *state = hot_reload->module_state;
        dcc_hot_reload_unlock(hot_reload);
        return state;
    }
    dcc_hot_reload_unlock(hot_reload);

    if (size > SIZE_MAX - (alignment - 1U)) {
        dcc_hot_reload_state_set_error(hot_reload, DCC_ERR_NOMEM, "hot reload state size overflow");
        return NULL;
    }
    size_t allocation_size = size + alignment - 1U;
    void *raw = calloc(1, allocation_size);
    if (raw == NULL) {
        dcc_hot_reload_state_set_error(hot_reload, DCC_ERR_NOMEM, "hot reload state allocation failed");
        return NULL;
    }

    uintptr_t base = (uintptr_t)raw;
    if (base > UINTPTR_MAX - (uintptr_t)(alignment - 1U)) {
        free(raw);
        dcc_hot_reload_state_set_error(hot_reload, DCC_ERR_NOMEM, "hot reload state pointer alignment overflow");
        return NULL;
    }
    uintptr_t aligned = (base + (uintptr_t)(alignment - 1U)) & ~(uintptr_t)(alignment - 1U);

    dcc_hot_reload_lock(hot_reload);
    if (hot_reload->module_state == NULL) {
        hot_reload->module_state_raw = raw;
        hot_reload->module_state = (void *)aligned;
        hot_reload->module_state_size = size;
        hot_reload->module_state_alignment = alignment;
        raw = NULL;
    } else if (size > hot_reload->module_state_size || alignment > hot_reload->module_state_alignment) {
        dcc_hot_reload_set_error(
            hot_reload,
            DCC_ERR_STATE,
            "hot reload state layout is already fixed for this host run"
        );
        dcc_hot_reload_broadcast(hot_reload);
    }
    void *state = hot_reload->module_state;
    dcc_hot_reload_unlock(hot_reload);

    free(raw);
    return state;
}
