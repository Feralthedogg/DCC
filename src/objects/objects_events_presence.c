#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_presence_t *dcc_presence_clone(const dcc_presence_t *presence) {
    if (presence == NULL) {
        return NULL;
    }

    dcc_presence_t *copy = (dcc_presence_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    if (dcc_clone_presence_fields(copy, presence) != 0) {
        dcc_presence_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_presence_free(dcc_presence_t *presence) {
    if (presence == NULL) {
        return;
    }
    dcc_free_presence_fields(presence);
    free(presence);
}
