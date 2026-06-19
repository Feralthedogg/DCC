#ifndef DCC_JSON_GATEWAY_PAYLOAD_THREAD_UPDATE_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_THREAD_UPDATE_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_THREAD_UPDATE_RESOURCE_FIELDS \
    int has_thread_member; \
    dcc_thread_member_t thread_member; \
    int has_thread_members_update; \
    dcc_thread_members_update_t thread_members_update; \
    int has_guild_members_chunk; \
    dcc_guild_members_chunk_t guild_members_chunk

#endif
