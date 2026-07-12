#ifndef DCC_CLUSTER_HEALTH_H
#define DCC_CLUSTER_HEALTH_H

#include <dcc/cluster/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_cluster_get_restart_policy(
    const dcc_cluster_t *cluster,
    dcc_cluster_restart_policy_t *out
);
DCC_API dcc_status_t dcc_cluster_set_restart_policy(
    dcc_cluster_t *cluster,
    const dcc_cluster_restart_policy_t *policy
);
DCC_API dcc_status_t dcc_cluster_shard_info(
    const dcc_cluster_t *cluster,
    uint32_t index,
    dcc_cluster_shard_info_t *out
);
DCC_API dcc_status_t dcc_cluster_health_summary(
    const dcc_cluster_t *cluster,
    dcc_cluster_health_summary_t *out
);
DCC_API dcc_status_t dcc_cluster_identify_stats(
    const dcc_cluster_t *cluster,
    dcc_cluster_identify_stats_t *out
);
DCC_API dcc_status_t dcc_cluster_wait_until_ready(
    dcc_cluster_t *cluster,
    uint32_t timeout_ms,
    dcc_cluster_health_summary_t *out
);
DCC_API void dcc_cluster_health_wait_options_init(
    dcc_cluster_health_wait_options_t *options
);
DCC_API dcc_status_t dcc_cluster_wait_until_health(
    dcc_cluster_t *cluster,
    const dcc_cluster_health_wait_options_t *options,
    dcc_cluster_recovery_plan_t *out
);
DCC_API dcc_status_t dcc_cluster_recovery_plan(
    const dcc_cluster_t *cluster,
    dcc_cluster_recovery_plan_t *out
);
DCC_API dcc_status_t dcc_cluster_remediate(
    dcc_cluster_t *cluster,
    const dcc_cluster_remediation_options_t *options,
    dcc_cluster_remediation_result_t *out
);
DCC_API void dcc_cluster_remediation_loop_options_init(
    dcc_cluster_remediation_loop_options_t *options
);
DCC_API dcc_status_t dcc_cluster_remediate_loop(
    dcc_cluster_t *cluster,
    const dcc_cluster_remediation_loop_options_t *options,
    dcc_cluster_remediation_loop_result_t *out
);
DCC_API dcc_status_t dcc_cluster_health_summary_json(
    const dcc_cluster_health_summary_t *summary,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API dcc_status_t dcc_cluster_health_summary_prometheus(
    const dcc_cluster_health_summary_t *summary,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API dcc_status_t dcc_cluster_identify_stats_prometheus(
    const dcc_cluster_identify_stats_t *stats,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API dcc_status_t dcc_cluster_recovery_plan_json(
    const dcc_cluster_recovery_plan_t *plan,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API dcc_status_t dcc_cluster_remediation_result_json(
    const dcc_cluster_remediation_result_t *result,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API dcc_status_t dcc_cluster_remediation_loop_result_json(
    const dcc_cluster_remediation_loop_result_t *result,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API dcc_status_t dcc_cluster_status_json(
    const dcc_cluster_t *cluster,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API dcc_status_t dcc_cluster_reset_shard_counters(dcc_cluster_t *cluster, uint32_t index);
DCC_API dcc_status_t dcc_cluster_reset_all_counters(dcc_cluster_t *cluster);
DCC_API dcc_status_t dcc_cluster_rearm_shard_restart(dcc_cluster_t *cluster, uint32_t index);
DCC_API dcc_status_t dcc_cluster_rearm_all_shard_restarts(dcc_cluster_t *cluster);
DCC_API dcc_cluster_health_state_t dcc_cluster_health_summary_state(
    const dcc_cluster_health_summary_t *summary
);
DCC_API const char *dcc_cluster_health_state_string(dcc_cluster_health_state_t state);
DCC_API dcc_cluster_recovery_action_t dcc_cluster_shard_recovery_action(
    const dcc_cluster_shard_info_t *info
);
DCC_API const char *dcc_cluster_recovery_action_string(dcc_cluster_recovery_action_t action);
DCC_API const char *dcc_cluster_shard_status_string(dcc_cluster_shard_status_t status);

#ifdef __cplusplus
}
#endif

#endif
