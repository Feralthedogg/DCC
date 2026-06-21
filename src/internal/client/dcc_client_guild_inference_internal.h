#ifndef DCC_CLIENT_GUILD_INFERENCE_INTERNAL_H
#define DCC_CLIENT_GUILD_INFERENCE_INTERNAL_H

#include "internal/client/dcc_client_state_internal.h"

#include <stdatomic.h>

static inline void dcc_client_channel_guild_inference_clear(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }
    atomic_store_explicit(&client->inferred_channel_id, 0U, memory_order_release);
    atomic_store_explicit(&client->inferred_guild_id, 0U, memory_order_release);
}

static inline void dcc_client_channel_guild_inference_store(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t guild_id
) {
    if (client == NULL || channel_id == 0U || guild_id == 0U) {
        return;
    }
    atomic_store_explicit(&client->inferred_guild_id, (uint_fast64_t)guild_id, memory_order_release);
    atomic_store_explicit(&client->inferred_channel_id, (uint_fast64_t)channel_id, memory_order_release);
}

static inline void dcc_client_channel_guild_inference_clear_if_channel(
    dcc_client_t *client,
    dcc_snowflake_t channel_id
) {
    if (client == NULL || channel_id == 0U) {
        return;
    }
    uint_fast64_t cached_channel =
        atomic_load_explicit(&client->inferred_channel_id, memory_order_acquire);
    if ((dcc_snowflake_t)cached_channel == channel_id) {
        dcc_client_channel_guild_inference_clear(client);
    }
}

static inline dcc_snowflake_t dcc_client_channel_guild_inference_load(
    const dcc_client_t *client,
    dcc_snowflake_t channel_id
) {
    if (client == NULL || channel_id == 0U) {
        return 0U;
    }
    uint_fast64_t cached_channel =
        atomic_load_explicit(&client->inferred_channel_id, memory_order_acquire);
    if ((dcc_snowflake_t)cached_channel != channel_id) {
        return 0U;
    }
    return (dcc_snowflake_t)atomic_load_explicit(&client->inferred_guild_id, memory_order_acquire);
}

#endif
