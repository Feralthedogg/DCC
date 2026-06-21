#include "internal/cache/dcc_cache_internal.h"
#include "internal/client/dcc_client_guild_inference_internal.h"

#include <stddef.h>
#include <string.h>

static void dcc_cache_trim_locked(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }
    dcc_cache_table_trim(&client->cache.guilds, client->cache_policy.max_guilds, dcc_cache_free_guild_value);
    dcc_cache_table_trim(&client->cache.channels, client->cache_policy.max_channels, dcc_cache_free_channel_value);
    dcc_cache_table_trim(&client->cache.roles, client->cache_policy.max_roles, dcc_cache_free_role_value);
    dcc_cache_table_trim(&client->cache.members, client->cache_policy.max_members, dcc_cache_free_member_value);
    dcc_cache_table_trim(&client->cache.users, client->cache_policy.max_users, dcc_cache_free_user_value);
    dcc_cache_table_trim(&client->cache.messages, client->cache_policy.max_messages, dcc_cache_free_message_value);
    dcc_cache_table_trim(
        &client->cache.voice_states,
        client->cache_policy.max_voice_states,
        dcc_cache_free_voice_state_value
    );
}

dcc_status_t dcc_cache_get_policy(const dcc_client_t *client, dcc_cache_policy_t *out) {
    if (client == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cache_t *cache = (dcc_cache_t *)&client->cache;
    dcc_cache_lock(cache);
    *out = client->cache_policy;
    out->size = sizeof(*out);
    dcc_cache_unlock(cache);
    return DCC_OK;
}

dcc_status_t dcc_cache_set_policy(dcc_client_t *client, const dcc_cache_policy_t *policy) {
    if (client == NULL || policy == NULL || policy->size < offsetof(dcc_cache_policy_t, max_guilds)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cache_policy_t next;
    memset(&next, 0, sizeof(next));
    next.size = sizeof(next);

#define DCC_CACHE_POLICY_HAS(field) \
    (policy->size >= offsetof(dcc_cache_policy_t, field) + sizeof(policy->field))
    if (DCC_CACHE_POLICY_HAS(max_guilds)) {
        next.max_guilds = policy->max_guilds;
    }
    if (DCC_CACHE_POLICY_HAS(max_channels)) {
        next.max_channels = policy->max_channels;
    }
    if (DCC_CACHE_POLICY_HAS(max_roles)) {
        next.max_roles = policy->max_roles;
    }
    if (DCC_CACHE_POLICY_HAS(max_members)) {
        next.max_members = policy->max_members;
    }
    if (DCC_CACHE_POLICY_HAS(max_users)) {
        next.max_users = policy->max_users;
    }
    if (DCC_CACHE_POLICY_HAS(max_messages)) {
        next.max_messages = policy->max_messages;
    }
    if (DCC_CACHE_POLICY_HAS(max_voice_states)) {
        next.max_voice_states = policy->max_voice_states;
    }
#undef DCC_CACHE_POLICY_HAS

    dcc_cache_lock(&client->cache);
    client->cache_policy = next;
    dcc_cache_trim_locked(client);
    dcc_client_channel_guild_inference_clear(client);
    dcc_cache_unlock(&client->cache);
    return DCC_OK;
}
