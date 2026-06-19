#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

int dcc_clone_guild_resource_collection_fields(dcc_guild_t *copy, const dcc_guild_t *guild) {
    if (guild->scheduled_events != NULL && guild->scheduled_events_count > 0) {
        dcc_scheduled_event_t *events = (dcc_scheduled_event_t *)calloc(
            guild->scheduled_events_count,
            sizeof(*events)
        );
        if (events == NULL) {
            return -1;
        }
        copy->scheduled_events = events;
        for (size_t i = 0; i < guild->scheduled_events_count; i++) {
            if (dcc_clone_scheduled_event_fields(&events[i], &guild->scheduled_events[i]) != 0) {
                return -1;
            }
        }
    }
    if (guild->stage_instances != NULL && guild->stage_instances_count > 0) {
        dcc_stage_instance_t *stage_instances = (dcc_stage_instance_t *)calloc(
            guild->stage_instances_count,
            sizeof(*stage_instances)
        );
        if (stage_instances == NULL) {
            return -1;
        }
        copy->stage_instances = stage_instances;
        for (size_t i = 0; i < guild->stage_instances_count; i++) {
            if (dcc_clone_stage_instance_fields(&stage_instances[i], &guild->stage_instances[i]) != 0) {
                return -1;
            }
        }
    }
    if (guild->presences != NULL && guild->presences_count > 0) {
        dcc_presence_t *presences = (dcc_presence_t *)calloc(guild->presences_count, sizeof(*presences));
        if (presences == NULL) {
            return -1;
        }
        copy->presences = presences;
        for (size_t i = 0; i < guild->presences_count; i++) {
            if (dcc_clone_presence_fields(&presences[i], &guild->presences[i]) != 0) {
                return -1;
            }
        }
    }
    if (guild->emojis != NULL && guild->emojis_count > 0) {
        dcc_emoji_t *emojis = (dcc_emoji_t *)calloc(guild->emojis_count, sizeof(*emojis));
        if (emojis == NULL) {
            return -1;
        }
        copy->emojis = emojis;
        for (size_t i = 0; i < guild->emojis_count; i++) {
            if (dcc_clone_emoji_fields(&emojis[i], &guild->emojis[i]) != 0) {
                return -1;
            }
        }
    }
    if (guild->stickers != NULL && guild->stickers_count > 0) {
        dcc_sticker_t *stickers = (dcc_sticker_t *)calloc(guild->stickers_count, sizeof(*stickers));
        if (stickers == NULL) {
            return -1;
        }
        copy->stickers = stickers;
        for (size_t i = 0; i < guild->stickers_count; i++) {
            if (dcc_clone_sticker_fields(&stickers[i], &guild->stickers[i]) != 0) {
                return -1;
            }
        }
    }
    if (guild->soundboard_sounds != NULL && guild->soundboard_sounds_count > 0) {
        dcc_soundboard_sound_t *sounds = (dcc_soundboard_sound_t *)calloc(
            guild->soundboard_sounds_count,
            sizeof(*sounds)
        );
        if (sounds == NULL) {
            return -1;
        }
        copy->soundboard_sounds = sounds;
        for (size_t i = 0; i < guild->soundboard_sounds_count; i++) {
            if (dcc_clone_soundboard_sound_fields(&sounds[i], &guild->soundboard_sounds[i]) != 0) {
                return -1;
            }
        }
    }
    return 0;
}
