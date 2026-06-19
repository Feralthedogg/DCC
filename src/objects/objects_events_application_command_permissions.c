#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_application_command_permissions_update_t *dcc_application_command_permissions_update_clone(
    const dcc_application_command_permissions_update_t *update
) {
    if (update == NULL) {
        return NULL;
    }

    dcc_application_command_permissions_update_t *copy = (dcc_application_command_permissions_update_t *)calloc(
        1,
        sizeof(*copy)
    );
    if (copy == NULL) {
        return NULL;
    }
    *copy = *update;
    copy->permissions = NULL;
    if (update->permissions != NULL && update->permissions_count > 0) {
        dcc_application_command_permission_t *permissions = (dcc_application_command_permission_t *)malloc(
            update->permissions_count * sizeof(*permissions)
        );
        if (permissions == NULL) {
            dcc_application_command_permissions_update_free(copy);
            return NULL;
        }
        memcpy(permissions, update->permissions, update->permissions_count * sizeof(*permissions));
        copy->permissions = permissions;
    }
    return copy;
}

void dcc_application_command_permissions_update_free(dcc_application_command_permissions_update_t *update) {
    if (update == NULL) {
        return;
    }
    free((void *)update->permissions);
    free(update);
}

dcc_application_command_permission_t *dcc_application_command_permission_clone(
    const dcc_application_command_permission_t *permission
) {
    if (permission == NULL) {
        return NULL;
    }

    dcc_application_command_permission_t *copy = (dcc_application_command_permission_t *)calloc(
        1,
        sizeof(*copy)
    );
    if (copy == NULL) {
        return NULL;
    }
    *copy = *permission;
    return copy;
}

void dcc_application_command_permission_free(dcc_application_command_permission_t *permission) {
    free(permission);
}
