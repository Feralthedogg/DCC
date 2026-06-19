#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_presence_fields(dcc_presence_t *presence) {
    if (presence == NULL) {
        return;
    }
    dcc_free_user_fields(&presence->user);
    free((void *)presence->status);
    free((void *)presence->desktop_status);
    free((void *)presence->mobile_status);
    free((void *)presence->web_status);
    if (presence->activities != NULL) {
        dcc_gateway_activity_t *activities = (dcc_gateway_activity_t *)presence->activities;
        for (size_t i = 0; i < presence->activities_count; i++) {
            dcc_gateway_activity_free_fields(&activities[i]);
        }
        free(activities);
    }
    presence->status = NULL;
    presence->desktop_status = NULL;
    presence->mobile_status = NULL;
    presence->web_status = NULL;
    presence->activities = NULL;
}

int dcc_clone_presence_fields(dcc_presence_t *copy, const dcc_presence_t *presence) {
    if (copy == NULL || presence == NULL) {
        return -1;
    }

    *copy = *presence;
    memset(&copy->user, 0, sizeof(copy->user));
    copy->activities = NULL;
    copy->status = dcc_clone_string_or_null(presence->status);
    copy->desktop_status = dcc_clone_string_or_null(presence->desktop_status);
    copy->mobile_status = dcc_clone_string_or_null(presence->mobile_status);
    copy->web_status = dcc_clone_string_or_null(presence->web_status);

    if (dcc_clone_user_fields(&copy->user, &presence->user) != 0) {
        dcc_free_presence_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    if (presence->activities != NULL && presence->activities_count > 0) {
        if (presence->activities_count > (size_t)-1 / sizeof(dcc_gateway_activity_t)) {
            dcc_free_presence_fields(copy);
            memset(copy, 0, sizeof(*copy));
            return -1;
        }
        dcc_gateway_activity_t *activities = (dcc_gateway_activity_t *)calloc(
            presence->activities_count,
            sizeof(*activities)
        );
        if (activities == NULL) {
            dcc_free_presence_fields(copy);
            memset(copy, 0, sizeof(*copy));
            return -1;
        }
        copy->activities = activities;
        for (size_t i = 0; i < presence->activities_count; i++) {
            if (dcc_gateway_activity_clone_fields(&activities[i], &presence->activities[i]) != 0) {
                dcc_free_presence_fields(copy);
                memset(copy, 0, sizeof(*copy));
                return -1;
            }
        }
    }

    if ((presence->status != NULL && copy->status == NULL) ||
        (presence->desktop_status != NULL && copy->desktop_status == NULL) ||
        (presence->mobile_status != NULL && copy->mobile_status == NULL) ||
        (presence->web_status != NULL && copy->web_status == NULL)) {
        dcc_free_presence_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}
