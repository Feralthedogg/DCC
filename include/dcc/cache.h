#ifndef DCC_CACHE_H
#define DCC_CACHE_H

#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/objects.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_client dcc_client_t;

typedef struct dcc_cache_counts {
    size_t size;
    size_t guilds;
    size_t channels;
    size_t roles;
    size_t members;
    size_t users;
    size_t messages;
    size_t voice_states;
} dcc_cache_counts_t;

typedef struct dcc_cache_policy {
    size_t size;
    size_t max_guilds;
    size_t max_channels;
    size_t max_roles;
    size_t max_members;
    size_t max_users;
    size_t max_messages;
    size_t max_voice_states;
} dcc_cache_policy_t;

/*
 * Cache getters return borrowed views owned by the client cache. They stay valid
 * until the matching object is replaced/removed/evicted, the cache is cleared,
 * or the client is destroyed. Use the dcc_*_clone() APIs for longer-lived
 * ownership.
 */
DCC_API uint8_t dcc_cache_enabled(const dcc_client_t *client);
DCC_API void dcc_cache_clear(dcc_client_t *client);
DCC_API dcc_status_t dcc_cache_counts(const dcc_client_t *client, dcc_cache_counts_t *out);
DCC_API dcc_status_t dcc_cache_get_policy(const dcc_client_t *client, dcc_cache_policy_t *out);
DCC_API dcc_status_t dcc_cache_set_policy(dcc_client_t *client, const dcc_cache_policy_t *policy);

DCC_API dcc_status_t dcc_cache_put_guild(dcc_client_t *client, const dcc_guild_t *guild);
DCC_API const dcc_guild_t *dcc_cache_get_guild(const dcc_client_t *client, dcc_snowflake_t guild_id);
DCC_API uint8_t dcc_cache_remove_guild(dcc_client_t *client, dcc_snowflake_t guild_id);

DCC_API dcc_status_t dcc_cache_put_channel(dcc_client_t *client, const dcc_channel_t *channel);
DCC_API const dcc_channel_t *dcc_cache_get_channel(const dcc_client_t *client, dcc_snowflake_t channel_id);
DCC_API uint8_t dcc_cache_remove_channel(dcc_client_t *client, dcc_snowflake_t channel_id);

DCC_API dcc_status_t dcc_cache_put_role(dcc_client_t *client, const dcc_role_t *role);
DCC_API const dcc_role_t *dcc_cache_get_role(
    const dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id
);
DCC_API uint8_t dcc_cache_remove_role(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id
);

DCC_API dcc_status_t dcc_cache_put_member(dcc_client_t *client, const dcc_member_t *member);
DCC_API const dcc_member_t *dcc_cache_get_member(
    const dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id
);
DCC_API uint8_t dcc_cache_remove_member(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id
);

DCC_API dcc_status_t dcc_cache_put_user(dcc_client_t *client, const dcc_user_t *user);
DCC_API const dcc_user_t *dcc_cache_get_user(const dcc_client_t *client, dcc_snowflake_t user_id);
DCC_API uint8_t dcc_cache_remove_user(dcc_client_t *client, dcc_snowflake_t user_id);

DCC_API dcc_status_t dcc_cache_put_message(dcc_client_t *client, const dcc_message_t *message);
DCC_API const dcc_message_t *dcc_cache_get_message(const dcc_client_t *client, dcc_snowflake_t message_id);
DCC_API uint8_t dcc_cache_remove_message(dcc_client_t *client, dcc_snowflake_t message_id);

DCC_API dcc_status_t dcc_cache_put_voice_state(dcc_client_t *client, const dcc_voice_state_t *voice_state);
DCC_API const dcc_voice_state_t *dcc_cache_get_voice_state(
    const dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id
);
DCC_API uint8_t dcc_cache_remove_voice_state(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id
);

#ifdef __cplusplus
}
#endif

#endif
