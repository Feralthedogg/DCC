#ifndef DCC_CLUSTER_HEALTH_TYPES_H
#define DCC_CLUSTER_HEALTH_TYPES_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_cluster_health_state {
    DCC_CLUSTER_HEALTH_UNKNOWN = 0,
    DCC_CLUSTER_HEALTH_HEALTHY,
    DCC_CLUSTER_HEALTH_STARTING,
    DCC_CLUSTER_HEALTH_DRAINING,
    DCC_CLUSTER_HEALTH_DEGRADED,
    DCC_CLUSTER_HEALTH_CRITICAL
} dcc_cluster_health_state_t;

typedef struct dcc_cluster_health_summary {
    size_t size;
    uint32_t total_shards;
    uint32_t created_shards;
    uint32_t starting_shards;
    uint32_t ready_shards;
    uint32_t stopping_shards;
    uint32_t stopped_shards;
    uint32_t error_shards;
    uint64_t starts;
    uint64_t restart_attempts;
    uint64_t restarts;
    uint64_t restart_suppressed;
    uint64_t restart_budget_used;
    uint64_t ready_events;
    uint64_t resumed_events;
    uint64_t socket_closes;
    uint64_t log_errors;
} dcc_cluster_health_summary_t;

typedef struct dcc_cluster_identify_stats {
    size_t size;
    uint64_t waits;
    uint64_t reservations;
    uint32_t remaining;
    uint32_t total;
    uint32_t max_concurrency;
    uint32_t reserved;
    uint64_t reset_at_ms;
} dcc_cluster_identify_stats_t;

typedef struct dcc_cluster_health_wait_options {
    size_t size;
    dcc_cluster_health_state_t target_health;
    uint32_t timeout_ms;
    uint32_t interval_ms;
    uint8_t fail_on_critical;
    uint8_t reserved[7];
} dcc_cluster_health_wait_options_t;

#ifdef __cplusplus
}
#endif

#endif
