#ifndef DCC_CACHE_STATE_INTERNAL_H
#define DCC_CACHE_STATE_INTERNAL_H

#include <dcc/dcc.h>

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_cache_entry {
    dcc_snowflake_t major;
    dcc_snowflake_t minor;
    uint64_t sequence;
    void *value;
    struct dcc_cache_entry *next;
} dcc_cache_entry_t;

typedef struct dcc_cache_table {
    dcc_cache_entry_t **buckets;
    size_t bucket_count;
    size_t size;
} dcc_cache_table_t;

typedef struct dcc_cache {
    atomic_flag lock;
    uint64_t sequence;
    dcc_cache_table_t guilds;
    dcc_cache_table_t channels;
    dcc_cache_table_t roles;
    dcc_cache_table_t members;
    dcc_cache_table_t users;
    dcc_cache_table_t messages;
    dcc_cache_table_t voice_states;
} dcc_cache_t;

void dcc_cache_init(dcc_cache_t *cache);
void dcc_cache_deinit(dcc_cache_t *cache);
dcc_status_t dcc_cache_merge_guild(dcc_client_t *client, const dcc_guild_t *guild, uint64_t fields);
dcc_status_t dcc_cache_merge_channel(dcc_client_t *client, const dcc_channel_t *channel, uint64_t fields);
dcc_status_t dcc_cache_merge_role(dcc_client_t *client, const dcc_role_t *role, uint64_t fields);
dcc_status_t dcc_cache_merge_member(dcc_client_t *client, const dcc_member_t *member, uint64_t fields);
dcc_status_t dcc_cache_merge_user(dcc_client_t *client, const dcc_user_t *user, uint64_t fields);
dcc_status_t dcc_cache_merge_message(dcc_client_t *client, const dcc_message_t *message, uint64_t fields);
dcc_status_t dcc_cache_merge_voice_state(dcc_client_t *client, const dcc_voice_state_t *voice_state, uint64_t fields);

#ifdef __cplusplus
}
#endif

#endif
