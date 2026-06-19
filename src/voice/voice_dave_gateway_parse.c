#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

dcc_status_t dcc_voice_parse_dave_mls_gateway_frame(
    const uint8_t *data,
    size_t data_size,
    dcc_voice_dave_mls_frame_t *out
) {
    if (data == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (data_size < 3U) {
        return DCC_ERR_JSON;
    }

    size_t out_size = out->size;
    memset(out, 0, sizeof(*out));
    out->size = out_size;
    out->sequence = ((uint16_t)data[0] << 8U) | (uint16_t)data[1];
    out->opcode = (dcc_voice_dave_mls_opcode_t)data[2];
    out->payload = data + 3U;
    out->payload_size = data_size - 3U;

    switch (out->opcode) {
        case DCC_VOICE_DAVE_MLS_EXTERNAL_SENDER:
        case DCC_VOICE_DAVE_MLS_PROPOSALS:
            return DCC_OK;
        case DCC_VOICE_DAVE_MLS_ANNOUNCE_COMMIT_TRANSITION:
        case DCC_VOICE_DAVE_MLS_WELCOME:
            if (out->payload_size < 2U) {
                return DCC_ERR_JSON;
            }
            out->transition_id = ((uint16_t)out->payload[0] << 8U) | (uint16_t)out->payload[1];
            out->payload += 2U;
            out->payload_size -= 2U;
            out->has_transition_id = 1U;
            return DCC_OK;
        default:
            return DCC_ERR_JSON;
    }
}
