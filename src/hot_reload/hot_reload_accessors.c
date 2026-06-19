#include "internal/hot_reload/dcc_hot_reload_internal.h"

const char *dcc_hot_reload_path(const dcc_hot_reload_t *hot_reload) {
    return hot_reload != NULL && hot_reload->path != NULL ? hot_reload->path : "";
}

uint64_t dcc_hot_reload_generation(const dcc_hot_reload_t *hot_reload) {
    return hot_reload != NULL ? hot_reload->generation : 0;
}

void *dcc_hot_reload_state(dcc_hot_reload_t *hot_reload, size_t size) {
    return dcc_hot_reload_state_reserve(hot_reload, size, 0);
}

size_t dcc_hot_reload_state_size(const dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return 0;
    }
    dcc_hot_reload_t *mutable_hot_reload = (dcc_hot_reload_t *)hot_reload;
    dcc_hot_reload_lock(mutable_hot_reload);
    size_t size = hot_reload->module_state_size;
    dcc_hot_reload_unlock(mutable_hot_reload);
    return size;
}

dcc_status_t dcc_hot_reload_last_status(const dcc_hot_reload_t *hot_reload) {
    return hot_reload != NULL ? hot_reload->last_status : DCC_ERR_INVALID_ARG;
}

const char *dcc_hot_reload_last_error(const dcc_hot_reload_t *hot_reload) {
    return hot_reload != NULL && hot_reload->last_error[0] != '\0' ? hot_reload->last_error : "";
}

uint8_t dcc_hot_reload_running(const dcc_hot_reload_t *hot_reload) {
    return hot_reload != NULL && atomic_load_explicit(&hot_reload->watch_running, memory_order_acquire) ? 1U : 0U;
}
