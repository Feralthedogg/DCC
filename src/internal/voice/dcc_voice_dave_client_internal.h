#ifndef DCC_VOICE_DAVE_CLIENT_INTERNAL_H
#define DCC_VOICE_DAVE_CLIENT_INTERNAL_H

#include <dcc/voice.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_voice_client_send_dave_mls_binary(
    dcc_voice_client_t *voice_client,
    dcc_voice_dave_mls_opcode_t opcode,
    const uint8_t *payload,
    size_t payload_size
);

#ifdef __cplusplus
}
#endif

#endif
