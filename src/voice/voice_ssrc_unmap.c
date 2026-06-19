#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_unmap_ssrc(dcc_voice_client_t *voice_client, uint32_t ssrc) {
    if (voice_client == NULL || ssrc == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t index = 0;
    if (dcc_voice_client_find_ssrc(voice_client, ssrc, &index)) {
        dcc_voice_client_remove_map_index(voice_client, index);
    }
    return DCC_OK;
}

void dcc_voice_client_unmap_user(dcc_voice_client_t *voice_client, dcc_snowflake_t user_id) {
    size_t index = 0;
    if (dcc_voice_client_find_user(voice_client, user_id, &index)) {
        dcc_voice_client_remove_map_index(voice_client, index);
    }
}
