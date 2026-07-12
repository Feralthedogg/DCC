#include "internal/client/dcc_cluster_internal.h"

#include <stddef.h>
#include <stdatomic.h>
#include <stdlib.h>

static void dcc_cluster_apply_supervisor_options(
    dcc_cluster_t *cluster,
    const dcc_cluster_options_t *options
) {
    if (options->size >= offsetof(dcc_cluster_options_t, supervisor_policy) +
            sizeof(options->supervisor_policy)) {
        cluster->supervisor_policy = options->supervisor_policy;
    }
    if (options->size >= offsetof(dcc_cluster_options_t, supervisor_policy_user_data) +
            sizeof(options->supervisor_policy_user_data)) {
        cluster->supervisor_policy_user_data = options->supervisor_policy_user_data;
    }
    if (options->size >= offsetof(dcc_cluster_options_t, supervisor_decision_callback) +
            sizeof(options->supervisor_decision_callback)) {
        cluster->supervisor_decision_callback = options->supervisor_decision_callback;
    }
    if (options->size >= offsetof(dcc_cluster_options_t, supervisor_decision_user_data) +
            sizeof(options->supervisor_decision_user_data)) {
        cluster->supervisor_decision_user_data = options->supervisor_decision_user_data;
    }
}

dcc_status_t dcc_cluster_alloc_from_plan(
    const dcc_cluster_options_t *options,
    const dcc_cluster_create_plan_t *plan,
    dcc_cluster_t **out
) {
    if (options == NULL || plan == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_cluster_t *cluster = (dcc_cluster_t *)calloc(1, sizeof(*cluster));
    if (cluster == NULL) {
        return DCC_ERR_NOMEM;
    }
    cluster->shards = (dcc_client_t **)calloc(plan->shard_count, sizeof(cluster->shards[0]));
    cluster->health = (dcc_cluster_shard_info_t *)calloc(plan->shard_count, sizeof(cluster->health[0]));
    if (cluster->shards == NULL || cluster->health == NULL) {
        dcc_cluster_destroy_partial(cluster);
        return DCC_ERR_NOMEM;
    }

    atomic_flag_clear(&cluster->health_lock);
    cluster->shard_count = plan->shard_count;
    cluster->first_shard_id = plan->first_shard_id;
    cluster->total_shard_count = plan->total_shard_count;
    cluster->gateway_max_concurrency = plan->max_concurrency;
    cluster->restart_policy = dcc_cluster_restart_policy_from_options(options);
    cluster->supervisor_status.size = sizeof(cluster->supervisor_status);
    cluster->supervisor_status.last_shard_index = DCC_CLUSTER_NO_SHARD_INDEX;
    cluster->supervisor_status.last_shard_id = DCC_CLUSTER_NO_SHARD_INDEX;
    dcc_cluster_apply_supervisor_options(cluster, options);
    dcc_cluster_operation_reset(&cluster->operation, 0);
    atomic_init(&cluster->supervisor_stop, false);
    atomic_init(&cluster->gateway_admission_open, true);
    atomic_flag_clear(&cluster->identify_coordinator.lock);
    cluster->identify_coordinator.max_concurrency = plan->max_concurrency;
    atomic_init(&cluster->identify_coordinator.waits, 0U);
    atomic_init(&cluster->identify_coordinator.reservations, 0U);

    *out = cluster;
    return DCC_OK;
}
