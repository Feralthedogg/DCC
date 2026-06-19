#include "internal/json/dcc_json.h"
#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_gateway_handle_speaking(dcc_voice_client_t *voice_client, const dcc_json_t *d) {
    uint64_t user_id = 0;
    uint32_t ssrc = 0;
    if (dcc_voice_gateway_json_u64(dcc_json_object_get(d, "user_id"), &user_id) != 0 ||
        dcc_voice_gateway_json_u32(dcc_json_object_get(d, "ssrc"), &ssrc) != 0) {
        return DCC_ERR_JSON;
    }
    voice_client->ssrc = ssrc;
    dcc_status_t status = dcc_voice_client_map_ssrc(voice_client, ssrc, user_id);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_voice_client_notify_speaking(voice_client, user_id, ssrc);
}

dcc_status_t dcc_voice_gateway_handle_client_disconnect(dcc_voice_client_t *voice_client, const dcc_json_t *d) {
    uint64_t user_id = 0;
    if (dcc_voice_gateway_json_u64(dcc_json_object_get(d, "user_id"), &user_id) != 0) {
        return DCC_ERR_JSON;
    }
    dcc_voice_client_unmap_user(voice_client, user_id);
    return dcc_voice_client_notify_disconnect(voice_client, user_id);
}

dcc_status_t dcc_voice_gateway_handle_client_platform(dcc_voice_client_t *voice_client, const dcc_json_t *d) {
    uint64_t user_id = 0;
    uint32_t platform = 0;
    if (dcc_voice_gateway_json_u64(dcc_json_object_get(d, "user_id"), &user_id) != 0 ||
        dcc_voice_gateway_json_u32(dcc_json_object_get(d, "platform"), &platform) != 0 ||
        platform > DCC_VOICE_CLIENT_PLATFORM_MOBILE) {
        return DCC_ERR_JSON;
    }
    return dcc_voice_client_notify_platform(voice_client, user_id, (dcc_voice_client_platform_t)platform);
}
