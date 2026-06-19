#include "package_consumer_checks.h"

#include <stdint.h>
#include <string.h>

int dcc_package_consumer_check_hot_reload_api(void) {
    dcc_package_hot_reload_attach_fn hot_reload_attach = dcc_hot_reload_attach;
    dcc_package_hot_reload_run_simple_fn hot_reload_run_simple = dcc_hot_reload_run_simple;
    dcc_package_hot_reload_run_simple_isolated_fn hot_reload_run_simple_isolated =
        dcc_hot_reload_run_simple_isolated;
    dcc_package_hot_reload_run_env_fn hot_reload_run_env = dcc_hot_reload_run_env;
    dcc_package_hot_reload_run_env_isolated_fn hot_reload_run_env_isolated = dcc_hot_reload_run_env_isolated;
    dcc_package_hot_reload_run_fn hot_reload_run = dcc_hot_reload_run;
    dcc_package_hot_reload_wait_fn hot_reload_wait = dcc_hot_reload_wait_for_generation;
    dcc_package_hot_reload_health_fn hot_reload_health = dcc_hot_reload_health_snapshot;
    dcc_package_hot_reload_health_string_fn hot_reload_health_string = dcc_hot_reload_health_string;
    dcc_package_hot_reload_health_json_fn hot_reload_health_json = dcc_hot_reload_health_snapshot_json;
    dcc_package_hot_reload_health_route_fn hot_reload_health_route = dcc_hot_reload_add_health_route;
    dcc_hot_reload_options_t hot_reload_worker_options = {
        .size = sizeof(hot_reload_worker_options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = "dcc_hot_reload_worker",
        .worker_health_timeout_ms = 1000,
        .worker_drain_timeout_ms = 100,
    };

    return hot_reload_attach != NULL &&
           hot_reload_run_simple != NULL &&
           hot_reload_run_simple_isolated != NULL &&
           hot_reload_run_env != NULL &&
           hot_reload_run_env_isolated != NULL &&
           hot_reload_run != NULL &&
           hot_reload_wait != NULL &&
           hot_reload_health != NULL &&
           hot_reload_health_string != NULL &&
           hot_reload_health_json != NULL &&
           hot_reload_health_route != NULL &&
           hot_reload_attach(NULL, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
           hot_reload_run(NULL) == DCC_ERR_INVALID_ARG &&
           hot_reload_health(NULL, NULL) == DCC_ERR_INVALID_ARG &&
           dcc_hot_reload_wait_until_loaded(NULL, 1, NULL) == DCC_ERR_INVALID_ARG &&
           hot_reload_health_route(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
           hot_reload_health_string(DCC_HOT_RELOAD_HEALTH_WATCHING) != NULL &&
           strcmp(DCC_HOT_RELOAD_DEFAULT_WORKER, "dcc_hot_reload_worker") == 0 &&
           hot_reload_worker_options.backend == DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER &&
           hot_reload_worker_options.worker_health_timeout_ms == 1000U &&
           sizeof(((dcc_hot_reload_health_snapshot_t *)0)->worker_dispatch_failures) == sizeof(uint64_t) &&
           DCC_HOT_RELOAD_BACKEND_IN_PROCESS == 0;
}
