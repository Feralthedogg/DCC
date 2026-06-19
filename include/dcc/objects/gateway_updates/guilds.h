#ifndef DCC_OBJECTS_GATEWAY_UPDATES_GUILDS_H
#define DCC_OBJECTS_GATEWAY_UPDATES_GUILDS_H

/* Guild gateway update payload objects. */

#include <dcc/objects/resources.h>

typedef struct dcc_guild_members_chunk {
    dcc_snowflake_t guild_id;
    uint32_t chunk_index;
    uint32_t chunk_count;
    const dcc_snowflake_t *not_found_ids;
    size_t not_found_ids_count;
    uint8_t not_found_ids_truncated;
    const dcc_snowflake_t *member_ids;
    size_t member_ids_count;
    uint8_t member_ids_truncated;
    const dcc_member_t *members;
    size_t members_count;
    uint8_t members_truncated;
    const char *nonce;
} dcc_guild_members_chunk_t;

typedef struct dcc_guild_audit_log_entry {
    dcc_snowflake_t id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t user_id;
    dcc_snowflake_t target_id;
    uint32_t action_type;
} dcc_guild_audit_log_entry_t;

typedef struct dcc_guild_ban {
    dcc_snowflake_t guild_id;
    dcc_user_t user;
} dcc_guild_ban_t;

typedef struct dcc_guild_id_list_update {
    dcc_snowflake_t guild_id;
    const dcc_snowflake_t *ids;
    size_t ids_count;
    uint8_t ids_truncated;
    const dcc_emoji_t *emojis;
    size_t emojis_count;
    uint8_t emojis_truncated;
    const dcc_sticker_t *stickers;
    size_t stickers_count;
    uint8_t stickers_truncated;
    const dcc_soundboard_sound_t *soundboard_sounds;
    size_t soundboard_sounds_count;
    uint8_t soundboard_sounds_truncated;
} dcc_guild_id_list_update_t;

typedef struct dcc_guild_update_notice {
    dcc_snowflake_t guild_id;
} dcc_guild_update_notice_t;

typedef struct dcc_scheduled_event_user_update {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t scheduled_event_id;
    dcc_snowflake_t user_id;
} dcc_scheduled_event_user_update_t;

typedef struct dcc_guild_join_request_update {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t user_id;
    const char *status;
} dcc_guild_join_request_update_t;

typedef struct dcc_guild_application_command_update {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t application_id;
} dcc_guild_application_command_update_t;

#endif
