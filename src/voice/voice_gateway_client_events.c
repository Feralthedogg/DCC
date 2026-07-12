#include "internal/json/dcc_json.h"
#include "internal/voice/dcc_voice_internal.h"

static void dcc_voice_dave_remove_participant(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id
) {
    for (size_t i = 0; i < voice_client->dave_participant_count; ++i) {
        if (voice_client->dave_participants[i] != user_id) {
            continue;
        }
        size_t last = voice_client->dave_participant_count - 1U;
        voice_client->dave_participants[i] = voice_client->dave_participants[last];
        voice_client->dave_participants[last] = 0U;
        voice_client->dave_participant_count--;
        dcc_voice_dave_backend_remove_participant(voice_client, user_id);
        return;
    }
}

dcc_status_t dcc_voice_gateway_handle_clients_connect(
    dcc_voice_client_t *voice_client,
    const dcc_json_t *d
) {
    if (voice_client == NULL || d == NULL) {
        return DCC_ERR_JSON;
    }
    const dcc_json_t *user_ids = dcc_json_object_get(d, "user_ids");
    size_t count = dcc_json_array_size(user_ids);
    if (count > DCC_VOICE_SSRC_MAP_CAP) {
        return DCC_ERR_NOMEM;
    }
    voice_client->dave_participant_count = 0U;
    for (size_t i = 0; i < count; ++i) {
        uint64_t user_id = 0U;
        if (dcc_voice_gateway_json_u64(dcc_json_array_get(user_ids, i), &user_id) != 0 ||
            user_id == 0U) {
            voice_client->dave_participant_count = 0U;
            return DCC_ERR_JSON;
        }
        voice_client->dave_participants[voice_client->dave_participant_count++] = user_id;
    }
    return DCC_OK;
}

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
    dcc_voice_dave_remove_participant(voice_client, user_id);
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
