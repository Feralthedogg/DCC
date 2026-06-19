#include "internal/cache/dcc_cache_merge_guild_internal.h"

void dcc_cache_merge_guild_identity_fields(dcc_guild_t *merged, const dcc_guild_t *existing) {
    if (merged->id == 0) {
        merged->id = existing->id;
    }
    if (merged->owner_id == 0) {
        merged->owner_id = existing->owner_id;
    }
    if (merged->application_id == 0) {
        merged->application_id = existing->application_id;
    }
    if (merged->afk_channel_id == 0) {
        merged->afk_channel_id = existing->afk_channel_id;
    }
    if (merged->system_channel_id == 0) {
        merged->system_channel_id = existing->system_channel_id;
    }
    if (merged->rules_channel_id == 0) {
        merged->rules_channel_id = existing->rules_channel_id;
    }
    if (merged->public_updates_channel_id == 0) {
        merged->public_updates_channel_id = existing->public_updates_channel_id;
    }
    if (merged->widget_channel_id == 0) {
        merged->widget_channel_id = existing->widget_channel_id;
    }
    if (merged->safety_alerts_channel_id == 0) {
        merged->safety_alerts_channel_id = existing->safety_alerts_channel_id;
    }
}
