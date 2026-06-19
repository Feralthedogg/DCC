#include "internal/client/dcc_client_state_internal.h"

static void dcc_client_dm_cache_lock(dcc_client_t *client) {
    while (atomic_flag_test_and_set_explicit(&client->dm_channel_cache_lock, memory_order_acquire)) {
    }
}

static void dcc_client_dm_cache_unlock(dcc_client_t *client) {
    atomic_flag_clear_explicit(&client->dm_channel_cache_lock, memory_order_release);
}

dcc_snowflake_t dcc_client_get_dm_channel(const dcc_client_t *client, dcc_snowflake_t user_id) {
    if (client == NULL || user_id == 0) {
        return 0;
    }

    dcc_client_t *mutable_client = (dcc_client_t *)client;
    dcc_client_dm_cache_lock(mutable_client);
    dcc_snowflake_t channel_id = 0;
    for (uint32_t i = 0; i < client->dm_channel_cache_len; ++i) {
        if (client->dm_channel_cache[i].user_id == user_id) {
            channel_id = client->dm_channel_cache[i].channel_id;
            break;
        }
    }
    dcc_client_dm_cache_unlock(mutable_client);
    return channel_id;
}

static void dcc_client_dm_cache_resequence(dcc_client_t *client) {
    uint64_t sequence = 0;
    for (uint32_t i = 0; i < client->dm_channel_cache_len; ++i) {
        client->dm_channel_cache[i].sequence = ++sequence;
    }
    client->dm_channel_cache_sequence = sequence;
}

dcc_status_t dcc_client_set_dm_channel(
    dcc_client_t *client,
    dcc_snowflake_t user_id,
    dcc_snowflake_t channel_id
) {
    if (client == NULL || user_id == 0 || channel_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_client_dm_cache_lock(client);
    if (client->dm_channel_cache_sequence == UINT64_MAX) {
        dcc_client_dm_cache_resequence(client);
    }
    uint64_t sequence = ++client->dm_channel_cache_sequence;

    for (uint32_t i = 0; i < client->dm_channel_cache_len; ++i) {
        if (client->dm_channel_cache[i].user_id == user_id) {
            client->dm_channel_cache[i].channel_id = channel_id;
            client->dm_channel_cache[i].sequence = sequence;
            dcc_client_dm_cache_unlock(client);
            return DCC_OK;
        }
    }

    if (client->dm_channel_cache_len < DCC_DM_CHANNEL_CACHE_CAP) {
        client->dm_channel_cache[client->dm_channel_cache_len++] = (dcc_dm_channel_cache_entry_t){
            .user_id = user_id,
            .channel_id = channel_id,
            .sequence = sequence,
        };
        dcc_client_dm_cache_unlock(client);
        return DCC_OK;
    }

    uint32_t oldest = 0;
    for (uint32_t i = 1; i < client->dm_channel_cache_len; ++i) {
        if (client->dm_channel_cache[i].sequence < client->dm_channel_cache[oldest].sequence) {
            oldest = i;
        }
    }
    client->dm_channel_cache[oldest] = (dcc_dm_channel_cache_entry_t){
        .user_id = user_id,
        .channel_id = channel_id,
        .sequence = sequence,
    };
    dcc_client_dm_cache_unlock(client);
    return DCC_OK;
}
