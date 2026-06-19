#ifndef DCC_VOICE_DAVE_H
#define DCC_VOICE_DAVE_H

#include <dcc/voice/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_voice_client_set_dave_enabled(dcc_voice_client_t *voice_client, uint8_t enabled);
DCC_API uint8_t dcc_voice_client_dave_enabled(const dcc_voice_client_t *voice_client);
DCC_API dcc_voice_dave_version_t dcc_voice_client_dave_version(const dcc_voice_client_t *voice_client);
DCC_API uint8_t dcc_voice_client_dave_transition_pending(const dcc_voice_client_t *voice_client);
DCC_API uint16_t dcc_voice_client_dave_transition_id(const dcc_voice_client_t *voice_client);
DCC_API dcc_status_t dcc_voice_client_set_dave_mls_handler(
    dcc_voice_client_t *voice_client,
    dcc_voice_dave_mls_fn handler,
    void *user_data
);
DCC_API const char *dcc_voice_dave_mls_opcode_string(dcc_voice_dave_mls_opcode_t opcode);
DCC_API dcc_status_t dcc_voice_parse_dave_mls_gateway_frame(
    const uint8_t *data,
    size_t data_size,
    dcc_voice_dave_mls_frame_t *out
);
DCC_API dcc_status_t dcc_voice_build_dave_mls_gateway_frame(
    uint16_t sequence,
    dcc_voice_dave_mls_opcode_t opcode,
    uint16_t transition_id,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *out,
    size_t out_cap,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_client_handle_gateway_binary_frame(
    dcc_voice_client_t *voice_client,
    const uint8_t *data,
    size_t data_size
);
DCC_API dcc_status_t dcc_voice_client_build_dave_transition_ready_payload(
    uint16_t transition_id,
    char *out,
    size_t out_cap,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_client_build_dave_invalid_commit_payload(
    uint16_t transition_id,
    char *out,
    size_t out_cap,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_client_build_dave_mls_frame(
    dcc_voice_dave_mls_opcode_t opcode,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *out,
    size_t out_cap,
    size_t *out_len
);
DCC_API dcc_status_t dcc_voice_client_send_dave_transition_ready(
    dcc_voice_client_t *voice_client,
    uint16_t transition_id
);
DCC_API dcc_status_t dcc_voice_client_send_dave_mls_key_package(
    dcc_voice_client_t *voice_client,
    const uint8_t *payload,
    size_t payload_size
);
DCC_API dcc_status_t dcc_voice_client_send_dave_mls_commit_welcome(
    dcc_voice_client_t *voice_client,
    const uint8_t *payload,
    size_t payload_size
);
DCC_API dcc_status_t dcc_voice_client_send_dave_invalid_commit_welcome(
    dcc_voice_client_t *voice_client,
    uint16_t transition_id
);

#ifdef __cplusplus
}
#endif

#endif
