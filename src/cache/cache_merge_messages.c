#include "internal/cache/dcc_cache_internal.h"

#include <string.h>

void dcc_cache_merge_message_view(dcc_message_t *merged, const dcc_message_t *existing, uint64_t fields) {
    if (merged == NULL || existing == NULL) {
        return;
    }
    if (merged->id == 0) {
        merged->id = existing->id;
    }
    if (merged->channel_id == 0) {
        merged->channel_id = existing->channel_id;
    }
    if (merged->guild_id == 0) {
        merged->guild_id = existing->guild_id;
    }
    if (merged->author.id == 0) {
        merged->author = existing->author;
    } else if (merged->author.id == existing->author.id) {
        dcc_cache_merge_user_view(&merged->author, &existing->author, fields);
    }
    if ((fields & DCC_CACHE_FIELD_MESSAGE_CONTENT) == 0 && merged->content == NULL) {
        merged->content = existing->content;
    }
    if ((fields & DCC_CACHE_FIELD_MESSAGE_FLAGS) == 0 && merged->flags == 0) {
        merged->flags = existing->flags;
    }
    if ((fields & DCC_CACHE_FIELD_MESSAGE_TIMESTAMP) == 0 && merged->timestamp == NULL) {
        merged->timestamp = existing->timestamp;
    }
    if ((fields & DCC_CACHE_FIELD_MESSAGE_EDITED_TIMESTAMP) == 0 && merged->edited_timestamp == NULL) {
        merged->edited_timestamp = existing->edited_timestamp;
    }
    if ((fields & DCC_CACHE_FIELD_MESSAGE_COMPONENTS) == 0 && merged->components_json == NULL) {
        merged->components_json = existing->components_json;
        merged->components_json_truncated = existing->components_json_truncated;
        merged->components = existing->components;
        merged->components_count = existing->components_count;
        memcpy(
            merged->component_root_indices,
            existing->component_root_indices,
            sizeof(merged->component_root_indices)
        );
        merged->component_root_count = existing->component_root_count;
        merged->components_truncated = existing->components_truncated;
    }
    if ((fields & DCC_CACHE_FIELD_MESSAGE_WEBHOOK_ID) == 0 && merged->webhook_id == 0) {
        merged->webhook_id = existing->webhook_id;
    }
    if ((fields & DCC_CACHE_FIELD_MESSAGE_TYPE) == 0 && merged->type == 0) {
        merged->type = existing->type;
    }
    if ((fields & DCC_CACHE_FIELD_MESSAGE_TTS) == 0 && merged->tts == 0) {
        merged->tts = existing->tts;
    }
    if ((fields & DCC_CACHE_FIELD_MESSAGE_MENTION_EVERYONE) == 0 && merged->mention_everyone == 0) {
        merged->mention_everyone = existing->mention_everyone;
    }
    if ((fields & DCC_CACHE_FIELD_MESSAGE_PINNED) == 0 && merged->pinned == 0) {
        merged->pinned = existing->pinned;
    }
}
