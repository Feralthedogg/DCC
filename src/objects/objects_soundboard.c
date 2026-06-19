#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_soundboard_sound_fields(dcc_soundboard_sound_t *sound) {
    if (sound == NULL) {
        return;
    }
    dcc_free_user_fields(&sound->user);
    free((void *)sound->name);
    free((void *)sound->emoji_name);
    sound->name = NULL;
    sound->emoji_name = NULL;
}

int dcc_clone_soundboard_sound_fields(
    dcc_soundboard_sound_t *copy,
    const dcc_soundboard_sound_t *sound
) {
    if (copy == NULL || sound == NULL) {
        return -1;
    }

    *copy = *sound;
    copy->user.username = NULL;
    copy->user.global_name = NULL;
    copy->user.avatar = NULL;
    copy->name = NULL;
    copy->emoji_name = NULL;
    if (dcc_clone_user_fields(&copy->user, &sound->user) != 0) {
        memset(copy, 0, sizeof(*copy));
        return -1;
    }
    copy->name = dcc_clone_string_or_null(sound->name);
    copy->emoji_name = dcc_clone_string_or_null(sound->emoji_name);
    if ((sound->name != NULL && copy->name == NULL) ||
        (sound->emoji_name != NULL && copy->emoji_name == NULL)) {
        dcc_free_soundboard_sound_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}

dcc_soundboard_sound_t *dcc_soundboard_sound_clone(const dcc_soundboard_sound_t *soundboard_sound) {
    if (soundboard_sound == NULL) {
        return NULL;
    }

    dcc_soundboard_sound_t *copy = (dcc_soundboard_sound_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    if (dcc_clone_soundboard_sound_fields(copy, soundboard_sound) != 0) {
        dcc_soundboard_sound_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_soundboard_sound_free(dcc_soundboard_sound_t *soundboard_sound) {
    if (soundboard_sound == NULL) {
        return;
    }
    dcc_free_soundboard_sound_fields(soundboard_sound);
    free(soundboard_sound);
}
