#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_set_dave_enabled(dcc_voice_client_t *voice_client, uint8_t enabled) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (enabled && voice_client->dave_mls_handler == NULL) {
        dcc_status_t backend_status = dcc_voice_dave_backend_load(voice_client);
        if (backend_status != DCC_OK) {
            voice_client->dave_requested = 1U;
            voice_client->dave_enabled = 0U;
            voice_client->dave_backend_ready = 0U;
            voice_client->dave_media_ready = 0U;
            voice_client->dave_version = DCC_VOICE_DAVE_NONE;
            if (voice_client->client != NULL) {
                dcc_set_error(
                    voice_client->client,
                    "DAVE requested but the official libdave backend is unavailable"
                );
            }
            return DCC_ERR_STATE;
        }
    }
    voice_client->dave_requested = enabled ? 1U : 0U;
    voice_client->dave_enabled = enabled ? 1U : 0U;
    voice_client->dave_backend_ready = enabled && voice_client->dave_backend != NULL ? 1U : 0U;
    voice_client->dave_media_ready = 0U;
    voice_client->dave_version = enabled ? DCC_VOICE_DAVE_VERSION_1 : DCC_VOICE_DAVE_NONE;
    voice_client->dave_pending_version = DCC_VOICE_DAVE_NONE;
    voice_client->dave_transition_pending = 0;
    voice_client->dave_transition_ready = 0;
    voice_client->dave_transition_id = 0;
    return DCC_OK;
}

uint8_t dcc_voice_client_dave_backend_available(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL &&
        (voice_client->dave_backend != NULL || voice_client->dave_mls_handler != NULL);
}

uint8_t dcc_voice_client_dave_enabled(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->dave_enabled : 0;
}

dcc_voice_dave_version_t dcc_voice_client_dave_version(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->dave_version : DCC_VOICE_DAVE_NONE;
}

uint8_t dcc_voice_client_dave_transition_pending(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->dave_transition_pending : 0;
}

uint16_t dcc_voice_client_dave_transition_id(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->dave_transition_id : 0;
}
