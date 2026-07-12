#ifndef DCC_VOICE_TYPES_SESSION_H
#define DCC_VOICE_TYPES_SESSION_H

#include <dcc/voice/types/enums.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_voice_session_info {
    size_t size;
    dcc_voice_session_state_t state;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    const char *session_id;
    const char *token;
    const char *endpoint;
    uint8_t self_mute;
    uint8_t self_deaf;
    uint8_t dave_enabled;
    dcc_voice_dave_version_t dave_version;
    uint8_t ready;
    uint8_t active;
    uint8_t reconnect_requested;
    uint8_t full_reconnect_requested;
    uint32_t reconnect_attempts;
    uint16_t last_close_code;
    const char *last_close_reason;
    uint32_t heartbeat_interval_ms;
    uint16_t dave_transition_id;
    dcc_voice_dave_version_t dave_pending_version;
    uint8_t dave_transition_pending;
    uint8_t dave_transition_ready;
    int32_t receive_sequence;
    const char *udp_host;
    uint16_t udp_port;
    const char *discovered_address;
    uint16_t discovered_port;
    uint8_t select_protocol_sent;
} dcc_voice_session_info_t;

typedef struct dcc_voice_client_stats {
    size_t size;
    dcc_voice_session_state_t state;
    uint8_t ready;
    uint8_t active;
    uint8_t reconnect_requested;
    uint8_t full_reconnect_requested;
    uint32_t reconnect_attempts;
    uint16_t last_close_code;
    uint32_t heartbeat_interval_ms;
    int32_t receive_sequence;
    size_t send_queue_size;
    uint32_t tracks_remaining;
    size_t mixed_source_count;
    uint32_t receive_packets_lost;
    uint32_t receive_packets_dropped;
    uint8_t receive_reorder_window;
    size_t receive_reorder_buffered;
    uint32_t receive_jitter_delay_ms;
    uint8_t receive_jitter_timer_running;
    uint32_t receive_jitter_timer_interval_ms;
    uint8_t websocket_loop_running;
    dcc_status_t websocket_loop_status;
    uint8_t udp_connected;
    uint32_t ssrc;
    uint16_t rtp_sequence;
    uint32_t rtp_timestamp;
    dcc_voice_encryption_mode_t encryption_mode;
    uint8_t has_secret_key;
    uint32_t packet_nonce;
    uint8_t select_protocol_sent;
    uint8_t dave_enabled;
    dcc_voice_dave_version_t dave_version;
    uint8_t dave_transition_pending;
    uint8_t dave_transition_ready;
    uint16_t dave_transition_id;
    dcc_voice_dave_version_t dave_pending_version;
} dcc_voice_client_stats_t;

typedef struct dcc_voice_dave_stats {
    size_t size;
    uint8_t backend_available;
    uint8_t media_ready;
    uint8_t reserved[6];
    uint64_t epoch;
    size_t participant_count;
    uint64_t encrypt_success;
    uint64_t encrypt_failures;
    uint64_t decrypt_success;
    uint64_t decrypt_failures;
    uint64_t missing_key_failures;
} dcc_voice_dave_stats_t;

typedef struct dcc_voice_health_snapshot {
    size_t size;
    dcc_voice_health_t health;
    dcc_voice_reconnect_action_t action;
    uint8_t ok;
    uint8_t media_ready;
    uint8_t websocket_ready;
    uint8_t udp_ready;
    dcc_voice_session_info_t session;
    dcc_voice_client_stats_t stats;
    char reason[160];
    dcc_voice_dave_stats_t dave;
} dcc_voice_health_snapshot_t;

#ifdef __cplusplus
}
#endif

#endif
