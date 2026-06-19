#include <dcc/dcc.h>

#include "support/cluster_smoke_support.h"

#include <stdio.h>

int main(void) {
    dcc_cluster_t *cluster = NULL;
    unsigned supervisor_policy_probe_user_data = 0;
    unsigned supervisor_decision_probe_user_data = 0;
    dcc_cluster_options_t opts = {
        .size = sizeof(opts),
        .client_options = {
            .size = sizeof(dcc_client_options_t),
            .token = "",
            .intents = DCC_INTENT_GUILDS,
        },
        .shard_count = 3,
        .total_shard_count = 3,
        .gateway_max_concurrency = 2,
        .supervisor_policy = cluster_smoke_supervisor_policy_probe,
        .supervisor_policy_user_data = &supervisor_policy_probe_user_data,
        .supervisor_decision_callback = cluster_smoke_supervisor_decision_probe,
        .supervisor_decision_user_data = &supervisor_decision_probe_user_data,
    };

    dcc_status_t status = dcc_cluster_create(&opts, &cluster);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_cluster_create failed: %s\n", dcc_status_string(status));
        return 1;
    }

    dcc_cluster_recovery_plan_t plan;
    dcc_cluster_remediation_result_t remediation;
    dcc_cluster_remediation_loop_result_t loop_result;
    if (cluster_smoke_check_supervisor_helpers(
            cluster,
            cluster_smoke_supervisor_policy_probe,
            &supervisor_policy_probe_user_data,
            cluster_smoke_supervisor_decision_probe,
            &supervisor_decision_probe_user_data
        ) ||
        cluster_smoke_check_shard_accessors(cluster) ||
        cluster_smoke_check_initial_recovery(cluster, &plan, &remediation, &loop_result) ||
        cluster_smoke_check_json_contracts(cluster, &plan, &remediation, &loop_result) ||
        cluster_smoke_check_health_classifiers() ||
        cluster_smoke_check_operation_and_admission_contracts(cluster) ||
        cluster_smoke_check_initial_shards(cluster)) {
        dcc_cluster_destroy(cluster);
        return 1;
    }

    cluster_smoke_state_t state;
    cluster_smoke_state_init(&state, cluster);

    dcc_listener_id_t ids[3];
    status = dcc_cluster_on(cluster, DCC_EVENT_READY, cluster_smoke_on_ready, &state, ids, 3);
    if (status != DCC_OK || ids[0] == 0 || ids[1] == 0 || ids[2] == 0) {
        fprintf(stderr, "dcc_cluster_on failed: %s\n", dcc_status_string(status));
        dcc_cluster_destroy(cluster);
        return 1;
    }

    status = dcc_cluster_start(cluster);
    if (status == DCC_OK) {
        status = dcc_cluster_wait(cluster);
    }
    if (status != DCC_OK) {
        fprintf(stderr, "cluster run failed: %s\n", dcc_status_string(status));
        dcc_cluster_destroy(cluster);
        return 1;
    }

    if (cluster_smoke_check_runtime_state(cluster, &state) ||
        cluster_smoke_check_post_run_health(cluster) ||
        cluster_smoke_check_counter_resets(cluster)) {
        dcc_cluster_destroy(cluster);
        return 1;
    }

    if (dcc_cluster_clear_operation(cluster) != DCC_OK) {
        fprintf(stderr, "cluster operation final clear failed\n");
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_destroy(cluster);
    return 0;
}
