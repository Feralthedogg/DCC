#include "internal/voice/dcc_voice_internal.h"

int dcc_voice_client_find_ssrc(
    const dcc_voice_client_t *voice_client,
    uint32_t ssrc,
    size_t *out_index
) {
    if (voice_client == NULL || ssrc == 0) {
        return 0;
    }
    for (size_t i = 0; i < voice_client->ssrc_map_len; ++i) {
        if (voice_client->ssrc_map_ssrcs[i] == ssrc) {
            if (out_index != NULL) {
                *out_index = i;
            }
            return 1;
        }
    }
    return 0;
}

int dcc_voice_client_find_user(
    const dcc_voice_client_t *voice_client,
    dcc_snowflake_t user_id,
    size_t *out_index
) {
    if (voice_client == NULL || user_id == 0) {
        return 0;
    }
    for (size_t i = 0; i < voice_client->ssrc_map_len; ++i) {
        if (voice_client->ssrc_map_user_ids[i] == user_id) {
            if (out_index != NULL) {
                *out_index = i;
            }
            return 1;
        }
    }
    return 0;
}

void dcc_voice_client_remove_map_index(dcc_voice_client_t *voice_client, size_t index) {
    if (voice_client == NULL || index >= voice_client->ssrc_map_len) {
        return;
    }
    size_t last = voice_client->ssrc_map_len - 1U;
    voice_client->ssrc_map_ssrcs[index] = voice_client->ssrc_map_ssrcs[last];
    voice_client->ssrc_map_user_ids[index] = voice_client->ssrc_map_user_ids[last];
    voice_client->ssrc_map_ssrcs[last] = 0;
    voice_client->ssrc_map_user_ids[last] = 0;
    voice_client->ssrc_map_len--;
}

dcc_status_t dcc_voice_client_map_ssrc(
    dcc_voice_client_t *voice_client,
    uint32_t ssrc,
    dcc_snowflake_t user_id
) {
    if (voice_client == NULL || ssrc == 0 || user_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t index = 0;
    if (dcc_voice_client_find_ssrc(voice_client, ssrc, &index)) {
        voice_client->ssrc_map_user_ids[index] = user_id;
        return DCC_OK;
    }
    if (dcc_voice_client_find_user(voice_client, user_id, &index)) {
        voice_client->ssrc_map_ssrcs[index] = ssrc;
        return DCC_OK;
    }
    if (voice_client->ssrc_map_len >= DCC_VOICE_SSRC_MAP_CAP) {
        return DCC_ERR_NOMEM;
    }

    index = voice_client->ssrc_map_len++;
    voice_client->ssrc_map_ssrcs[index] = ssrc;
    voice_client->ssrc_map_user_ids[index] = user_id;
    return DCC_OK;
}
