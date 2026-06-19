#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_thread_member_fields(dcc_thread_member_t *thread_member) {
    if (thread_member == NULL) {
        return;
    }
    free((void *)thread_member->join_timestamp);
    thread_member->join_timestamp = NULL;
}

int dcc_clone_thread_member_fields(dcc_thread_member_t *copy, const dcc_thread_member_t *thread_member) {
    if (copy == NULL || thread_member == NULL) {
        return -1;
    }

    *copy = *thread_member;
    copy->join_timestamp = dcc_clone_string_or_null(thread_member->join_timestamp);
    if (thread_member->join_timestamp != NULL && copy->join_timestamp == NULL) {
        dcc_free_thread_member_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}
