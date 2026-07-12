#include "internal/client/dcc_cluster_internal.h"

dcc_status_t dcc_cluster_create_shards(
    dcc_cluster_t *cluster,
    const dcc_cluster_options_t *options,
    const dcc_cluster_create_plan_t *plan
) {
    if (cluster == NULL || options == NULL || plan == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    for (uint32_t i = 0; i < plan->shard_count; ++i) {
        dcc_client_options_t client_options = options->client_options;
        client_options.size = sizeof(client_options);
        client_options.shard_id = plan->first_shard_id + i;
        client_options.shard_count = plan->total_shard_count;
        client_options.gateway_max_concurrency = plan->max_concurrency;

        dcc_status_t status = dcc_client_create(&client_options, &cluster->shards[i]);
        if (status != DCC_OK) {
            return status;
        }
        cluster->shards[i]->gateway_identify_coordinator = &cluster->identify_coordinator;

        dcc_cluster_shard_info_t *health = &cluster->health[i];
        health->size = sizeof(*health);
        health->shard_id = client_options.shard_id;
        health->shard_count = client_options.shard_count;
        health->status = DCC_CLUSTER_SHARD_CREATED;

        status = dcc_cluster_register_health_events(cluster, i);
        if (status != DCC_OK) {
            return status;
        }
    }

    return dcc_cluster_apply_identify_delays(cluster);
}
