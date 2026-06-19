#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

int dcc_gateway_clone_thread_member_collection(
    dcc_gateway_event_data_t *copy,
    const dcc_gateway_event_data_t *data
) {
    if (data->added_members == NULL || data->added_members_count == 0) {
        return 0;
    }
    if (data->added_members_count > (size_t)-1 / sizeof(dcc_thread_member_t)) {
        return -1;
    }

    dcc_thread_member_t *members =
        (dcc_thread_member_t *)calloc(data->added_members_count, sizeof(*members));
    if (members == NULL) {
        return -1;
    }
    copy->added_members = members;
    for (size_t i = 0; i < data->added_members_count; i++) {
        if (dcc_clone_thread_member_fields(&members[i], &data->added_members[i]) != 0) {
            return -1;
        }
    }
    return 0;
}

int dcc_gateway_clone_member_collection(
    dcc_gateway_event_data_t *copy,
    const dcc_gateway_event_data_t *data
) {
    if (data->members == NULL || data->members_count == 0) {
        return 0;
    }
    if (data->members_count > (size_t)-1 / sizeof(dcc_member_t)) {
        return -1;
    }

    dcc_member_t *members = (dcc_member_t *)calloc(data->members_count, sizeof(*members));
    if (members == NULL) {
        return -1;
    }
    copy->members = members;
    for (size_t i = 0; i < data->members_count; i++) {
        if (dcc_clone_member_fields(&members[i], &data->members[i]) != 0) {
            return -1;
        }
    }
    return 0;
}
