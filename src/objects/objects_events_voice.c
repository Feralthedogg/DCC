#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_voice_channel_effect_t *dcc_voice_channel_effect_clone(const dcc_voice_channel_effect_t *effect) {
    if (effect == NULL) {
        return NULL;
    }

    dcc_voice_channel_effect_t *copy = (dcc_voice_channel_effect_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *effect;
    copy->emoji_name = dcc_clone_string_or_null(effect->emoji_name);
    if (effect->emoji_name != NULL && copy->emoji_name == NULL) {
        dcc_voice_channel_effect_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_voice_channel_effect_free(dcc_voice_channel_effect_t *effect) {
    if (effect == NULL) {
        return;
    }
    free((void *)effect->emoji_name);
    free(effect);
}
