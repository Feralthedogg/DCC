#include <dcc/dcc.h>

#include <stddef.h>

/*
 * These declarations preserve the public layouts shipped before 1.5.0.
 * Keep them independent from the current typedefs so that adding a field to
 * an embedded legacy structure fails this translation unit at compile time.
 */
typedef struct legacy_cluster_health_summary {
    size_t size;
    uint32_t total_shards;
    uint32_t created_shards;
    uint32_t starting_shards;
    uint32_t ready_shards;
    uint32_t stopping_shards;
    uint32_t stopped_shards;
    uint32_t error_shards;
    uint64_t starts;
    uint64_t restart_attempts;
    uint64_t restarts;
    uint64_t restart_suppressed;
    uint64_t restart_budget_used;
    uint64_t ready_events;
    uint64_t resumed_events;
    uint64_t socket_closes;
    uint64_t log_errors;
} legacy_cluster_health_summary_t;

typedef struct legacy_interaction_server_options {
    size_t size;
    const char *public_key_hex;
    const char *address;
    uint16_t port;
    uint32_t backlog;
    size_t max_header_size;
    size_t max_body_size;
    dcc_interaction_server_cb callback;
    void *user_data;
} legacy_interaction_server_options_t;

typedef struct legacy_interaction_server_stats {
    size_t size;
    uint64_t accepted_connections;
    uint64_t active_requests;
    uint64_t completed_requests;
    uint64_t read_errors;
    uint64_t write_errors;
    uint64_t spawn_errors;
    uint64_t response_2xx;
    uint64_t response_3xx;
    uint64_t response_4xx;
    uint64_t response_5xx;
    uint64_t bad_request_responses;
    uint64_t unauthorized_responses;
    uint64_t not_found_responses;
    uint64_t method_not_allowed_responses;
    uint64_t payload_too_large_responses;
} legacy_interaction_server_stats_t;

typedef struct legacy_interaction_server_state {
    size_t size;
    uint8_t started;
    uint8_t stopping;
    uint8_t listening;
    uint8_t draining;
    uint16_t requested_port;
    uint16_t bound_port;
    uint32_t backlog;
    size_t route_count;
    size_t max_header_size;
    size_t max_body_size;
    uint64_t accepted_connections;
    uint64_t active_requests;
    uint64_t completed_requests;
} legacy_interaction_server_state_t;

typedef struct legacy_interaction_server_health_snapshot {
    size_t size;
    dcc_interaction_server_health_t health;
    uint8_t ok;
    uint8_t ready;
    uint8_t accepting;
    uint8_t drain_complete;
    legacy_interaction_server_state_t state;
    legacy_interaction_server_stats_t stats;
    char reason[160];
} legacy_interaction_server_health_snapshot_t;

typedef struct legacy_voice_session_info {
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
} legacy_voice_session_info_t;

typedef struct legacy_voice_client_stats {
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
} legacy_voice_client_stats_t;

typedef struct legacy_voice_health_snapshot {
    size_t size;
    dcc_voice_health_t health;
    dcc_voice_reconnect_action_t action;
    uint8_t ok;
    uint8_t media_ready;
    uint8_t websocket_ready;
    uint8_t udp_ready;
    legacy_voice_session_info_t session;
    legacy_voice_client_stats_t stats;
    char reason[160];
} legacy_voice_health_snapshot_t;

_Static_assert(
    sizeof(legacy_cluster_health_summary_t) == sizeof(dcc_cluster_health_summary_t),
    "cluster health summary ABI changed"
);
_Static_assert(
    sizeof(legacy_interaction_server_options_t) ==
        offsetof(dcc_interaction_server_options_t, max_active_requests),
    "interaction options legacy prefix changed"
);
_Static_assert(
    sizeof(legacy_interaction_server_stats_t) == sizeof(dcc_interaction_server_stats_t),
    "interaction stats ABI changed"
);
_Static_assert(
    sizeof(legacy_interaction_server_state_t) == sizeof(dcc_interaction_server_state_t),
    "interaction state ABI changed"
);
_Static_assert(
    sizeof(legacy_interaction_server_health_snapshot_t) ==
        offsetof(dcc_interaction_server_health_snapshot_t, protection),
    "interaction health legacy prefix changed"
);
_Static_assert(
    sizeof(legacy_voice_session_info_t) == sizeof(dcc_voice_session_info_t),
    "voice session ABI changed"
);
_Static_assert(
    sizeof(legacy_voice_client_stats_t) == sizeof(dcc_voice_client_stats_t),
    "voice stats ABI changed"
);
_Static_assert(
    sizeof(legacy_voice_health_snapshot_t) ==
        offsetof(dcc_voice_health_snapshot_t, dave),
    "voice health legacy prefix changed"
);

int main(void) {
    return 0;
}
