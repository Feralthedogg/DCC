#include "internal/voice/dcc_voice_internal.h"

dcc_snowflake_t dcc_voice_client_user_for_ssrc(const dcc_voice_client_t *voice_client, uint32_t ssrc) {
    size_t index = 0;
    if (!dcc_voice_client_find_ssrc(voice_client, ssrc, &index)) {
        return 0;
    }
    return voice_client->ssrc_map_user_ids[index];
}
