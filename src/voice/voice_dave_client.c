#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_set_dave_mls_handler(
    dcc_voice_client_t *voice_client,
    dcc_voice_dave_mls_fn handler,
    void *user_data
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    voice_client->dave_mls_handler = handler;
    voice_client->dave_mls_user_data = user_data;
    if (handler != NULL) {
        voice_client->dave_backend_ready = 1U;
    } else if (voice_client->dave_backend == NULL) {
        voice_client->dave_backend_ready = 0U;
    }
    return DCC_OK;
}

dcc_status_t dcc_voice_client_send_dave_transition_ready(
    dcc_voice_client_t *voice_client,
    uint16_t transition_id
) {
    if (voice_client == NULL || transition_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    char payload[64];
    size_t payload_len = 0;
    dcc_status_t status = dcc_voice_client_build_dave_transition_ready_payload(
        transition_id,
        payload,
        sizeof(payload),
        &payload_len
    );
    if (status != DCC_OK) {
        return status;
    }
    return dcc_voice_client_send_current_ws(voice_client, DCC_WS_TEXT, payload, payload_len);
}

dcc_status_t dcc_voice_client_send_dave_mls_key_package(
    dcc_voice_client_t *voice_client,
    const uint8_t *payload,
    size_t payload_size
) {
    return dcc_voice_client_send_dave_mls_binary(
        voice_client,
        DCC_VOICE_DAVE_MLS_KEY_PACKAGE,
        payload,
        payload_size
    );
}

dcc_status_t dcc_voice_client_send_dave_mls_commit_welcome(
    dcc_voice_client_t *voice_client,
    const uint8_t *payload,
    size_t payload_size
) {
    return dcc_voice_client_send_dave_mls_binary(
        voice_client,
        DCC_VOICE_DAVE_MLS_COMMIT_WELCOME,
        payload,
        payload_size
    );
}

dcc_status_t dcc_voice_client_send_dave_invalid_commit_welcome(
    dcc_voice_client_t *voice_client,
    uint16_t transition_id
) {
    if (voice_client == NULL || transition_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    char payload[64];
    size_t payload_len = 0;
    dcc_status_t status = dcc_voice_client_build_dave_invalid_commit_payload(
        transition_id,
        payload,
        sizeof(payload),
        &payload_len
    );
    if (status != DCC_OK) {
        return status;
    }
    return dcc_voice_client_send_current_ws(voice_client, DCC_WS_TEXT, payload, payload_len);
}
