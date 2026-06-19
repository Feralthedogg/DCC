#ifndef DCC_CLUSTER_RECOVERY_TYPES_H
#define DCC_CLUSTER_RECOVERY_TYPES_H

#include <dcc/cluster/health_types.h>
#include <dcc/cluster/shard_types.h>
#include <dcc/error.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_cluster_recovery_action {
    DCC_CLUSTER_RECOVERY_UNKNOWN = 0,
    DCC_CLUSTER_RECOVERY_NONE,
    DCC_CLUSTER_RECOVERY_WAIT,
    DCC_CLUSTER_RECOVERY_RECONNECT,
    DCC_CLUSTER_RECOVERY_REARM_RESTART,
    DCC_CLUSTER_RECOVERY_INVESTIGATE
} dcc_cluster_recovery_action_t;

typedef struct dcc_cluster_recovery_plan {
    size_t size;
    dcc_cluster_health_state_t health;
    dcc_cluster_recovery_action_t action;
    uint32_t shard_index;
    uint32_t shard_id;
    dcc_cluster_shard_status_t shard_status;
    dcc_cluster_health_summary_t summary;
    dcc_cluster_shard_info_t shard;
    char reason[256];
} dcc_cluster_recovery_plan_t;

typedef struct dcc_cluster_remediation_options {
    size_t size;
    uint8_t allow_reconnect;
    uint8_t resume;
    uint8_t allow_rearm_restart;
    uint8_t reserved;
} dcc_cluster_remediation_options_t;

typedef struct dcc_cluster_remediation_result {
    size_t size;
    dcc_cluster_recovery_plan_t plan;
    dcc_status_t action_status;
    uint8_t action_attempted;
    uint8_t action_allowed;
    uint8_t reserved[6];
} dcc_cluster_remediation_result_t;

typedef struct dcc_cluster_remediation_loop_options {
    size_t size;
    dcc_cluster_remediation_options_t remediation;
    uint32_t max_attempts;
    uint32_t interval_ms;
    uint8_t stop_on_healthy;
    uint8_t stop_on_no_action;
    uint8_t reserved[6];
} dcc_cluster_remediation_loop_options_t;

typedef struct dcc_cluster_remediation_loop_result {
    size_t size;
    dcc_cluster_remediation_result_t last;
    dcc_status_t last_status;
    uint32_t attempts;
    uint32_t actions_attempted;
    dcc_cluster_health_state_t final_health;
    dcc_cluster_recovery_action_t final_action;
    uint8_t reached_healthy;
    uint8_t exhausted_attempts;
    uint8_t stopped_no_action;
    uint8_t stopped_error;
    uint8_t reserved[4];
} dcc_cluster_remediation_loop_result_t;

#ifdef __cplusplus
}
#endif

#endif
