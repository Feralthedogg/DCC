#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdlib.h>

const char *dcc_hot_reload_env_nonempty(const char *name) {
    const char *value = getenv(name);
    return value != NULL && value[0] != '\0' ? value : NULL;
}

const char *dcc_hot_reload_run_worker_path_or_default(const char *worker_path) {
    if (worker_path != NULL && worker_path[0] != '\0') {
        return worker_path;
    }
    worker_path = dcc_hot_reload_env_nonempty("DCC_HOT_RELOAD_WORKER");
    return worker_path != NULL ? worker_path : DCC_HOT_RELOAD_DEFAULT_WORKER;
}
