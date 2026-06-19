#include "internal/cache/dcc_cache_internal.h"

void dcc_cache_merge_voice_state_view(
    dcc_voice_state_t *merged,
    const dcc_voice_state_t *existing,
    uint64_t fields
) {
    if (merged == NULL || existing == NULL) {
        return;
    }
    if (merged->guild_id == 0) {
        merged->guild_id = existing->guild_id;
    }
    if ((fields & DCC_CACHE_FIELD_VOICE_CHANNEL_ID) == 0 && merged->channel_id == 0) {
        merged->channel_id = existing->channel_id;
    }
    if (merged->user_id == 0) {
        merged->user_id = existing->user_id;
    }
    if (merged->session_id == NULL) {
        merged->session_id = existing->session_id;
    }
    if (merged->request_to_speak_timestamp == NULL) {
        merged->request_to_speak_timestamp = existing->request_to_speak_timestamp;
    }
    if ((fields & DCC_CACHE_FIELD_VOICE_FLAGS) == 0 && merged->deaf == 0) {
        merged->deaf = existing->deaf;
    }
    if ((fields & DCC_CACHE_FIELD_VOICE_FLAGS) == 0 && merged->mute == 0) {
        merged->mute = existing->mute;
    }
    if ((fields & DCC_CACHE_FIELD_VOICE_FLAGS) == 0 && merged->self_deaf == 0) {
        merged->self_deaf = existing->self_deaf;
    }
    if ((fields & DCC_CACHE_FIELD_VOICE_FLAGS) == 0 && merged->self_mute == 0) {
        merged->self_mute = existing->self_mute;
    }
    if ((fields & DCC_CACHE_FIELD_VOICE_FLAGS) == 0 && merged->self_stream == 0) {
        merged->self_stream = existing->self_stream;
    }
    if ((fields & DCC_CACHE_FIELD_VOICE_FLAGS) == 0 && merged->self_video == 0) {
        merged->self_video = existing->self_video;
    }
    if ((fields & DCC_CACHE_FIELD_VOICE_FLAGS) == 0 && merged->suppress == 0) {
        merged->suppress = existing->suppress;
    }
}
