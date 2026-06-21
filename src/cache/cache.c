#include "internal/cache/dcc_cache_internal.h"
#include "internal/client/dcc_client_guild_inference_internal.h"

#include <stddef.h>
#include <string.h>

void dcc_cache_init(dcc_cache_t *cache) {
    if (cache == NULL) {
        return;
    }
    memset(cache, 0, sizeof(*cache));
    atomic_flag_clear(&cache->lock);
}

void dcc_cache_deinit(dcc_cache_t *cache) {
    if (cache == NULL) {
        return;
    }
    dcc_cache_table_deinit(&cache->guilds, dcc_cache_free_guild_value);
    dcc_cache_table_deinit(&cache->channels, dcc_cache_free_channel_value);
    dcc_cache_table_deinit(&cache->roles, dcc_cache_free_role_value);
    dcc_cache_table_deinit(&cache->members, dcc_cache_free_member_value);
    dcc_cache_table_deinit(&cache->users, dcc_cache_free_user_value);
    dcc_cache_table_deinit(&cache->messages, dcc_cache_free_message_value);
    dcc_cache_table_deinit(&cache->voice_states, dcc_cache_free_voice_state_value);
}

uint8_t dcc_cache_enabled(const dcc_client_t *client) {
    return client != NULL && client->enable_cache ? 1U : 0U;
}

void dcc_cache_clear(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }
    dcc_cache_lock(&client->cache);
    dcc_cache_table_clear(&client->cache.guilds, dcc_cache_free_guild_value);
    dcc_cache_table_clear(&client->cache.channels, dcc_cache_free_channel_value);
    dcc_cache_table_clear(&client->cache.roles, dcc_cache_free_role_value);
    dcc_cache_table_clear(&client->cache.members, dcc_cache_free_member_value);
    dcc_cache_table_clear(&client->cache.users, dcc_cache_free_user_value);
    dcc_cache_table_clear(&client->cache.messages, dcc_cache_free_message_value);
    dcc_cache_table_clear(&client->cache.voice_states, dcc_cache_free_voice_state_value);
    client->cache.sequence = 0;
    dcc_client_channel_guild_inference_clear(client);
    dcc_cache_unlock(&client->cache);
}

dcc_status_t dcc_cache_counts(const dcc_client_t *client, dcc_cache_counts_t *out) {
    if (client == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cache_t *cache = (dcc_cache_t *)&client->cache;
    dcc_cache_lock(cache);
    out->guilds = client->cache.guilds.size;
    out->channels = client->cache.channels.size;
    out->roles = client->cache.roles.size;
    out->members = client->cache.members.size;
    out->users = client->cache.users.size;
    out->messages = client->cache.messages.size;
    out->voice_states = client->cache.voice_states.size;
    dcc_cache_unlock(cache);
    return DCC_OK;
}
