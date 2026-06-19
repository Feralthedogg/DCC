#ifndef DCC_EVENTS_PAYLOADS_H
#define DCC_EVENTS_PAYLOADS_H

#include <dcc/events/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_ready_event {
    uint32_t shard_id;
    uint32_t shard_count;
    const char *session_id;
    const dcc_snowflake_t *guild_ids;
    size_t guild_ids_count;
    uint32_t guild_count;
    uint8_t guild_ids_truncated;
} dcc_ready_event_t;

typedef struct dcc_log_event {
    dcc_log_level_t level;
    const char *message;
} dcc_log_event_t;

typedef struct dcc_raw_event {
    dcc_event_type_t type;
    uint64_t sequence;
    const char *name;
    const char *json;
    size_t json_len;
} dcc_raw_event_t;

typedef struct dcc_socket_close_event {
    intptr_t fd;
    uint16_t code;
    const char *reason;
} dcc_socket_close_event_t;

typedef struct dcc_rate_limited_event {
    uint16_t status;
    double retry_after;
    uint8_t global;
    const char *method;
    const char *path;
    const char *body;
    size_t body_len;
} dcc_rate_limited_event_t;

typedef struct dcc_voice_buffer_send_event {
    dcc_voice_client_t *voice_client;
    uint64_t buffer_size;
    size_t packets_left;
} dcc_voice_buffer_send_event_t;

typedef struct dcc_voice_ready_event {
    dcc_voice_client_t *voice_client;
    dcc_snowflake_t voice_channel_id;
} dcc_voice_ready_event_t;

typedef struct dcc_voice_receive_event {
    dcc_voice_client_t *voice_client;
    dcc_snowflake_t user_id;
    const uint8_t *audio;
    size_t audio_size;
} dcc_voice_receive_event_t;

typedef struct dcc_voice_track_marker_event {
    dcc_voice_client_t *voice_client;
    const char *track_meta;
} dcc_voice_track_marker_event_t;

typedef struct dcc_voice_client_speaking_event {
    dcc_voice_client_t *voice_client;
    dcc_snowflake_t user_id;
    uint32_t ssrc;
} dcc_voice_client_speaking_event_t;

typedef struct dcc_voice_client_disconnect_event {
    dcc_voice_client_t *voice_client;
    dcc_snowflake_t user_id;
} dcc_voice_client_disconnect_event_t;

typedef struct dcc_voice_client_platform_event {
    dcc_voice_client_t *voice_client;
    dcc_snowflake_t user_id;
    uint8_t platform;
} dcc_voice_client_platform_event_t;

#ifdef __cplusplus
}
#endif

#endif
