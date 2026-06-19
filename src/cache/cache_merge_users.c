#include "internal/cache/dcc_cache_internal.h"

void dcc_cache_merge_user_view(dcc_user_t *merged, const dcc_user_t *existing, uint64_t fields) {
    if (merged == NULL || existing == NULL) {
        return;
    }
    if (merged->id == 0) {
        merged->id = existing->id;
    }
    if ((fields & DCC_CACHE_FIELD_USER_PROFILE) == 0 && merged->username == NULL) {
        merged->username = existing->username;
    }
    if ((fields & DCC_CACHE_FIELD_USER_PROFILE) == 0 && merged->global_name == NULL) {
        merged->global_name = existing->global_name;
    }
    if ((fields & DCC_CACHE_FIELD_USER_PROFILE) == 0 && merged->discriminator == 0) {
        merged->discriminator = existing->discriminator;
    }
    if ((fields & DCC_CACHE_FIELD_USER_BOT) == 0 && merged->bot == 0) {
        merged->bot = existing->bot;
    }
    if ((fields & DCC_CACHE_FIELD_USER_FLAGS) == 0 && merged->flags == 0) {
        merged->flags = existing->flags;
    }
    if ((fields & DCC_CACHE_FIELD_USER_PROFILE) == 0 && merged->avatar == NULL) {
        merged->avatar = existing->avatar;
    }
}

void dcc_cache_merge_member_view(dcc_member_t *merged, const dcc_member_t *existing, uint64_t fields) {
    if (merged == NULL || existing == NULL) {
        return;
    }
    if (merged->guild_id == 0) {
        merged->guild_id = existing->guild_id;
    }
    if (merged->user.id == 0) {
        merged->user = existing->user;
    } else if (merged->user.id == existing->user.id) {
        dcc_cache_merge_user_view(&merged->user, &existing->user, fields);
    }
    if ((fields & DCC_CACHE_FIELD_MEMBER_ROLES) == 0 && merged->role_ids == NULL && merged->role_ids_count == 0) {
        merged->role_ids = existing->role_ids;
        merged->role_ids_count = existing->role_ids_count;
        merged->role_ids_truncated = existing->role_ids_truncated;
    }
    if ((fields & DCC_CACHE_FIELD_MEMBER_FLAGS) == 0 && merged->flags == 0) {
        merged->flags = existing->flags;
    }
    if ((fields & DCC_CACHE_FIELD_MEMBER_PENDING) == 0 && merged->pending == 0) {
        merged->pending = existing->pending;
    }
    if (merged->nick == NULL) {
        merged->nick = existing->nick;
    }
    if (merged->avatar == NULL) {
        merged->avatar = existing->avatar;
    }
    if (merged->joined_at == NULL) {
        merged->joined_at = existing->joined_at;
    }
    if (merged->premium_since == NULL) {
        merged->premium_since = existing->premium_since;
    }
    if (merged->communication_disabled_until == NULL) {
        merged->communication_disabled_until = existing->communication_disabled_until;
    }
}
