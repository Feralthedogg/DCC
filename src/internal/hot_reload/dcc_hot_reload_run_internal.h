#ifndef DCC_HOT_RELOAD_RUN_INTERNAL_H
#define DCC_HOT_RELOAD_RUN_INTERNAL_H

#include <dcc/hot_reload.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_hot_reload_run_options_has(
    const dcc_hot_reload_run_options_t *options,
    size_t offset,
    size_t size
);
const char *dcc_hot_reload_env_nonempty(const char *name);
const char *dcc_hot_reload_run_worker_path_or_default(const char *worker_path);
dcc_status_t dcc_hot_reload_run_resolve_client_options(
    const dcc_hot_reload_run_options_t *options,
    dcc_client_options_t *out
);
dcc_status_t dcc_hot_reload_run_resolve_library_path(
    const dcc_hot_reload_run_options_t *options,
    const char **out
);
dcc_status_t dcc_hot_reload_run_resolve_options(
    const dcc_hot_reload_run_options_t *options,
    dcc_client_options_t *client_options,
    const char **library_path,
    dcc_hot_reload_options_t *hot_reload_options,
    const dcc_hot_reload_options_t **hot_reload_options_ptr
);

#ifdef __cplusplus
}
#endif

#endif
