#include "internal/json/dcc_json.h"
#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_gateway_handle_dave_prepare_transition(
    dcc_voice_client_t *voice_client,
    const dcc_json_t *d
) {
    uint16_t transition_id = 0;
    uint32_t protocol_version = 0;
    if (dcc_voice_gateway_json_u16(dcc_json_object_get(d, "transition_id"), &transition_id) != 0 ||
        dcc_voice_gateway_json_u32(dcc_json_object_get(d, "protocol_version"), &protocol_version) != 0) {
        return DCC_ERR_JSON;
    }
    voice_client->dave_transition_id = transition_id;
    voice_client->dave_pending_version = dcc_voice_gateway_dave_version_from_u32(protocol_version);
    if (transition_id == 0) {
        voice_client->dave_version = voice_client->dave_pending_version;
        voice_client->dave_enabled = voice_client->dave_version != DCC_VOICE_DAVE_NONE ? 1U : 0U;
        voice_client->dave_pending_version = DCC_VOICE_DAVE_NONE;
        voice_client->dave_transition_pending = 0;
        voice_client->dave_transition_ready = 0;
        if (voice_client->dave_backend != NULL) {
            dcc_status_t status = dcc_voice_dave_backend_set_protocol(
                voice_client,
                voice_client->dave_version,
                voice_client->dave_version == DCC_VOICE_DAVE_NONE ? 1U : 0U
            );
            if (status != DCC_OK) return status;
        }
    } else {
        voice_client->dave_transition_pending = 1;
        voice_client->dave_transition_ready = 0;
        voice_client->dave_media_ready = 0;
    }
    return DCC_OK;
}

dcc_status_t dcc_voice_gateway_handle_dave_execute_transition(
    dcc_voice_client_t *voice_client,
    const dcc_json_t *d
) {
    uint16_t transition_id = 0;
    if (dcc_voice_gateway_json_u16(dcc_json_object_get(d, "transition_id"), &transition_id) != 0) {
        return DCC_ERR_JSON;
    }
    if (voice_client->dave_transition_pending &&
        voice_client->dave_transition_id != 0 &&
        voice_client->dave_transition_id != transition_id) {
        return DCC_ERR_JSON;
    }
    voice_client->dave_transition_id = transition_id;
    if (voice_client->dave_transition_pending &&
        voice_client->dave_pending_version != DCC_VOICE_DAVE_NONE) {
        voice_client->dave_version = voice_client->dave_pending_version;
        voice_client->dave_enabled = 1;
    } else if (voice_client->dave_transition_pending) {
        voice_client->dave_version = DCC_VOICE_DAVE_NONE;
        voice_client->dave_enabled = 0;
    }
    voice_client->dave_pending_version = DCC_VOICE_DAVE_NONE;
    voice_client->dave_transition_pending = 0;
    if (voice_client->dave_backend != NULL) {
        if (voice_client->dave_version != DCC_VOICE_DAVE_NONE &&
            (!voice_client->dave_transition_ready || !voice_client->dave_media_ready)) {
            return DCC_ERR_STATE;
        }
        dcc_status_t status = dcc_voice_dave_backend_set_protocol(
            voice_client,
            voice_client->dave_version,
            1U
        );
        if (status != DCC_OK) return status;
    }
    voice_client->dave_transition_ready = 0;
    return DCC_OK;
}

dcc_status_t dcc_voice_gateway_handle_dave_session_update(dcc_voice_client_t *voice_client, const dcc_json_t *d) {
    uint32_t protocol_version = 0;
    uint32_t epoch = 0;
    if (dcc_voice_gateway_json_u32(dcc_json_object_get(d, "protocol_version"), &protocol_version) != 0 ||
        dcc_voice_gateway_json_u32(dcc_json_object_get(d, "epoch"), &epoch) != 0) {
        return DCC_ERR_JSON;
    }
    voice_client->dave_epoch = epoch;
    voice_client->dave_pending_version = dcc_voice_gateway_dave_version_from_u32(protocol_version);
    voice_client->dave_transition_pending = 1U;
    voice_client->dave_transition_ready = 0U;
    voice_client->dave_media_ready = 0U;
    if (voice_client->dave_backend != NULL) {
        dcc_status_t status = dcc_voice_dave_backend_set_protocol(
            voice_client,
            voice_client->dave_pending_version,
            0U
        );
        if (status != DCC_OK) return status;
    }
    return DCC_OK;
}

dcc_status_t dcc_voice_gateway_handle_dave_transition_result(
    dcc_voice_client_t *voice_client,
    const dcc_json_t *d
) {
    uint16_t transition_id = 0;
    if (dcc_voice_gateway_json_u16(dcc_json_object_get(d, "transition_id"), &transition_id) != 0) {
        return DCC_ERR_JSON;
    }
    if (voice_client->dave_transition_pending &&
        voice_client->dave_transition_id != 0 &&
        voice_client->dave_transition_id != transition_id) {
        return DCC_ERR_JSON;
    }
    voice_client->dave_transition_id = transition_id;
    voice_client->dave_pending_version = DCC_VOICE_DAVE_NONE;
    voice_client->dave_transition_pending = 0;
    voice_client->dave_transition_ready = 0;
    return DCC_OK;
}
