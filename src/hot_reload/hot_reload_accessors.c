#include "internal/hot_reload/dcc_hot_reload_internal.h"

const char *dcc_hot_reload_path(const dcc_hot_reload_t *hot_reload) {
    return hot_reload != NULL && hot_reload->path != NULL ? hot_reload->path : "";
}

uint64_t dcc_hot_reload_generation(const dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return 0;
    }
    dcc_hot_reload_t *mutable_hot_reload = (dcc_hot_reload_t *)hot_reload;
    dcc_hot_reload_lock(mutable_hot_reload);
    uint64_t generation = mutable_hot_reload->generation;
    dcc_hot_reload_unlock(mutable_hot_reload);
    return generation;
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
    if (hot_reload == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_hot_reload_t *mutable_hot_reload = (dcc_hot_reload_t *)hot_reload;
    dcc_hot_reload_lock(mutable_hot_reload);
    dcc_status_t status = mutable_hot_reload->last_status;
    dcc_hot_reload_unlock(mutable_hot_reload);
    return status;
}

const char *dcc_hot_reload_last_error(const dcc_hot_reload_t *hot_reload) {
    static _Thread_local char last_error[256];
    if (hot_reload == NULL) {
        return "";
    }
    dcc_hot_reload_t *mutable_hot_reload = (dcc_hot_reload_t *)hot_reload;
    dcc_hot_reload_lock(mutable_hot_reload);
    dcc_hot_reload_copy_text(last_error, sizeof(last_error), mutable_hot_reload->last_error);
    dcc_hot_reload_unlock(mutable_hot_reload);
    return last_error[0] != '\0' ? last_error : "";
}

uint8_t dcc_hot_reload_running(const dcc_hot_reload_t *hot_reload) {
    return hot_reload != NULL && atomic_load_explicit(&hot_reload->watch_running, memory_order_acquire) ? 1U : 0U;
}
