#ifndef DCC_VOICE_WS_INTERNAL_H
#define DCC_VOICE_WS_INTERNAL_H

#include "internal/voice/dcc_voice_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_voice_ws_session {
    dcc_voice_client_t *voice_client;
    dcc_ws_t *ws;
    atomic_bool stop_heartbeat;
    atomic_bool heartbeat_failed;
} dcc_voice_ws_session_t;

uint8_t dcc_voice_client_websocket_stop_requested(const dcc_voice_client_t *voice_client);
void dcc_voice_client_websocket_bind_current(dcc_voice_client_t *voice_client, dcc_ws_t *ws);
void dcc_voice_client_websocket_clear_current(dcc_voice_client_t *voice_client, dcc_ws_t *ws);
void dcc_voice_client_websocket_reset_current(dcc_voice_client_t *voice_client);
void dcc_voice_client_websocket_abort_current(dcc_voice_client_t *voice_client);
dcc_status_t dcc_voice_client_websocket_retain_current(dcc_voice_client_t *voice_client, dcc_ws_t **out);
uint8_t dcc_voice_client_websocket_sleep(dcc_voice_client_t *voice_client, uint32_t ms);
dcc_status_t dcc_voice_client_request_full_reconnect(dcc_voice_client_t *voice_client);
void dcc_voice_ws_heartbeat_task(void *arg);
dcc_status_t dcc_voice_ws_read_hello(dcc_voice_client_t *voice_client, dcc_ws_t *ws);
dcc_status_t dcc_voice_ws_maybe_select_protocol(dcc_voice_ws_session_t *session);
dcc_status_t dcc_voice_ws_send_initial_payload(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    uint8_t resume,
    dcc_ws_t *ws
);
dcc_status_t dcc_voice_ws_receive_loop(dcc_voice_ws_session_t *session);
void dcc_voice_client_websocket_loop_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif
