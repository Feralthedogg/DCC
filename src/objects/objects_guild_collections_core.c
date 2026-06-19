#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

int dcc_clone_guild_core_collection_fields(dcc_guild_t *copy, const dcc_guild_t *guild) {
    if (guild->roles != NULL && guild->roles_count > 0) {
        dcc_role_t *roles = (dcc_role_t *)calloc(guild->roles_count, sizeof(*roles));
        if (roles == NULL) {
            return -1;
        }
        copy->roles = roles;
        for (size_t i = 0; i < guild->roles_count; i++) {
            if (dcc_clone_role_fields(&roles[i], &guild->roles[i]) != 0) {
                return -1;
            }
        }
    }
    if (guild->channels != NULL && guild->channels_count > 0) {
        dcc_channel_t *channels = (dcc_channel_t *)calloc(guild->channels_count, sizeof(*channels));
        if (channels == NULL) {
            return -1;
        }
        copy->channels = channels;
        for (size_t i = 0; i < guild->channels_count; i++) {
            if (dcc_clone_channel_fields(&channels[i], &guild->channels[i]) != 0) {
                return -1;
            }
        }
    }
    if (guild->threads != NULL && guild->threads_count > 0) {
        dcc_channel_t *threads = (dcc_channel_t *)calloc(guild->threads_count, sizeof(*threads));
        if (threads == NULL) {
            return -1;
        }
        copy->threads = threads;
        for (size_t i = 0; i < guild->threads_count; i++) {
            if (dcc_clone_channel_fields(&threads[i], &guild->threads[i]) != 0) {
                return -1;
            }
        }
    }
    return 0;
}
