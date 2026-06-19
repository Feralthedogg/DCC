#ifndef DCC_REST_TYPES_GUILD_MEMBERS_H
#define DCC_REST_TYPES_GUILD_MEMBERS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_guild_member_update_flag {
    DCC_GUILD_MEMBER_UPDATE_TIMEOUT = 1U << 0,
    DCC_GUILD_MEMBER_UPDATE_NICK = 1U << 1,
    DCC_GUILD_MEMBER_UPDATE_ROLES = 1U << 2,
    DCC_GUILD_MEMBER_UPDATE_VOICE = 1U << 3
} dcc_guild_member_update_flag_t;

typedef struct dcc_guild_member_params {
    size_t size;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t user_id;
    uint32_t flags;
    const char *communication_disabled_until;
    const char *nick;
    const dcc_snowflake_t *roles;
    size_t role_count;
    uint8_t mute;
    uint8_t deaf;
    const char *access_token;
} dcc_guild_member_params_t;

typedef struct dcc_current_guild_member_params {
    size_t size;
    dcc_snowflake_t guild_id;
    const char *nick;
    const char *banner;
    const char *avatar;
    const char *bio;
} dcc_current_guild_member_params_t;

typedef struct dcc_guild_member_nickname_params {
    size_t size;
    dcc_snowflake_t guild_id;
    const char *nick;
} dcc_guild_member_nickname_params_t;

typedef struct dcc_guild_member_role_params {
    size_t size;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t user_id;
    dcc_snowflake_t role_id;
} dcc_guild_member_role_params_t;

typedef struct dcc_guild_ban_params {
    size_t size;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t user_id;
    uint32_t delete_message_seconds;
} dcc_guild_ban_params_t;

typedef struct dcc_guild_prune_params {
    size_t size;
    dcc_snowflake_t guild_id;
    uint32_t days;
    const dcc_snowflake_t *include_roles;
    size_t include_role_count;
    uint8_t compute_prune_count;
} dcc_guild_prune_params_t;

#ifdef __cplusplus
}
#endif

#endif
