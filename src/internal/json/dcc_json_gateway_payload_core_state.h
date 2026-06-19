#ifndef DCC_JSON_GATEWAY_PAYLOAD_CORE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_CORE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_CORE_FIELDS \
    int has_op; \
    int64_t op; \
    int has_seq; \
    uint64_t seq; \
    int has_event_name; \
    char event_name[64]; \
    int has_heartbeat_interval; \
    int64_t heartbeat_interval; \
    int has_session_id; \
    char session_id[256]; \
    const dcc_snowflake_t *ready_guild_ids; \
    size_t ready_guild_ids_count; \
    size_t ready_guild_count; \
    uint8_t ready_guild_ids_truncated; \
    dcc_snowflake_t ready_guild_id_storage[DCC_JSON_GATEWAY_ID_LIST_CAP]; \
    int has_resume_gateway_url; \
    char resume_gateway_url[1024]; \
    int has_invalid_session_resumable; \
    int invalid_session_resumable; \
    uint64_t cache_fields

#endif
