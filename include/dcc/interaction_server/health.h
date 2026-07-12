#ifndef DCC_INTERACTION_SERVER_HEALTH_H
#define DCC_INTERACTION_SERVER_HEALTH_H

#include <dcc/interaction_server/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_interaction_server_stats {
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
} dcc_interaction_server_stats_t;

typedef struct dcc_interaction_server_state {
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
} dcc_interaction_server_state_t;

typedef struct dcc_interaction_server_protection_stats {
    size_t size;
    uint64_t max_active_requests;
    uint32_t response_deadline_ms;
    uint32_t replay_window_ms;
    uint64_t overloaded_responses;
    uint64_t replayed_requests;
    uint64_t deadline_exceeded_requests;
} dcc_interaction_server_protection_stats_t;

typedef enum dcc_interaction_server_health {
    DCC_INTERACTION_SERVER_HEALTH_UNKNOWN = 0,
    DCC_INTERACTION_SERVER_HEALTH_READY,
    DCC_INTERACTION_SERVER_HEALTH_DEGRADED,
    DCC_INTERACTION_SERVER_HEALTH_DRAINING,
    DCC_INTERACTION_SERVER_HEALTH_STOPPED
} dcc_interaction_server_health_t;

typedef struct dcc_interaction_server_health_snapshot {
    size_t size;
    dcc_interaction_server_health_t health;
    uint8_t ok;
    uint8_t ready;
    uint8_t accepting;
    uint8_t drain_complete;
    dcc_interaction_server_state_t state;
    dcc_interaction_server_stats_t stats;
    char reason[160];
    dcc_interaction_server_protection_stats_t protection;
} dcc_interaction_server_health_snapshot_t;

DCC_API dcc_status_t dcc_interaction_server_stats(
    const dcc_interaction_server_t *server,
    dcc_interaction_server_stats_t *out
);
DCC_API dcc_status_t dcc_interaction_server_get_state(
    const dcc_interaction_server_t *server,
    dcc_interaction_server_state_t *out
);
DCC_API dcc_status_t dcc_interaction_server_protection_stats(
    const dcc_interaction_server_t *server,
    dcc_interaction_server_protection_stats_t *out
);
DCC_API dcc_status_t dcc_interaction_server_health_snapshot(
    const dcc_interaction_server_t *server,
    dcc_interaction_server_health_snapshot_t *out
);
DCC_API const char *dcc_interaction_server_health_string(dcc_interaction_server_health_t health);
DCC_API dcc_status_t dcc_interaction_server_health_snapshot_json(
    const dcc_interaction_server_health_snapshot_t *snapshot,
    char *out,
    size_t out_size,
    size_t *out_len
);
DCC_API dcc_status_t dcc_interaction_server_health_snapshot_prometheus(
    const dcc_interaction_server_health_snapshot_t *snapshot,
    char *out,
    size_t out_size,
    size_t *out_len
);

#ifdef __cplusplus
}
#endif

#endif
