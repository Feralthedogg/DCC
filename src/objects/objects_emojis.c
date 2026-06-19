#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_emoji_t *dcc_emoji_clone(const dcc_emoji_t *emoji) {
    if (emoji == NULL) {
        return NULL;
    }

    dcc_emoji_t *copy = (dcc_emoji_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    if (dcc_clone_emoji_fields(copy, emoji) != 0) {
        dcc_emoji_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_emoji_free(dcc_emoji_t *emoji) {
    if (emoji == NULL) {
        return;
    }
    dcc_free_emoji_fields(emoji);
    free(emoji);
}
