#include "internal/cache/dcc_cache_merge_guild_internal.h"
#include "internal/dcc_cache_fields.h"

void dcc_cache_merge_guild_limit_fields(dcc_guild_t *merged, const dcc_guild_t *existing, uint64_t fields) {
    if ((fields & DCC_CACHE_FIELD_GUILD_MEMBER_COUNT) == 0 && merged->member_count == 0) {
        merged->member_count = existing->member_count;
    }
    if ((fields & DCC_CACHE_FIELD_GUILD_LIMITS) != 0) {
        return;
    }
    if (merged->max_presences == 0) {
        merged->max_presences = existing->max_presences;
    }
    if (merged->max_members == 0) {
        merged->max_members = existing->max_members;
    }
    if (merged->premium_subscription_count == 0) {
        merged->premium_subscription_count = existing->premium_subscription_count;
    }
    if (merged->afk_timeout == 0) {
        merged->afk_timeout = existing->afk_timeout;
    }
    if (merged->max_video_channel_users == 0) {
        merged->max_video_channel_users = existing->max_video_channel_users;
    }
    if (merged->default_message_notifications == 0) {
        merged->default_message_notifications = existing->default_message_notifications;
    }
    if (merged->premium_tier == 0) {
        merged->premium_tier = existing->premium_tier;
    }
    if (merged->verification_level == 0) {
        merged->verification_level = existing->verification_level;
    }
    if (merged->explicit_content_filter == 0) {
        merged->explicit_content_filter = existing->explicit_content_filter;
    }
    if (merged->mfa_level == 0) {
        merged->mfa_level = existing->mfa_level;
    }
    if (merged->nsfw_level == 0) {
        merged->nsfw_level = existing->nsfw_level;
    }
}
