#ifndef DCC_VOICE_SESSION_INTERNAL_H
#define DCC_VOICE_SESSION_INTERNAL_H

#include "internal/ws/dcc_ws.h"

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_voice_dispatch(dcc_voice_client_t *voice_client, const dcc_event_t *event);
void dcc_voice_clear_session_descriptor(dcc_voice_client_t *voice_client);
uint8_t dcc_voice_session_descriptor_ready(const dcc_voice_client_t *voice_client);
void dcc_voice_refresh_session_state(dcc_voice_client_t *voice_client);
void dcc_voice_reset_transport_state(dcc_voice_client_t *voice_client);
dcc_status_t dcc_voice_client_send_current_ws(
    dcc_voice_client_t *voice_client,
    dcc_ws_opcode_t opcode,
    const void *data,
    size_t data_size
);
void dcc_voice_opus_backend_destroy(dcc_voice_client_t *voice_client);

#ifdef __cplusplus
}
#endif

#endif
