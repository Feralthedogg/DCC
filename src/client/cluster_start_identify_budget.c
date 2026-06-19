#include "internal/client/dcc_cluster_internal.h"

static int dcc_cluster_has_known_identify_limit(const dcc_client_t *client) {
    return client != NULL &&
        (client->gateway_session_total_identifies != 0 ||
         client->gateway_session_reset_after_ms != 0 ||
         client->gateway_session_reset_at_ms != 0);
}

dcc_status_t dcc_cluster_check_initial_identify_budget(dcc_cluster_t *cluster) {
    if (cluster == NULL || cluster->shard_count == 0 || cluster->shards[0] == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_client_t *first = cluster->shards[0];
    if (!dcc_cluster_has_known_identify_limit(first) ||
        first->gateway_remaining_identifies >= cluster->shard_count) {
        return DCC_OK;
    }

    const char *message = "not enough remaining gateway identify sessions";
    dcc_set_error(first, message);
    dcc_cluster_health_lock(cluster);
    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        cluster->health[i].status = DCC_CLUSTER_SHARD_ERROR;
        dcc_cluster_copy_text(cluster->health[i].last_error, sizeof(cluster->health[i].last_error), message);
    }
    dcc_cluster_health_unlock(cluster);
    return DCC_ERR_RATE_LIMITED;
}
