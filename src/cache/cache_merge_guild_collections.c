#include "internal/cache/dcc_cache_merge_guild_internal.h"
#include "internal/dcc_cache_fields.h"

void dcc_cache_merge_guild_collection_fields(dcc_guild_t *merged, const dcc_guild_t *existing, uint64_t fields) {
    if ((fields & DCC_CACHE_FIELD_GUILD_COLLECTIONS) != 0) {
        return;
    }
    if (merged->roles == NULL && merged->roles_count == 0) {
        merged->roles = existing->roles;
        merged->roles_count = existing->roles_count;
        merged->roles_truncated = existing->roles_truncated;
    }
    if (merged->channels == NULL && merged->channels_count == 0) {
        merged->channels = existing->channels;
        merged->channels_count = existing->channels_count;
        merged->channels_truncated = existing->channels_truncated;
    }
    if (merged->threads == NULL && merged->threads_count == 0) {
        merged->threads = existing->threads;
        merged->threads_count = existing->threads_count;
        merged->threads_truncated = existing->threads_truncated;
    }
    if (merged->scheduled_events == NULL && merged->scheduled_events_count == 0) {
        merged->scheduled_events = existing->scheduled_events;
        merged->scheduled_events_count = existing->scheduled_events_count;
        merged->scheduled_events_truncated = existing->scheduled_events_truncated;
    }
    if (merged->stage_instances == NULL && merged->stage_instances_count == 0) {
        merged->stage_instances = existing->stage_instances;
        merged->stage_instances_count = existing->stage_instances_count;
        merged->stage_instances_truncated = existing->stage_instances_truncated;
    }
    if (merged->presences == NULL && merged->presences_count == 0) {
        merged->presences = existing->presences;
        merged->presences_count = existing->presences_count;
        merged->presences_truncated = existing->presences_truncated;
    }
    if (merged->emojis == NULL && merged->emojis_count == 0) {
        merged->emojis = existing->emojis;
        merged->emojis_count = existing->emojis_count;
        merged->emojis_truncated = existing->emojis_truncated;
    }
    if (merged->stickers == NULL && merged->stickers_count == 0) {
        merged->stickers = existing->stickers;
        merged->stickers_count = existing->stickers_count;
        merged->stickers_truncated = existing->stickers_truncated;
    }
    if (merged->soundboard_sounds == NULL && merged->soundboard_sounds_count == 0) {
        merged->soundboard_sounds = existing->soundboard_sounds;
        merged->soundboard_sounds_count = existing->soundboard_sounds_count;
        merged->soundboard_sounds_truncated = existing->soundboard_sounds_truncated;
    }
}
