#include "internal/cache/dcc_cache_internal.h"

dcc_status_t dcc_cache_merge_voice_state(
    dcc_client_t *client,
    const dcc_voice_state_t *voice_state,
    uint64_t fields
) {
    if (voice_state == NULL || voice_state->guild_id == 0 || voice_state->user_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cache_lock(&client->cache);
    const dcc_voice_state_t *existing = (const dcc_voice_state_t *)dcc_cache_table_get(
        &client->cache.voice_states,
        voice_state->guild_id,
        voice_state->user_id
    );
    dcc_voice_state_t merged = *voice_state;
    if (existing != NULL) {
        dcc_cache_merge_voice_state_view(&merged, existing, fields);
    }
    dcc_voice_state_t *copy = dcc_voice_state_clone(&merged);
    if (copy == NULL) {
        dcc_cache_unlock(&client->cache);
        return DCC_ERR_NOMEM;
    }
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
