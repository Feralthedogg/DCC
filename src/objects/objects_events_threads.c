#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_thread_member_t *dcc_thread_member_clone(const dcc_thread_member_t *thread_member) {
    if (thread_member == NULL) {
        return NULL;
    }

    dcc_thread_member_t *copy = (dcc_thread_member_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    if (dcc_clone_thread_member_fields(copy, thread_member) != 0) {
        dcc_thread_member_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_thread_member_free(dcc_thread_member_t *thread_member) {
    if (thread_member == NULL) {
        return;
    }
    free((void *)thread_member->join_timestamp);
    free(thread_member);
}

dcc_thread_members_update_t *dcc_thread_members_update_clone(
    const dcc_thread_members_update_t *thread_members
) {
    if (thread_members == NULL) {
        return NULL;
    }

    dcc_thread_members_update_t *copy = (dcc_thread_members_update_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *thread_members;
    copy->added_members = NULL;
    copy->removed_member_ids = NULL;
    copy->member_ids = NULL;
    if (thread_members->added_members != NULL && thread_members->added_members_count > 0) {
        dcc_thread_member_t *members = (dcc_thread_member_t *)calloc(
            thread_members->added_members_count,
            sizeof(*members)
        );
        if (members == NULL) {
            dcc_thread_members_update_free(copy);
            return NULL;
        }
        copy->added_members = members;
        for (size_t i = 0; i < thread_members->added_members_count; i++) {
            if (dcc_clone_thread_member_fields(&members[i], &thread_members->added_members[i]) != 0) {
                dcc_thread_members_update_free(copy);
                return NULL;
            }
        }
    }
    if (thread_members->removed_member_ids != NULL && thread_members->removed_member_ids_count > 0) {
        copy->removed_member_ids = dcc_clone_snowflake_array_or_null(
            thread_members->removed_member_ids,
            thread_members->removed_member_ids_count
        );
        if (copy->removed_member_ids == NULL) {
            dcc_thread_members_update_free(copy);
            return NULL;
        }
    }
    if (thread_members->member_ids != NULL && thread_members->member_ids_count > 0) {
        copy->member_ids = dcc_clone_snowflake_array_or_null(
            thread_members->member_ids,
            thread_members->member_ids_count
        );
        if (copy->member_ids == NULL) {
            dcc_thread_members_update_free(copy);
            return NULL;
        }
    }

    return copy;
}

void dcc_thread_members_update_free(dcc_thread_members_update_t *thread_members) {
    if (thread_members == NULL) {
        return;
    }
    if (thread_members->added_members != NULL) {
        dcc_thread_member_t *members = (dcc_thread_member_t *)thread_members->added_members;
        for (size_t i = 0; i < thread_members->added_members_count; i++) {
            dcc_free_thread_member_fields(&members[i]);
        }
        free(members);
    }
    free((void *)thread_members->removed_member_ids);
    free((void *)thread_members->member_ids);
    free(thread_members);
}
