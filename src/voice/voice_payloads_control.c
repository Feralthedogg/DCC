#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_build_heartbeat_payload(
    const dcc_voice_client_t *voice_client,
    uint64_t nonce,
    char *out,
    size_t out_cap,
    size_t *out_len
) {
    if (voice_client == NULL || out == NULL || out_cap == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_json_buffer_t payload = {
        .data = out,
        .cap = out_cap,
    };
    if (dcc_voice_json_append_cstr(&payload, "{\"op\":3,\"d\":{\"t\":") != 0 ||
        dcc_voice_json_append_u64(&payload, nonce) != 0 ||
        dcc_voice_json_append_cstr(&payload, ",\"seq_ack\":") != 0 ||
        dcc_voice_json_append_i32(&payload, voice_client->receive_sequence) != 0 ||
        dcc_voice_json_append_cstr(&payload, "}}") != 0) {
        return dcc_voice_json_finish(&payload, out_len);
    }
    return dcc_voice_json_finish(&payload, out_len);
}

dcc_status_t dcc_voice_client_build_speaking_payload(
    const dcc_voice_client_t *voice_client,
    uint8_t speaking,
    char *out,
    size_t out_cap,
    size_t *out_len
) {
    if (voice_client == NULL || out == NULL || out_cap == 0 || voice_client->ssrc == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_json_buffer_t payload = {
        .data = out,
        .cap = out_cap,
    };
    if (dcc_voice_json_append_cstr(&payload, "{\"op\":5,\"d\":{\"speaking\":") != 0 ||
        dcc_voice_json_append_u64(&payload, speaking ? 1U : 0U) != 0 ||
        dcc_voice_json_append_cstr(&payload, ",\"delay\":0,\"ssrc\":") != 0 ||
        dcc_voice_json_append_u64(&payload, (uint64_t)voice_client->ssrc) != 0 ||
        dcc_voice_json_append_cstr(&payload, "}}") != 0) {
        return dcc_voice_json_finish(&payload, out_len);
    }
    return dcc_voice_json_finish(&payload, out_len);
}
