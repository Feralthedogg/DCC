#ifndef DCC_VOICE_GATEWAY_HANDLERS_INTERNAL_H
#define DCC_VOICE_GATEWAY_HANDLERS_INTERNAL_H

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_json dcc_json_t;

dcc_status_t dcc_voice_gateway_handle_ready(dcc_voice_client_t *voice_client, const dcc_json_t *d);
dcc_status_t dcc_voice_gateway_handle_session_description(dcc_voice_client_t *voice_client, const dcc_json_t *d);
dcc_status_t dcc_voice_gateway_handle_hello(dcc_voice_client_t *voice_client, const dcc_json_t *d);
dcc_status_t dcc_voice_gateway_handle_speaking(dcc_voice_client_t *voice_client, const dcc_json_t *d);
dcc_status_t dcc_voice_gateway_handle_clients_connect(dcc_voice_client_t *voice_client, const dcc_json_t *d);
dcc_status_t dcc_voice_gateway_handle_client_disconnect(dcc_voice_client_t *voice_client, const dcc_json_t *d);
dcc_status_t dcc_voice_gateway_handle_client_platform(dcc_voice_client_t *voice_client, const dcc_json_t *d);
dcc_status_t dcc_voice_gateway_handle_dave_prepare_transition(
    dcc_voice_client_t *voice_client,
    const dcc_json_t *d
);
dcc_status_t dcc_voice_gateway_handle_dave_execute_transition(
    dcc_voice_client_t *voice_client,
    const dcc_json_t *d
);
dcc_status_t dcc_voice_gateway_handle_dave_session_update(dcc_voice_client_t *voice_client, const dcc_json_t *d);
dcc_status_t dcc_voice_gateway_handle_dave_transition_result(
    dcc_voice_client_t *voice_client,
    const dcc_json_t *d
);

#ifdef __cplusplus
}
#endif

#endif
