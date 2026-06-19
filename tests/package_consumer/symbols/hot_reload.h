#ifndef DCC_PACKAGE_CONSUMER_SYMBOLS_HOT_RELOAD_H
#define DCC_PACKAGE_CONSUMER_SYMBOLS_HOT_RELOAD_H

#include <dcc/dcc.h>

typedef dcc_status_t (*dcc_package_hot_reload_attach_fn)(
        dcc_client_t *,
        const char *,
        const dcc_hot_reload_options_t *,
        dcc_hot_reload_t **
    );
typedef dcc_status_t (*dcc_package_hot_reload_run_simple_fn)(
        const char *,
        const char *,
        dcc_intents_t
    );
typedef dcc_status_t (*dcc_package_hot_reload_run_simple_isolated_fn)(
        const char *,
        const char *,
        const char *,
        dcc_intents_t
    );
typedef dcc_status_t (*dcc_package_hot_reload_run_env_fn)(dcc_intents_t);
typedef dcc_status_t (*dcc_package_hot_reload_run_env_isolated_fn)(dcc_intents_t);
typedef dcc_status_t (*dcc_package_hot_reload_run_fn)(const dcc_hot_reload_run_options_t *);
typedef dcc_status_t (*dcc_package_hot_reload_wait_fn)(dcc_hot_reload_t *, uint64_t, uint32_t);
typedef dcc_status_t (*dcc_package_hot_reload_health_fn)(
        const dcc_hot_reload_t *,
        dcc_hot_reload_health_snapshot_t *
    );
typedef const char *(*dcc_package_hot_reload_health_string_fn)(dcc_hot_reload_health_t);
typedef dcc_status_t (*dcc_package_hot_reload_health_json_fn)(
        const dcc_hot_reload_health_snapshot_t *,
        char *,
        size_t,
        size_t *
    );
typedef dcc_status_t (*dcc_package_hot_reload_health_route_fn)(
        dcc_interaction_server_t *,
        const char *,
        dcc_hot_reload_t *
    );

#endif
