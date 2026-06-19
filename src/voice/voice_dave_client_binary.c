#include "internal/voice/dcc_voice_internal.h"

#include <stdlib.h>

dcc_status_t dcc_voice_client_send_dave_mls_binary(
    dcc_voice_client_t *voice_client,
    dcc_voice_dave_mls_opcode_t opcode,
    const uint8_t *payload,
    size_t payload_size
) {
    if (voice_client == NULL || payload == NULL || payload_size == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (payload_size > SIZE_MAX - 1U) {
        return DCC_ERR_NOMEM;
    }

    size_t frame_size = payload_size + 1U;
    uint8_t *frame = (uint8_t *)malloc(frame_size);
    if (frame == NULL) {
        return DCC_ERR_NOMEM;
    }
    size_t built_len = 0;
    dcc_status_t status = dcc_voice_client_build_dave_mls_frame(
        opcode,
        payload,
        payload_size,
        frame,
        frame_size,
        &built_len
    );
    if (status == DCC_OK) {
        status = dcc_voice_client_send_current_ws(voice_client, DCC_WS_BINARY, frame, built_len);
    }
    free(frame);
    return status;
}
