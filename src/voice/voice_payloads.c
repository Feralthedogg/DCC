#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_build_identify_payload(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    char *out,
    size_t out_cap,
    size_t *out_len
) {
    if (voice_client == NULL || self_user_id == 0 || out == NULL || out_cap == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!dcc_voice_session_descriptor_ready(voice_client)) {
        return DCC_ERR_STATE;
    }

    dcc_voice_json_buffer_t payload = {
        .data = out,
        .cap = out_cap,
    };
    if (dcc_voice_json_append_cstr(&payload, "{\"op\":0,\"d\":{\"user_id\":") != 0 ||
        dcc_voice_json_append_cstr(&payload, "\"") != 0 ||
        dcc_voice_json_append_u64(&payload, self_user_id) != 0 ||
        dcc_voice_json_append_cstr(&payload, "\",\"server_id\":") != 0 ||
        dcc_voice_json_append_cstr(&payload, "\"") != 0 ||
        dcc_voice_json_append_u64(&payload, voice_client->guild_id) != 0 ||
        dcc_voice_json_append_cstr(&payload, "\",\"session_id\":") != 0 ||
        dcc_voice_json_append_string(&payload, voice_client->session_id) != 0 ||
        dcc_voice_json_append_cstr(&payload, ",\"token\":") != 0 ||
        dcc_voice_json_append_string(&payload, voice_client->session_token) != 0 ||
        dcc_voice_json_append_cstr(&payload, ",\"max_dave_protocol_version\":") != 0 ||
        dcc_voice_json_append_u64(&payload, (uint64_t)voice_client->dave_version) != 0 ||
        dcc_voice_json_append_cstr(&payload, "}}") != 0) {
        return dcc_voice_json_finish(&payload, out_len);
    }

    voice_client->receive_sequence = -1;
    return dcc_voice_json_finish(&payload, out_len);
}

dcc_status_t dcc_voice_client_build_resume_payload(
    const dcc_voice_client_t *voice_client,
    char *out,
    size_t out_cap,
    size_t *out_len
) {
    if (voice_client == NULL || out == NULL || out_cap == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!dcc_voice_session_descriptor_ready(voice_client)) {
        return DCC_ERR_STATE;
    }

    dcc_voice_json_buffer_t payload = {
        .data = out,
        .cap = out_cap,
    };
    if (dcc_voice_json_append_cstr(&payload, "{\"op\":7,\"d\":{\"server_id\":") != 0 ||
        dcc_voice_json_append_cstr(&payload, "\"") != 0 ||
        dcc_voice_json_append_u64(&payload, voice_client->guild_id) != 0 ||
        dcc_voice_json_append_cstr(&payload, "\",\"session_id\":") != 0 ||
        dcc_voice_json_append_string(&payload, voice_client->session_id) != 0 ||
        dcc_voice_json_append_cstr(&payload, ",\"token\":") != 0 ||
        dcc_voice_json_append_string(&payload, voice_client->session_token) != 0 ||
        dcc_voice_json_append_cstr(&payload, ",\"seq_ack\":") != 0 ||
        dcc_voice_json_append_i32(&payload, voice_client->receive_sequence) != 0 ||
        dcc_voice_json_append_cstr(&payload, "}}") != 0) {
        return dcc_voice_json_finish(&payload, out_len);
    }
    return dcc_voice_json_finish(&payload, out_len);
}

dcc_status_t dcc_voice_client_build_select_protocol_payload(
    const char *address,
    uint16_t port,
    dcc_voice_encryption_mode_t mode,
    char *out,
    size_t out_cap,
    size_t *out_len
) {
    if (address == NULL || address[0] == '\0' || port == 0 || out == NULL || out_cap == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (mode != DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_json_buffer_t payload = {
        .data = out,
        .cap = out_cap,
    };
    if (dcc_voice_json_append_cstr(&payload, "{\"op\":1,\"d\":{\"protocol\":\"udp\",\"data\":{\"address\":") != 0 ||
        dcc_voice_json_append_string(&payload, address) != 0 ||
        dcc_voice_json_append_cstr(&payload, ",\"port\":") != 0 ||
        dcc_voice_json_append_u64(&payload, (uint64_t)port) != 0 ||
        dcc_voice_json_append_cstr(
            &payload,
            ",\"mode\":\"aead_xchacha20_poly1305_rtpsize\"}}}"
        ) != 0) {
        return dcc_voice_json_finish(&payload, out_len);
    }
    return dcc_voice_json_finish(&payload, out_len);
}
