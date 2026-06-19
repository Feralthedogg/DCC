#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_sticker_t *dcc_sticker_clone(const dcc_sticker_t *sticker) {
    if (sticker == NULL) {
        return NULL;
    }

    dcc_sticker_t *copy = (dcc_sticker_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    if (dcc_clone_sticker_fields(copy, sticker) != 0) {
        dcc_sticker_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_sticker_free(dcc_sticker_t *sticker) {
    if (sticker == NULL) {
        return;
    }
    dcc_free_sticker_fields(sticker);
    free(sticker);
}
