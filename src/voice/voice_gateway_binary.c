#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_handle_gateway_binary_frame(
    dcc_voice_client_t *voice_client,
    const uint8_t *data,
    size_t data_size
) {
    if (voice_client == NULL || data == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_dave_mls_frame_t frame = {
        .size = sizeof(frame),
    };
    dcc_status_t parse_status = dcc_voice_parse_dave_mls_gateway_frame(data, data_size, &frame);
    if (parse_status != DCC_OK) {
        return parse_status;
    }

    if (voice_client->dave_mls_handler != NULL) {
        dcc_status_t status = voice_client->dave_mls_handler(
            voice_client->dave_mls_user_data,
            voice_client,
            frame.opcode,
            frame.sequence,
            frame.transition_id,
            frame.payload,
            frame.payload_size
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    voice_client->receive_sequence = (int32_t)frame.sequence;
    if (frame.has_transition_id) {
        voice_client->dave_transition_id = frame.transition_id;
        voice_client->dave_transition_pending = 1U;
        voice_client->dave_transition_ready = 0U;
    }
    return DCC_OK;
}
