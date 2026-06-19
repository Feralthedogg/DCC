#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_channel_t *dcc_channel_clone(const dcc_channel_t *channel) {
    if (channel == NULL) {
        return NULL;
    }

    dcc_channel_t *copy = (dcc_channel_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    if (dcc_clone_channel_fields(copy, channel) != 0) {
        dcc_channel_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_channel_free(dcc_channel_t *channel) {
    if (channel == NULL) {
        return;
    }
    dcc_free_channel_fields(channel);
    free(channel);
}
