#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

void dcc_free_guild_collection_fields(dcc_guild_t *guild) {
    if (guild->roles != NULL) {
        dcc_role_t *roles = (dcc_role_t *)guild->roles;
        for (size_t i = 0; i < guild->roles_count; i++) {
            dcc_free_role_fields(&roles[i]);
        }
        free(roles);
    }
    if (guild->channels != NULL) {
        dcc_channel_t *channels = (dcc_channel_t *)guild->channels;
        for (size_t i = 0; i < guild->channels_count; i++) {
            dcc_free_channel_fields(&channels[i]);
        }
        free(channels);
    }
    if (guild->threads != NULL) {
        dcc_channel_t *threads = (dcc_channel_t *)guild->threads;
        for (size_t i = 0; i < guild->threads_count; i++) {
            dcc_free_channel_fields(&threads[i]);
        }
        free(threads);
    }
    if (guild->scheduled_events != NULL) {
        dcc_scheduled_event_t *events = (dcc_scheduled_event_t *)guild->scheduled_events;
        for (size_t i = 0; i < guild->scheduled_events_count; i++) {
            dcc_free_scheduled_event_fields(&events[i]);
        }
        free(events);
    }
    if (guild->stage_instances != NULL) {
        dcc_stage_instance_t *stage_instances = (dcc_stage_instance_t *)guild->stage_instances;
        for (size_t i = 0; i < guild->stage_instances_count; i++) {
            dcc_free_stage_instance_fields(&stage_instances[i]);
        }
        free(stage_instances);
    }
    if (guild->presences != NULL) {
        dcc_presence_t *presences = (dcc_presence_t *)guild->presences;
        for (size_t i = 0; i < guild->presences_count; i++) {
            dcc_free_presence_fields(&presences[i]);
        }
        free(presences);
    }
    if (guild->emojis != NULL) {
        dcc_emoji_t *emojis = (dcc_emoji_t *)guild->emojis;
        for (size_t i = 0; i < guild->emojis_count; i++) {
            dcc_free_emoji_fields(&emojis[i]);
        }
        free(emojis);
    }
    if (guild->stickers != NULL) {
        dcc_sticker_t *stickers = (dcc_sticker_t *)guild->stickers;
        for (size_t i = 0; i < guild->stickers_count; i++) {
            dcc_free_sticker_fields(&stickers[i]);
        }
        free(stickers);
    }
    if (guild->soundboard_sounds != NULL) {
        dcc_soundboard_sound_t *sounds = (dcc_soundboard_sound_t *)guild->soundboard_sounds;
        for (size_t i = 0; i < guild->soundboard_sounds_count; i++) {
            dcc_free_soundboard_sound_fields(&sounds[i]);
        }
        free(sounds);
    }
}
