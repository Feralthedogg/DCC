#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdlib.h>

dcc_status_t dcc_hot_reload_create(
    dcc_client_t *client,
    const char *library_path,
    const dcc_hot_reload_options_t *options,
    dcc_hot_reload_t **out
) {
    if (client == NULL || library_path == NULL || library_path[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_hot_reload_t *hot_reload = (dcc_hot_reload_t *)calloc(1, sizeof(*hot_reload));
    if (hot_reload == NULL) {
        return DCC_ERR_NOMEM;
    }
    hot_reload->client = client;
    hot_reload->last_status = DCC_OK;
    atomic_init(&hot_reload->watch_running, false);
    atomic_init(&hot_reload->watch_stop, false);
    atomic_init(&hot_reload->worker_supervisor_running, false);
    atomic_init(&hot_reload->worker_supervisor_stop, false);
#if defined(_WIN32)
    InitializeCriticalSection(&hot_reload->mutex);
    InitializeConditionVariable(&hot_reload->cond);
#else
    (void)pthread_mutex_init(&hot_reload->mutex, NULL);
    (void)pthread_cond_init(&hot_reload->cond, NULL);
#endif

    dcc_status_t status = dcc_hot_reload_apply_create_options(hot_reload, library_path, options);
    if (status != DCC_OK) {
        dcc_hot_reload_destroy(hot_reload);
        return status;
    }

    *out = hot_reload;
    return DCC_OK;
}

void dcc_hot_reload_destroy(dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return;
    }
    (void)dcc_hot_reload_stop(hot_reload);

    for (int i = 0; i < DCC_EVENT_MAX; ++i) {
        if (hot_reload->listener_installed[i]) {
            (void)dcc_client_off(hot_reload->client, (dcc_event_type_t)i, hot_reload->listener_ids[i]);
        }
    }

    dcc_hot_reload_lock(hot_reload);
    while (hot_reload->active_calls > 0) {
        dcc_hot_reload_wait_locked(hot_reload);
    }
    dcc_hot_reload_unlock(hot_reload);
    dcc_hot_reload_module_deinit(hot_reload, &hot_reload->module);
    dcc_hot_reload_worker_destroy_all(hot_reload);
    free(hot_reload->module_state_raw);

#if defined(_WIN32)
    DeleteCriticalSection(&hot_reload->mutex);
#else
    (void)pthread_cond_destroy(&hot_reload->cond);
    (void)pthread_mutex_destroy(&hot_reload->mutex);
#endif
    free(hot_reload->deinit_symbol);
    free(hot_reload->init_symbol);
    free(hot_reload->worker_path);
    free(hot_reload->path);
    free(hot_reload);
}
