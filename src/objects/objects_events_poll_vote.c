#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_poll_vote_t *dcc_poll_vote_clone(const dcc_poll_vote_t *poll_vote) {
    if (poll_vote == NULL) {
        return NULL;
    }

    dcc_poll_vote_t *copy = (dcc_poll_vote_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *poll_vote;
    return copy;
}

void dcc_poll_vote_free(dcc_poll_vote_t *poll_vote) {
    free(poll_vote);
}
