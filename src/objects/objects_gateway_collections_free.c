#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

void dcc_gateway_event_data_free_collections(dcc_gateway_event_data_t *data) {
    if (data == NULL) {
        return;
    }
    free((void *)data->ids);
    free((void *)data->channel_ids);
    free((void *)data->removed_member_ids);
    free((void *)data->member_ids);
    if (data->added_members != NULL) {
        dcc_thread_member_t *members = (dcc_thread_member_t *)data->added_members;
        for (size_t i = 0; i < data->added_members_count; i++) {
            dcc_free_thread_member_fields(&members[i]);
        }
        free(members);
    }
    if (data->members != NULL) {
        dcc_member_t *members = (dcc_member_t *)data->members;
        for (size_t i = 0; i < data->members_count; i++) {
            dcc_free_member_fields(&members[i]);
        }
        free(members);
    }
    free((void *)data->role_ids);
    free((void *)data->not_found_ids);
    free((void *)data->sku_ids);
    free((void *)data->entitlement_ids);
    if (data->activities != NULL) {
        dcc_gateway_activity_t *activities = (dcc_gateway_activity_t *)data->activities;
        for (size_t i = 0; i < data->activities_count; i++) {
            dcc_gateway_activity_free_fields(&activities[i]);
        }
        free(activities);
    }
}
