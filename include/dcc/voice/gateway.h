#ifndef DCC_VOICE_GATEWAY_H
#define DCC_VOICE_GATEWAY_H

#include <dcc/voice/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_voice_client_set_receive_sequence(
    dcc_voice_client_t *voice_client,
    int32_t sequence
);
DCC_API int32_t dcc_voice_client_receive_sequence(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_build_identify_payload(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    char *out,
    size_t out_cap,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_client_build_resume_payload(
    const dcc_voice_client_t *voice_client,
    char *out,
    size_t out_cap,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_client_build_heartbeat_payload(
    const dcc_voice_client_t *voice_client,
    uint64_t nonce,
    char *out,
    size_t out_cap,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_client_build_select_protocol_payload(
    const char *address,
    uint16_t port,
    dcc_voice_encryption_mode_t mode,
    char *out,
    size_t out_cap,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_client_build_speaking_payload(
    const dcc_voice_client_t *voice_client,
    uint8_t speaking,
    char *out,
    size_t out_cap,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_client_build_websocket_url(
    const dcc_voice_client_t *voice_client,
    char *out,
    size_t out_cap,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_client_run_websocket_url(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    const char *url,
    uint8_t resume
);
DCC_API dcc_status_t dcc_voice_client_run_websocket(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    uint8_t resume
);
DCC_API dcc_status_t dcc_voice_client_run_websocket_loop_url(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    const char *url,
    uint32_t reconnect_delay_ms
);
DCC_API dcc_status_t dcc_voice_client_run_websocket_loop(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    uint32_t reconnect_delay_ms
);
DCC_API dcc_status_t dcc_voice_client_start_websocket_loop(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    uint32_t reconnect_delay_ms
);
DCC_API dcc_status_t dcc_voice_client_stop_websocket_loop(dcc_voice_client_t *voice_client);
DCC_API uint8_t dcc_voice_client_websocket_loop_running(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_websocket_loop_status(
    const dcc_voice_client_t *voice_client,
    dcc_status_t *out_status
);

#ifdef __cplusplus
}
#endif

#endif
