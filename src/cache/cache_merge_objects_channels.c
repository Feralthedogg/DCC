#include "internal/cache/dcc_cache_internal.h"
#include "internal/client/dcc_client_guild_inference_internal.h"

dcc_status_t dcc_cache_merge_channel(dcc_client_t *client, const dcc_channel_t *channel, uint64_t fields) {
    if (channel == NULL || channel->id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cache_lock(&client->cache);
    const dcc_channel_t *existing =
        (const dcc_channel_t *)dcc_cache_table_get(&client->cache.channels, channel->id, 0);
    dcc_channel_t merged = *channel;
    if (existing != NULL) {
        dcc_cache_merge_channel_view(&merged, existing, fields);
    }
    dcc_channel_t *copy = dcc_channel_clone(&merged);
    if (copy == NULL) {
        dcc_cache_unlock(&client->cache);
        return DCC_ERR_NOMEM;
    }
    dcc_snowflake_t merged_channel_id = copy->id;
    dcc_snowflake_t merged_guild_id = copy->guild_id;
    status = dcc_cache_table_put(
        &client->cache.channels,
        channel->id,
        0,
        copy,
        dcc_cache_free_channel_value,
        dcc_cache_next_sequence(&client->cache),
        client->cache_policy.max_channels
    );
    dcc_cache_unlock(&client->cache);
    if (status != DCC_OK) {
        dcc_channel_free(copy);
    } else if (merged_guild_id != 0U) {
        dcc_client_channel_guild_inference_store(client, merged_channel_id, merged_guild_id);
    } else {
        dcc_client_channel_guild_inference_clear(client);
    }
    return status;
}

dcc_status_t dcc_cache_merge_role(dcc_client_t *client, const dcc_role_t *role, uint64_t fields) {
    if (role == NULL || role->guild_id == 0 || role->id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cache_lock(&client->cache);
    const dcc_role_t *existing =
        (const dcc_role_t *)dcc_cache_table_get(&client->cache.roles, role->guild_id, role->id);
    dcc_role_t merged = *role;
    if (existing != NULL) {
        dcc_cache_merge_role_view(&merged, existing, fields);
    }
    dcc_role_t *copy = dcc_role_clone(&merged);
    if (copy == NULL) {
        dcc_cache_unlock(&client->cache);
        return DCC_ERR_NOMEM;
    }
    status = dcc_cache_table_put(
        &client->cache.roles,
        role->guild_id,
        role->id,
        copy,
        dcc_cache_free_role_value,
        dcc_cache_next_sequence(&client->cache),
        client->cache_policy.max_roles
    );
    dcc_cache_unlock(&client->cache);
    if (status != DCC_OK) {
        dcc_role_free(copy);
    }
    return status;
}
