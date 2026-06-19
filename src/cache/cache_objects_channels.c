#include "internal/cache/dcc_cache_internal.h"

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

uint8_t dcc_cache_remove_channel(dcc_client_t *client, dcc_snowflake_t channel_id) {
    if (client == NULL || !client->enable_cache || channel_id == 0) {
        return 0;
    }
    dcc_cache_lock(&client->cache);
    uint8_t removed = dcc_cache_table_remove(&client->cache.channels, channel_id, 0, dcc_cache_free_channel_value);
    dcc_cache_unlock(&client->cache);
    return removed;
}
