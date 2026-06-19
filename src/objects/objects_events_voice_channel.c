#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_voice_channel_start_time_update_t *dcc_voice_channel_start_time_update_clone(
    const dcc_voice_channel_start_time_update_t *update
) {
    if (update == NULL) {
        return NULL;
    }

    dcc_voice_channel_start_time_update_t *copy = (dcc_voice_channel_start_time_update_t *)calloc(
        1,
        sizeof(*copy)
    );
    if (copy == NULL) {
        return NULL;
    }
    *copy = *update;
    return copy;
}

void dcc_voice_channel_start_time_update_free(dcc_voice_channel_start_time_update_t *update) {
    free(update);
}

dcc_voice_channel_status_update_t *dcc_voice_channel_status_update_clone(
    const dcc_voice_channel_status_update_t *update
) {
    if (update == NULL) {
        return NULL;
    }

    dcc_voice_channel_status_update_t *copy = (dcc_voice_channel_status_update_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    *copy = *update;
    copy->status = dcc_clone_string_or_null(update->status);
    if (update->status != NULL && copy->status == NULL) {
        dcc_voice_channel_status_update_free(copy);
        return NULL;
    }
    return copy;
}

void dcc_voice_channel_status_update_free(dcc_voice_channel_status_update_t *update) {
    if (update == NULL) {
        return;
    }
    free((void *)update->status);
    free(update);
}
