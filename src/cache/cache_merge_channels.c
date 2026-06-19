#include "internal/cache/dcc_cache_internal.h"

void dcc_cache_merge_channel_view(dcc_channel_t *merged, const dcc_channel_t *existing, uint64_t fields) {
    if (merged == NULL || existing == NULL) {
        return;
    }
    if (merged->id == 0) {
        merged->id = existing->id;
    }
    if (merged->guild_id == 0) {
        merged->guild_id = existing->guild_id;
    }
    if ((fields & DCC_CACHE_FIELD_CHANNEL_PARENT_ID) == 0 && merged->parent_id == 0) {
        merged->parent_id = existing->parent_id;
    }
    if ((fields & DCC_CACHE_FIELD_CHANNEL_LAST_MESSAGE_ID) == 0 && merged->last_message_id == 0) {
        merged->last_message_id = existing->last_message_id;
    }
    if ((fields & DCC_CACHE_FIELD_CHANNEL_TYPE) == 0 && merged->type == 0) {
        merged->type = existing->type;
    }
    if ((fields & DCC_CACHE_FIELD_CHANNEL_POSITION) == 0 && merged->position == 0) {
        merged->position = existing->position;
    }
    if ((fields & DCC_CACHE_FIELD_CHANNEL_RATE_LIMIT_PER_USER) == 0 && merged->rate_limit_per_user == 0) {
        merged->rate_limit_per_user = existing->rate_limit_per_user;
    }
    if ((fields & DCC_CACHE_FIELD_CHANNEL_BITRATE) == 0 && merged->bitrate == 0) {
        merged->bitrate = existing->bitrate;
    }
    if ((fields & DCC_CACHE_FIELD_CHANNEL_USER_LIMIT) == 0 && merged->user_limit == 0) {
        merged->user_limit = existing->user_limit;
    }
    if ((fields & DCC_CACHE_FIELD_CHANNEL_NSFW) == 0 && merged->nsfw == 0) {
        merged->nsfw = existing->nsfw;
    }
    if ((fields & DCC_CACHE_FIELD_CHANNEL_NAME) == 0 && merged->name == NULL) {
        merged->name = existing->name;
    }
    if (merged->description == NULL) {
        merged->description = existing->description;
    }
    if ((fields & DCC_CACHE_FIELD_CHANNEL_TOPIC) == 0 && merged->topic == NULL) {
        merged->topic = existing->topic;
    }
}

void dcc_cache_merge_role_view(dcc_role_t *merged, const dcc_role_t *existing, uint64_t fields) {
    if (merged == NULL || existing == NULL) {
        return;
    }
    if (merged->id == 0) {
        merged->id = existing->id;
    }
    if (merged->guild_id == 0) {
        merged->guild_id = existing->guild_id;
    }
    if ((fields & DCC_CACHE_FIELD_ROLE_PERMISSIONS) == 0 && merged->permissions == 0) {
        merged->permissions = existing->permissions;
    }
    if ((fields & DCC_CACHE_FIELD_ROLE_COLOR) == 0 && merged->color == 0) {
        merged->color = existing->color;
    }
    if ((fields & DCC_CACHE_FIELD_ROLE_POSITION) == 0 && merged->position == 0) {
        merged->position = existing->position;
    }
    if ((fields & DCC_CACHE_FIELD_ROLE_HOIST) == 0 && merged->hoist == 0) {
        merged->hoist = existing->hoist;
    }
    if ((fields & DCC_CACHE_FIELD_ROLE_MANAGED) == 0 && merged->managed == 0) {
        merged->managed = existing->managed;
    }
    if ((fields & DCC_CACHE_FIELD_ROLE_MENTIONABLE) == 0 && merged->mentionable == 0) {
        merged->mentionable = existing->mentionable;
    }
    if ((fields & DCC_CACHE_FIELD_ROLE_NAME) == 0 && merged->name == NULL) {
        merged->name = existing->name;
    }
}
