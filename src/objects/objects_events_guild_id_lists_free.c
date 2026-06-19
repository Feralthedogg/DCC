#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

void dcc_guild_id_list_update_free(dcc_guild_id_list_update_t *update) {
    if (update == NULL) {
        return;
    }
    free((void *)update->ids);
    if (update->emojis != NULL) {
        dcc_emoji_t *emojis = (dcc_emoji_t *)update->emojis;
        for (size_t i = 0; i < update->emojis_count; i++) {
            dcc_free_emoji_fields(&emojis[i]);
        }
        free(emojis);
    }
    if (update->stickers != NULL) {
        dcc_sticker_t *stickers = (dcc_sticker_t *)update->stickers;
        for (size_t i = 0; i < update->stickers_count; i++) {
            dcc_free_sticker_fields(&stickers[i]);
        }
        free(stickers);
    }
    if (update->soundboard_sounds != NULL) {
        dcc_soundboard_sound_t *sounds = (dcc_soundboard_sound_t *)update->soundboard_sounds;
        for (size_t i = 0; i < update->soundboard_sounds_count; i++) {
            dcc_free_soundboard_sound_fields(&sounds[i]);
        }
        free(sounds);
    }
    free(update);
}
