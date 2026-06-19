#include "internal/client/dcc_cluster_internal.h"

#include <stddef.h>
#include <string.h>

static uint32_t dcc_cluster_option_u32(
    const dcc_cluster_options_t *options,
    size_t field_offset,
    uint32_t value
) {
    return options->size >= field_offset + sizeof(uint32_t) ? value : 0U;
}

static void dcc_cluster_restart_policy_normalize(dcc_cluster_restart_policy_t *policy) {
    if (policy == NULL) {
        return;
    }
    policy->size = sizeof(*policy);
    if (policy->base_delay_ms == 0) {
        policy->base_delay_ms = 250U;
    }
    if (policy->max_delay_ms == 0) {
        policy->max_delay_ms = 5000U;
    }
    if (policy->max_delay_ms < policy->base_delay_ms) {
        policy->max_delay_ms = policy->base_delay_ms;
    }
}

dcc_cluster_restart_policy_t dcc_cluster_restart_policy_from_options(
    const dcc_cluster_options_t *options
) {
    dcc_cluster_restart_policy_t policy;
    memset(&policy, 0, sizeof(policy));
    policy.size = sizeof(policy);
    policy.max_restarts = dcc_cluster_option_u32(
        options,
        offsetof(dcc_cluster_options_t, shard_restart_max),
        options->shard_restart_max
    );
    policy.base_delay_ms = dcc_cluster_option_u32(
        options,
        offsetof(dcc_cluster_options_t, shard_restart_base_delay_ms),
        options->shard_restart_base_delay_ms
    );
    policy.max_delay_ms = dcc_cluster_option_u32(
        options,
        offsetof(dcc_cluster_options_t, shard_restart_max_delay_ms),
        options->shard_restart_max_delay_ms
    );
    dcc_cluster_restart_policy_normalize(&policy);
    return policy;
}

uint32_t dcc_cluster_restart_delay_ms(
    const dcc_cluster_restart_policy_t *policy,
    uint64_t restarts
) {
    uint64_t delay = policy->base_delay_ms == 0 ? 250ULL : policy->base_delay_ms;
    uint32_t max_delay = policy->max_delay_ms == 0 ? 5000U : policy->max_delay_ms;
    uint64_t shifts = restarts < 16U ? restarts : 16U;
    for (uint64_t i = 0; i < shifts && delay < max_delay; ++i) {
        delay *= 2ULL;
        if (delay > max_delay) {
            delay = max_delay;
        }
    }
    return (uint32_t)delay;
}

dcc_status_t dcc_cluster_get_restart_policy(
    const dcc_cluster_t *cluster,
    dcc_cluster_restart_policy_t *out
) {
    if (cluster == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = cluster->restart_policy;
    out->size = sizeof(*out);
    return DCC_OK;
}

dcc_status_t dcc_cluster_set_restart_policy(
    dcc_cluster_t *cluster,
    const dcc_cluster_restart_policy_t *policy
) {
    if (cluster == NULL || policy == NULL || policy->size < sizeof(*policy)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (cluster->started) {
        return DCC_ERR_STATE;
    }
    cluster->restart_policy = *policy;
    dcc_cluster_restart_policy_normalize(&cluster->restart_policy);
    return DCC_OK;
}
