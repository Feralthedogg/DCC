#ifndef DCC_GATEWAY_SESSION_INTERNAL_H
#define DCC_GATEWAY_SESSION_INTERNAL_H

#include "internal/client/dcc_client_state_internal.h"
#include "internal/ws/dcc_ws.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_gateway_next {
    DCC_GATEWAY_NEXT_STOP = 0,
    DCC_GATEWAY_NEXT_FATAL,
    DCC_GATEWAY_NEXT_RECONNECT_IDENTIFY,
    DCC_GATEWAY_NEXT_RECONNECT_RESUME
} dcc_gateway_next_t;

typedef struct dcc_gateway_heartbeat_state {
    dcc_ws_t *ws;
    atomic_uint refs;
    atomic_bool stop;
    atomic_bool waiting_ack;
    atomic_bool failed;
    atomic_bool has_seq;
    atomic_ullong seq;
    uint32_t interval_ms;
    uint32_t jitter_seed;
} dcc_gateway_heartbeat_state_t;

typedef struct dcc_gateway_session {
    dcc_client_t *client;
    dcc_ws_t *ws;
    dcc_gateway_heartbeat_state_t *heartbeat;
    bool heartbeat_failed;
    uint64_t seq;
    bool has_seq;
    uint32_t heartbeat_interval_ms;
    bool resume;
    dcc_gateway_next_t next;
} dcc_gateway_session_t;

#ifdef __cplusplus
}
#endif

#endif
