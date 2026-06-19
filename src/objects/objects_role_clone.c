#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_role_t *dcc_role_clone(const dcc_role_t *role) {
    if (role == NULL) {
        return NULL;
    }

    dcc_role_t *copy = (dcc_role_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    if (dcc_clone_role_fields(copy, role) != 0) {
        dcc_role_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_role_free(dcc_role_t *role) {
    if (role == NULL) {
        return;
    }
    dcc_free_role_fields(role);
    free(role);
}
