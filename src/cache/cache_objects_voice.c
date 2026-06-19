#include "internal/cache/dcc_cache_internal.h"

dcc_status_t dcc_cache_put_voice_state(dcc_client_t *client, const dcc_voice_state_t *voice_state) {
    if (voice_state == NULL || voice_state->guild_id == 0 || voice_state->user_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }
    dcc_voice_state_t *copy = dcc_voice_state_clone(voice_state);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_cache_lock(&client->cache);
    status = dcc_cache_table_put(
        &client->cache.voice_states,
        voice_state->guild_id,
        voice_state->user_id,
        copy,
        dcc_cache_free_voice_state_value,
        dcc_cache_next_sequence(&client->cache),
        client->cache_policy.max_voice_states
    );
    dcc_cache_unlock(&client->cache);
    if (status != DCC_OK) {
        dcc_voice_state_free(copy);
    }
    return status;
}

const dcc_voice_state_t *dcc_cache_get_voice_state(
    const dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id
) {
    if (client == NULL || !client->enable_cache || guild_id == 0 || user_id == 0) {
        return NULL;
    }
    dcc_cache_t *cache = (dcc_cache_t *)&client->cache;
    dcc_cache_lock(cache);
    const dcc_voice_state_t *value = (const dcc_voice_state_t *)dcc_cache_table_get(
        &client->cache.voice_states,
        guild_id,
        user_id
    );
    dcc_cache_unlock(cache);
    return value;
}

uint8_t dcc_cache_remove_voice_state(dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t user_id) {
    if (client == NULL || !client->enable_cache || guild_id == 0 || user_id == 0) {
        return 0;
    }
    dcc_cache_lock(&client->cache);
    uint8_t removed = dcc_cache_table_remove(
        &client->cache.voice_states,
        guild_id,
        user_id,
        dcc_cache_free_voice_state_value
    );
    dcc_cache_unlock(&client->cache);
    return removed;
}
