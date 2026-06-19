#include "internal/cache/dcc_cache_internal.h"

dcc_status_t dcc_cache_put_message(dcc_client_t *client, const dcc_message_t *message) {
    if (message == NULL || message->id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }
    dcc_message_t *copy = dcc_message_clone(message);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_cache_lock(&client->cache);
    status = dcc_cache_table_put(
        &client->cache.messages,
        message->id,
        0,
        copy,
        dcc_cache_free_message_value,
        dcc_cache_next_sequence(&client->cache),
        client->cache_policy.max_messages
    );
    dcc_cache_unlock(&client->cache);
    if (status != DCC_OK) {
        dcc_message_free(copy);
    }
    return status;
}

const dcc_message_t *dcc_cache_get_message(const dcc_client_t *client, dcc_snowflake_t message_id) {
    if (client == NULL || !client->enable_cache || message_id == 0) {
        return NULL;
    }
    dcc_cache_t *cache = (dcc_cache_t *)&client->cache;
    dcc_cache_lock(cache);
    const dcc_message_t *value = (const dcc_message_t *)dcc_cache_table_get(&client->cache.messages, message_id, 0);
    dcc_cache_unlock(cache);
    return value;
}

uint8_t dcc_cache_remove_message(dcc_client_t *client, dcc_snowflake_t message_id) {
    if (client == NULL || !client->enable_cache || message_id == 0) {
        return 0;
    }
    dcc_cache_lock(&client->cache);
    uint8_t removed = dcc_cache_table_remove(&client->cache.messages, message_id, 0, dcc_cache_free_message_value);
    dcc_cache_unlock(&client->cache);
    return removed;
}
