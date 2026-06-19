#include "internal/objects/dcc_interaction_resolved_clone_internal.h"

#include <stdlib.h>

int dcc_clone_interaction_resolved_resources(
    dcc_interaction_resolved_t *copy,
    const dcc_interaction_resolved_t *resolved
) {
    if (resolved->roles_count > 0) {
        if (resolved->roles == NULL || resolved->roles_count > (size_t)-1 / sizeof(dcc_role_t)) {
            return -1;
        }
        dcc_role_t *roles = (dcc_role_t *)calloc(resolved->roles_count, sizeof(*roles));
        if (roles == NULL) {
            return -1;
        }
        copy->roles = roles;
        for (size_t i = 0; i < resolved->roles_count; i++) {
            if (dcc_clone_role_fields(&roles[i], &resolved->roles[i]) != 0) {
                return -1;
            }
        }
    }

    if (resolved->channels_count > 0) {
        if (resolved->channels == NULL || resolved->channels_count > (size_t)-1 / sizeof(dcc_channel_t)) {
            return -1;
        }
        dcc_channel_t *channels = (dcc_channel_t *)calloc(resolved->channels_count, sizeof(*channels));
        if (channels == NULL) {
            return -1;
        }
        copy->channels = channels;
        for (size_t i = 0; i < resolved->channels_count; i++) {
            if (dcc_clone_channel_fields(&channels[i], &resolved->channels[i]) != 0) {
                return -1;
            }
        }
    }

    if (resolved->messages_count > 0) {
        if (resolved->messages == NULL || resolved->messages_count > (size_t)-1 / sizeof(dcc_message_t)) {
            return -1;
        }
        dcc_message_t *messages = (dcc_message_t *)calloc(resolved->messages_count, sizeof(*messages));
        if (messages == NULL) {
            return -1;
        }
        copy->messages = messages;
        for (size_t i = 0; i < resolved->messages_count; i++) {
            if (dcc_clone_message_fields(&messages[i], &resolved->messages[i]) != 0) {
                return -1;
            }
        }
    }

    return 0;
}
