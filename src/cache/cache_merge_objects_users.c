#include "internal/cache/dcc_cache_internal.h"

dcc_status_t dcc_cache_merge_member(dcc_client_t *client, const dcc_member_t *member, uint64_t fields) {
    if (member == NULL || member->guild_id == 0 || member->user.id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cache_lock(&client->cache);
    const dcc_member_t *existing =
        (const dcc_member_t *)dcc_cache_table_get(&client->cache.members, member->guild_id, member->user.id);
    dcc_member_t merged = *member;
    if (existing != NULL) {
        dcc_cache_merge_member_view(&merged, existing, fields);
    }
    dcc_member_t *copy = dcc_member_clone(&merged);
    if (copy == NULL) {
        dcc_cache_unlock(&client->cache);
        return DCC_ERR_NOMEM;
    }
    status = dcc_cache_table_put(
        &client->cache.members,
        member->guild_id,
        member->user.id,
        copy,
        dcc_cache_free_member_value,
        dcc_cache_next_sequence(&client->cache),
        client->cache_policy.max_members
    );
    dcc_cache_unlock(&client->cache);
    if (status != DCC_OK) {
        dcc_member_free(copy);
    }
    return status;
}

dcc_status_t dcc_cache_merge_user(dcc_client_t *client, const dcc_user_t *user, uint64_t fields) {
    if (user == NULL || user->id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cache_lock(&client->cache);
    const dcc_user_t *existing = (const dcc_user_t *)dcc_cache_table_get(&client->cache.users, user->id, 0);
    dcc_user_t merged = *user;
    if (existing != NULL) {
        dcc_cache_merge_user_view(&merged, existing, fields);
    }
    dcc_user_t *copy = dcc_user_clone(&merged);
    if (copy == NULL) {
        dcc_cache_unlock(&client->cache);
        return DCC_ERR_NOMEM;
    }
    status = dcc_cache_table_put(
        &client->cache.users,
        user->id,
        0,
        copy,
        dcc_cache_free_user_value,
        dcc_cache_next_sequence(&client->cache),
        client->cache_policy.max_users
    );
    dcc_cache_unlock(&client->cache);
    if (status != DCC_OK) {
        dcc_user_free(copy);
    }
    return status;
}
