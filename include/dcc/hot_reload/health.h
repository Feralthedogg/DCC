#ifndef DCC_HOT_RELOAD_HEALTH_H
#define DCC_HOT_RELOAD_HEALTH_H

#include <dcc/hot_reload/options.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_hot_reload_health {
    DCC_HOT_RELOAD_HEALTH_UNKNOWN = 0,
    DCC_HOT_RELOAD_HEALTH_STOPPED,
    DCC_HOT_RELOAD_HEALTH_WATCHING,
    DCC_HOT_RELOAD_HEALTH_RELOADING,
    DCC_HOT_RELOAD_HEALTH_DEGRADED
} dcc_hot_reload_health_t;

typedef struct dcc_hot_reload_health_snapshot {
    size_t size;
    dcc_hot_reload_health_t health;
    dcc_hot_reload_backend_t backend;
    uint8_t ok;
    uint8_t running;
    uint8_t loaded;
    uint8_t reloading;
    uint8_t pending_reload;
    uint8_t worker_supervisor_running;
    uint8_t active_worker_loaded;
    uint8_t last_good_worker_loaded;
    uint8_t candidate_worker_loaded;
    uint8_t worker_canary_enabled;
    uint8_t worker_canary_active;
    uint8_t worker_canary_percent;
    uint32_t active_calls;
    uint32_t poll_interval_ms;
    uint32_t settle_interval_ms;
    uint32_t worker_health_timeout_ms;
    uint32_t worker_drain_timeout_ms;
    uint64_t generation;
    uint64_t active_worker_generation;
    uint64_t last_good_worker_generation;
    uint64_t candidate_worker_generation;
    uint64_t active_worker_pid;
    uint64_t last_good_worker_pid;
    uint64_t candidate_worker_pid;
    uint64_t worker_active_failures;
    uint64_t worker_last_good_promotions;
    uint64_t worker_dispatch_failures;
    uint64_t worker_temp_error_responses;
    uint64_t worker_canary_routed;
    uint64_t worker_canary_successes;
    uint64_t worker_canary_failures;
    uint64_t worker_canary_promotions;
    uint64_t worker_canary_rollbacks;
    dcc_status_t last_status;
    dcc_hot_reload_canary_rollback_reason_t worker_canary_rollback_reason;
    char path[512];
    char last_error[256];
    char reason[160];
} dcc_hot_reload_health_snapshot_t;

DCC_API dcc_status_t dcc_hot_reload_health_snapshot(
    const dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_health_snapshot_t *out
);

DCC_API const char *dcc_hot_reload_health_string(dcc_hot_reload_health_t health);

DCC_API dcc_status_t dcc_hot_reload_health_snapshot_json(
    const dcc_hot_reload_health_snapshot_t *snapshot,
    char *out,
    size_t out_size,
    size_t *out_len
);

#ifdef __cplusplus
}
#endif

#endif
