#include "internal/voice/dcc_voice_internal.h"

#include <string.h>

dcc_status_t dcc_voice_client_build_dave_transition_ready_payload(
    uint16_t transition_id,
    char *out,
    size_t out_cap,
    size_t *out_len
) {
    if (transition_id == 0 || out == NULL || out_cap == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_json_buffer_t payload = {
        .data = out,
        .cap = out_cap,
    };
    if (dcc_voice_json_append_cstr(&payload, "{\"op\":23,\"d\":{\"transition_id\":") != 0 ||
        dcc_voice_json_append_u64(&payload, (uint64_t)transition_id) != 0 ||
        dcc_voice_json_append_cstr(&payload, "}}") != 0) {
        return dcc_voice_json_finish(&payload, out_len);
    }
    return dcc_voice_json_finish(&payload, out_len);
}

dcc_status_t dcc_voice_client_build_dave_invalid_commit_payload(
    uint16_t transition_id,
    char *out,
    size_t out_cap,
    size_t *out_len
) {
    if (transition_id == 0 || out == NULL || out_cap == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_json_buffer_t payload = {
        .data = out,
        .cap = out_cap,
    };
    if (dcc_voice_json_append_cstr(&payload, "{\"op\":31,\"d\":{\"transition_id\":") != 0 ||
        dcc_voice_json_append_u64(&payload, (uint64_t)transition_id) != 0 ||
        dcc_voice_json_append_cstr(&payload, "}}") != 0) {
        return dcc_voice_json_finish(&payload, out_len);
    }
    return dcc_voice_json_finish(&payload, out_len);
}

dcc_status_t dcc_voice_client_build_dave_mls_frame(
    dcc_voice_dave_mls_opcode_t opcode,
    const uint8_t *payload,
    size_t payload_size,
    uint8_t *out,
    size_t out_cap,
    size_t *out_len
) {
    if ((opcode != DCC_VOICE_DAVE_MLS_KEY_PACKAGE &&
            opcode != DCC_VOICE_DAVE_MLS_COMMIT_WELCOME) ||
        payload == NULL ||
        payload_size == 0 ||
        out == NULL ||
        out_cap == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (payload_size > SIZE_MAX - 1U) {
        return DCC_ERR_NOMEM;
    }

    size_t frame_size = payload_size + 1U;
    if (out_len != NULL) {
        *out_len = frame_size;
    }
    if (out_cap < frame_size) {
        return DCC_ERR_NOMEM;
    }

    out[0] = (uint8_t)opcode;
    memcpy(out + 1U, payload, payload_size);
    return DCC_OK;
}
