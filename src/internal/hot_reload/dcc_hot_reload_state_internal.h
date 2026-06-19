#ifndef DCC_HOT_RELOAD_STATE_INTERNAL_H
#define DCC_HOT_RELOAD_STATE_INTERNAL_H

#include "internal/dcc_windows_internal.h"

#include "internal/client/dcc_client_state_internal.h"
#include "internal/hot_reload/dcc_hot_reload_file_sig_internal.h"
#include "internal/hot_reload/dcc_hot_reload_module_state_internal.h"

#include <dcc/hot_reload/canary.h>

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(_WIN32)
#include <pthread.h>
#endif

typedef struct dcc_hot_reload_worker_process dcc_hot_reload_worker_process_t;

struct dcc_hot_reload {
    dcc_client_t *client;
    char *path;
    char *init_symbol;
    char *deinit_symbol;
    char *worker_path;
    uint32_t poll_interval_ms;
    uint32_t settle_interval_ms;
    uint32_t worker_health_timeout_ms;
    uint32_t worker_drain_timeout_ms;
    dcc_hot_reload_backend_t backend;
    void *user_data;
    dcc_hot_reload_module_t module;
    dcc_hot_reload_worker_process_t *active_worker;
    dcc_hot_reload_worker_process_t *last_good_worker;
    dcc_hot_reload_worker_process_t *candidate_worker;
    dcc_hot_reload_canary_options_t worker_canary_options;
    dcc_hot_reload_canary_state_t worker_canary;
    dcc_listener_id_t listener_ids[DCC_EVENT_MAX];
    uint8_t listener_installed[DCC_EVENT_MAX];
    dcc_hot_reload_file_sig_t file_sig;
    uint8_t has_file_sig;
    dcc_hot_reload_file_sig_t pending_file_sig;
    uint8_t has_pending_file_sig;
    uint64_t pending_since_ms;
    uint64_t generation;
    uint64_t worker_active_failures;
    uint64_t worker_last_good_promotions;
    uint64_t worker_dispatch_failures;
    uint64_t worker_temp_error_responses;
    uint64_t worker_canary_promotions;
    uint64_t worker_canary_rollbacks;
    uint32_t active_calls;
    uint8_t worker_canary_enabled;
    uint8_t reloading;
    dcc_status_t last_status;
    char last_error[256];
    void *module_state;
    void *module_state_raw;
    size_t module_state_size;
    size_t module_state_alignment;
    atomic_bool watch_running;
    atomic_bool watch_stop;
    atomic_bool worker_supervisor_running;
    atomic_bool worker_supervisor_stop;
#if defined(_WIN32)
    CRITICAL_SECTION mutex;
    CONDITION_VARIABLE cond;
#else
    pthread_mutex_t mutex;
    pthread_cond_t cond;
#endif
};

#endif
