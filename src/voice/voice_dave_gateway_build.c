#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

dcc_status_t dcc_voice_build_dave_mls_gateway_frame(
    uint16_t sequence,
    dcc_voice_dave_mls_opcode_t opcode,
    uint16_t transition_id,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *out,
    size_t out_cap,
    size_t *out_len
) {
    uint8_t has_transition_id = 0U;
    switch (opcode) {
        case DCC_VOICE_DAVE_MLS_EXTERNAL_SENDER:
        case DCC_VOICE_DAVE_MLS_PROPOSALS:
            break;
        case DCC_VOICE_DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION:
        case DCC_VOICE_DAVE_MLS_WELCOME:
            has_transition_id = 1U;
            break;
        default:
            return DCC_ERR_INVALID_ARG;
    }
    if ((payload == NULL && payload_size != 0U) || out == NULL || out_cap == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t frame_size = 3U + (has_transition_id ? 2U : 0U);
    if (payload_size > SIZE_MAX - frame_size) {
        return DCC_ERR_NOMEM;
    }
    frame_size += payload_size;
    if (out_len != NULL) {
        *out_len = frame_size;
    }
    if (out_cap < frame_size) {
        return DCC_ERR_NOMEM;
    }

    out[0] = (uint8_t)(sequence >> 8U);
    out[1] = (uint8_t)sequence;
    out[2] = (uint8_t)opcode;
    size_t offset = 3U;
    if (has_transition_id) {
        out[offset++] = (uint8_t)(transition_id >> 8U);
        out[offset++] = (uint8_t)transition_id;
    }
    if (payload_size != 0U) {
        memcpy(out + offset, payload, payload_size);
    }
    return DCC_OK;
}
