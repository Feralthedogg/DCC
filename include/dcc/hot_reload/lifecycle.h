#ifndef DCC_HOT_RELOAD_LIFECYCLE_H
#define DCC_HOT_RELOAD_LIFECYCLE_H

#include <dcc/hot_reload/health.h>
#include <dcc/hot_reload/options.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_hot_reload_create(
    dcc_client_t *client,
    const char *library_path,
    const dcc_hot_reload_options_t *options,
    dcc_hot_reload_t **out
);
DCC_API void dcc_hot_reload_destroy(dcc_hot_reload_t *hot_reload);

DCC_API dcc_status_t dcc_hot_reload_attach(
    dcc_client_t *client,
    const char *library_path,
    const dcc_hot_reload_options_t *options,
    dcc_hot_reload_t **out
);
DCC_API dcc_status_t dcc_hot_reload_run_simple(
    const char *library_path,
    const char *token,
    dcc_intents_t intents
);
DCC_API dcc_status_t dcc_hot_reload_run_simple_isolated(
    const char *library_path,
    const char *worker_path,
    const char *token,
    dcc_intents_t intents
);
DCC_API dcc_status_t dcc_hot_reload_run_env(dcc_intents_t intents);
DCC_API dcc_status_t dcc_hot_reload_run_env_isolated(dcc_intents_t intents);
DCC_API dcc_status_t dcc_hot_reload_run(const dcc_hot_reload_run_options_t *options);
DCC_API dcc_status_t dcc_hot_reload_reload(dcc_hot_reload_t *hot_reload);
DCC_API dcc_status_t dcc_hot_reload_poll(dcc_hot_reload_t *hot_reload);
DCC_API dcc_status_t dcc_hot_reload_start(dcc_hot_reload_t *hot_reload);
DCC_API dcc_status_t dcc_hot_reload_stop(dcc_hot_reload_t *hot_reload);
DCC_API dcc_status_t dcc_hot_reload_wait_for_generation(
    dcc_hot_reload_t *hot_reload,
    uint64_t generation,
    uint32_t timeout_ms
);
DCC_API dcc_status_t dcc_hot_reload_wait_until_loaded(
    dcc_hot_reload_t *hot_reload,
    uint32_t timeout_ms,
    dcc_hot_reload_health_snapshot_t *out
);

#ifdef __cplusplus
}
#endif

#endif
