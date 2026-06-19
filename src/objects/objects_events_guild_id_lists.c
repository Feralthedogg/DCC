#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_guild_id_list_update_t *dcc_guild_id_list_update_clone(const dcc_guild_id_list_update_t *update) {
    if (update == NULL) {
        return NULL;
    }

    dcc_guild_id_list_update_t *copy = (dcc_guild_id_list_update_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }
    *copy = *update;
    copy->ids = NULL;
    copy->emojis = NULL;
    copy->stickers = NULL;
    copy->soundboard_sounds = NULL;
    if (update->ids != NULL && update->ids_count > 0) {
        copy->ids = dcc_clone_snowflake_array_or_null(update->ids, update->ids_count);
        if (copy->ids == NULL) {
            dcc_guild_id_list_update_free(copy);
            return NULL;
        }
    }
    if (update->emojis != NULL && update->emojis_count > 0) {
        dcc_emoji_t *emojis = (dcc_emoji_t *)calloc(update->emojis_count, sizeof(*emojis));
        if (emojis == NULL) {
            dcc_guild_id_list_update_free(copy);
            return NULL;
        }
        copy->emojis = emojis;
        for (size_t i = 0; i < update->emojis_count; i++) {
            dcc_emoji_t *item = dcc_emoji_clone(&update->emojis[i]);
            if (item == NULL) {
                dcc_guild_id_list_update_free(copy);
                return NULL;
            }
            emojis[i] = *item;
            free(item);
        }
    }
    if (update->stickers != NULL && update->stickers_count > 0) {
        dcc_sticker_t *stickers = (dcc_sticker_t *)calloc(update->stickers_count, sizeof(*stickers));
        if (stickers == NULL) {
            dcc_guild_id_list_update_free(copy);
            return NULL;
        }
        copy->stickers = stickers;
        for (size_t i = 0; i < update->stickers_count; i++) {
            dcc_sticker_t *item = dcc_sticker_clone(&update->stickers[i]);
            if (item == NULL) {
                dcc_guild_id_list_update_free(copy);
                return NULL;
            }
            stickers[i] = *item;
            free(item);
        }
    }
    if (update->soundboard_sounds != NULL && update->soundboard_sounds_count > 0) {
        dcc_soundboard_sound_t *sounds = (dcc_soundboard_sound_t *)calloc(
            update->soundboard_sounds_count,
            sizeof(*sounds)
        );
        if (sounds == NULL) {
            dcc_guild_id_list_update_free(copy);
            return NULL;
        }
        copy->soundboard_sounds = sounds;
        for (size_t i = 0; i < update->soundboard_sounds_count; i++) {
            dcc_soundboard_sound_t *item = dcc_soundboard_sound_clone(&update->soundboard_sounds[i]);
            if (item == NULL) {
                dcc_guild_id_list_update_free(copy);
                return NULL;
            }
            sounds[i] = *item;
            free(item);
        }
    }
    return copy;
}
