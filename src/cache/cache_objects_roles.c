#include "internal/cache/dcc_cache_internal.h"

dcc_status_t dcc_cache_put_role(dcc_client_t *client, const dcc_role_t *role) {
    if (role == NULL || role->guild_id == 0 || role->id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }
    dcc_role_t *copy = dcc_role_clone(role);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_cache_lock(&client->cache);
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

const dcc_role_t *dcc_cache_get_role(
    const dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id
) {
    if (client == NULL || !client->enable_cache || guild_id == 0 || role_id == 0) {
        return NULL;
    }
    dcc_cache_t *cache = (dcc_cache_t *)&client->cache;
    dcc_cache_lock(cache);
    const dcc_role_t *value = (const dcc_role_t *)dcc_cache_table_get(&client->cache.roles, guild_id, role_id);
    dcc_cache_unlock(cache);
    return value;
}

uint8_t dcc_cache_remove_role(dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t role_id) {
    if (client == NULL || !client->enable_cache || guild_id == 0 || role_id == 0) {
        return 0;
    }
    dcc_cache_lock(&client->cache);
    uint8_t removed = dcc_cache_table_remove(&client->cache.roles, guild_id, role_id, dcc_cache_free_role_value);
    dcc_cache_unlock(&client->cache);
    return removed;
}
