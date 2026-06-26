/**
 * @file include/dcc/objects/resources/lobbies.h
 * @brief Public DCC objects resources lobbies declarations.
 */

#ifndef DCC_OBJECTS_RESOURCES_LOBBIES_H
#define DCC_OBJECTS_RESOURCES_LOBBIES_H

/* Lobby resource objects. */

#include <dcc/objects/base.h>

struct dcc_channel;

typedef enum dcc_lobby_member_flag {
    DCC_LOBBY_MEMBER_CAN_LINK_LOBBY = 1U << 0
} dcc_lobby_member_flag_t;

typedef struct dcc_lobby_member {
    dcc_snowflake_t id;
    uint32_t flags;
    uint8_t has_flags;
    const char *metadata_json;
} dcc_lobby_member_t;

typedef struct dcc_lobby {
    dcc_snowflake_t id;
    dcc_snowflake_t application_id;
    const dcc_lobby_member_t *members;
    size_t members_count;
    uint8_t members_truncated;
    const struct dcc_channel *linked_channel;
    uint8_t has_linked_channel;
    const char *metadata_json;
} dcc_lobby_t;

#endif
