#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_member_fields(dcc_member_t *member) {
    if (member == NULL) {
        return;
    }
    dcc_free_user_fields(&member->user);
    free((void *)member->role_ids);
    free((void *)member->nick);
    free((void *)member->avatar);
    free((void *)member->joined_at);
    free((void *)member->premium_since);
    free((void *)member->communication_disabled_until);
    member->role_ids = NULL;
    member->nick = NULL;
    member->avatar = NULL;
    member->joined_at = NULL;
    member->premium_since = NULL;
    member->communication_disabled_until = NULL;
}

int dcc_clone_member_fields(dcc_member_t *copy, const dcc_member_t *member) {
    if (copy == NULL || member == NULL) {
        return -1;
    }

    *copy = *member;
    copy->role_ids = NULL;
    copy->user.username = dcc_clone_string_or_null(member->user.username);
    copy->user.global_name = dcc_clone_string_or_null(member->user.global_name);
    copy->user.avatar = dcc_clone_string_or_null(member->user.avatar);
    copy->nick = dcc_clone_string_or_null(member->nick);
    copy->avatar = dcc_clone_string_or_null(member->avatar);
    copy->joined_at = dcc_clone_string_or_null(member->joined_at);
    copy->premium_since = dcc_clone_string_or_null(member->premium_since);
    copy->communication_disabled_until = dcc_clone_string_or_null(member->communication_disabled_until);

    if (member->role_ids != NULL && member->role_ids_count > 0) {
        copy->role_ids = dcc_clone_snowflake_array_or_null(member->role_ids, member->role_ids_count);
        if (copy->role_ids == NULL) {
            dcc_free_member_fields(copy);
            memset(copy, 0, sizeof(*copy));
            return -1;
        }
    }

    if ((member->user.username != NULL && copy->user.username == NULL) ||
        (member->user.global_name != NULL && copy->user.global_name == NULL) ||
        (member->user.avatar != NULL && copy->user.avatar == NULL) ||
        (member->nick != NULL && copy->nick == NULL) ||
        (member->avatar != NULL && copy->avatar == NULL) ||
        (member->joined_at != NULL && copy->joined_at == NULL) ||
        (member->premium_since != NULL && copy->premium_since == NULL) ||
        (member->communication_disabled_until != NULL && copy->communication_disabled_until == NULL)) {
        dcc_free_member_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}
