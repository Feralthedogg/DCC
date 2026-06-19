#include "internal/cache/dcc_cache_internal.h"

dcc_status_t dcc_cache_merge_message(dcc_client_t *client, const dcc_message_t *message, uint64_t fields) {
    if (message == NULL || message->id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cache_lock(&client->cache);
    const dcc_message_t *existing =
        (const dcc_message_t *)dcc_cache_table_get(&client->cache.messages, message->id, 0);
    dcc_message_t merged = *message;
    if (existing != NULL) {
        dcc_cache_merge_message_view(&merged, existing, fields);
    }
    dcc_message_t *copy = dcc_message_clone(&merged);
    if (copy == NULL) {
        dcc_cache_unlock(&client->cache);
        return DCC_ERR_NOMEM;
    }
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
