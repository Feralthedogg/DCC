#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_channel_fields(dcc_channel_t *channel) {
    if (channel == NULL) {
        return;
    }
    free((void *)channel->name);
    free((void *)channel->description);
    free((void *)channel->topic);
    channel->name = NULL;
    channel->description = NULL;
    channel->topic = NULL;
}

int dcc_clone_channel_fields(dcc_channel_t *copy, const dcc_channel_t *channel) {
    if (copy == NULL || channel == NULL) {
        return -1;
    }

    *copy = *channel;
    copy->name = dcc_clone_string_or_null(channel->name);
    copy->description = dcc_clone_string_or_null(channel->description);
    copy->topic = dcc_clone_string_or_null(channel->topic);
    if ((channel->name != NULL && copy->name == NULL) ||
        (channel->description != NULL && copy->description == NULL) ||
        (channel->topic != NULL && copy->topic == NULL)) {
        dcc_free_channel_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}
