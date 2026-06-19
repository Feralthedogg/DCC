#ifndef DCC_HOT_RELOAD_OPTIONS_H
#define DCC_HOT_RELOAD_OPTIONS_H

#include <dcc/hot_reload/base.h>
#include <dcc/hot_reload/canary.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_HOT_RELOAD_DEFAULT_WORKER "dcc_hot_reload_worker"

typedef enum dcc_hot_reload_backend {
    DCC_HOT_RELOAD_BACKEND_IN_PROCESS = 0,
    DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER = 1
} dcc_hot_reload_backend_t;

typedef struct dcc_hot_reload_options {
    size_t size;
    uint32_t poll_interval_ms;
    void *user_data;
    const char *init_symbol;
    const char *deinit_symbol;
    uint32_t settle_interval_ms;
    dcc_hot_reload_backend_t backend;
    const char *worker_path;
    uint32_t worker_health_timeout_ms;
    uint32_t worker_drain_timeout_ms;
    uint8_t worker_canary_enabled;
    dcc_hot_reload_canary_options_t worker_canary_options;
} dcc_hot_reload_options_t;

typedef struct dcc_hot_reload_run_options {
    size_t size;
    dcc_client_options_t client_options;
    const char *library_path;
    dcc_hot_reload_options_t hot_reload_options;
} dcc_hot_reload_run_options_t;

#ifdef __cplusplus
}
#endif

#endif
