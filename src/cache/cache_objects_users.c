#include "internal/cache/dcc_cache_internal.h"

dcc_status_t dcc_cache_put_user(dcc_client_t *client, const dcc_user_t *user) {
    if (user == NULL || user->id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }
    dcc_user_t *copy = dcc_user_clone(user);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_cache_lock(&client->cache);
    status = dcc_cache_table_put(
        &client->cache.users,
        user->id,
        0,
        copy,
        dcc_cache_free_user_value,
        dcc_cache_next_sequence(&client->cache),
        client->cache_policy.max_users
    );
    dcc_cache_unlock(&client->cache);
    if (status != DCC_OK) {
        dcc_user_free(copy);
    }
    return status;
}

const dcc_user_t *dcc_cache_get_user(const dcc_client_t *client, dcc_snowflake_t user_id) {
    if (client == NULL || !client->enable_cache || user_id == 0) {
        return NULL;
    }
    dcc_cache_t *cache = (dcc_cache_t *)&client->cache;
    dcc_cache_lock(cache);
    const dcc_user_t *value = (const dcc_user_t *)dcc_cache_table_get(&client->cache.users, user_id, 0);
    dcc_cache_unlock(cache);
    return value;
}

uint8_t dcc_cache_remove_user(dcc_client_t *client, dcc_snowflake_t user_id) {
    if (client == NULL || !client->enable_cache || user_id == 0) {
        return 0;
    }
    dcc_cache_lock(&client->cache);
    uint8_t removed = dcc_cache_table_remove(&client->cache.users, user_id, 0, dcc_cache_free_user_value);
    dcc_cache_unlock(&client->cache);
    return removed;
}
