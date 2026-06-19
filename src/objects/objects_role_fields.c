#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_role_fields(dcc_role_t *role) {
    if (role == NULL) {
        return;
    }
    free((void *)role->name);
    role->name = NULL;
}

int dcc_clone_role_fields(dcc_role_t *copy, const dcc_role_t *role) {
    if (copy == NULL || role == NULL) {
        return -1;
    }

    *copy = *role;
    copy->name = dcc_clone_string_or_null(role->name);
    if (role->name != NULL && copy->name == NULL) {
        dcc_free_role_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}
