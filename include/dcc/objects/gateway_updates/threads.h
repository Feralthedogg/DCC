#ifndef DCC_OBJECTS_GATEWAY_UPDATES_THREADS_H
#define DCC_OBJECTS_GATEWAY_UPDATES_THREADS_H

/* Thread gateway update payload objects. */

#include <dcc/objects/resources.h>

typedef struct dcc_thread_member {
    dcc_snowflake_t id;
    dcc_snowflake_t user_id;
    dcc_snowflake_t guild_id;
    uint64_t flags;
    const char *join_timestamp;
} dcc_thread_member_t;

typedef struct dcc_thread_members_update {
    dcc_snowflake_t id;
    dcc_snowflake_t guild_id;
    uint32_t count;
    const dcc_thread_member_t *added_members;
    size_t added_members_count;
    uint8_t added_members_truncated;
    const dcc_snowflake_t *removed_member_ids;
    size_t removed_member_ids_count;
    uint8_t removed_member_ids_truncated;
    const dcc_snowflake_t *member_ids;
    size_t member_ids_count;
    uint8_t member_ids_truncated;
} dcc_thread_members_update_t;

typedef struct dcc_thread_list_sync {
    dcc_snowflake_t guild_id;
    const dcc_snowflake_t *channel_ids;
    size_t channel_ids_count;
    uint8_t channel_ids_truncated;
    const dcc_snowflake_t *thread_ids;
    size_t thread_ids_count;
    uint8_t thread_ids_truncated;
    const dcc_snowflake_t *member_ids;
    size_t member_ids_count;
    uint8_t member_ids_truncated;
    const dcc_channel_t *threads;
    size_t threads_count;
    uint8_t threads_truncated;
    const dcc_thread_member_t *members;
    size_t members_count;
    uint8_t members_truncated;
} dcc_thread_list_sync_t;

#endif
