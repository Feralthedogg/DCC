#include "internal/cache/dcc_cache_internal.h"
#include "internal/client/dcc_client_guild_inference_internal.h"

dcc_status_t dcc_cache_put_channel(dcc_client_t *client, const dcc_channel_t *channel) {
    if (channel == NULL || channel->id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }
    dcc_channel_t *copy = dcc_channel_clone(channel);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_cache_lock(&client->cache);
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
    } else if (channel->guild_id != 0U) {
        dcc_client_channel_guild_inference_store(client, channel->id, channel->guild_id);
    } else {
        dcc_client_channel_guild_inference_clear(client);
    }
    return status;
}

const dcc_channel_t *dcc_cache_get_channel(const dcc_client_t *client, dcc_snowflake_t channel_id) {
    if (client == NULL || !client->enable_cache || channel_id == 0) {
        return NULL;
    }
    dcc_cache_t *cache = (dcc_cache_t *)&client->cache;
    dcc_cache_lock(cache);
    const dcc_channel_t *value = (const dcc_channel_t *)dcc_cache_table_get(&client->cache.channels, channel_id, 0);
    dcc_cache_unlock(cache);
    return value;
}

dcc_status_t dcc_cache_infer_guild_id_from_channel(
    const dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t *out_guild_id
) {
    if (client == NULL || out_guild_id == NULL || channel_id == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_guild_id = 0U;
    if (!client->enable_cache) {
        return DCC_ERR_STATE;
    }

    dcc_snowflake_t cached_guild =
        dcc_client_channel_guild_inference_load(client, channel_id);
    if (cached_guild != 0U) {
        *out_guild_id = cached_guild;
        return DCC_OK;
    }

    dcc_cache_t *cache = (dcc_cache_t *)&client->cache;
    dcc_cache_lock(cache);
    const dcc_channel_t *value =
        (const dcc_channel_t *)dcc_cache_table_get(&client->cache.channels, channel_id, 0);
    dcc_snowflake_t guild_id = value != NULL ? value->guild_id : 0U;
    dcc_cache_unlock(cache);

    if (guild_id == 0U) {
        return DCC_ERR_NOT_FOUND;
    }
    *out_guild_id = guild_id;
    dcc_client_channel_guild_inference_store((dcc_client_t *)client, channel_id, guild_id);
    return DCC_OK;
}

uint8_t dcc_cache_remove_channel(dcc_client_t *client, dcc_snowflake_t channel_id) {
    if (client == NULL || !client->enable_cache || channel_id == 0) {
        return 0;
    }
    dcc_cache_lock(&client->cache);
    uint8_t removed = dcc_cache_table_remove(&client->cache.channels, channel_id, 0, dcc_cache_free_channel_value);
    dcc_cache_unlock(&client->cache);
    if (removed) {
        dcc_client_channel_guild_inference_clear_if_channel(client, channel_id);
    }
    return removed;
}
