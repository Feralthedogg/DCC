#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_member_t *dcc_member_clone(const dcc_member_t *member) {
    if (member == NULL) {
        return NULL;
    }

    dcc_member_t *copy = (dcc_member_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    if (dcc_clone_member_fields(copy, member) != 0) {
        dcc_member_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_member_free(dcc_member_t *member) {
    if (member == NULL) {
        return;
    }
    dcc_free_member_fields(member);
    free(member);
}
