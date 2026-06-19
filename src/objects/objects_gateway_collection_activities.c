#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_gateway_activity_free_fields(dcc_gateway_activity_t *activity) {
    if (activity == NULL) {
        return;
    }
    free((void *)activity->name);
    free((void *)activity->state);
    free((void *)activity->details);
    free((void *)activity->url);
    free((void *)activity->emoji_name);
    activity->name = NULL;
    activity->state = NULL;
    activity->details = NULL;
    activity->url = NULL;
    activity->emoji_name = NULL;
}

int dcc_gateway_activity_clone_fields(
    dcc_gateway_activity_t *copy,
    const dcc_gateway_activity_t *activity
) {
    if (copy == NULL || activity == NULL) {
        return -1;
    }

    *copy = *activity;
    copy->name = dcc_clone_string_or_null(activity->name);
    copy->state = dcc_clone_string_or_null(activity->state);
    copy->details = dcc_clone_string_or_null(activity->details);
    copy->url = dcc_clone_string_or_null(activity->url);
    copy->emoji_name = dcc_clone_string_or_null(activity->emoji_name);
    if ((activity->name != NULL && copy->name == NULL) ||
        (activity->state != NULL && copy->state == NULL) ||
        (activity->details != NULL && copy->details == NULL) ||
        (activity->url != NULL && copy->url == NULL) ||
        (activity->emoji_name != NULL && copy->emoji_name == NULL)) {
        dcc_gateway_activity_free_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}

int dcc_gateway_clone_activity_collection(
    dcc_gateway_event_data_t *copy,
    const dcc_gateway_event_data_t *data
) {
    if (data->activities == NULL || data->activities_count == 0) {
        return 0;
    }
    if (data->activities_count > (size_t)-1 / sizeof(dcc_gateway_activity_t)) {
        return -1;
    }

    dcc_gateway_activity_t *activities =
        (dcc_gateway_activity_t *)calloc(data->activities_count, sizeof(*activities));
    if (activities == NULL) {
        return -1;
    }
    copy->activities = activities;
    for (size_t i = 0; i < data->activities_count; i++) {
        if (dcc_gateway_activity_clone_fields(&activities[i], &data->activities[i]) != 0) {
            return -1;
        }
    }
    return 0;
}
