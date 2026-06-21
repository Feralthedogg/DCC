#ifndef DCC_CLIENT_H
#define DCC_CLIENT_H

#include <dcc/error.h>
#include <dcc/events.h>
#include <dcc/export.h>
#include <dcc/intents.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*dcc_log_fn)(dcc_log_level_t level, const char *message, void *user_data);

typedef struct dcc_client_options {
    size_t size;
    const char *token;
    dcc_intents_t intents;
    uint32_t shard_id;
    uint32_t shard_count;
    uint32_t rest_concurrency;
    uint32_t enable_cache;
    dcc_log_fn log_fn;
    void *log_user_data;
    const char *gateway_url;
    size_t cache_max_guilds;
    size_t cache_max_channels;
    size_t cache_max_roles;
    size_t cache_max_members;
    size_t cache_max_users;
    size_t cache_max_messages;
    size_t cache_max_voice_states;
    uint32_t gateway_max_concurrency;
    uint32_t infer_guild_id_from_channel;
} dcc_client_options_t;

typedef struct dcc_runtime_stats {
    size_t size;
    uint64_t tasks_spawned;
    uint64_t events_dispatched;
} dcc_runtime_stats_t;

typedef struct dcc_gateway_info {
    size_t size;
    const char *url;
    const char *resume_url;
    uint32_t recommended_shards;
    uint32_t max_concurrency;
    uint32_t remaining_identifies;
    uint32_t total_identifies;
    uint64_t reset_after_ms;
    uint64_t reset_at_ms;
} dcc_gateway_info_t;

DCC_API dcc_status_t dcc_client_create(const dcc_client_options_t *options, dcc_client_t **out);
DCC_API void dcc_client_destroy(dcc_client_t *client);

DCC_API dcc_status_t dcc_client_start(dcc_client_t *client);
DCC_API dcc_status_t dcc_client_stop(dcc_client_t *client);
DCC_API dcc_status_t dcc_client_wait(dcc_client_t *client);
DCC_API dcc_status_t dcc_client_gateway_start(dcc_client_t *client);
DCC_API dcc_status_t dcc_client_gateway_stop(dcc_client_t *client);
DCC_API dcc_status_t dcc_client_gateway_reconnect(dcc_client_t *client, uint8_t resume);
DCC_API dcc_status_t dcc_client_update_voice_state(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t channel_id,
    uint8_t self_mute,
    uint8_t self_deaf
);
DCC_API dcc_status_t dcc_client_infer_guild_id_from_channel(
    const dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t *out_guild_id
);

DCC_API dcc_status_t dcc_client_runtime_stats(dcc_client_t *client, dcc_runtime_stats_t *out);
DCC_API dcc_status_t dcc_client_gateway_info(const dcc_client_t *client, dcc_gateway_info_t *out);
DCC_API dcc_snowflake_t dcc_client_get_dm_channel(const dcc_client_t *client, dcc_snowflake_t user_id);
DCC_API dcc_status_t dcc_client_set_dm_channel(
    dcc_client_t *client,
    dcc_snowflake_t user_id,
    dcc_snowflake_t channel_id
);
DCC_API const char *dcc_client_last_error(const dcc_client_t *client);

#ifdef __cplusplus
}
#endif

#endif
