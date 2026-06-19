#include "internal/objects/dcc_interaction_resolved_clone_internal.h"

#include <stdlib.h>

int dcc_clone_interaction_resolved_people(
    dcc_interaction_resolved_t *copy,
    const dcc_interaction_resolved_t *resolved
) {
    if (resolved->users_count > 0) {
        if (resolved->users == NULL || resolved->users_count > (size_t)-1 / sizeof(dcc_user_t)) {
            return -1;
        }
        dcc_user_t *users = (dcc_user_t *)calloc(resolved->users_count, sizeof(*users));
        if (users == NULL) {
            return -1;
        }
        copy->users = users;
        for (size_t i = 0; i < resolved->users_count; i++) {
            if (dcc_clone_user_fields(&users[i], &resolved->users[i]) != 0) {
                return -1;
            }
        }
    }

    if (resolved->members_count > 0) {
        if (resolved->members == NULL || resolved->members_count > (size_t)-1 / sizeof(dcc_member_t)) {
            return -1;
        }
        dcc_member_t *members = (dcc_member_t *)calloc(resolved->members_count, sizeof(*members));
        if (members == NULL) {
            return -1;
        }
        copy->members = members;
        for (size_t i = 0; i < resolved->members_count; i++) {
            if (dcc_clone_member_fields(&members[i], &resolved->members[i]) != 0) {
                return -1;
            }
        }
    }

    return 0;
}
