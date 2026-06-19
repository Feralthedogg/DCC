#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_scheduled_event_fields(dcc_scheduled_event_t *scheduled_event) {
    if (scheduled_event == NULL) {
        return;
    }
    dcc_free_user_fields(&scheduled_event->creator);
    free((void *)scheduled_event->name);
    free((void *)scheduled_event->description);
    free((void *)scheduled_event->image);
    free((void *)scheduled_event->location);
    free((void *)scheduled_event->start_time);
    free((void *)scheduled_event->end_time);
    scheduled_event->name = NULL;
    scheduled_event->description = NULL;
    scheduled_event->image = NULL;
    scheduled_event->location = NULL;
    scheduled_event->start_time = NULL;
    scheduled_event->end_time = NULL;
}

int dcc_clone_scheduled_event_fields(
    dcc_scheduled_event_t *copy,
    const dcc_scheduled_event_t *scheduled_event
) {
    if (copy == NULL || scheduled_event == NULL) {
        return -1;
    }

    *copy = *scheduled_event;
    copy->creator.username = NULL;
    copy->creator.global_name = NULL;
    copy->creator.avatar = NULL;
    copy->name = NULL;
    copy->description = NULL;
    copy->image = NULL;
    copy->location = NULL;
    copy->start_time = NULL;
    copy->end_time = NULL;
    if (dcc_clone_user_fields(&copy->creator, &scheduled_event->creator) != 0) {
        dcc_free_scheduled_event_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }
    copy->name = dcc_clone_string_or_null(scheduled_event->name);
    copy->description = dcc_clone_string_or_null(scheduled_event->description);
    copy->image = dcc_clone_string_or_null(scheduled_event->image);
    copy->location = dcc_clone_string_or_null(scheduled_event->location);
    copy->start_time = dcc_clone_string_or_null(scheduled_event->start_time);
    copy->end_time = dcc_clone_string_or_null(scheduled_event->end_time);
    if ((scheduled_event->name != NULL && copy->name == NULL) ||
        (scheduled_event->description != NULL && copy->description == NULL) ||
        (scheduled_event->image != NULL && copy->image == NULL) ||
        (scheduled_event->location != NULL && copy->location == NULL) ||
        (scheduled_event->start_time != NULL && copy->start_time == NULL) ||
        (scheduled_event->end_time != NULL && copy->end_time == NULL)) {
        dcc_free_scheduled_event_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}

dcc_scheduled_event_t *dcc_scheduled_event_clone(const dcc_scheduled_event_t *scheduled_event) {
    if (scheduled_event == NULL) {
        return NULL;
    }

    dcc_scheduled_event_t *copy = (dcc_scheduled_event_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    if (dcc_clone_scheduled_event_fields(copy, scheduled_event) != 0) {
        dcc_scheduled_event_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_scheduled_event_free(dcc_scheduled_event_t *scheduled_event) {
    if (scheduled_event == NULL) {
        return;
    }
    dcc_free_scheduled_event_fields(scheduled_event);
    free(scheduled_event);
}
